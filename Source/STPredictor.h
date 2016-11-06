
#ifndef STPREDICTOR_H
#define STPREDICTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <time.h>
#include <windows.h>
#include "UCT.h"
#include "NoBB.h"
#include "config.h"
using namespace std;

#define FEATURE_NUM 20

class BoardFeatures
{
public:
	double features[BOARD_SIZE*BOARD_SIZE][FEATURE_NUM];
	int feature_num = FEATURE_NUM;
	BoardFeatures() {
		for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; ++i)
		for (int n = 0; n < FEATURE_NUM; ++n) {
			setFeature(i, n, 0);
		}
	}
	void setFeature(int move, int n, double value) {
		features[move][n] = value;
	}
};

class STPredictor
{
public:
	STPredictor() {}
	~STPredictor() {}

	void train(vector<int*> boards, vector<int*> territories);
	vector<double*> predict(vector<int*> boards);
	double* predict(int* boards);
	void readDataset(vector<string> txtFiles, vector<int*> boards, vector<int*> territories);
	void readDataset(string txtFile, vector<int*> boards, vector<int*> territories);
	vector<BoardFeatures> getFeatures(vector<int*> boards);
	BoardFeatures getFeatures(int* board);
	BoardFeatures getFeatures(int* board, NoBB* nobb);
	void complementNoBBonBoard(NoBB* nobb, int* board);

};



#endif
