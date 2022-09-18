#ifndef _SIMUL_DEVICE_HPP_INCLUDED
#define _SIMUL_DEVICE_HPP_INCLUDED

#include <string>
#include <vector>
#include "C_bit.hpp"

namespace simul::device
{

struct C8051_data
{
    std::string _device;
    std::vector<uint8_t> _dataFlash;
    std::vector<uint8_t> _dataRam;
    simul::BitBank<uint8_t> _dataBit;
    std::vector<uint8_t> _dataXRam;
    uint16_t _programCounter; //PC
    uint16_t _cursor;

    uint8_t* _stackPointer; //SP
    uint16_t* _dptr; //DPTR
    uint8_t* _registerBank; //Rn
    uint8_t* _accumulator; //A
    uint8_t* _b; //B
    simul::Bits<uint8_t> _psw; //PSW
};

enum BitNamePsw
{
    BIT_PSW_PARITY = 0,

    BIT_PSW_OVERFLOW = 2,
    BIT_PSW_RS0 = 3,
    BIT_PSW_RS1 = 4,

    BIT_PSW_FLAG0 = 5,
    BIT_PSW_AUXCARRY = 6,
    BIT_PSW_CARRY = 7
};

}//simul::device

#endif //SIMUL_DEVICE_HPP_INCLUDED
