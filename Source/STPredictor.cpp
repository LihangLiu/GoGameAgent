#include <Windows.h>
#include <io.h>
#include "STPredictor.h"


void STPredictor::train(vector<int*> boards, vector<double*> territories) {
	vector<BoardFeatures> features = getFeatures(boards);
	// interact with matlab

}

vector<double*> STPredictor::predict(vector<int*> boards) {
	vector<double*> res;
	for (vector<int*>::iterator it = boards.begin(); it != boards.end(); ++it) {
		int* board = *it;
		double* territory = predict(board);
		res.push_back(territory);
	}
	return res;
}

double* STPredictor::predict(int* boards) {
	double* res = new double[BOARD_SIZE*BOARD_SIZE];
	// interact with matlab

	return res;
}

//获取特定格式的文件名  
void STPredictor::GetAllFormatFiles(string path, vector<string>& files, string format)
{
	string ses[] = { "ST-10-02027.txt", "ST-15-32011.txt", "ST-20-224026.txt", "ST-26-214418.txt", "ST-35-153514.txt", "ST-40-21054.txt",
		"ST-5-222511.txt", "ST-10-0314.txt", "ST-15-34921.txt", "ST-20-22456.txt", "ST-26-34841.txt", "ST-35-175049.txt",
		"ST-40-21248.txt", "ST-55-41758.txt", "ST-10-172357.txt", "ST-16-03229.txt", "ST-20-3334.txt", "ST-26-43825.txt",
		"ST-35-175431.txt", "ST-40-42112.txt", "ST-60-5039.txt", "ST-10-184816.txt", "ST-16-1195.txt", "ST-20-34724.txt",
		"ST-30-02622.txt", "ST-35-184121.txt", "ST-40-45329.txt", "ST-8-16136.txt", "ST-10-191419.txt", "ST-16-182041.txt",
		"ST-20-45453.txt", "ST-30-13039.txt", "ST-35-25655.txt", "ST-45-25245.txt", "ST-8-16277.txt", "ST-10-2237.txt",
		"ST-16-1850.txt", "ST-25-02625.txt", "ST-30-225521.txt", "ST-35-33218.txt", "ST-50-34248.txt", "ST-8-224312.txt",
		"ST-10-23836.txt", "ST-20-194919.txt", "ST-25-155.txt", "ST-30-231913.txt", "ST-36-01813.txt", "ST-5-13131.txt",
		"ST-8-233111.txt", "ST-15-203634.txt", "ST-20-20532.txt", "ST-25-23253.txt", "ST-30-32817.txt", "ST-36-0526.txt",
		"ST-5-142945.txt", "ST-15-20655.txt", "ST-20-21318.txt", "ST-25-23593.txt", "ST-30-52016.txt", "ST-40-13946.txt",
		"ST-5-165541.txt", "ST-15-21212.txt", "ST-20-222018.txt", "ST-25-52338.txt", "ST-30-65521.txt", "ST-40-195313.txt",
		"ST-5-172243.txt", "ST-15-2623.txt", "ST-20-222058.txt", "ST-26-212455.txt", "ST-35-11813.txt", "ST-40-19555.txt",
		"ST-5-212844.txt", "ST-8-16186.txt", "ST-16-18747.txt", "ST-20-195421.txt", "ST-26-213031.txt", "ST-30-3723.txt", 
		"ST-36-43323.txt", "ST-40-6018.txt"};
	for (string s : ses) {
		files.push_back(s);
	}

}

void STPredictor::readDataset(vector<string> txtFiles, vector<int*>& boards, vector<double*>& territories, string phase) {
	for (vector<string>::iterator it = txtFiles.begin(); it != txtFiles.end(); ++it) {
		string txtfile = *it;
		readDataset(txtfile, boards, territories, phase);
	}
}

void STPredictor::readDataset(string txtFile, vector<int*>& boards, vector<double*>& territories, string phase) {
	vector<int*> boards_; 
	vector<double*> territories_;

	ifstream file(txtFile);
	string line;
	// read txtfiles
	while (!file.eof()) {
		// get board
		int* board = new int[BOARD_SIZE*BOARD_SIZE];
		for (int i = 0; i < BOARD_SIZE; ++i) {
			getline(file, line);
			for (int j = 0; j < BOARD_SIZE; ++j) {
				board[POS(i, j)] = line[j] - '0';
			}
		}
		if (line.length()<BOARD_SIZE)		// check end of file
			break;
		boards_.push_back(board);
		// get a blank line
		getline(file, line);
		// get territory
		double* t = new double[BOARD_SIZE*BOARD_SIZE];
		for (int i = 0; i < BOARD_SIZE; ++i) {
			getline(file, line);
			for (int j = 0; j < BOARD_SIZE; ++j) {
				t[POS(i, j)] = line[j] - '0';
			}
		}
		territories_.push_back(t);
		// get a blank line
		getline(file, line);
	}

	// put data to boards and territories
	if (phase == "train") {
		boards.insert(boards.end(), boards_.begin(), boards_.end());
		territories.insert(territories.end(), territories_.begin(), territories_.end());

	} else if (phase == "test") {
		double* probabilities = new double[BOARD_SIZE*BOARD_SIZE];
		for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; ++i) {
			probabilities[i] = 0;
		}
		for (double* t : territories_) {
			for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; ++i) {
				probabilities[i] += (t[i] - 1);
			}
		}
		for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; ++i) {
			probabilities[i] /= territories_.size();
		}
		boards.push_back(boards_[0]);
		territories.push_back(probabilities);

	} else {
		cout << "error phase";
		exit(0);
	}

}

vector<BoardFeatures> STPredictor::getFeatures(vector<int*> boards) {
	vector<BoardFeatures> res;
	for (vector<int*>::iterator it = boards.begin(); it != boards.end(); ++it) {
		int* board = *it;
		res.push_back(getFeatures(board));
	}
	return res;
}

