
#include "STDatasetGenerator.h"

using namespace std;

STDatasetGenerator::STDatasetGenerator(int startStep, int color, int monte_carlo_num) {
	nobb = new NoBB();
	this->startStep = startStep;
	log_file_name = "../../STDataset/ST-" + std::to_string(startStep) + "-" + getLocalTime() + ".txt";
	// go to start step
	int move;
	int cur_color = OTHER_COLOR(color);
	int count = 0;
	int num_pass = 0;
	while (count <= startStep) {
		move = nobb->generate_move(cur_color);		// generate uct move
		nobb->play_move(move, cur_color);
		std::cout << "play " << move << endl;
		// nobb->show_board();
		cur_color = OTHER_COLOR(cur_color);
		count++;
	}
	// show board
	nobb->show_board();
	// iterate from start step
	NoBB_Context context = nobb->get_context();
	int startColor = cur_color;
	for (int i = 0; i < monte_carlo_num; i++)
	{
		cout << "iteration " << i << endl;
		try {
			delete nobb;
			nobb = new NoBB();
			nobb->set_context(context);
			// record current state
			writeBoardToTxt();
			num_pass = 0;
			cur_color = startColor;
			while (true) {
				// cout << "in" << endl;
				move = nobb->generate_move(cur_color);		// generate uct move
				// cout << "gene " << move << endl;
				if (nobb->pass_move(move)) {
					if ((++num_pass) == 2)
						break;
				}
				nobb->play_move(move, cur_color);
				cout << "step " << startStep << " iteration " << i << " play " << move << endl;
				// nobb->show_board();
				cur_color = OTHER_COLOR(cur_color);
			}
			// record final territory
			writeTerritoryToTxt();
		}
		catch (int error) {
			cout << "error" << error << endl;
			i--;
		}

	}
}


void STDatasetGenerator::writeBoardToTxt() {
	log_files.open(log_file_name, ios::app);
	for (int i = 0; i<nobb->board_size; ++i) {
		for (int j = 0; j<nobb->board_size; ++j) {
			log_files << nobb->get_board(i, j);
		}
		log_files << "\n";
	}
	log_files << "\n";
	log_files.close();
}
void STDatasetGenerator::writeTerritoryToTxt() {
	log_files.open(log_file_name, ios::app);
	nobb->compute_final_status();
	for (int i = 0; i < nobb->board_size; i++) {
		for (int j = 0; j < nobb->board_size; j++) {
			int status = nobb->get_final_status(i, j);
			if (status == BLACK_TERRITORY)
				log_files << BLACK;
			else if (status == WHITE_TERRITORY)
				log_files << WHITE;
			else if ((status == ALIVE) ^ (nobb->get_board(i, j) == WHITE))
				log_files << BLACK;
			else
				log_files << WHITE;
		}
		log_files << "\n";
	}
	log_files << "\n";
	log_files.close();
}