#include "elorating.h"

/* Offsets for the four directly adjacent neighbors. Used for looping. */
static int deltai[4] = {-1, 1, 0, 0};
static int deltaj[4] = {0, 0, -1, 1};

void elorating::calelo(NoBB * bb, int prepos, int preprepos, int color, double * rating)
{
	int board_size = bb->board_size;
	memset(rating, 0, board_size*board_size*sizeof(double));

	for (int ai = 0; ai < board_size; ai++)
	{
    	for (int aj = 0; aj < board_size; aj++)
		{
            if (bb->legal_move(ai, aj, color) && !bb->suicide(ai, aj, color))
            {
                /* Further require the move not to be suicide for the opponent... */
                if (!bb->suicide(ai, aj, OTHER_COLOR(color)))
                    rating[POS(ai,aj)] = 1;
                else
                {
                    /* ...however, if the move captures at least one stone,
                     * consider it anyway.
                     */
                    for (int k = 0; k < 4; k++) {
                        int bi = ai + deltai[k];
                        int bj = aj + deltaj[k];
                        if (bb->on_board(bi, bj) && bb->get_board(bi, bj) == OTHER_COLOR(color)) {
                            rating[POS(ai,aj)] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }

    for (int ai = 0; ai < board_size; ai++)
	{
    	for (int aj = 0; aj < board_size; aj++)
		{
			if (rating[POS(ai,aj)] == 1)
			{
				rating[POS(ai,aj)] = 0;
				bool defense = false;
				bool atari = false;
				bool defence = false;
				bool capture = false;
				bool neighborInAtalri = false;
				//feature 1: pass
				//not consider


				//feature 2: capture
				double big = 0;
				for (int k = 0; k < 4; k++)
				{
					int bi = ai + deltai[k];
					int bj = aj + deltaj[k];
					if (bb->on_board(bi, bj) && bb->get_board(bi, bj) == OTHER_COLOR(color) && !bb->has_additional_liberty(bi, bj, ai, aj))
					{
						for (int l = 0; l < 4; ++l)
						{
							if (l==k) continue;
							int ci = ai + deltai[l];
							int cj = aj + deltaj[l];

							if (!bb->same_string(POS(bi,bj),POS(ci,cj)) && bb->next_stone[POS(ci,cj)] != POS(ci,cj))
							{
								big = 30.68;
								break;
							}

							if (!bb->same_string(POS(bi,bj),POS(ci,cj)) && bb->next_stone[POS(ci,cj)] == POS(ci,cj) && POS(ci,cj)==prepos)
							{
								big = max(big, 2.88);
								break;
							}
						}
					}
				}
				big = max(big, 3.43);
				rating[POS(ai,aj)] += big;

				//atari defense
				if (bb->if_defense(ai,aj,color)) {
					rating[POS(ai,aj)] += 6.43;
				}


				//feature 5: atari
				for (int k = 0; k < 4; k++)
				{
					int bi = ai + deltai[k];
					int bj = aj + deltaj[k];
					if (bb->on_board(bi, bj) && bb->get_board(bi, bj) == OTHER_COLOR(color) && !bb->calliberty(bi, bj, ai, aj) == 1)
					{
						rating[POS(ai,aj)] += 1.7;
						break;
					}
				}



				//feature 6: distance to border
				rating[POS(ai,aj)] += dis2boarder(ai, aj, board_size)/2;

				//feature 7: distance to previous move
				rating[POS(ai,aj)] += dist1(ai, aj, prepos, board_size);

				//feature 8: distance to the move before previous move
				rating[POS(ai,aj)] += dist2(ai, aj, preprepos, board_size);


			}
		}
	}

	// feature 9: mc
	/*
	int res[MAX_BOARD * MAX_BOARD];
	NoBB_Context backup = bb->get_context();
	for (int i = 1; i <= 64; ++i)
	{
		bb->monte_carlo(OTHER_COLOR(color));
		//res += board_size();
		int c, ct;
		if (color == WHITE){
			c = WHITE;
			ct = WHITE_TERRITORY;
		}
		else {
			c = BLACK;
			ct = BLACK_TERRITORY;
		}
		for (int i = 0; i<board_size; ++i) {
			for (int j = 0; j<board_size; ++j) {
				if (bb->get_final_status(i, j) == ct || bb->get_board(i, j) == c) {
					res[i*bb->board_size+j]++;
				}
			}
		}
		bb->set_context(backup);
	}
	

    for (int ai = 0; ai < board_size; ai++)
	{
    	for (int aj = 0; aj < board_size; aj++)
		{
			if (rating[POS(ai,aj)]!=0)
			{
				int posi = POS(ai, aj);
				int d = res[posi];
				switch (d / 8)
				{
					case 1: rating[POS(ai,aj)] +=  0.04; break;
					case 2: rating[POS(ai,aj)] +=  1.02; break;
					case 3: rating[POS(ai,aj)] +=  2.41; break;
					case 4: rating[POS(ai,aj)] +=  1.41; break;
					case 5: rating[POS(ai,aj)] +=  0.72; break;
					case 6: rating[POS(ai,aj)] +=  0.65; break;
					case 7: rating[POS(ai,aj)] +=  0.68; break;
					case 8: rating[POS(ai,aj)] +=  0.13; break;
					default: rating[POS(ai,aj)] +=  0;
				}
			}



		}
		
	}
	*/

}


double elorating::dis2boarder(int x, int y, int board_size)
{
	int dis = min(x, board_size - 1 - x);
	double s = 0;
	switch (dis){
		case 1: s+= 0.89; break;
		case 2: s+= 1.49; break;
		case 3: s+= 1.75; break;
		case 4: s+= 1.28; break;
		default: s+= 0; break;
	}
	dis = min(y, board_size - 1 - y);
	switch (dis){
		case 1: s+= 0.89; break;
		case 2: s+= 1.49; break;
		case 3: s+= 1.75; break;
		case 4: s+= 1.28; break;
		default: s+= 0; break;
	}
	return s;
}

double elorating::dist1(int x, int y, int prepos, int board_size)
{
	if (prepos < 0) return 0;
	int xx = I(prepos);
	int yy = J(prepos);
	int deltax = abs(x - xx);
	int deltay = abs(y - yy);
	int d = deltay + deltax + max(deltax , deltay);
	double result = 0;
	switch (d){
		case 2: result =  4.32; break;
		case 3: result =  2.84; break;
		case 4: result =  2.22; break;
		case 5: result =  1.58; break;
		//...
		case 16: result =  0.33; break;
		default: result =  0.21; break;
	}
	return result;
}

double elorating::dist2(int x, int y, int preprepos, int board_size)
{
	if (preprepos < 0) return 0;
	int xx = I(preprepos);
	int yy = J(preprepos);
	int deltax = abs(x - xx);
	int deltay = abs(y - yy);
	int d = deltay + deltax + max(deltax , deltay);
	double result = 0;
	switch (d){
		case 2: result = 3.08; break;
		case 3: result = 2.38; break;
		case 4: result = 2.27; break;
		case 5: result = 1.68; break;
		//...
		case 16: result = 0.66; break;
		default: result = 0.70; break;
	}
	return result;
}