BoardFeatures STPredictor::getFeatures(int* board) {
	NoBB* nobb = new NoBB();
	complementNoBBonBoard(nobb, board);
	BoardFeatures fs = getFeatures(board, nobb);
	delete nobb;
	return fs;
}

BoardFeatures STPredictor::getFeatures(int* board, NoBB* nobb) {
	BoardFeatures fs = BoardFeatures();
	for (int i = 0; i<BOARD_SIZE; ++i) {
		for (int j = 0; j<BOARD_SIZE; ++j) {
			int n = 0;
			// black or not
			if (nobb->get_board(i, j) == BLACK)
				fs.setFeature(POS(i, j), n, 1);
			n++;
			// white or not
			if (nobb->get_board(i, j) == WHITE)
				fs.setFeature(POS(i, j), n, 1);
			n++;
			// liberty
			if (nobb->get_board(i,j)!=EMPTY)
				fs.setFeature(POS(i, j), n, nobb->calliberty(i, j, -1, -1));
			n++;
			// is in the corner
			if (i == 0 || i == BOARD_SIZE - 1 || j == 0 || j == BOARD_SIZE - 1)
				fs.setFeature(POS(i, j), n, 1);
			n++;
			// distance to board in x
			fs.setFeature(POS(i, j), n, min(i, BOARD_SIZE - 1 - i));
			n++;
			// distance to board in y
			fs.setFeature(POS(i, j), n, min(j, BOARD_SIZE - 1 - j));
			n++;
			// surrounding black colors (8)
			int count_b = 0;
			for (int k1 = -1; k1 <= 1; ++k1)
			for (int k2 = -1; k2 <= 1; ++k2) {
				if (k1 == 0 && k2 == 0)
					continue;
				int ii = i + k1;
				int jj = j + k2;
				if (nobb->on_board(ii, jj) && nobb->get_board(ii, jj) == BLACK) {
					fs.setFeature(POS(i, j), n, nobb->calliberty(ii, jj, -1, -1));
					// fs.setFeature(POS(i, j), n, 1);
					count_b++;
				}
				n++;
			}
			// count surrounding black  3*3
			fs.setFeature(POS(i, j), n, count_b);
			n++;

			// surrounding white colors (8)
			int count_w = 0;
			for (int k1 = -1; k1 <= 1; ++k1)
			for (int k2 = -1; k2 <= 1; ++k2) {
				if (k1 == 0 && k2 == 0)
					continue;
				int ii = i + k1;
				int jj = j + k2;
				if (nobb->on_board(ii, jj) && nobb->get_board(ii, jj) == WHITE) {
					fs.setFeature(POS(i, j), n, nobb->calliberty(ii, jj, -1, -1));
					// fs.setFeature(POS(i, j), n, 1);
					count_w++;
				}
				n++;
			}
			// count surrounding white 3*3
			fs.setFeature(POS(i, j), n, count_w);
			n++;

			// diff of count surrounding white and black 3*3
			fs.setFeature(POS(i, j), n, count_w-count_b);
			n++;
			
			// count surrounding black, white and empty 5*5 (3)
			count_b = 0;
			count_w = 0;
			int count_e = 0;
			for (int k1 = -2; k1 <= 2; ++k1)
			for (int k2 = -2; k2 <= 2; ++k2) {
				int ii = i + k1;
				int jj = j + k2;
				if (!nobb->on_board(ii, jj))
					continue;
				if (nobb->get_board(ii, jj) == BLACK)
					count_b++;
				else if (nobb->get_board(ii, jj) == WHITE)
					count_w++;
				else
					count_e++;
			}
			fs.setFeature(POS(i, j), n, count_b);
			n++;
			fs.setFeature(POS(i, j), n, count_w);
			n++;
			fs.setFeature(POS(i, j), n, count_e);
			n++;
			fs.setFeature(POS(i, j), n, count_w-count_b);
			n++;
		}
	}
	return fs;
}

void STPredictor::complementNoBBonBoard(NoBB* nobb, int* board) {
	for (int i = 0; i<BOARD_SIZE; ++i) {
		for (int j = 0; j<BOARD_SIZE; ++j) {
			int move = POS(i, j);
			int c = board[move];
			if (c != EMPTY)
				nobb->play_move(move, c);
		}
	}

}

void STPredictor::print(int* board) {
	cout << endl;
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			cout << board[POS(i, j)] << ",";
		}
		cout << endl;
	}
	cout << endl;
}

void STPredictor::print(vector<int*> boards) {
	cout << endl;
	for (vector<int*>::iterator it = boards.begin(); it != boards.end(); ++it) {
		int* board = *it;
		print(board);
	}
	cout << endl;
}

void STPredictor::print(double* board) {
	cout << endl;
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			cout << board[POS(i, j)] << ",";
		}
		cout << endl;
	}
	cout << endl;
}

void STPredictor::print(vector<double*> boards) {
	cout << endl;
	for (vector<double*>::iterator it = boards.begin(); it != boards.end(); ++it) {
		double* board = *it;
		print(board);
	}
	cout << endl;
}

void STPredictor::print(BoardFeatures f) {
	cout << endl;
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			cout << "(";
			for (int c = 0; c < FEATURE_NUM; ++c) {
				cout << f.get(POS(i, j), c) << ",";
			}
			cout << ")";
		}
		cout << endl;
	}
	cout << endl;

}

void STPredictor::print(vector<BoardFeatures> features) {
	cout << endl;
	for (vector<BoardFeatures>::iterator it = features.begin(); it != features.end(); ++it) {
		BoardFeatures f = *it;
		print(f);
	}
	cout << endl;
}