
#include "HandCodedPattern.h"

HandCodedPattern::HandCodedPattern(int board_size)
{
    this->board_size = board_size;

	const int hane0[9] = {B,W,B,
                     E,P,E,
                     IG,IG,IG};
    const int hane1[9] = {B,W,E,
                     E,P,E,
                     IG,E,IG};
    const int hane2[9] = {B,W,IG,
                     B,P,E,
                     IG,E,IG};
    const int hane3[9] = {B,W,W,
                     E,P,E,
                     IG,E,IG};

    const int cut1[9] =  {B,W,IG,
                    W,P,IG,
                    IG,IG,IG};
    const int cut1_ex0[9] = {B,W,IG,
                    W,P,W,
                    IG,E,IG};
    const int cut1_ex1[9] = {B,W,IG,
                        W,P,E,
                        IG,W,IG};

    const int cut2[9] = {IG,B,IG,
                    W,P,W,
                    NW,NW,NW};

	//board corner
	const int corner0[9] = {IG,IG,IG,
							B,E,IG,
							W,P,IG};
	const int corner1[9] = {IG,IG,IG,
							W,B,IG,
							NB,P,IG};
	const int corner2[9] = {IG,IG,IG,
							W,B,IG,
							B,P,W};
	//board size
	const int side0[9] = {IG,IG,IG,
						  IG,B,IG,
						  NB,P,W};
	const int side1[9] = {IG,IG,IG,
						  IG,B,W,
						  IG,P,IG};
    memcpy(hane0_set[0],hane0,sizeof(hane0));
    memcpy(hane1_set[0],hane1,sizeof(hane0));
    memcpy(hane2_set[0],hane2,sizeof(hane0));
    memcpy(hane3_set[0],hane3,sizeof(hane0));

    memcpy(cut1_set[0],cut1,sizeof(hane0));
    memcpy(cut1_ex0_set[0],cut1_ex0,sizeof(hane0));
    memcpy(cut1_ex1_set[0],cut1_ex1,sizeof(hane0));
    memcpy(cut2_set[0],cut2,sizeof(hane0));

	memcpy(corner0_set[0],corner0,sizeof(hane0));
	memcpy(corner1_set[0],corner1,sizeof(hane0));
	memcpy(corner2_set[0],corner2,sizeof(hane0));
	memcpy(side0_set[0],side0,sizeof(hane0));
	memcpy(side1_set[0],side1,sizeof(hane0));
    for (int i=1;i<4;++i) {
        rotate_pattern(hane0_set[i],hane0_set[i-1]);
        rotate_pattern(hane1_set[i],hane1_set[i-1]);
        rotate_pattern(hane2_set[i],hane2_set[i-1]);
        rotate_pattern(hane3_set[i],hane3_set[i-1]);

        rotate_pattern(cut1_set[i],cut1_set[i-1]);
        rotate_pattern(cut1_ex0_set[i],cut1_ex0_set[i-1]);
        rotate_pattern(cut1_ex1_set[i],cut1_ex1_set[i-1]);
        rotate_pattern(cut2_set[i],cut2_set[i-1]);

		rotate_pattern(corner0_set[i],corner0_set[i-1]);
        rotate_pattern(corner1_set[i],corner1_set[i-1]);
        rotate_pattern(corner2_set[i],corner2_set[i-1]);
        rotate_pattern(side0_set[i],side0_set[i-1]);
		rotate_pattern(side1_set[i],side1_set[i-1]);
    }
    for (int i=0;i<4;++i) {
        mirror_pattern(hane0_set[i+4],hane0_set[i]);
        mirror_pattern(hane1_set[i+4],hane1_set[i]);
        mirror_pattern(hane2_set[i+4],hane2_set[i]);
        mirror_pattern(hane3_set[i+4],hane3_set[i]);

        mirror_pattern(cut1_set[i+4],cut1_set[i]);
        mirror_pattern(cut1_ex0_set[i+4],cut1_ex0_set[i]);
        mirror_pattern(cut1_ex1_set[i+4],cut1_ex1_set[i]);
        mirror_pattern(cut2_set[i+4],cut2_set[i]);

        mirror_pattern(corner0_set[i+4],corner0_set[i]);
        mirror_pattern(corner1_set[i+4],corner1_set[i]);
        mirror_pattern(corner2_set[i+4],corner2_set[i]);
        mirror_pattern(side0_set[i+4],side0_set[i]);
        mirror_pattern(side1_set[i+4],side1_set[i]);
    }

	memset(board_region,0,sizeof(board_region));
    for (int i=0;i<board_size;++i)
    for (int j=0;j<board_size;++j){
        if (i>=1 && i<=board_size-2 && j>=1 && j<=board_size-2 ) {
            board_region[POS(i,j)] = 1;

        } else if (i==board_size-1 && j>=2 && j<=board_size-3) {
            board_region[POS(i,j)] = 2;
        } else if (j==0 && i>=2 && i<=board_size-3) {
            board_region[POS(i,j)] = 3;
        } else if (i==0 && j>=2 && j<=board_size-3) {
            board_region[POS(i,j)] = 4;
        } else if (j==board_size-1 && i>=2 && i<=board_size-3) {
            board_region[POS(i,j)] = 5;

        } else if (i==board_size-1 && j==1) {
            board_region[POS(i,j)] = 6;
        } else if (i==board_size-2 && j==0) {
            board_region[POS(i,j)] = 10;
        } else if (i==1 && j==0) {
            board_region[POS(i,j)] = 7;
        } else if (i==0 && j==1) {
            board_region[POS(i,j)] = 11;
        } else if (i==0 && j==board_size-2) {
            board_region[POS(i,j)] = 8;
        } else if (i==1 && j==board_size-1) {
            board_region[POS(i,j)] = 12;
        } else if (i==board_size-2 && j==board_size-1) {
            board_region[POS(i,j)] = 9;
        } else if (i==board_size-1 && j==board_size-2) {
            board_region[POS(i,j)] = 13;
        } else
            board_region[POS(i,j)] = 0;
    }

}

