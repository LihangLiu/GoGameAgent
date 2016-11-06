

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "NoBB.h"

/* Offsets for the four directly adjacent neighbors. Used for looping. */
int deltai[4] = {-1, 1, 0, 0};
int deltaj[4] = {0, 0, -1, 1};

/* variables for statistics by backchord */
int bc_mc_count = 0;

void NoBB::init_brown()
{
    //int k;
    //int i, j;

    /* The GTP specification leaves the initial board configuration as
    * well as the board configuration after a boardsize command to the
    * discretion of the engine. We choose to start with up to 20 random
    * stones on the board.
    */
    clear_board();
    memset(board,0,sizeof(board));
    /*
    for (k = 0; k < 20; k++) {
    int color = rand() % 2 ? BLACK : WHITE;
    generate_move(&i, &j, color);
    play_move(i, j, color);
    }
     */
}

double NoBB::monte_carlo(int color,int pre_pos,vector<int> *actions)
{

	int move;
	int pre_i = I(pre_pos), pre_j = J(pre_pos);
	int cur_color = OTHER_COLOR(color);
	//printf("\n--------------------   begin   ----------------\n");
	int count = 0;
	int num_pass = 0;
	while (true && count <= MC_LIMIT) {
		move = nakade(cur_color, pre_i, pre_j);
		if (pass_move(move)) move = atari_defense(cur_color, pre_i, pre_j);
		if (pass_move(move)) move = fill_board(6);
		if (pass_move(move)) move = generate_local_random_move_with_pattern(cur_color,pre_i,pre_j,1);
		if (pass_move(move)) move = generate_capture_move(cur_color);
		if (pass_move(move)) move = generate_random_move(cur_color);
		if (pass_move(move)) {
			if((++num_pass) == 2)
				break;
		}
		play_move(I(move), J(move), cur_color);
		actions->push_back(move);
		cur_color = OTHER_COLOR(cur_color);
		pre_i = I(move);
		pre_j = J(move);
		count++;
	}
	//printf("\n--------------------   end   ----------------\n");

	double A1 = get_black_win();
	A1 = (color==BLACK) ? A1 : -A1;

	return A1;// A / (board_size*board_size);
}


/* generate a move by searching neighbors of (pre_i,pre_j) and set i=-1 j=-1 if no move found */
int NoBB::generate_local_random_move(int color, int pre_i, int pre_j, int radius)
{
	int common_moves[MAX_BOARD * MAX_BOARD];
	int capture_moves[MAX_BOARD * MAX_BOARD];
	int defence_moves[MAX_BOARD * MAX_BOARD];
	int num_com_moves = 0;
	int num_cap_moves = 0;
	int num_def_moves = 0;
	int move;

	memset(common_moves, 0, sizeof(common_moves));
	memset(capture_moves, 0, sizeof(capture_moves));
	memset(defence_moves, 0, sizeof(defence_moves));
	for (int ai = pre_i - radius; ai <= pre_i + radius; ai++)
		for (int aj = pre_j - radius; aj <= pre_j + radius; aj++) {
			if (!on_board(ai, aj) || !legal_move(ai, aj, color))
				continue;
			//if not suicide for self,
			if (!suicide(ai, aj, color)) {
				/* Further require the move not to be suicide for the opponent... */
				if (!suicide(ai, aj, OTHER_COLOR(color)))
					common_moves[num_com_moves++] = POS(ai, aj);
				//if can defense self
				if (if_defense(ai,aj,color))
					defence_moves[num_def_moves++] = POS(ai,aj);
				//if can capture stones,
				if (if_capture_stone(ai,aj,color))
					capture_moves[num_cap_moves++] = POS(ai, aj);
			}
		}


	if (num_def_moves > 0) {                        //check defense
		return defence_moves[rand()%num_def_moves];
	} else if (num_cap_moves > 0) {					//check capture
		return capture_moves[rand()%num_cap_moves];
	} else if (num_com_moves > 0) {					//check other moves
		return common_moves[rand() % num_com_moves];
	} else {
		/* But pass if no move was considered. */
		return PASS_MOVE;
	}
}

