
#ifndef STPREDICTOR_H
#define STPREDICTOR_H

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
#include "CRFonMatlab.h"
using namespace std;




class STPredictor
{
public:
	// crf;
	STPredictor() {
		CRFonMatlab* crf = new CRFonMatlab();

		string txtfile = "../../STDataset/ST-20-22456.txt";
		vector<int*> boards;
		vector<double*> territories;
		vector<BoardFeatures> features;
		// load boards, territories and features
		readDataset(txtfile, boards, territories);
		features = getFeatures(boards);

		// CRF Training
		crf->train(features, territories);
		delete crf;

	}
	~STPredictor() {
		// delete crf;
	}

	void train(vector<int*> boards, vector<double*> territories);
	vector<double*> predict(vector<int*> boards);
	double* predict(int* boards);
	void readDataset(vector<string> txtFiles, vector<int*>& boards, vector<double*>& territories);
	void readDataset(string txtFile, vector<int*>& boards, vector<double*>& territories);
	vector<BoardFeatures> getFeatures(vector<int*> boards);
	BoardFeatures getFeatures(int* board);
	BoardFeatures getFeatures(int* board, NoBB* nobb);
	void complementNoBBonBoard(NoBB* nobb, int* board);

	void print(int* boards);
	void print(vector<int*> boards);
	void print(BoardFeatures features);
	void print(vector<BoardFeatures> features);
};



#endif
