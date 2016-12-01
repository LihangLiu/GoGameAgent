
#ifndef CONFIG_H
#define CONFIG_H

// config.h
// configurations for nobb and uctree
// by backchord
// 2015.1.11
// all rights reserverd

#define NoBB_NAME     "NoBB_ST2"    //team name string
#define NoBB_VERSION  "1.0"     //version string

#define BOARD_SIZE 9			// Board size
#define GENERATE_STDATASET 0	// automatically generate ST Dataset, see generate_STDataset() in interface.cpp

#define SELF_PLAY 0				//whether to perform self-play in terminal
#define TEST_MODE 0             //whether printf or output to log files
#define THREAD_NUM 3            //thread number used for uctree, 5 is tested to be optimal
#define TIME_LIMIT 4000         //time limit for one step (ms)
#define NODE_EXPAND_LIMIT   1   //minimum mc times to expand children in uctree
#define MC_LIMIT 300            //limit for monte carlo simulation steps
#define UCT_STARTEGY 5          /* 0 hungry:   scorej = delta_wins/nj + 0.5*sqrt(log(n)/nj)
                                 * 1 standard: scorej = wins/nj + sqrt(2*log(n)/nj)
                                 * 2 heuristic:scorej = (1-b)*wins/nj + b*1/(1+exp(-p*H);   b = sqrt(K/(K+3*nj));
								 * 3 amaf :    scorej = (1-b-c)*wins/nj + b*amaf_wins/namaf;
								 * 4 mogo :	   scorej = (1-b-c)*wins/nj + b*amaf_wins/namaf + c*1/(1+exp(-p*H); 
								 * 5 amaf+ST   scorej = (1-b-c)*wins/nj + b*amaf_wins/namaf + c*(0.5-abs(0.5-ST_wins/nST));
                                 */





#endif