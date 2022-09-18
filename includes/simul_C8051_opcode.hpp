#ifndef _CMCS_SIMUL_C8051_OPCODE_HPP_INCLUDED
#define _CMCS_SIMUL_C8051_OPCODE_HPP_INCLUDED

#include <string>

namespace cmcs::core
{

typedef bool (*OpcodeFunction) ();

struct OpcodeInfo
{
    std::string _opcode;
    std::string _operands;
    OpcodeFunction _func;
};

namespace opcode
{

extern OpcodeInfo _C8051_opcode[256];

}//end opcode

}//end cmcs::core

#endif // _CMCS_SIMUL_C8051_OPCODE_HPP_INCLUDED