int NoBB::atari_defense(int color,int pre_i,int pre_j)
{

	if(pass_move(pre_i,pre_j)) {
		return PASS_MOVE;
	}
	int def_move[MAX_BOARD*MAX_BOARD] = {0};
	int def_num = 0;
	int move;

	for (int k=0;k<4;++k) {
		int bi = pre_i+deltai[k];
		int bj = pre_j+deltaj[k];
		//whether in atari
		if (!on_board(bi,bj) || get_board(bi,bj)!=color || calliberty(bi,bj,-1,-1)>1)	
			continue;
		//whether can defense by capturing stones
		int pos = POS(bi, bj);
		do {
			int ai = I(pos);
			int aj = J(pos);
			for (int kk = 0; kk < 4; kk++) {
				int ci = ai + deltai[kk];
				int cj = aj + deltaj[kk];
				if (on_board(ci, cj) && get_board(ci, cj) == OTHER_COLOR(color) && calliberty(ci,cj,-1,-1)==1) {
					move = find_unique_liberty(ci,cj,OTHER_COLOR(color));
					if (legal_move(I(move), J(move), color) && rational_move(I(move), J(move), color))
						def_move[def_num++] = move;
				}
			}
			pos = next_stone[pos];
		} while (pos != POS(bi, bj));

		//whether can purely defense
		move = find_unique_liberty(bi,bj,color);
		if (legal_move(I(move), J(move), color) && rational_move(I(move), J(move), color)
				&& if_defense(I(move), J(move), color)) {
			def_move[def_num++] = move;
		}
	}

	//if can defense
	if (def_num > 0) {
		return def_move[rand()%def_num];
	}
	//can not defense
	return PASS_MOVE;
}

/* assume (i,j) is already in atari , find the empty position wihch provides liberty for (i,j) */
int NoBB::find_unique_liberty(int i,int j,int color)
{
	int pos = POS(i, j);
	do {
		int ai = I(pos);
		int aj = J(pos);
		int k;
		for (k = 0; k < 4; k++) {
			int bi = ai + deltai[k];
			int bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY) {
				return POS(bi,bj);
			}
		}

		pos = next_stone[pos];
	} while (pos != POS(i, j));
	return PASS_MOVE;
}

int NoBB::generate_local_cap_def(int color, int pre_i, int pre_j, int radius)
{
	int common_moves[MAX_BOARD * MAX_BOARD];
	int capture_moves[MAX_BOARD * MAX_BOARD];
	int defence_moves[MAX_BOARD * MAX_BOARD];
	int num_com_moves = 0;
	int num_cap_moves = 0;
	int num_def_moves = 0;
	int move;

	memset(common_moves, 0, sizeof(common_moves));
	memset(capture_moves, 0, sizeof(capture_moves));
	memset(defence_moves, 0, sizeof(defence_moves));
	for (int ai = pre_i - radius; ai <= pre_i + radius; ai++)
		for (int aj = pre_j - radius; aj <= pre_j + radius; aj++) {
			if (!on_board(ai, aj) || !legal_move(ai, aj, color))
				continue;
			//if not suicide for self,
			if (!suicide(ai, aj, color)) {
				/* Further require the move not to be suicide for the opponent... */
				if (!suicide(ai, aj, OTHER_COLOR(color)))
					common_moves[num_com_moves++] = POS(ai, aj);
				//if can defense self
				if (if_defense(ai,aj,color))
					defence_moves[num_def_moves++] = POS(ai,aj);
				//if can capture stones,
				if (if_capture_stone(ai,aj,color))
					capture_moves[num_cap_moves++] = POS(ai, aj);
			}
		}


	if (num_def_moves > 0) {                        //check defense
		return defence_moves[rand()%num_def_moves];
	} 
	//else if (num_cap_moves > 0) {					//check capture
	//	move = capture_moves[rand()%num_cap_moves];
	//	*i = I(move);
	//	*j = J(move);
	//}
	else {
		/* But pass if no move was considered. */
		return PASS_MOVE;
	}
}

