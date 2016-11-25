
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
		double train_test_ratio = 0.8;

		// txt files
		string datasetPath = "../../STDataset/";
		vector<string> txtfiles;
		GetAllFormatFiles(datasetPath, txtfiles, ".txt");		// ill implemented = = f*ck windows c++

		// load boards, territories and features for train
		string phase = "train";
		vector<int*> train_boards;
		vector<double*> train_territories;
		vector<BoardFeatures> train_features;
		for (int i = 0; i < txtfiles.size()*train_test_ratio; ++i) {
			string txtfile = datasetPath + txtfiles[i];
			readDataset(txtfile, train_boards, train_territories, phase);
		}
		train_features = getFeatures(train_boards);

		cout << train_boards.size() << endl;
		cout << train_territories.size() << endl;
		cout << train_features.size() << endl;

		// load boards, territories and features for test
		phase = "test";
		vector<int*> test_boards;
		vector<double*> test_probabilities;
		vector<BoardFeatures> test_features;
		for (int i = txtfiles.size()*train_test_ratio; i < txtfiles.size(); ++i) {
			string txtfile = datasetPath + txtfiles[i];
			readDataset(txtfile, test_boards, test_probabilities, phase);
		}
		test_features = getFeatures(test_boards);

		cout << test_boards.size() << endl;
		cout << test_probabilities.size() << endl;
		cout << test_features.size() << endl;


		// CRF Training
		CRFonMatlab* crf = new CRFonMatlab();
		crf->train(train_boards, train_features, train_territories);
		crf->test(test_boards, test_features, test_probabilities);
		delete crf;

	}
	~STPredictor() {
		// delete crf;
	}

	void train(vector<int*> boards, vector<double*> territories);
	vector<double*> predict(vector<int*> boards);
	double* predict(int* boards);
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
