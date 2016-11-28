
#ifndef STRATEGYCOMPARATOR_H
#define STRATEGYCOMPARATOR_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <time.h>
#include <windows.h>
#include "UCT.h"
#include "NoBB.h"
#include "config.h"
using namespace std;

static int uct_strategy = -1;	// -1: use default. Refer to config.h

class StrategyComparator {
public:
	StrategyComparator() {
		int black_wins = 0;
		int count = 0;
		for (int i = 0; i < 10; ++i) {
			NoBB* nobb = new NoBB();
			int pass_count = 0;
			while (true) {
				if (pass_count >= 2)
					break;

				int color = BLACK;
				uct_strategy = 3;
				int move = nobb->generate_move(color);
				nobb->play_move(I(move), J(move), color);
				if (nobb->pass_move(move))
					pass_count++;

				color = WHITE;
				uct_strategy = 5;
				move = nobb->generate_move(color);
				nobb->play_move(I(move), J(move), color);
				if (nobb->pass_move(move))
					pass_count++;

				std::cout << black_wins << "/" << count << std::endl;
				nobb->show_board();
			}
			if (nobb->get_black_win() > 0)
				black_wins++;
			count++;
			std::cout << black_wins << "/" << count << std::endl;
			delete nobb;
		}
	}
};





#endif