int NoBB::nakade(int color, int pre_i, int pre_j)
{
	
	for (int k=0;k<4;++k) {
		int ai = pre_i + deltai[k];
		int aj = pre_j + deltaj[k];
		if (on_board(ai,aj) && get_board(ai,aj)==EMPTY) {
			//check if nakade
			int i,j;
			if(check_nakade(ai,aj,color,&i,&j))
				return POS(i,j); 
		}
	}
	return PASS_MOVE;
}
/* if POS(i,j) is a nakade, return center point (cpi,cpj) */
bool NoBB::check_nakade(int i, int j, int color,int *cpi,int *cpj)
{
	set<int> string;
	int cu_pos = POS(i,j);
	string.insert(cu_pos);
	bool no_more_pos = true;
	while(true) {
		if(string.size() > 3)
			return false;
		//scan string
		set<int>::iterator it;
		for(it=string.begin();it!=string.end();it++) {
			cu_pos = *it;
			int count = 0;
			for(int k=0;k<4;++k) {
				int bi = I(cu_pos)+deltai[k];
				int bj = J(cu_pos)+deltaj[k];
				int nei = POS(bi,bj);
				if (!on_board(bi,bj) || get_board(bi,bj)==OTHER_COLOR(color))
					continue;
				else if (get_board(bi,bj)==color)
					return false;
				else if (get_board(bi,bj)==EMPTY) {
					count ++;
					if(count==2) {
						*cpi = I(cu_pos);
						*cpj = J(cu_pos);
					} 
					if (string.find(nei)==string.end()) {
						string.insert(nei);
						no_more_pos = false;
					}
				}
			}
		}
		//check if no more pos can be found
		if (no_more_pos)
			break;
		no_more_pos = true;
	}
	if(string.size()!=3)
		return false;

	return true;
}

int NoBB::fill_board(int N)
{
	for (int n=0;n<N;++n) {
		int move = rand()%(board_size*board_size);
		int ai = I(move);
		int aj = J(move);
		int flag = true;
		if (get_board(ai,aj)==EMPTY) {
			for (int ii=-1;ii<2;++ii) 
				for (int jj=-1;jj<2;++jj){
					int bi = ai+ii;
					int bj = aj+jj;
					if (!on_board(bi,bj) || get_board(bi,bj)!=EMPTY) {
						flag = false;
						break;
					}
			}
			if (flag) {
				return POS(ai,aj);
			}
		}	
	}
	return PASS_MOVE;
}

int NoBB::generate_capture_move(int color)
{
	int move;
	if (color == BLACK) {
		if (white_in_atari.size()!=0) 
			move =  white_in_atari[rand()%white_in_atari.size()];
	} else {
		if (black_in_atari.size()!=0) 
			move = black_in_atari[rand()%black_in_atari.size()];
	}
	if (legal_move(I(move), J(move), color) && rational_move(I(move), J(move), color))
		return move;
	return PASS_MOVE;
}

int NoBB::generate_local_random_move_with_pattern(int color, int pre_i, int pre_j, int radius)
{
	int moves[MAX_BOARD * MAX_BOARD];
	int num_moves = 0;
	int move;

	memset(moves, 0, sizeof(moves));
	for (int ai = pre_i - radius; ai <= pre_i + radius; ai++)
		for (int aj = pre_j - radius; aj <= pre_j + radius; aj++) {
			if (!on_board(ai, aj) || !legal_move(ai, aj, color))
				continue;
			//if not suicide for self,
			if (!suicide(ai, aj, color)) {
				/* Further require the move not to be suicide for the opponent... */
				for (int k=0;k<4;++k) {				//not self-atari
					int bi = ai+deltai[k];
					int bj = aj+deltaj[k];
					if (calliberty(bi,bj,ai,aj)==1)
						continue;
				}
				if (!suicide(ai, aj, OTHER_COLOR(color)) && hc_patterns->if_match_3_3_patterns(POS(ai, aj), board, board_size, color)) {
					moves[num_moves++] = POS(ai, aj);
				}
			}

		}


	if (num_moves > 0) {
		return moves[rand() % num_moves];
	}
	else {
		/* But pass if no move was considered. */
		return PASS_MOVE;
	}
}

bool NoBB::if_capture_stone(int i, int j, int color)
{
	if (color == BLACK) {
		if (find(white_in_atari.begin(),white_in_atari.end(),POS(i,j))!=white_in_atari.end())
			return true;
		return false;
	} else {
		if (find(black_in_atari.begin(),black_in_atari.end(),POS(i,j))!=black_in_atari.end())
			return true;
		return false;
	}

	//old version
	/*for (int k = 0; k < 4; ++k) {
		int bi = i + deltai[k];
		int bj = j + deltaj[k];
		if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color) && !has_additional_liberty(bi, bj, i, j))
			return true;
	}

	return false;*/
}

