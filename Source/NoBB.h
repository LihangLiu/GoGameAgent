

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <windows.h>
#include <time.h>
#include <set>
#include "UCT.h"
#include "HandCodedPattern.h"
#include "config.h"
using namespace std;

#define VERSION_STRING "1.0"

#define MIN_BOARD 2
#define MAX_BOARD 19

/* These must agree with the corresponding defines in gtp.c. */
#define EMPTY 0
#define WHITE 1
#define BLACK 2

/* Used in the final_status[] array. */
#define DEAD 0
#define ALIVE 1
#define SEKI 2
#define WHITE_TERRITORY 3
#define BLACK_TERRITORY 4
#define UNKNOWN 5

/* Macros to convert between 1D and 2D coordinates. The 2D coordinate
 * (i, j) points to row i and column j, starting with (0,0) in the
 * upper left corner.
 */
#define POS(i, j) ((i) * BOARD_SIZE + (j))
#define I(pos) ((pos) / BOARD_SIZE)
#define J(pos) ((pos) % BOARD_SIZE)
#define PASS_MOVE POS(-1,-1)

/* Macro to find the opposite color. */
#define OTHER_COLOR(color) (WHITE + BLACK - (color))

#ifndef NOBB_H
#define NOBB_H

struct NoBB_Context
{
    int board[MAX_BOARD * MAX_BOARD];
    int next_stone[MAX_BOARD * MAX_BOARD];
    int ko_i,ko_j;
	int step;
	int pre_i,pre_j;
	vector<int> black_in_atari;
	vector<int> white_in_atari;

    NoBB_Context() {
        memset(board, 0, sizeof(board));
        memset(next_stone, 0, sizeof(next_stone));
    }
    NoBB_Context(int *board_, int *next_stone_, int ko_i_, int ko_j_, int step_, int pre_i_, int pre_j_,vector<int> b_i_a,vector<int> w_i_a) {
        memcpy(board, board_, MAX_BOARD * MAX_BOARD*sizeof(int));
        memcpy(next_stone, next_stone_, MAX_BOARD * MAX_BOARD*sizeof(int));
        ko_i = ko_i_;
        ko_j = ko_j_;
		step = step_;
		pre_i = pre_i_;
		pre_j = pre_j_;
		black_in_atari = b_i_a;
		white_in_atari = w_i_a;
    }
};

class SNAKE;

class NoBB
{
public:
    float komi;
    int board_size;
	int step;

    int board[MAX_BOARD * MAX_BOARD];           /* Board represented by a 1D array. The first board_size*board_size
                                                 * elements are used. Vertices are indexed row by row, starting with 0
                                                 * in the upper left corner.
                                                 */
	int next_stone[MAX_BOARD * MAX_BOARD];      /* Stones are linked together in a circular list for each string. */

	HandCodedPattern *hc_patterns;

    NoBB() {
        srand(time(NULL));
        this->komi = -6.5;//-3.14;
        this->board_size = BOARD_SIZE;
		this->step = 0;
        this->pre_i = -100;
        this->pre_j = -100;
        this->init_brown();
		this->hc_patterns = new HandCodedPattern(board_size);
    }
	~NoBB() {
		delete hc_patterns;
	}

    void init_brown(void);
    double monte_carlo(int color,int pre_pos,vector<int> *actions);                      /*old monte carlo ：
                                                         * color x: simulate for color x, and the next step is the opposite color
                                                         * return the rate (0-1) of color x in the final state.
                                                         */
    void clear_board(void);
    int board_empty(void);
    int get_board(int i, int j);
    int get_string(int i, int j, int *stonei, int *stonej);
    int legal_move(int i, int j, int color);
	bool rational_move(int i, int j, int color);
	int pass_move(int i, int j);
	int pass_move(int move);
    int suicide(int i, int j, int color);
	int same_string(int pos1, int pos2);
	int has_additional_liberty(int i, int j, int libi, int libj);
	int calliberty(int bi, int bj, int ai, int aj);
    void play_move(int i, int j, int color);
	void play_move(int move, int color);
    int generate_move(int color);      //generate a move by uctree
    void compute_final_status(void);
    int get_final_status(int i, int j);
    void set_final_status(int i, int j, int status);
    int valid_fixed_handicap(int handicap);
    void place_fixed_handicap(int handicap);
    void place_free_handicap(int handicap);
	double get_black_win();
	void show_board();

	int atari_defense(int color,int pre_i,int pre_j);
	int generate_local_cap_def(int color, int pre_i, int pre_j, int radius);
	int nakade(int color, int pre_i, int pre_j);
	bool check_nakade(int i, int j, int color,int *cpi,int *cpj);				//
	int fill_board(int N);										//randomly test N positions. if one of them are empty with 8 empty neighbors, return it 
    int generate_local_random_move(int color,int pre_i, int pre_j,int radius);   //generate a move by searching neighbor of (pre_i,pre_j)
	int generate_local_random_move_with_pattern(int color, int pre_i, int pre_j, int radius);
	int generate_capture_move(int color);   //generate a capture move of global
    int generate_random_move(int color);   //generate a move by brown which is a random algorithm
	bool if_capture_stone(int i, int j, int color);
	bool if_defense(int i, int j, int color);
    /* method for context switch */
    void set_context(NoBB_Context context);
    NoBB_Context get_context();
    int on_board(int i, int j);

    double final_board[MAX_BOARD * MAX_BOARD];	// only used for statistics for GUI

private:
    int final_status[MAX_BOARD * MAX_BOARD];    /* Storage for final status computations. */
    int ko_i, ko_j;                             /* Point which would be an illegal ko recapture. */
    int pre_i, pre_j;
	vector<int> black_in_atari;
	vector<int> white_in_atari;
 
    int provides_liberty(int ai, int aj, int i, int j, int color);
    int remove_string(int i, int j, int color);
    void set_final_status_string(int pos, int status);
    int get_final_win(int color);
	static DWORD WINAPI free_uctree(LPVOID Param);   //thread function to free uctree memory
	int find_unique_liberty(int i,int j,int color);
	bool erase_atari_vector(int move,int color);	//help delete changed atari
	void push_new_atari_2_vector(int move,int color);
};


#endif
