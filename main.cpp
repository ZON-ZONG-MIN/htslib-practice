#include <fstream>
#include <iostream>
#include "Phasing.h"

#define PROGRAM_BIN "main"
#define VERSION "1.7"

static std::string version = VERSION;

static const char *STRIDE_USAGE_MESSAGE =
"Version: " VERSION " \n"
"Usage: " PROGRAM_BIN " <command> [options]\n"  
"               phase      run phasing algorithm.\n"
"\n";

int main(int argc, char** argv)
{
    if(argc <= 1)
    {
        std::cout << STRIDE_USAGE_MESSAGE;
        return 0;
    }
    
    std::string command(argv[1]);
    
    if(command=="phase")
    {
        PhasingMain(argc - 1, argv + 1, version);
	//std::cout << "command = " << command << std::endl; 
    }else{
        std::cout << STRIDE_USAGE_MESSAGE;
        return 0;
    }

    return 0;
}