bool NoBB::if_defense(int i, int j, int color)
{
	//if on board size , no
	if (i==0 || i==board_size-1 || j==0 || j==board_size-1)
		return false;

	
	bool in_danger = false;
	for (int k = 0; k < 4; ++k){
		int ai = i + deltai[k];
		int aj = j + deltaj[k];
		if (on_board(ai,aj) && get_board(ai,aj)==color && calliberty(ai,aj,-1,-1)==1) {
				in_danger = true;
				break;
		}
	}
	if (!in_danger) return false;

	NoBB newnobb = NoBB();
	newnobb.set_context(get_context());
	newnobb.play_move(i,j,color);
	if (newnobb.get_board(i,j)==EMPTY || newnobb.calliberty(i,j,-1,-1)==1)
		return false;
	return true;
}


void NoBB::clear_board()
{
  memset(board, 0, sizeof(board));
}

int NoBB::board_empty()
{
  int i;
  for (i = 0; i < board_size * board_size; i++)
    if (board[i] != EMPTY)
      return 0;

  return 1;
}

int NoBB::get_board(int i, int j)
{
  return board[i * board_size + j];
}

/* Get the stones of a string. stonei and stonej must point to arrays
 * sufficiently large to hold any string on the board. The number of
 * stones in the string is returned.
 */
int NoBB::get_string(int i, int j, int *stonei, int *stonej)
{
  int num_stones = 0;
  int pos = POS(i, j);
  do {
    stonei[num_stones] = I(pos);
    stonej[num_stones] = J(pos);
    num_stones++;
    pos = next_stone[pos];
  } while (pos != POS(i, j));

  return num_stones;
}

int NoBB::pass_move(int i, int j)
{
  return i == -1 && j == -1;
}
int NoBB::pass_move(int move)
{
	return move == PASS_MOVE;
}

int NoBB::on_board(int i, int j)
{
  return i >= 0 && i < board_size && j >= 0 && j < board_size;
}

int NoBB::legal_move(int i, int j, int color)
{
  int other = OTHER_COLOR(color);

  /* Pass is always legal. */
  if (pass_move(i, j))
    return 1;

  /* Already occupied. */
  if (get_board(i, j) != EMPTY)
    return 0;

  /* Illegal ko recapture. It is not illegal to fill the ko so we must
   * check the color of at least one neighbor.
   */
  if (i == ko_i && j == ko_j
      && ((on_board(i - 1, j) && get_board(i - 1, j) == other)
	  || (on_board(i + 1, j) && get_board(i + 1, j) == other)))
    return 0;

  return 1;
}

bool NoBB::rational_move(int i, int j, int color)
{
	if (!on_board(i,j) || !legal_move(i,j,color))
		return false;

	if (suicide(i, j, color))
		return false;

	if (suicide(i, j, OTHER_COLOR(color))) {
		if (!if_capture_stone(i, j, color))
			return false;
	}
	return true;

	//if (!suicide(i, j, color)) {
	//	if (!suicide(i, j, OTHER_COLOR(color)))
	//		return true;
	//	//if suicide for opposite color, this is my eye. check if can capture or defense
	//	if (if_capture_stone(i,j,color) || if_defense(i,j,color))
	//		return true;
	//}

	//return false;
}

/* Does the string at (i, j) have any more liberty than the one at
 * (libi, libj)?
 */
int NoBB::has_additional_liberty(int i, int j, int libi, int libj)
{
  int pos = POS(i, j);
  do {
    int ai = I(pos);
    int aj = J(pos);
    int k;
    for (k = 0; k < 4; k++) {
      int bi = ai + deltai[k];
      int bj = aj + deltaj[k];
      if (on_board(bi, bj) && get_board(bi, bj) == EMPTY && (bi != libi || bj != libj))
        return 1;
    }

    pos = next_stone[pos];
  } while (pos != POS(i, j));

  return 0;
}

int NoBB::calliberty(int i, int j, int libi, int libj){
	int pos = POS(i, j), ai, aj, bi, bj, k, res = 0;
	bool lib[MAX_BOARD*MAX_BOARD] = {};

	do {
		ai = I(pos);
		aj = J(pos);
		for (k = 0; k < 4; ++k){
			bi = ai + deltai[k];
			bj = aj + deltaj[k];
			if (on_board(bi, bj) && get_board(bi, bj) == EMPTY && (bi != libi || bj != libj))
				lib[POS(bi, bj)] = true;
		}
		pos = next_stone[pos];
	} while (pos != POS(i, j));

	/*if (libi >= 0 && libj >= 0)
		lib[POS(libi, libj)] = false;*/
	for (int i = 0; i < MAX_BOARD*MAX_BOARD; ++i){
		if (lib[i])
			res++;
	}
	return res;
}


