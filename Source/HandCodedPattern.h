//
//  hand coded pattern used for monte carlo simulation
//  assume the next move is a black
//  patern formulation : [  0 1 2
//                          3 4 5
//                          6 7 8 ]
//  reference : modification of uct with patterns in monte-carlo go, Sylvain Gelly...
//  by backchord
//  2015.1.9
//  all rights reserved

#include <memory.h>
#include <cstdio>
#include "config.h"

/* These must agree with the corresponding defines in gtp.c. */
#define EMPTY 0
#define WHITE 1
#define BLACK 2

#define POS(i, j) ((i) * board_size + (j))
#define I(pos) ((pos) / board_size)
#define J(pos) ((pos) % board_size)

#define MAX_BOARD BOARD_SIZE

#define E 0         //empty for patern
#define W 1         //white for patern
#define B 2         //black for patern
#define IG 3         //ignored for patern
#define NW 4        //not white for patern
#define NB 5        //not black for patern
#define P 6         //center point

#define PATTERN_POS(i,j) ((i)*3+(j))

#ifndef hand_coded_pattern_backchord
#define hand_coded_pattern_backchord

class HandCodedPattern
{
public:
    HandCodedPattern(int board_size);
    int if_match_3_3_patterns(int pos, int *board, int board_size,int color);

private:
    int board_size;
    int my_board[MAX_BOARD*MAX_BOARD];
    int board_region[MAX_BOARD*MAX_BOARD];

    int hane0_set[8][9];
    int hane1_set[8][9];
    int hane2_set[8][9];
    int hane3_set[8][9];
    int cut1_set[8][9] ;
    int cut1_ex0_set[8][9] ;
    int cut1_ex1_set[8][9] ;
    int cut2_set[8][9];

	int corner0_set[8][9];
	int corner1_set[8][9];
	int corner2_set[8][9];
	int side0_set[8][9];
	int side1_set[8][9];

    bool match_3_3_pattern(int pos, int board_size, int patterns[8][9]);
    bool match_cor_side_pattern(int pos, int board_size, int pattern[9]);
    bool match_on_pos(int b_pos_value, int p_pos_value);

    void revert_board(int *to_board, int *from_board);
    void rotate_pattern(int *to_pattern, const int *from_pattern);
    void mirror_pattern(int *to_pattern, const int *from_pattern);
};


#endif
