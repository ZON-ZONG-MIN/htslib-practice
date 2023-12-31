#include "Phasing.h"
#include "PhasingProcess.h"
#include <cstddef>
#include "Util.h"
#include <getopt.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>


#define SUBPROGRAM "phase"

static const char *CORRECT_USAGE_MESSAGE =
"Usage: "  " " SUBPROGRAM " [OPTION] ... READSFILE\n"
"   --help                                 display this help and exit.\n\n"
"require arguments:\n"
"   -s, --snp-file=NAME                    input SNP vcf file.\n"
"   -b, --bam-file=NAME                    input bam file.\n"
"   -r, --reference=NAME                   reference fasta.\n"
"   --ont, --pb                            ont: Oxford Nanopore genomic reads.\n"
"                                          pb: PacBio HiFi/CCS genomic reads.\n\n"
"optional arguments:\n"
"   --sv-file=NAME                         input SV vcf file.\n"
"   --mod-file=NAME                        input modified vcf file.(produce by longphase modcall)\n"
"   -t, --threads=Num                      number of thread. default:1\n"
"   -o, --out-prefix=NAME                  prefix of phasing result.\n"
"   --indels                               phase small indel. default: False\n"
"   --dot                                  each contig/chromosome will generate dot file. \n\n"

"parse alignment arguments:\n"
"   -q, --mappingQuality=Num               filter alignment if mapping quality is lower than threshold. default:1\n\n"

"phasing graph arguments:\n"
"   -a, --connectAdjacent=Num              connect adjacent N SNPs. default:6\n"
"   -d, --distance=Num                     phasing two variant if distance less than threshold. default:300000\n"
"   -c, --crossSNP=Num                     block phasing step. sample N SNPs in each block. default:15\n"
"   -1, --readsThreshold=[0~1]             give up SNP-SNP phasing pair if the number of reads of the \n"
"                                          two combinations are similar. default:0.05\n"
"   -v, --confidentHaplotype=[0~1]         the haplotype of the current SNP is judged by the haplotype of the previous N SNPs.\n"
"                                          if the threshold is higher, the consistency of SNP needs to be higher. default:0.5\n"
"   -j, --judgeInconsistent=[0~1]          the proportion of inconsistent haplotypes among the haplotypes of the previous N SNPs.\n"
"                                          inconsistent SNPs are tagged if the proportion is below the threshold. default:0.4\n"
"   -i, --inconsistentThreshold=Num        phased genotype correction is performed when a SNP is tagged multiple times. default:5\n\n"

"haplotag read correction arguments:\n"
"   -m, --readConfidence=[0.5~1]           The maximum proportion of alleles with the same haplotype in a read. default:0.6\n"
"\n";

static const char* shortopts = "s:b:o:t:r:d:c:1:a:q:j:i:v:n:m:";

enum { OPT_HELP = 1 , DOT_FILE, SV_FILE, MOD_FILE, IS_ONT, IS_PB, PHASE_INDEL, VERSION};

static const struct option longopts[] = { 
    { "help",                 no_argument,        NULL, OPT_HELP },
    { "dot",                  no_argument,        NULL, DOT_FILE },  
    { "ont",                  no_argument,        NULL, IS_ONT }, 
    { "pb",                   no_argument,        NULL, IS_PB }, 
    { "version",              no_argument,        NULL, VERSION }, 
    { "indels",               no_argument,        NULL, PHASE_INDEL },   
    { "sv-file",              required_argument,  NULL, SV_FILE },  
    { "mod-file",             required_argument,  NULL, MOD_FILE },
    { "reference",            required_argument,  NULL, 'r' },
    { "snp-file",             required_argument,  NULL, 's' },
    { "bam-file",             required_argument,  NULL, 'b' },
    { "out-prefix",           required_argument,  NULL, 'o' },
    { "threads",              required_argument,  NULL, 't' },
    { "distance",             required_argument,  NULL, 'd' },
    { "crossSNP",             required_argument,  NULL, 'c' },
    { "readsThreshold",       required_argument,  NULL, '1' },
    { "connectAdjacent",      required_argument,  NULL, 'a' },
    { "mappingQuality",       required_argument,  NULL, 'q' },
    { "judgeInconsistent",    required_argument,  NULL, 'j' },
    { "inconsistentThreshold",required_argument,  NULL, 'i' },
    { "confidentHaplotype",   required_argument,  NULL, 'v' },
    { "snpConfidence",      required_argument,  NULL, 'n' },
    { "readConfidence",       required_argument,  NULL, 'm' },
    { NULL, 0, NULL, 0 }
};