/* Does (ai, aj) provide a liberty for a stone at (i, j)? */
int NoBB::provides_liberty(int ai, int aj, int i, int j, int color)
{
  /* A vertex off the board does not provide a liberty. */
  if (!on_board(ai, aj))
    return 0;

  /* An empty vertex IS a liberty. */
  if (get_board(ai, aj) == EMPTY)
    return 1;

  /* A friendly string provides a liberty to (i, j) if it currently
   * has more liberties than the one at (i, j).
   */
  if (get_board(ai, aj) == color)
    return has_additional_liberty(ai, aj, i, j);

  /* An unfriendly string provides a liberty if and only if it is
   * captured, i.e. if it currently only has the liberty at (i, j).
   */
  return !has_additional_liberty(ai, aj, i, j);
}

/* Is a move at (i, j) suicide for color? */
int NoBB::suicide(int i, int j, int color)
{
    for (int k = 0; k < 4; k++)
        if (provides_liberty(i + deltai[k], j + deltaj[k], i, j, color))
          return 0;

    return 1;
}


/* Remove a string from the board array. There is no need to modify
 * the next_stone array since this only matters where there are
 * stones present and the entire string is removed.
 */
int NoBB::remove_string(int i, int j,int color)
{
	int pos = POS(i, j);
	int removed = 0;
	do {
		for (int kk = 0; kk < 4; kk++) {
			int bi = I(pos) + deltai[kk];
			int bj = J(pos) + deltaj[kk];
			if (on_board(bi,bj) && get_board(bi,bj)==OTHER_COLOR(color) && calliberty(bi,bj,-1,-1)<=1) {
				int pos1 = find_unique_liberty(bi,bj,OTHER_COLOR(color));
				erase_atari_vector(pos1,OTHER_COLOR(color));
			}
		}

		board[pos] = EMPTY;
		removed++;
		pos = next_stone[pos];
	} while (pos != POS(i, j));

	return removed;
}

/* Do two vertices belong to the same string. It is required that both
 * pos1 and pos2 point to vertices with stones.
 */
int NoBB::same_string(int pos1, int pos2)
{
    int pos = pos1;
    do {
    if (pos == pos2)
      return 1;
    pos = next_stone[pos];
    } while (pos != pos1);

    return 0;
}

bool NoBB::erase_atari_vector(int move,int color)
{
	if (color == WHITE) {
		vector<int>::iterator itr;
		if ((itr=find(white_in_atari.begin(),white_in_atari.end(),move))!=white_in_atari.end()) {
			white_in_atari.erase(itr);
			return true;
		}
	} else {
		vector<int>::iterator itr;
		if ((itr=find(black_in_atari.begin(),black_in_atari.end(),move))!=black_in_atari.end()) {
			black_in_atari.erase(itr);
			return true;
		}
	}
	return false;
}

void NoBB::push_new_atari_2_vector(int move,int color)
{
	if (color == BLACK) {
		black_in_atari.push_back(move);
	} else {
		white_in_atari.push_back(move);
	}
}

/* Play at (i, j) for color. No legality check is done here. We need
 * to properly update the board array, the next_stone array, and the
 * ko point.
 */
