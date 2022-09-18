#ifndef _SIMUL_MAIN_HPP_INCLUDED
#define _SIMUL_MAIN_HPP_INCLUDED

#include <string>
#include <replxx.hxx>

#define S_PRINT_FORMAT(tag_, str_) "\x1b[1;33m[" tag_"]\x1b[0m " str_

namespace simul
{

enum SIMUL_STATS
{
    SIMUL_STAT_IDLE,
    SIMUL_STAT_RUNNING
};

extern replxx::Replxx* gTerminalPtr;

}//end simul

#endif // _SIMUL_MAIN_HPP_INCLUDED
