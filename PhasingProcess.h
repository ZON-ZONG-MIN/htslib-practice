#ifndef PHASINGPROCESS_H
#define PHASINGPROCESS_H
#include "Util.h"
#include <string>

struct PhasingParameters{
	int numThreads;
	int distance;
	int crossSNP;
	
	std::string snpFile;
	std::string svFile;
	std::string bamFile;
	std::string modFile="";
	std::string fastaFile;
	std::string resultPrefix;

	bool generateDot;
	bool isONT;
	bool isPB;
	bool phaseIndel;

	int connectAdjacent;
	int mappingQuality;

	double confidentHaplotype;
	double judgeInconsistent;
	int inconsistentThreshold;

	double snpConfidence;
	double readConfidence;

	double readsThreshold;

	std::string version;
	std::string command;
};

class PhasingProcess{
	public:
		PhasingProcess(PhasingParameters params);
		~PhasingProcess();
};

#endif