void NoBB::play_move(int move, int color) {
	play_move(I(move), J(move), color);
}
void NoBB::play_move(int i, int j, int color)
{
    int pos = POS(i, j);
    int captured_stones = 0;
    int k;

    /* Reset the ko point. */
    ko_i = -1;
    ko_j = -1;
	step ++;
    pre_i = i;
    pre_j = j;

    /* Nothing more happens if the move was a pass. */
    if (pass_move(i, j))
        return;

	
	erase_atari_vector(POS(i,j),color);
	erase_atari_vector(POS(i,j),OTHER_COLOR(color));
    /* If the move is a suicide we only need to remove the adjacent
    * friendly stones.
    */
    if (suicide(i, j, color)) {
        for (k = 0; k < 4; k++) {
            int ai = i + deltai[k];
            int aj = j + deltaj[k];
            if (on_board(ai, aj) && get_board(ai, aj) == color) {
                remove_string(ai, aj,color);
                break;
            }
        }
        return;
    }

    /* Not suicide. Remove captured opponent strings. */
    for (k = 0; k < 4; k++) {
        int ai = i + deltai[k];
        int aj = j + deltaj[k];
        if (on_board(ai, aj) && get_board(ai, aj) == OTHER_COLOR(color)) {
			if (!has_additional_liberty(ai, aj, i, j)) 
				captured_stones += remove_string(ai, aj,color);
		}
    }

    /* Put down the new stone. Initially build a single stone string by
    * setting next_stone[pos] and father_stone[pos] pointing to itself.
    */
    board[pos] = color;
    next_stone[pos] = pos;

  /* If we have friendly neighbor strings we need to link the strings
   * together.
   */
    for (k = 0; k < 4; k++) {
        int ai = i + deltai[k];
        int aj = j + deltaj[k];
        int pos2 = POS(ai, aj);
        if (on_board(ai, aj) && board[pos2] == color && !same_string(pos, pos2)) {
          int tmp = next_stone[pos2];
          next_stone[pos2] = next_stone[pos];
          next_stone[pos] = tmp;
        }
    }

  /* If we have captured exactly one stone and the new string is a
   * single stone it may have been a ko capture.
   */
    if (captured_stones == 1 && next_stone[pos] == pos) {
        int ai=-1, aj=-1;
        for (k = 0; k < 4; k++) {
          ai = i + deltai[k];
          aj = j + deltaj[k];
          if (on_board(ai, aj) && get_board(ai, aj) == EMPTY)
            break;
        }

        if (!has_additional_liberty(i, j, ai, aj)) {
          ko_i = ai;
          ko_j = aj;
        }
    }

	/*refresh white_in_atari and black_in_atari 
	 *induced changes include capture, suicide, atari_defense, fake_atari_defense, new atari 
	*/
	if (calliberty(i,j,-1,-1)==1) {
		int atari_liberty = find_unique_liberty(i,j,color);
		push_new_atari_2_vector(atari_liberty,color);
	} 
	for (k = 0; k < 4; k++) {
        int ai = i + deltai[k];
        int aj = j + deltaj[k];
        if (on_board(ai, aj) && get_board(ai, aj) == OTHER_COLOR(color)) {
			if (calliberty(ai,aj,-1,-1)==1) {
				int atari_liberty = find_unique_liberty(ai,aj,OTHER_COLOR(color));
				push_new_atari_2_vector(atari_liberty,OTHER_COLOR(color));
			} 
		}
    }
}

/* Generate a move. */
int NoBB::generate_move(int color)
{
	int move;

	/*NoBB *newnobb = new NoBB();
	NoBB_Context backup = this->get_context();
	int wins = 0;
	vector<int> actions;
	for(int i=0;i<1000;++i) {
		actions.clear();
		double mc = newnobb->monte_carlo(OTHER_COLOR(color),-100,&actions);
		if (mc>0.5)
			wins++;
		newnobb->set_context(backup);
	}
	printf("rate %d\n",wins);
	delete newnobb;
	*i = -1;
	*j = -1;
	return;*/

	/* generate monte carlo move*/
	/*move = atari_defense(color, pre_i, pre_j);
	double type = 0;
	if (pass_move(move)) {
		move = nakade(color, pre_i, pre_j); 
		type = 1;
	}
	if (pass_move(move)) {
		move = fill_board(6); 
		type = 2;
	}
	if (pass_move(move)) {
		move = generate_local_random_move_with_pattern(color, pre_i, pre_j, 1); 
		type = 3;
	}
	if (pass_move(move)) {
		move = generate_capture_move(color); 
		type = 4;
	}
	if (pass_move(move)) {
		move = generate_random_move(color); 
		type = 5;
	}
	return move;*/


	/*NoBB *newnobb = new NoBB();
	int max_pos = 0;
	double max_mc = -1000;
	for (int ii=0;ii<board_size;++ii)
		for (int jj=0;jj<board_size;++jj) {
			if (!legal_move(ii,jj,color)) {
				final_board[POS(ii,jj)] = 0;
				continue;
			}
			newnobb->set_context(this->get_context());
			newnobb->play_move(ii,jj,color);
			int wins = 0;
			vector<int> actions;
			NoBB_Context backup = newnobb->get_context();
			for (int kk=0;kk<500;++kk) {
				newnobb->set_context(backup);
				actions.clear();
				double mc = newnobb->monte_carlo(color,POS(ii,jj),&actions);
				if(mc>0.5)
					wins++;
			}
			final_board[POS(ii,jj)] = (double)(wins)/500;
			if ((double)(wins)/500 > max_mc) {
				max_mc = (double)(wins)/500;
				max_pos = POS(ii,jj);
			}
		}
	delete newnobb;
	*i = I(max_pos);
	*j = J(max_pos);
	return;*/

    UCTree *uctree = new UCTree(this->get_context(),board_size,color,POS(pre_i,pre_j));
    move = uctree->getOptimalPos(final_board);

    CreateThread(NULL, 0, free_uctree, uctree, 0, NULL);
	// delete uctree;

	if (!rational_move(I(move), J(move), color))
		return generate_random_move(color);
	return move;
}

