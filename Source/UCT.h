#ifndef UCT_H
#define UCT_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <windows.h>
#include <fstream>

#include "NoBB.h"
#include "config.h"
#include "elorating.h"
using namespace std;

#define MAXNUM_CHILDREN (MAX_BOARD*MAX_BOARD)   //max number of children node
#define oo   1350400000     //define an infinite number in calculate ucb

static int CREATED_NODES = 0; //gobal varibales to count nodes
static int DELETED_NODES = 0;

static ofstream log_files;

static int MC_COUNT;

class Stat
{
public:
	int max_depth;
	int mc_count;
	int subtree_count;
	int loop_count;
	Stat() {
		max_depth = 0;
		mc_count = 0;
		subtree_count = 0;
		loop_count = 0;
	}
	void print(ofstream *out) {
		(*out)<<"- - - - stat - - -\n";
		(*out)<<"max depth :"<<max_depth<<endl;
		(*out)<<"mc count  :"<< mc_count<<endl;
	}
};

class Node
{
public:
	Node * parent;
	Node * eldest_child;
	Node * sibling;

	int color, depth, pos;

	double MC_rate,H;
	int delta_wins,wins;
	int nMC;
	int AMAF_wins,nAMAF;

	Node(Node * parent, int color, int depth,  int pos){
		this->MC_rate=0;
		this->H = 0;
		this->delta_wins = 0;
		this->wins = 0;
		this->nMC = 0;
		this->AMAF_wins = 0;
		this->nAMAF = 0;

		this->parent = parent;
		this->eldest_child = NULL;
		this->sibling = NULL;
		this->color = color;
		this->depth = depth;
		this->pos = pos;
		CREATED_NODES++;
	}
	~Node() {
		DELETED_NODES++;
	}

	double getScore() {
		double score = 0;
		double belta, gamma;
		switch (UCT_STARTEGY) {
		case 0:   //hungry 
			if (nMC == 0) 
				score = 1000+rand()*GetCurrentThreadId()%clock();
			else 
				score = (double)(delta_wins)/nMC + 0.5*sqrt(log(1.0*this->parent->nMC) / nMC);
			break;

		case 1:		//uct
		    if (nMC == 0)
            	score = 1000+rand()*GetCurrentThreadId()%clock();
            else
            	score = (double)(wins)/nMC + sqrt(2*log(1.0*this->parent->nMC) / nMC);
			break;

		case 2:		//heuristic
            gamma = sqrt(50.0/(50.0+3*nMC));
            if (nMC == 0)
                score = 1.0/(1.0+exp(-1.0*0.2*H)); 
            else
                score = (1-gamma)*(double)(wins)/nMC + gamma*1.0/(1+exp(-1.0*0.2*H));
			break;

		case 3:		//amaf
			belta = (double)(nAMAF)/(nMC+nAMAF+0.015*nMC*nAMAF);// sqrt(1000.0/(1000.0+3*nMC)); 
            if (nMC==0 || nAMAF==0) {
                score = 1000+rand()*GetCurrentThreadId()%clock();
			} else
                score = (1-belta)*((double)(wins)/nMC) 
						  + belta*((double)(AMAF_wins)/nAMAF);
			break;

		case 4:		//mogo
			belta = sqrt(1000.0/(1000.0+3*nMC));  //2*20.0*nMC/(nMC*nMC+100);
			gamma = 0;//sqrt(10.0/(10.0+3*nMC));
            if (nMC == 0) 
                score = 1000+rand()*GetCurrentThreadId()%clock();
			else
                score = (1-belta-gamma)*((double)(wins)/nMC) 
								+ belta*((double)(AMAF_wins)/nAMAF)
								+ gamma*((double)(H)/(H+1));
			break;

		default:
			printf("please select a strategy !\n");
		}
		return score;
	}

	void update(double mc, vector<int> *actions);

	void addChild(Node * child) {
		if (eldest_child == NULL) {
			eldest_child = child;
			return;
		}
		Node *cur_child = eldest_child;
		while (cur_child->sibling != NULL) {
			cur_child = cur_child->sibling;
		}
		cur_child->sibling = child;
	}

	Node * get_best_child() {
		if (eldest_child == NULL) {
			return NULL;
		}
		Node * cadidates[MAXNUM_CHILDREN];
		int num_cadidates = 0;

		Node * cur_child = eldest_child;
		double best_score = -1000;
		while(cur_child != NULL) {
            double cur_score = cur_child->getScore();
            if (best_score < cur_score) {
                best_score = cur_score;
				num_cadidates = 1;
				cadidates[0] = cur_child;
            } else if (best_score == cur_score) {
				cadidates[num_cadidates++] = cur_child;
			}
            cur_child = cur_child->sibling;
		}

		return cadidates[rand()%num_cadidates];
	}
	Node * get_most_visited_child() {
	    if (eldest_child == NULL) {
			return NULL;
		}

		Node * cur_child = eldest_child;
		Node * best_node = NULL;
		double max_visit = 0;
		while(cur_child != NULL) {
            double cur_visit = cur_child->nMC;
            if (max_visit < cur_visit) {
                max_visit = cur_visit;
                best_node = cur_child;
            }
            cur_child = cur_child->sibling;
		}
		return best_node;
	}
	Node * get_best_rate_child() {
		if (eldest_child == NULL) {
			return NULL;
		}
		Node * cadidates[MAXNUM_CHILDREN];
		int num_cadidates = 0;

		Node * cur_child = eldest_child;
		double best_rate = -1000;
		while(cur_child != NULL) {
            double cur_rate = cur_child->MC_rate/cur_child->nMC;
            if (best_rate < cur_rate) {
                best_rate = cur_rate;
				num_cadidates = 1;
				cadidates[0] = cur_child;
            } else if (best_rate == cur_rate) {
				cadidates[num_cadidates++] = cur_child;
			}
            cur_child = cur_child->sibling;
		}

		return cadidates[rand()%num_cadidates];
	}
	Node * get_best_move() {
		Node *best_node = get_most_visited_child();
		if (best_node->getScore()==1 || best_node->getScore()==0) 
			best_node = get_best_rate_child();
		return best_node;
	}
};


class UCTree
{
private:
	Node * root;
	int board_size;
	int color;
	NoBB_Context root_context;
	Stat * stat;				//statistic for uctree efficiency
	HANDLE mutex;				//lock for multi-thread
	HANDLE ThreadHandles[THREAD_NUM];

	void loop(NoBB *newnobb);
	void expand_current_node(Node * current_node, NoBB *newnobb);
	void printBoard(int * board);
	void delete_subtree(Node * cur_node);

	static DWORD WINAPI run_loops(LPVOID Param);   //thread function

public:
	UCTree(NoBB_Context context, int board_size, int color, int pos);
	~UCTree();

	int getOptimalPos(double * final_board);
};

#endif
