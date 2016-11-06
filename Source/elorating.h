#ifndef ELORATING_H
#define ELORATING_H

#include "stdio.h"
#include "NoBB.h"

class NoBB;

class elorating
{
public:
	void calelo(NoBB * bb, int prepos, int preprepos, int color, double * rating);

private:
	double dis2boarder(int x, int y, int board_size);
	double dist1(int x, int y, int prepos, int board_size);
	double dist2(int x, int y, int preprepos, int board_size);
};

#endif
