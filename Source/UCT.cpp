#include "UCT.h"

/* Offsets for the four directly adjacent neighbors. Used for looping. */
static int deltai[4] = {-1, 1, 0, 0};
static int deltaj[4] = {0, 0, -1, 1};

void Node::update(double mc, vector<int> *actions)
{
    if (mc > 0) {
        this->delta_wins ++;
        this->wins ++;
    } else {
        this->delta_wins --;
    }
    this->MC_rate += mc;
	this->nMC++;

	if (this->parent != NULL){
        /* AMAF */
		static bool visited[MAXNUM_CHILDREN];
		memset(visited,0,sizeof(visited));
		for(int t=depth;t<(int)actions->size();t+=2)
			if (actions->at(t)>=0) visited[actions->at(t)] = true;

		Node *parent_ = this->parent;
		Node *cur_child = parent_->eldest_child;
		while (cur_child != NULL) {
			if (visited[cur_child->pos]){
				cur_child->nAMAF++;
				if (mc > 0){
					cur_child->AMAF_wins += 1;
				} else {
					cur_child->AMAF_wins += 0;
				}
			}
			cur_child = cur_child->sibling;
		}

		this->parent->update(-mc, actions);
	}
}

void UCTree::loop(NoBB *newnobb)
{
	vector<int> actions;
	Node * cur_node = root;

    newnobb->set_context(root_context);
	actions.push_back(cur_node->pos);
	while (cur_node->eldest_child!=NULL) {
		cur_node = cur_node->get_best_child();
		//check if pass move happens
		if (cur_node->pos == PASS_MOVE) break;

		newnobb->play_move(I(cur_node->pos),J(cur_node->pos),cur_node->color);
		actions.push_back(cur_node->pos);
	}
	NoBB_Context leaf_context = newnobb->get_context();   //backup context of leaf node

	stat->max_depth = (stat->max_depth < cur_node->depth) ? cur_node->depth : stat->max_depth;

	for(int i=0;i<NODE_EXPAND_LIMIT;++i) {
		newnobb->set_context(leaf_context);
		double mc = newnobb->monte_carlo(cur_node->color,cur_node->pos,&actions);
		this->stat->mc_count++;
		WaitForSingleObject(mutex, INFINITE);
		cur_node->update(mc, &actions);
		ReleaseMutex(mutex);            
	}

	WaitForSingleObject(mutex, INFINITE);
	if (cur_node->eldest_child == NULL) {
		/* multi-thread may expand node twice, so be careful to avoid it            */
		newnobb->set_context(leaf_context);
		expand_current_node(cur_node,newnobb);
	}
	ReleaseMutex(mutex);                    //release mutex
}

void UCTree::expand_current_node(Node *cu_node, NoBB * newnobb)
{
	int color = OTHER_COLOR(cu_node->color);
	int pre_pos = cu_node->pos;
	int prepre_pos = -100;
	if (cu_node->parent != NULL){
		prepre_pos = cu_node->parent->pos;
	}
	double rating[MAX_BOARD*MAX_BOARD];
	if (UCT_STARTEGY==2 || UCT_STARTEGY==4) {
        elorating elo = elorating();
        elo.calelo(newnobb,pre_pos,prepre_pos,color,rating);
	}

	for (int i = 0; i<board_size; ++i)
		for (int j = 0; j<board_size; ++j) {
			bool if_move = false;
			if (newnobb->legal_move(i, j, color) && !newnobb->suicide(i, j, color)) {
				Node * new_child = new Node(cu_node, color, cu_node->depth + 1, POS(i, j));
				if (UCT_STARTEGY==2 || UCT_STARTEGY==4)
                    new_child->H = rating[POS(i,j)];
				cu_node->addChild(new_child);
			}
		}

	//if doesn't exist children, return pass
	if (cu_node->eldest_child == NULL) {
		Node * new_child = new Node(cu_node, color, cu_node->depth + 1, POS(-1, -1));
		cu_node->addChild(new_child);
	}
}

void UCTree::printBoard(int *board)
{
	printf("-------%d\n", board_size);
	for (int i = 0; i<board_size; ++i) {
		for (int j = 0; j<board_size; ++j) {
			printf("%d ", board[POS(i, j)]);
		}
		printf("\n");
	}
	printf("\n");
}

