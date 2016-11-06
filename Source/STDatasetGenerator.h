#ifndef STDATASETGENNERATOR_H
#define STDATASETGENNERATOR_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <string>
#include <time.h>
#include <windows.h>
#include "UCT.h"
#include "NoBB.h"
#include "config.h"
using namespace std;

static string getLocalTime()
{
	time_t timeObj;
	time(&timeObj);
	tm *pTime = gmtime(&timeObj);
	char buffer[1000];
	sprintf(buffer, "%d%d%d", pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
	return buffer;
}

class STDatasetGenerator{
public:
	NoBB* nobb;
	int startStep;
	string log_file_name;
	ofstream log_files;
	STDatasetGenerator(int startStep, int color, int monte_carlo_num);
	~STDatasetGenerator() { delete nobb; }

private:
	void writeBoardToTxt();
	void writeTerritoryToTxt();
};




#endif