DWORD WINAPI NoBB::free_uctree(LPVOID Param)
{
	UCTree *uctree = (UCTree *)Param;
	delete uctree;
	return 0;
}


/* generate a move randomly by brown */
int NoBB::generate_random_move(int color)
{
	for (int k =0;k<18;++k) {
		int move = rand()%(board_size*board_size);
		if (rational_move(I(move),J(move),color)) {
			return move;
		}
	}	

	int common_moves[MAX_BOARD * MAX_BOARD];
	int num_com_moves = 0;
	int move;

	memset(common_moves, 0, sizeof(common_moves));
	for (int ai = 0; ai < board_size; ai++)
		for (int aj = 0; aj < board_size; aj++) {
			if (rational_move(ai,aj,color))
				common_moves[num_com_moves++] = POS(ai, aj);
		}


	if (num_com_moves > 0) {
		move = common_moves[rand() % num_com_moves];
		return move;
	} else {
		/* But pass if no move was considered. */
		return PASS_MOVE;
	}
}

/* Set a final status value for an entire string. */
void NoBB::set_final_status_string(int pos, int status)
{
  int pos2 = pos;
  do {
    final_status[pos2] = status;
    pos2 = next_stone[pos2];
  } while (pos2 != pos);
}

/* Compute final status. This function is only valid to call in a
 * position where generate_move() would return pass for at least one
 * color.
 *
 * Due to the nature of the move generation algorithm, the final
 * status of stones can be determined by a very simple algorithm:
 *
 * 1. Stones with two or more liberties are alive with territory.
 * 2. Stones in atari are dead.
 *
 * Moreover alive stones are unconditionally alive even if the
 * opponent is allowed an arbitrary number of consecutive moves.
 * Similarly dead stones cannot be brought alive even by an arbitrary
 * number of consecutive moves.
 *
 * Seki is not an option. The move generation algorithm would never
 * leave a seki on the board.
 *
 * Comment: This algorithm doesn't work properly if the game ends with
 *          an unfilled ko. If three passes are required for game end,
 *          that will not happen.
 */
void NoBB::compute_final_status(void)
{
  int i, j;
  int pos;
  int k;

  for (pos = 0; pos < board_size * board_size; pos++)
    final_status[pos] = UNKNOWN;

  for (i = 0; i < board_size; i++)
    for (j = 0; j < board_size; j++)
      if (get_board(i, j) == EMPTY)
        for (k = 0; k < 4; k++) {
          int ai = i + deltai[k];
          int aj = j + deltaj[k];
          if (!on_board(ai, aj))
            continue;
          /* When the game is finished, we know for sure that (ai, aj)
               * contains a stone. The move generation algorithm would
               * never leave two adjacent empty vertices. Check the number
               * of liberties to decide its status, unless it's known
               * already.
           *
           * If we should be called in a non-final position, just make
           * sure we don't call set_final_status_string() on an empty
           * vertex.
           */
          pos = POS(ai, aj);
          if (final_status[pos] == UNKNOWN) {
            if (get_board(ai, aj) != EMPTY) {
              if (has_additional_liberty(ai, aj, i, j))
                set_final_status_string(pos, ALIVE);
              else
                set_final_status_string(pos, DEAD);
            }
          }
          /* Set the final status of the (i, j) vertex to either black
               * or white territory.
           */
          if (final_status[POS(i, j)] == UNKNOWN) {
            if ((final_status[pos] == ALIVE) ^ (get_board(ai, aj) == WHITE))
              final_status[POS(i, j)] = BLACK_TERRITORY;
            else
              final_status[POS(i, j)] = WHITE_TERRITORY;
          }
        }
}

