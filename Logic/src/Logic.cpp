/*******************************************************************************
* Project:  Beacon
* @file     Beacon.cpp
* @brief 
* @author   bwar
* @date:    Apr 7, 2018
* @note
* Modify history:
******************************************************************************/

#include "util/proctitle_helper.h"
#include "labor/Manager.hpp"

int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);
    if (argc != 2)
    {
        std::cerr << "para num error!" << std::endl;
        exit(-1);
    }
    ngx_init_setproctitle(argc, argv);
    neb::Manager oManager(argv[1]);
    oManager.Run();
    return(0);
}