DWORD WINAPI UCTree::run_loops(LPVOID Param)
{

	UCTree *pointer = (UCTree *)Param;
	NoBB *newnobb = new NoBB();

	time_t ts;
	ts = clock();
	int loop_count = 0;
	while (true) {
		pointer->loop(newnobb);
		//check time limit
		if (clock()-ts>TIME_LIMIT)
			break;
		loop_count++;
		//        printf(" %d:%d ",ts,td);
	}
	
	if (TEST_MODE)
		log_files<<"thread : run "<<loop_count*NODE_EXPAND_LIMIT <<"mcs\n";
	delete newnobb;
	return 0;
}


UCTree::UCTree(NoBB_Context context, int board_size, int color, int pos) {
    log_files.open("d://nobb_log_2.1.txt",ios::app);
    log_files << ">>>>>>>>>>>> premove:"<<pos<<"  color:"<< ((color==BLACK)?"black":"white")<<" <<<<<<<<<<<<<\n";

	this->board_size = board_size;
	this->color = color;
	this->root_context = context;
	//init root node
	NoBB *newnobb = new NoBB();
	newnobb->set_context(root_context);
	root = new Node(NULL, OTHER_COLOR(color), 0, pos);    //the root node must be the opposite color ,pos is invalid
	expand_current_node(root,newnobb);
	delete newnobb;

	this->stat = new Stat();
}

int UCTree::getOptimalPos(double * final_board)
{
	int result_pos = PASS_MOVE;
	//check if root node has no children
	if (root->eldest_child->pos == PASS_MOVE) {
		return PASS_MOVE;
	}
	//quick judge
	/*int i,j;
	if (quick_judge(&i,&j,OTHER_COLOR(root->color)))
		return POS(i,j);*/

	//use multi-thread to run loops
	mutex = CreateMutex(NULL, false, NULL);
	DWORD Threadld;
	for (int i = 0; i<THREAD_NUM; i++) {
		ThreadHandles[i] = CreateThread(NULL, 0, run_loops, this, 0, &Threadld);
		//        if (Threadld == 0)
		//            printf("error : %d\n",GetLastError());
	}
	//waiting for multi threads to terminate
	WaitForMultipleObjects(THREAD_NUM, ThreadHandles, true, INFINITE);
	//close all thread handles
	for (int i = 0; i<THREAD_NUM; i++) {
		CloseHandle(ThreadHandles[i]);
	}
	CloseHandle(mutex);

	//find max ucb child of root node
	Node *result_node = root->get_best_move();
	result_pos = result_node->pos;

	if (TEST_MODE) {
		stat->print(&log_files);
		Node *cur_child = root->eldest_child;
		while (cur_child != NULL) {
            int pos = cur_child->pos;
            log_files <<(char)(J(pos)+'a')<<(board_size-I(pos)) <<" | ";
			log_files<<" mc "<<(cur_child->wins)
					<<" nmc "<<cur_child->nMC
                    <<" amaf "<<(double)(cur_child->AMAF_wins)/cur_child->nAMAF
					<<" namaf "<<cur_child->nAMAF
					<<" score "<<cur_child->getScore()<<endl;
			cur_child = cur_child->sibling;
			stat->subtree_count ++;
		}
		log_files<<endl;
	}
	//    printf("\nchoose : %c:%d\n",I(result_pos)+'a',J(result_pos));
	memset(final_board, 0, sizeof(double)*MAX_BOARD*MAX_BOARD);
	Node *cur_child = root->eldest_child;
	while (cur_child != NULL) {
        final_board[cur_child->pos] = (cur_child->nMC);	//
        cur_child = cur_child->sibling;
	}
	return result_pos;
}

UCTree::~UCTree()
{
    log_files.close();
	delete stat;
	delete_subtree(root);
	if (TEST_MODE) {
		log_files<<"out \n";
		log_files<<"create node :"<<CREATED_NODES<<endl;
		log_files<<"delete node :"<<DELETED_NODES<<endl;
	}
}

void UCTree::delete_subtree(Node *node)
{
	if (node == NULL)
		return;
	if (node->eldest_child == NULL) {
		//printf("l depth: %d %d\n",node->depth,node->pos);
		delete node;
		return;
	}
	Node *next_child = NULL;
	Node *cur_child = node->eldest_child;
    while (true) {
        if (cur_child == NULL)
            break;
        next_child = cur_child->sibling;
        delete_subtree(cur_child);
        cur_child = next_child;
	}

	delete node;
}