int NoBB::get_final_status(int i, int j)
{
  return final_status[POS(i, j)];
}

int NoBB::get_final_win(int color){
	int score =0;
  int i, j;

  compute_final_status();
  for (i = 0; i < board_size; i++)
    for (j = 0; j < board_size; j++) {
      int status = get_final_status(i, j);
      if (status == BLACK_TERRITORY)
		score+=0;
      else if (status == WHITE_TERRITORY)
		score++;
      else if ((status == ALIVE) ^ (get_board(i, j) == WHITE))
		score+=0;
      else
		score++;
    }
	if (color==WHITE)
		return score;
	else
		return board_size*board_size-score;

}

double NoBB::get_black_win()
{
	double score = komi;
  int i, j;

  compute_final_status();
  for (i = 0; i < board_size; i++)
    for (j = 0; j < board_size; j++) {
      int status = get_final_status(i, j);
      if (status == BLACK_TERRITORY)
		score++;
      else if (status == WHITE_TERRITORY)
		score--;
      else if ((status == ALIVE) ^ (get_board(i, j) == WHITE))
		score++;
      else
		score--;
    }
	
	return score;
}

void NoBB::show_board()
{
	for (int i=0;i<board_size;++i) {
		for (int j=0;j<board_size;++j) {
			if (get_board(i,j)==BLACK) printf(" X");
			else if (get_board(i,j)==WHITE) printf(" O");
			else printf(" .");
		}
		printf("\n");
	}
	printf("\n");
}

void NoBB::set_final_status(int i, int j, int status)
{
  final_status[POS(i, j)] = status;
}

/* Valid number of stones for fixed placement handicaps. These are
 * compatible with the GTP fixed handicap placement rules.
 */
int NoBB::valid_fixed_handicap(int handicap)
{
  if (handicap < 2 || handicap > 9)
    return 0;
  if (board_size % 2 == 0 && handicap > 4)
    return 0;
  if (board_size == 7 && handicap > 4)
    return 0;
  if (board_size < 7 && handicap > 0)
    return 0;

  return 1;
}

/* Put fixed placement handicap stones on the board. The placement is
 * compatible with the GTP fixed handicap placement rules.
 */
void NoBB::place_fixed_handicap(int handicap)
{
    int low = board_size >= 13 ? 3 : 2;
  int mid = board_size / 2;
  int high = board_size - 1 - low;

  if (handicap >= 2) {
    play_move(high, low, BLACK);   /* bottom left corner */
    play_move(low, high, BLACK);   /* top right corner */
  }

  if (handicap >= 3)
    play_move(low, low, BLACK);    /* top left corner */

  if (handicap >= 4)
    play_move(high, high, BLACK);  /* bottom right corner */

  if (handicap >= 5 && handicap % 2 == 1)
    play_move(mid, mid, BLACK);    /* tengen */

  if (handicap >= 6) {
    play_move(mid, low, BLACK);    /* left edge */
    play_move(mid, high, BLACK);   /* right edge */
  }

  if (handicap >= 8) {
    play_move(low, mid, BLACK);    /* top edge */
    play_move(high, mid, BLACK);   /* bottom edge */
  }
}

/* Put free placement handicap stones on the board. We do this simply
 * by generating successive black moves.
 */
void NoBB::place_free_handicap(int handicap)
{
  int k;

  for (k = 0; k < handicap; k++) {
    int move = generate_move(BLACK);
    play_move(I(move), J(move), BLACK);
  }
}

/* change the context of nobb including board, next_stone, ko_i and ko_j */
void NoBB::set_context(NoBB_Context context)
{
    memcpy(this->board, context.board, MAX_BOARD * MAX_BOARD*sizeof(int));
    memcpy(this->next_stone, context.next_stone, MAX_BOARD * MAX_BOARD*sizeof(int));
    this->ko_i = context.ko_i;
    this->ko_j = context.ko_j;
	this->step = context.step;
	this->pre_i = context.pre_i;
	this->pre_j = context.pre_j;
	this->black_in_atari = context.black_in_atari;
	this->white_in_atari = context.white_in_atari;
}
NoBB_Context NoBB::get_context()
{
    NoBB_Context context = NoBB_Context(board, next_stone, ko_i, ko_j,step, pre_i,pre_j,black_in_atari,white_in_atari);
    return context;
}

/*
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 2
 * End:
 */
