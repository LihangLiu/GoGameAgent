// config.h
// configurations for nobb and uctree
// by backchord
// 2015.1.11
// all rights reserverd

#define NoBB_NAME     "NoBB_AMAF_9x9"    //team name string
#define NoBB_VERSION  "2.2"     //version string

#define BOARD_SIZE 9			// Board size
#define GENERATE_STDATASET 1	// automatically generate ST Dataset, see generate_STDataset() in interface.cpp

#define TEST_MODE 1             //whether printf or output to log files
#define THREAD_NUM 2            //thread number used for uctree, 5 is tested to be optimal
#define TIME_LIMIT 2000         //time limit for one step (ms)
#define NODE_EXPAND_LIMIT   1   //limit to expand children in uctree
#define MC_LIMIT 300            //limit for monte carlo simulation steps
#define UCT_STARTEGY 3          /* 0 hungry:   scorej = delta_wins/nj + 0.5*sqrt(log(n)/nj)
                                 * 1 standard: scorej = wins/nj + sqrt(2*log(n)/nj)
                                 * 2 heuristic:scorej = (1-b)*wins/nj + b*1/(1+exp(-p*H);   b = sqrt(K/(K+3*nj));
								 * 3 amaf :    scorej = (1-b-c)*wins/nj + b*amaf_wins/namaf;
								 * 4 mogo :	   scorej = (1-b-c)*wins/nj + b*amaf_wins/namaf + c*1/(1+exp(-p*H); 
                                 */