namespace opt
{
    static int numThreads = 1;
    static int distance = 300000;
    static int crossSNP = 15;
    static std::string snpFile="";
    static std::string svFile="";
    static std::string modFile="";
    static std::vector<std::string> bamFile;
    static std::string fastaFile="";
    static std::string resultPrefix="result";
    static bool generateDot=false;
    static bool isONT=false;
    static bool isPB=false;
    static bool phaseIndel=false;

    static int connectAdjacent = 6;
    static int mappingQuality =1;

    static double confidentHaplotype = 0.5;
    static double judgeInconsistent = 0.4 ;
    static int inconsistentThreshold = 5 ;

    static double snpConfidence = 0.6;
    static double readConfidence = 0.6;

    double readsThreshold = 0.05;

    std::string command;
}

void PhasingOptions(int argc, char** argv)
{
    optind=1;    //reset getopt

    bool die = false;
    for (char c; (c = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1;)
    {
        std::istringstream arg(optarg != NULL ? optarg : "");
        switch (c)
        {
        case 's': arg >> opt::snpFile; break;
        case 't': arg >> opt::numThreads; break;
        case 'o': arg >> opt::resultPrefix; break;
        case 'r': arg >> opt::fastaFile; break;
        case 'd': arg >> opt::distance; break;
        case 'c': arg >> opt::crossSNP; break;
        case '1': arg >> opt::readsThreshold; break;
        case 'a': arg >> opt::connectAdjacent; break;
        case 'q': arg >> opt::mappingQuality; break;
        case 'j': arg >> opt::judgeInconsistent; break;
        case 'i': arg >> opt::inconsistentThreshold; break;
        case 'v': arg >> opt::confidentHaplotype; break;
        case 'n': arg >> opt::snpConfidence; break;
        case 'm': arg >> opt::readConfidence; break;
        case 'b': {
            std::string bamFile;
            arg >> bamFile;
            opt::bamFile.push_back(bamFile); break;
        }
        case SV_FILE:  arg >> opt::svFile; break;
        case MOD_FILE: arg >> opt::modFile; break;
        case PHASE_INDEL: opt::phaseIndel=true; break;
        case DOT_FILE: opt::generateDot=true; break;
        case IS_ONT: opt::isONT=true; break;
        case IS_PB: opt::isPB=true; break;

        case OPT_HELP:
            std::cout << CORRECT_USAGE_MESSAGE;
            exit(EXIT_SUCCESS);
        }
    }

    for(int i = 0; i < argc; ++i){
        opt::command.append(argv[i]);
        opt::command.append(" ");
    }

    if (argc - optind < 0 )
    {
        std::cerr << SUBPROGRAM ": missing arguments\n";
        die = true;
    }

    if( opt::isONT == false && opt::isPB == false ){
        std::cerr << SUBPROGRAM ": missing arguments. --ont or --pb\n";
        die = true;
    }

    if( opt::isONT == true && opt::isPB == true ){
        std::cerr << SUBPROGRAM ": conflict arguments. --ont or --pb\n";
        die = true;
    }

    if( opt::snpFile != "")
    {
        std::ifstream openFile( opt::snpFile.c_str() );
        if( !openFile.is_open() )
        {
            std::cerr<< "File " << opt::snpFile << " not exist.\n\n";
            die = true;
        }
    }
    else{
        std::cerr << SUBPROGRAM ": missing SNP file.\n";
        die = true;
    }

    if( opt::fastaFile != "")
    {
        std::ifstream openFile( opt::snpFile.c_str() );
        if( !openFile.is_open() )
        {
            std::cerr<< "File " << opt::fastaFile << " not exist.\n\n";
            die = true;
        }
    }
    else{
        std::cerr << SUBPROGRAM ": missing reference.\n";
        die = true;
    }

    if ( opt::numThreads < 1 ){
        std::cerr << SUBPROGRAM " invalid threads. value: "
                  << opt::numThreads
                  << "\n please check -t, --threads=Num\n";
        die = true;
    }

    if ( opt::distance < 0 ){
        std::cerr << SUBPROGRAM " invalid distance. value: "
                  << opt::distance
                  << "\n please check -d or --distance=Num\n";
        die = true;
    }

    if ( opt::crossSNP < 0 ){
        std::cerr << SUBPROGRAM " invalid crossSNP. value: "
                  << opt::crossSNP
                  << "\n please check -c, --crossSNP=Num\n";
        die = true;
    }

    if ( opt::connectAdjacent < 0 ){
        std::cerr << SUBPROGRAM " invalid connectAdjacent. value: "
                  << opt::connectAdjacent
                  << "\n please check -a, --connectAdjacent=Num\n";
        die = true;
    }

    if ( opt::mappingQuality < 0 ){
        std::cerr << SUBPROGRAM " invalid mappingQuality. value: "
                  << opt::mappingQuality
                  << "\n please check -m, --mappingQuality=Num\n";
        die = true;
    }

    if ( opt::readsThreshold < 0 || opt::readsThreshold > 1 ){
        std::cerr << SUBPROGRAM " invalid readsThreshold. value: "
                  << opt::readsThreshold
                  << "\n please check -1, --readsThreshold=[0~1]\n";
        die = true;
    }

    if (die)
    {
        std::cerr << "\n" << CORRECT_USAGE_MESSAGE;
        exit(EXIT_FAILURE);
    }

}

int PhasingMain(int argc, char** argv, std::string in_version)
{
    PhasingParameters ecParams;
    // set parameters
    PhasingOptions(argc, argv);
    // no file in command line

    ecParams.numThreads=opt::numThreads;
    ecParams.distance=opt::distance;
    ecParams.crossSNP=opt::crossSNP;
    ecParams.snpFile=opt::snpFile;
    ecParams.svFile=opt::svFile;
    ecParams.modFile=opt::modFile;
   // ecParams.bamFile=opt::bamFile;
    ecParams.fastaFile=opt::fastaFile;
    ecParams.resultPrefix=opt::resultPrefix;
    ecParams.generateDot=opt::generateDot;
    ecParams.isONT=opt::isONT;
    ecParams.isPB=opt::isPB;
    ecParams.phaseIndel=opt::phaseIndel;
    
    ecParams.connectAdjacent=opt::connectAdjacent;
    ecParams.mappingQuality=opt::mappingQuality;
    
    ecParams.confidentHaplotype=opt::confidentHaplotype;
    ecParams.judgeInconsistent=opt::judgeInconsistent;
    ecParams.inconsistentThreshold=opt::inconsistentThreshold;
    
    ecParams.readsThreshold=opt::readsThreshold;
    
    ecParams.snpConfidence=opt::snpConfidence;
    ecParams.readConfidence=opt::readConfidence;
    
    ecParams.version=in_version;
    ecParams.command=opt::command;

    std::cout << "=================== Phasing.cpp 289 ===================" << std::endl;
    std::cout << "distance = " << opt::distance << std::endl;
    std::cout << "crossSNP = " << opt::crossSNP << std::endl; 
    std::cout << "generateDot = " << opt::generateDot << std::endl;    
    std::cout << "connectAdjacent = " << opt::connectAdjacent << std::endl;
    std::cout << "mappingQuality = " << opt::mappingQuality << std::endl;
    std::cout << "confidentHaplotype = " << opt::confidentHaplotype << std::endl;
    std::cout << "judgeInconsistent = " << opt::judgeInconsistent << std::endl;
    std::cout << "inconsistentThreshold = " << opt::inconsistentThreshold << std::endl;
    std::cout << "readsThreshold = " << opt::readsThreshold << std::endl;
    std::cout << "snpConfidence = " << opt::snpConfidence << std::endl;    
    std::cout << "readConfidence = " << opt::readConfidence << std::endl;

    PhasingProcess processor(ecParams);

    return 0;
}