int HandCodedPattern::if_match_3_3_patterns(int pos, int *board, int board_size, int color)
{
    if (color == BLACK)
        memcpy(my_board, board, sizeof(my_board));
    else
        revert_board(my_board, board);


    int off_p_x = I(pos);
    int off_p_y = J(pos);
    if (I(pos)==0 )
        off_p_x = 1;
    else if (I(pos)==board_size-1)
        off_p_x = board_size-2;
    else if (J(pos)==0 )
        off_p_y = 1;
    else if (J(pos)==board_size-1)
        off_p_y = board_size-2;

    switch (board_region[pos]) {
        case 0:
            return 0;
        case 1:
            if (match_3_3_pattern(pos,board_size,hane0_set))
                return 1;
            else if (match_3_3_pattern(pos,board_size,hane1_set))
                return 2;
            else if (match_3_3_pattern(pos,board_size,hane2_set))
                return 3;
            else if (match_3_3_pattern(pos,board_size,hane3_set))
                return 4;
            else if (match_3_3_pattern(pos,board_size,cut1_set)
                     &&!match_3_3_pattern(pos,board_size,cut1_ex0_set)
                     &&!match_3_3_pattern(pos,board_size,cut1_ex1_set))
                return 5;
            else if (match_3_3_pattern(pos,board_size,cut2_set))
                return 6;
            break;
        case 2:case 3:case 4:case 5:
            if (match_cor_side_pattern(POS(off_p_x,off_p_y),board_size,side0_set[board_region[pos]-2])
            || match_cor_side_pattern(POS(off_p_x,off_p_y),board_size,side0_set[board_region[pos]-2+4])
            || match_cor_side_pattern(POS(off_p_x,off_p_y),board_size,side1_set[board_region[pos]-2])
            || match_cor_side_pattern(POS(off_p_x,off_p_y),board_size,side1_set[board_region[pos]-2+4]))
                return 7;
            break;
        case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:
            if (match_cor_side_pattern(POS(off_p_x,off_p_y),board_size,corner0_set[board_region[pos]-6])
                || match_cor_side_pattern(POS(off_p_x,off_p_y),board_size,corner1_set[board_region[pos]-6])
                || match_cor_side_pattern(POS(off_p_x,off_p_y),board_size,corner2_set[board_region[pos]-6]))
                return 8;
            break;
    }

    return 0;
}

bool HandCodedPattern::match_3_3_pattern(int pos, int board_size, int patterns[8][9])
{
    int x = I(pos);
    int y = J(pos);
    bool cur_flag = true;
    for (int k=0;k<8;++k) {
        //try one matching
		cur_flag = true;
        for (int i=0;i<3&&cur_flag;++i) {
            for(int j=0;j<3;++j){
                if (!match_on_pos(my_board[POS(x+i-1,y+j-1)],patterns[k][PATTERN_POS(i,j)])) {
                    cur_flag = false;
                    break;
                }
            }
        }
        if (cur_flag)
            return true;
    }
    return false;
}

bool HandCodedPattern::match_cor_side_pattern(int pos, int board_size, int pattern[9])
{
    int x = I(pos);
    int y = J(pos);
    bool cur_flag = true;

    for (int i=0;i<3&&cur_flag;++i) {
        for(int j=0;j<3;++j){
            if (!match_on_pos(my_board[POS(x+i-1,y+j-1)],pattern[PATTERN_POS(i,j)])) {
                cur_flag = false;
                break;
            }
        }
    }
    if (cur_flag)
        return true;
    return false;
}

bool HandCodedPattern::match_on_pos(int b_pos, int p_pos)
{
    switch(p_pos) {
        case B:
            if (b_pos==BLACK)
                return true;
			break;
        case W:
            if (b_pos==WHITE)
                return true;
			break;
        case E:
            if (b_pos==EMPTY)
                return true;
			break;
        case IG:
            return true;
			break;
        case P:
			if(b_pos==EMPTY)
				return true;
			break;
        case NW:
            if (b_pos!=WHITE)
                return true;
			break;
        case NB:
            if (b_pos!=BLACK)
                return true;
			break;
    }
    return false;
}

void HandCodedPattern::revert_board(int *to_board, int *from_board)
{
    for (int i=0;i<board_size;++i)
    for (int j=0;j<board_size;++j) {
        if (from_board[POS(i,j)] == EMPTY)
            to_board[POS(i,j)] = EMPTY;
        else if (from_board[POS(i,j)] == BLACK)
            to_board[POS(i,j)] = WHITE;
        else
            to_board[POS(i,j)] = BLACK;
    }
}

void HandCodedPattern::rotate_pattern(int *to_pattern, const int *from_pattern)
{
    int to_order[9] = {6,3,0,7,4,1,8,5,2};
    for (int i=0;i<9;++i)
        to_pattern[i] = from_pattern[to_order[i]];
}
void HandCodedPattern::mirror_pattern(int *to_pattern, const int *from_pattern)
{
    int to_order[9] = {2,1,0,5,4,3,8,7,6};
    for (int i=0;i<9;++i)
        to_pattern[i] = from_pattern[to_order[i]];
}





