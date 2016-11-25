
#ifndef CRFONMATLAB_H
#define CRFONMATLAB_H

#include <stdio.h>
#include "engine.h"
#include "STPredictor.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libeng.lib")
#pragma comment(lib, "libmx.lib")
#pragma comment(lib, "libmat.lib")

#define FEATURE_NUM 40

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
	double get(int move, int n) {
		return features[move][n];
	}
	void setFeature(int move, int n, double value) {
		features[move][n] = value;
	}
};



class CRFonMatlab 
{
public:
	Engine* pEng;
	ofstream log_files;
	CRFonMatlab() {
		pEng = engOpen(NULL);
		if (!pEng)
		{
			printf("Open matlab enging fail!");
			getchar();
			return;
		}
	}
	~CRFonMatlab() {
		if (pEng)
			engClose(pEng);
	}

	void train(vector<int*> boards, vector<BoardFeatures> features, vector<double*> territories) {
		if (features.size() != territories.size()) {
			cout << "size doesn't match" << endl;
			exit(0);
		}
		int N = features.size();
		int X = BOARD_SIZE*BOARD_SIZE;
		int C = FEATURE_NUM;
		int* boards_1d = matrix_2_vector(boards, N, X);
		double* features_1d = matrix_2_vector(features, N, X, C);
		double* territories_1d = matrix_2_vector(territories, N, X);

		
		log_files.open("loadData.m", ios::app);
		log_files << "train_boards_1d = [";
		print(boards_1d, N*X);
		log_files << "]; \n\ntrain_features_1d = [";
		print(features_1d, N*X*C);
		log_files << "]; \n\ntrain_territories_1d = [";
		print(territories_1d, N*X);
		log_files << "];" << endl;
		log_files.close();

		return;


		// put size into matlab
		engPutVariable(pEng, "N", mxCreateDoubleScalar((double)N));
		engPutVariable(pEng, "B", mxCreateDoubleScalar((double)BOARD_SIZE));
		engPutVariable(pEng, "C", mxCreateDoubleScalar((double)C));

		// put features into matlab
		mxArray *features_1d_m = mxCreateDoubleMatrix(1, N*X*C, mxREAL);
		memcpy(mxGetPr(features_1d_m), features_1d, N*X*C*sizeof(features_1d[0]));
		engPutVariable(pEng, "features_1d", features_1d_m);

		// put territories into matlab
		mxArray *territories_1d_m = mxCreateDoubleMatrix(1, N*X, mxREAL);
		memcpy(mxGetPr(territories_1d_m), territories_1d, N*X*sizeof(territories_1d[0]));
		engPutVariable(pEng, "territories_1d", territories_1d_m);

		// run
		engEvalString(pEng, "cd('D:\Course\Robot Learning-CRF2D');");
		engEvalString(pEng, "addpath(genpath(pwd))");
		engEvalString(pEng, "STTrain(features_1d, territories_1d, N,B,C);");

		
		// clean up
		delete features_1d, territories_1d;
		mxDestroyArray(features_1d_m);
		mxDestroyArray(territories_1d_m);
	}
	void test(vector<int*> boards, vector<BoardFeatures> features, vector<double*> probabilities) {
		if (features.size() != probabilities.size()) {
			cout << "size doesn't match" << endl;
			exit(0);
		}
		int N = features.size();
		int X = BOARD_SIZE*BOARD_SIZE;
		int C = FEATURE_NUM;
		int* boards_1d = matrix_2_vector(boards, N, X);
		double* features_1d = matrix_2_vector(features, N, X, C);
		double* probabilities_1d = matrix_2_vector(probabilities, N, X);
		
		log_files.open("loadData.m", ios::app);
		log_files << "test_boards_1d = [";
		print(boards_1d, N*X);
		log_files << "]; \n\ntest_features_1d = [";
		print(features_1d, N*X*C);
		log_files << "]; \n\ntest_probabilities_1d = [";
		print(probabilities_1d, N*X);
		log_files << "];" << endl;
		log_files.close();

	}

private:
	void print(double *x, int N) {
		log_files << endl;
		for (int i = 0; i < N; ++i)
			log_files << x[i] << ",";
		log_files << endl;
	}
	void print(int *x, int N) {
		log_files << endl;
		for (int i = 0; i < N; ++i)
			log_files << x[i] << ",";
		log_files << endl;
	}

	int* matrix_2_vector(vector<int*> matrix, int N, int X) {
		int* v = new int[N*X];
		for (int n = 0; n < N; ++n) {
			for (int x = 0; x < X; ++x) {
				v[n*X + x] = matrix[n][x];
			}
		}
		return v;
	}
	double* matrix_2_vector(vector<double*> matrix, int N, int X) {
		double* v = new double[N*X];
		for (int n = 0; n < N; ++n) {
			for (int x = 0; x < X; ++x) {
				v[n*X + x] = matrix[n][x];
			}
		}
		return v;
	}
	double* matrix_2_vector(vector<BoardFeatures> matrix, int N, int X, int C) {
		double* v = new double[N*X*C];
		for (int n = 0; n < N; ++n) {
			for (int x = 0; x < X; ++x) {
				BoardFeatures f = matrix[n];
				for (int c = 0; c < C; ++c) {
					v[(n*X+x)*C + c] = f.get(x,c);
				}
			}
		}
		return v;
	}
	vector<int*> vector_2_matrix(int* vec, int N, int X) {
		vector<int*> matrix;
		for (int n = 0; n < N; ++n) {
			for (int x = 0; x < X; ++x) {
				matrix[n][x] = vec[n*X + x];
			}
		}
		return matrix;
	}
	vector<double*> vector_2_matrix(double* vec, int N, int X) {
		vector<double*> matrix;
		for (int n = 0; n < N; ++n) {
			for (int x = 0; x < X; ++x) {
				matrix[n][x] = vec[n*X + x];
			}
		}
		return matrix;
	}
};



#endif
