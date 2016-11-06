
#include "STPredictor.h"


void STPredictor::train(vector<int*> boards, vector<int*> territories) {
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

void STPredictor::readDataset(vector<string> txtFiles, vector<int*> boards, vector<int*> territories) {
	for (vector<string>::iterator it = txtFiles.begin(); it != txtFiles.end(); ++it) {
		string txtfile = *it;
		readDataset(txtfile, boards, territories);
	}
}

void STPredictor::readDataset(string txtFile, vector<int*> boards, vector<int*> territories) {
	ifstream file(txtFile);
	string line;
	while (!file.eof()) {
		// get board
		int* board = new int[BOARD_SIZE*BOARD_SIZE];
		for (int i = 0; i < BOARD_SIZE; ++i) {
			getline(file, line);
			for (int j = 0; j < BOARD_SIZE; ++j) {
				board[POS(i, j)] = line[j] - '0';
			}
		}
		boards.push_back(board);
		// get a blank line
		getline(file, line);
		// get territory
		int* t = new int[BOARD_SIZE*BOARD_SIZE];
		for (int i = 0; i < BOARD_SIZE; ++i) {
			getline(file, line);
			for (int j = 0; j < BOARD_SIZE; ++j) {
				t[POS(i, j)] = line[j] - '0';
			}
		}
		territories.push_back(t);
		// get a blank line
		getline(file, line);
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
			// black or white or empty
			if (nobb->get_board(i, j) == BLACK)
				fs.setFeature(POS(i, j), n, -1);
			else if (nobb->get_board(i, j) == WHITE)
				fs.setFeature(POS(i, j), n, 1);
			else
				fs.setFeature(POS(i, j), n, 0);
			n++;
			// liberty
			fs.setFeature(POS(i, j), n, nobb->calliberty(i, j, -1, -1));
			n++;
			// is in the corner
			if (i == 0 || i == BOARD_SIZE - 1 || j == 0 || j == BOARD_SIZE - 1)
				fs.setFeature(POS(i, j), n, 1);
			else
				fs.setFeature(POS(i, j), n, 0);
			n++;
			// surrounding colors
			for (int k1 = -1; k1 <= 1; ++k1)
			for (int k2 = -1; k2 <= 1; ++k2) {
				if (!nobb->on_board(i + k1, j + k2))
					fs.setFeature(POS(i, j), n, 0);
				else if (nobb->get_board(i, j) == BLACK)
					fs.setFeature(POS(i, j), n, -1);
				else if (nobb->get_board(i, j) == WHITE)
					fs.setFeature(POS(i, j), n, 1);
				else
					fs.setFeature(POS(i, j), n, 0);
				n++;
			}
			// count surrounding white
			int count = 0;
			for (int k1 = -1; k1 <= 1; ++k1)
			for (int k2 = -1; k2 <= 1; ++k2) {
				if (!nobb->on_board(i + k1, j + k2))
					continue;
				if (nobb->get_board(i, j) == WHITE)
					count++;
			}
			fs.setFeature(POS(i, j), n, count);
			n++;
			// count surrounding black
			count = 0;
			for (int k1 = -2; k1 <= 2; ++k1)
			for (int k2 = -2; k2 <= 2; ++k2) {
				if (!nobb->on_board(i + k1, j + k2))
					continue;
				if (nobb->get_board(i, j) == BLACK)
					count++;
			}
			fs.setFeature(POS(i, j), n, count);
			n++;
			// count surrounding empty
			count = 0;
			for (int k1 = -2; k1 <= 2; ++k1)
			for (int k2 = -2; k2 <= 2; ++k2) {
				if (!nobb->on_board(i + k1, j + k2))
					continue;
				if (nobb->get_board(i, j) == EMPTY)
					count++;
			}
			fs.setFeature(POS(i, j), n, count);
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