#include "PhasingProcess.h"
#include <iostream>
#include <string>

PhasingProcess::PhasingProcess(PhasingParameters params)
{
	std::cout << "=================== PhasingProcess.cpp ===================" << std::endl;

    // load SNP vcf file
    std::cerr<< "parsing VCF ... ";

    // parsing ref fasta
    std::cerr<< "reading reference ... ";

        std::cerr<< "parsing contig/chromosome: " << " ... ";

        std::cerr<< "fetch SNP ... ";

            std::cerr<< "filter SNP ... ";

        std::cerr<< "run algorithm ... ";

    std::cerr<< "writeResult ... ";
    
    std::cout << std::endl;

    return;
};

PhasingProcess::~PhasingProcess(){

};
