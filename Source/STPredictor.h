
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
	CRFonMatlab* crf;
	STPredictor() {
		crf = new CRFonMatlab();
	}
	~STPredictor() {
		// delete crf;
		delete crf;
	}

	void overall_test();
	void train(vector<int*> boards, vector<double*> territories);
	vector<double*> predict(vector<int*> boards);
	double* predict(int* boards);
	void predict(int* board, double* probability);
	void GetAllFormatFiles(string path, vector<string>& files, string format);
	void readDataset(vector<string> txtFiles, vector<int*>& boards, vector<double*>& territories, string phase);
	void readDataset(string txtFile, vector<int*>& boards, vector<double*>& territories, string phase);
	vector<BoardFeatures> getFeatures(vector<int*> boards);
	BoardFeatures getFeatures(int* board);
	BoardFeatures getFeatures(int* board, NoBB* nobb);
	void complementNoBBonBoard(NoBB* nobb, int* board);

	void print(int* boards);
	void print(vector<int*> boards);
	void print(double* boards);
	void print(vector<double*> boards);
	void print(BoardFeatures features);
	void print(vector<BoardFeatures> features);
};



#endif
