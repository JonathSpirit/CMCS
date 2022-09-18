#ifndef _SIMUL_C8051_OPCODE_HPP_INCLUDED
#define _SIMUL_C8051_OPCODE_HPP_INCLUDED

#include <string>

namespace simul
{
namespace core
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

}//end core
}//end simul

#endif // _SIMUL_C8051_OPCODE_HPP_INCLUDED
