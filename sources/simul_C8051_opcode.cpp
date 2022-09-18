#include "simul_C8051_opcode.hpp"
#include "simul_core.hpp"
#include "simul_math.hpp"

//http://www.keil.com/support/docs/1584.htm
//http://www.keil.com/support/man/docs/is51/is51_opcodes.htm
//http://www.keil.com/support/man/docs/is51/is51_instructions.htm
//http://www.keil.com/support/man/docs/is51/is51_ov_cpu8051variants.htm
//http://www.keil.com/support/man/docs/is51/is51_ov_cpupsw.htm
//http://what-when-how.com/8051-microcontroller/8051-register-banks-and-stack/
//http://what-when-how.com/8051-microcontroller/bit-addresses-for-io-and-ram/
//http://what-when-how.com/8051-microcontroller/8051-flag-bits-and-the-psw-register/
//https://www.silabs.com/documents/public/presentations/8051_Instruction_Set.pdf

// PSW Parity bit:
// par is 1 for odd, 0 for even.

/**
:10246200464C5549442050524F46494C4500464C33
|||||||||||                              CC->Checksum
|||||||||DD->Data
|||||||TT->Record Type
|||AAAA->Address
|LL->Record Length
:->Colon
**/

namespace cmcs::core::opcode
{

bool Op_NOP()
{
    gDeviceData._programCounter+=1;
    return true;
}
bool Op_AJMP()
{
    gDeviceData._programCounter+=2;
    gDeviceData._programCounter&=~0x7F;
    gDeviceData._programCounter |= static_cast<unsigned short>(gDeviceData._dataFlash[gDeviceData._cursor]&0xE0)<<3;
    gDeviceData._programCounter |= gDeviceData._dataFlash[gDeviceData._cursor+1];
    return true;
}
bool Op_LJMP()
{
    gDeviceData._programCounter = (static_cast<unsigned short>(gDeviceData._dataFlash[gDeviceData._cursor+1])<<8) | gDeviceData._dataFlash[gDeviceData._cursor+2];
    return true;
}
bool Op_RR()
{
    gDeviceData._programCounter+=1;

    bool tmpBit0 = (*gDeviceData._accumulator)&0x01;

    (*gDeviceData._accumulator) = ((*gDeviceData._accumulator)>>1) | (tmpBit0?0x80:0x00);
    return true;
}
bool Op_INC()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];

    switch (opcode)
    {
    case 0x04: //INC A
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator)+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    case 0x05: //INC direct
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ] += 1;
        return true;
    case 0x06: //INC @R0
        gDeviceData._programCounter+=1;
        gDeviceData._dataRam[ gDeviceData._registerBank[0] ] += 1;
        return true;
    case 0x07: //INC @R1
        gDeviceData._programCounter+=1;
        gDeviceData._dataRam[ gDeviceData._registerBank[1] ] += 1;
        return true;
    case 0xA3: //INC DPTR
        gDeviceData._programCounter+=1;
        (*gDeviceData._dptr) += 1;
        return true;
    }
    if ( (opcode&0xF8)==0x08 )
    {//INC Rn
        gDeviceData._programCounter+=1;
        gDeviceData._registerBank[opcode&0x07] +=1;
        return true;
    }

    return false;
}
bool Op_JBC()
{
    gDeviceData._programCounter+=3;

    if ( gDeviceData._dataBit.getBit( gDeviceData._dataFlash[gDeviceData._cursor+1] ) )
    {
        gDeviceData._dataBit.getBit( gDeviceData._dataFlash[gDeviceData._cursor+1] ) = 0;
        gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
    }
    return true;
}
bool Op_ACALL()
{
    gDeviceData._programCounter+=2;
    *gDeviceData._stackPointer += 1;
    gDeviceData._dataRam[ *gDeviceData._stackPointer ] = gDeviceData._programCounter&0x00FF;
    *gDeviceData._stackPointer += 1;
    gDeviceData._dataRam[ *gDeviceData._stackPointer ] = gDeviceData._programCounter>>8;
    gDeviceData._programCounter = (static_cast<unsigned short>(gDeviceData._dataFlash[gDeviceData._cursor]&0xE0)<<3) | gDeviceData._dataFlash[gDeviceData._cursor+1];
    return true;
}
bool Op_LCALL()
{
    gDeviceData._programCounter+=3;
    (*gDeviceData._stackPointer) += 1;
    gDeviceData._dataRam[ *gDeviceData._stackPointer ] = gDeviceData._programCounter&0x00FF;
    (*gDeviceData._stackPointer) += 1;
    gDeviceData._dataRam[ *gDeviceData._stackPointer ] = gDeviceData._programCounter>>8;
    gDeviceData._programCounter = (static_cast<unsigned short>(gDeviceData._dataFlash[gDeviceData._cursor+1])<<8) | gDeviceData._dataFlash[gDeviceData._cursor+2];
    return true;
}
bool Op_RRC()
{
    gDeviceData._programCounter+=1;

    bool tmpCarry = gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    gDeviceData._psw = (*gDeviceData._accumulator)&0x01;

    (*gDeviceData._accumulator) = ((*gDeviceData._accumulator)>>1) | (tmpCarry?0x80:0x00);
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
    return true;
}
bool Op_DEC()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];

    switch (opcode)
    {
    case 0x14: //DEC A
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator)-=1;
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    case 0x15: //DEC direct
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ] -= 1;
        return true;
    case 0x16: //DEC @R0
        gDeviceData._programCounter+=1;
        gDeviceData._dataRam[ gDeviceData._registerBank[0] ] -= 1;
        return true;
    case 0x17: //DEC @R1
        gDeviceData._programCounter+=1;
        gDeviceData._dataRam[ gDeviceData._registerBank[1] ] -= 1;
        return true;
    }
    if ( (opcode&0xF8)==0x18 )
    {//DEC Rn
        gDeviceData._programCounter+=1;
        gDeviceData._registerBank[opcode&0x07] -=1;
        return true;
    }

    return false;
}
bool Op_JB()
{
    gDeviceData._programCounter+=3;
    if ( gDeviceData._dataBit.getBit( gDeviceData._dataFlash[gDeviceData._cursor+1] ) )
    {
        gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
    }
    return true;
}
bool Op_RET()
{
    gDeviceData._programCounter = static_cast<unsigned short>(gDeviceData._dataRam[*gDeviceData._stackPointer])<<8;
    (*gDeviceData._stackPointer) -= 1;
    gDeviceData._programCounter |= gDeviceData._dataRam[*gDeviceData._stackPointer];
    (*gDeviceData._stackPointer) -= 1;
    return true;
}
bool Op_RL()
{
    gDeviceData._programCounter+=1;

    bool tmpBit7 = (*gDeviceData._accumulator)&0x80;

    (*gDeviceData._accumulator) = ((*gDeviceData._accumulator)<<1) | (tmpBit7?0x01:0x00);
    return true;
}
bool Op_ADD()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];

    switch (opcode)
    {
    case 0x24: //ADD A, #immediate
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._dataFlash[gDeviceData._cursor+1], 1);
        (*gDeviceData._accumulator)+=gDeviceData._dataFlash[gDeviceData._cursor+1];
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x25: //ADD direct
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]], 1);
        (*gDeviceData._accumulator)+=gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]];
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x26: //ADD @R0
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._dataRam[gDeviceData._registerBank[0]], 1);
        (*gDeviceData._accumulator)+=gDeviceData._dataRam[gDeviceData._registerBank[0]];
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x27: //ADD @R1
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._dataRam[gDeviceData._registerBank[1]], 1);
        (*gDeviceData._accumulator)+=gDeviceData._dataRam[gDeviceData._registerBank[1]];
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    }
    if ( (opcode&0xF8)==0x28 )
    {//ADD Rn
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._registerBank[opcode&0x07], 1);
        (*gDeviceData._accumulator)+=gDeviceData._registerBank[opcode&0x07];
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    }

    return false;
}
bool Op_JNB()
{
    gDeviceData._programCounter+=3;
    if ( !gDeviceData._dataBit.getBit( gDeviceData._dataFlash[gDeviceData._cursor+1] ) )
    {
        gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
    }
    return true;
}
bool Op_RETI()
{
    gDeviceData._programCounter+=1;
    gDeviceData._programCounter = static_cast<unsigned short>(gDeviceData._dataRam[*gDeviceData._stackPointer])<<8;
    *gDeviceData._stackPointer -= 1;
    gDeviceData._programCounter |= gDeviceData._dataRam[*gDeviceData._stackPointer];
    *gDeviceData._stackPointer -= 1;
    return true;
}
bool Op_RLC()
{
    gDeviceData._programCounter+=1;

    bool tmpCarry = gDeviceData._psw.setPos(device::BIT_PSW_CARRY);

    gDeviceData._psw = (*gDeviceData._accumulator)&0x80;

    (*gDeviceData._accumulator) = ((*gDeviceData._accumulator)<<1) | (tmpCarry?0x01:0x00);
    gDeviceData._psw = cmcs::math::GetParity(*gDeviceData._accumulator);
    return true;
}
bool Op_ADDC()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_CARRY);

    switch (opcode)
    {
    case 0x34: //ADDC A, #immediate
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._dataFlash[gDeviceData._cursor+1], 1);
        (*gDeviceData._accumulator)+=gDeviceData._dataFlash[gDeviceData._cursor+1] + gDeviceData._psw.setPos(device::BIT_PSW_CARRY);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x35: //ADDC direct
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]], 1);
        (*gDeviceData._accumulator)+=gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] + gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x36: //ADDC @R0
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._dataRam[gDeviceData._registerBank[0]], 1);
        (*gDeviceData._accumulator)+=gDeviceData._dataRam[gDeviceData._registerBank[0]] + gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x37: //ADDC @R1
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._dataRam[gDeviceData._registerBank[1]], 1);
        (*gDeviceData._accumulator)+=gDeviceData._dataRam[gDeviceData._registerBank[1]] + gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    }
    if ( (opcode&0xF8)==0x38 )
    {//ADDC Rn
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow((*gDeviceData._accumulator), gDeviceData._registerBank[opcode&0x07], 1);
        (*gDeviceData._accumulator)+=gDeviceData._registerBank[opcode&0x07] + gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) = cmcs::math::GetCarry((*gDeviceData._accumulator), 1);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    }

    return false;
}
bool Op_JC()
{
    gDeviceData._programCounter+=2;

    if ( gDeviceData._psw.setPos(device::BIT_PSW_PARITY) )
    {
        gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]);
    }
    return true;
}
bool Op_ORL()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0x44: //ORL A, #immediate
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) |= gDeviceData._dataFlash[gDeviceData._cursor+1];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x45: //ORL direct
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) |= gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x46: //ORL @R0
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) |= gDeviceData._dataRam[gDeviceData._registerBank[0]];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x47: //ORL @R1
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) |= gDeviceData._dataRam[gDeviceData._registerBank[1]];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0xA0: //ORL C, /bit
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) |= !gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]);
        return true;
    case 0x72: //ORL C, bit
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) |= gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]);
        return true;
    case 0x43: //ORL direct, #immediate
        gDeviceData._programCounter+=3;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] |= gDeviceData._dataFlash[gDeviceData._cursor+2];
        return true;
    case 0x42: //ORL direct, A
        gDeviceData._programCounter+=3;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] |= *gDeviceData._accumulator;
        return true;
    }
    if ( (opcode&0xF8)==0x48 )
    {//ORL Rn
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) |= gDeviceData._registerBank[opcode&0x07];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    }

    return false;
}
bool Op_JNC()
{
    gDeviceData._programCounter+=2;

    if ( !gDeviceData._psw.setPos(device::BIT_PSW_PARITY) )
    {
        gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]);
    }
    return true;
}
bool Op_ANL()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0x54: //ANL A, #immediate
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) &= gDeviceData._dataFlash[gDeviceData._cursor+1];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x55: //ANL direct
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) &= gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x56: //ANL @R0
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) &= gDeviceData._dataRam[gDeviceData._registerBank[0]];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x57: //ANL @R1
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) &= gDeviceData._dataRam[gDeviceData._registerBank[1]];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0xB0: //ANL C, /bit
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) &= !gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]);
        return true;
    case 0x82: //ANL C, bit
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_CARRY) &= gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]);
        return true;
    case 0x52: //ANL direct, A
        gDeviceData._programCounter+=3;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] &= *gDeviceData._accumulator;
        return true;
    case 0x53: //ANL direct, #immediate
        gDeviceData._programCounter+=3;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] &= gDeviceData._dataFlash[gDeviceData._cursor+2];
        return true;
    }
    if ( (opcode&0xF8)==0x58 )
    {//ANL Rn
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) &= gDeviceData._registerBank[opcode&0x07];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    }

    return false;
}
bool Op_JZ()
{
    gDeviceData._programCounter+=2;

    if ( !(*gDeviceData._accumulator) )
    {
        gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]);
    }
    return true;
}
bool Op_XRL()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0x64: //XRL A, #immediate
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) ^= gDeviceData._dataFlash[gDeviceData._cursor+1];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x65: //XRL direct
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) ^= gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x66: //XRL @R0
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) ^= gDeviceData._dataRam[gDeviceData._registerBank[0]];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x67: //XRL @R1
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) ^= gDeviceData._dataRam[gDeviceData._registerBank[1]];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x62: //XRL direct, A
        gDeviceData._programCounter+=3;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] ^= *gDeviceData._accumulator;
        return true;
    case 0x63: //XRL direct, #immediate
        gDeviceData._programCounter+=3;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] ^= gDeviceData._dataFlash[gDeviceData._cursor+2];
        return true;
    }
    if ( (opcode&0xF8)==0x68 )
    {//XRL Rn
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) ^= gDeviceData._registerBank[opcode&0x07];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    }

    return false;
}
bool Op_JNZ()
{
    gDeviceData._programCounter+=2;

    if ( *gDeviceData._accumulator )
    {
        gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]);
    }
    return true;
}
bool Op_JMP()
{
    gDeviceData._programCounter+=1;
    gDeviceData._programCounter += (*gDeviceData._dptr) + (*gDeviceData._accumulator);
    return true;
}
bool Op_MOV()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];

    switch (opcode)
    {
    case 0x74: //MOV A, #immediate
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) = gDeviceData._dataFlash[gDeviceData._cursor+1];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0xE5: //MOV A, direct
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) = gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0xE6: //MOV A, @R0
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) = gDeviceData._dataRam[gDeviceData._registerBank[0]];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0xE7: //MOV A, @R1
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) = gDeviceData._dataRam[gDeviceData._registerBank[1]];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;

    case 0x76: //MOV @R0, #immediate
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[gDeviceData._registerBank[0]] = gDeviceData._dataFlash[gDeviceData._cursor+1];
        return true;
    case 0x77: //MOV @R1, #immediate
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[gDeviceData._registerBank[1]] = gDeviceData._dataFlash[gDeviceData._cursor+1];
        return true;
    case 0xF6: //MOV @R0, A
        gDeviceData._programCounter+=1;
        gDeviceData._dataRam[gDeviceData._registerBank[0]] = (*gDeviceData._accumulator);
        return true;
    case 0xF7: //MOV @R1, A
        gDeviceData._programCounter+=1;
        gDeviceData._dataRam[gDeviceData._registerBank[1]] = (*gDeviceData._accumulator);
        return true;
    case 0xA6: //MOV @R0, direct
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[gDeviceData._registerBank[0]] = gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ];
        return true;
    case 0xA7: //MOV @R1, direct
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[gDeviceData._registerBank[1]] = gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ];
        return true;

    case 0x92: //MOV bit, C
        gDeviceData._programCounter+=2;
        gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]) = gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        return true;
    case 0xA2: //MOV C, bit
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]);
        return true;

    case 0x85: //MOV dest_direct, src_direct
        gDeviceData._programCounter+=3;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+2]] = gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]];
        return true;
    case 0x75: //MOV direct, #immediate
        gDeviceData._programCounter+=3;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] = gDeviceData._dataFlash[gDeviceData._cursor+2];
        return true;
    case 0x86: //MOV direct, @R0
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] = gDeviceData._dataRam[gDeviceData._registerBank[0]];
        return true;
    case 0x87: //MOV direct, @R1
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] = gDeviceData._dataRam[gDeviceData._registerBank[1]];
        return true;

    case 0xF5: //MOV direct, A
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] = (*gDeviceData._accumulator);
        return true;
    case 0x90: //MOV DPTR, #immediate
        gDeviceData._programCounter+=3;
        (*gDeviceData._dptr) = (static_cast<unsigned short>(gDeviceData._dataFlash[gDeviceData._cursor+1])<<8) | gDeviceData._dataFlash[gDeviceData._cursor+2];
        return true;
    }
    if ( (opcode&0xF8)==0xE8 )
    {//MOV A, Rn
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) = gDeviceData._registerBank[opcode&0x07];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    }
    if ( (opcode&0xF8)==0x88 )
    {//MOV direct, Rn
        gDeviceData._programCounter+=2;
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] = gDeviceData._registerBank[opcode&0x07];
        return true;
    }
    if ( (opcode&0xF8)==0x78 )
    {//MOV Rn, #immediate
        gDeviceData._programCounter+=2;
        gDeviceData._registerBank[opcode&0x07] = gDeviceData._dataFlash[gDeviceData._cursor+1];
        return true;
    }
    if ( (opcode&0xF8)==0xF8 )
    {//MOV Rn, A
        gDeviceData._programCounter+=1;
        gDeviceData._registerBank[opcode&0x07] = (*gDeviceData._accumulator);
        return true;
    }
    if ( (opcode&0xF8)==0xA8 )
    {//MOV Rn, direct
        gDeviceData._programCounter+=2;
        gDeviceData._registerBank[opcode&0x07] = gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]];
        return true;
    }

    return false;
}
bool Op_SJMP()
{
    gDeviceData._programCounter+=2;
    gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]);
    return true;
}
bool Op_MOVC()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];

    switch (opcode)
    {
    case 0x93: //MOVC A, @A+DPTR
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) = gDeviceData._dataRam[*gDeviceData._accumulator] + (*gDeviceData._dptr);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;
    case 0x83: //MOVC A, @A+PC
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) = gDeviceData._dataRam[*gDeviceData._accumulator] + gDeviceData._programCounter;
        return true;
    }

    return false;
}
bool Op_DIV()
{
    gDeviceData._programCounter+=1;
    unsigned char div_remainder = (*gDeviceData._accumulator) % (*gDeviceData._b);
    unsigned char div_quotient  = (*gDeviceData._accumulator) / (*gDeviceData._b);

    (*gDeviceData._accumulator) = div_quotient;
    (*gDeviceData._b) = div_remainder;
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetCarry(*gDeviceData._accumulator, false);
    gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = 0;

    return true;
}
bool Op_SUBB()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0x94: //SUBB A, #immediate
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow(*gDeviceData._accumulator, gDeviceData._psw.setPos(device::BIT_PSW_PARITY)+gDeviceData._dataFlash[gDeviceData._cursor+1], false);
        (*gDeviceData._accumulator) -= gDeviceData._dataFlash[gDeviceData._cursor+1] - gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry(*gDeviceData._accumulator, false);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetCarry(*gDeviceData._accumulator, false);
        return true;
    case 0x95: //SUBB A, direct
        gDeviceData._programCounter+=2;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow(*gDeviceData._accumulator, gDeviceData._psw.setPos(device::BIT_PSW_PARITY)+gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ], false);
        (*gDeviceData._accumulator) -= gDeviceData._dataRam[ gDeviceData._dataFlash[gDeviceData._cursor+1] ] - gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry(*gDeviceData._accumulator, false);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetCarry(*gDeviceData._accumulator, false);
        return true;
    case 0x96: //SUBB A, @R0
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow(*gDeviceData._accumulator, gDeviceData._psw.setPos(device::BIT_PSW_PARITY)+gDeviceData._dataRam[gDeviceData._registerBank[0]], false);
        (*gDeviceData._accumulator) -= gDeviceData._dataRam[gDeviceData._registerBank[0]] - gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry(*gDeviceData._accumulator, false);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetCarry(*gDeviceData._accumulator, false);
        return true;
    case 0x97: //SUBB A, @R1
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow(*gDeviceData._accumulator, gDeviceData._psw.setPos(device::BIT_PSW_PARITY)+gDeviceData._dataRam[gDeviceData._registerBank[1]], false);
        (*gDeviceData._accumulator) -= gDeviceData._dataRam[gDeviceData._registerBank[1]] - gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry(*gDeviceData._accumulator, false);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetCarry(*gDeviceData._accumulator, false);
        return true;
    }
    if ( (opcode&0xF8)==0x98 )
    {//SUBB A, Rn
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = cmcs::math::WillOverflow(*gDeviceData._accumulator, gDeviceData._psw.setPos(device::BIT_PSW_PARITY)+gDeviceData._dataRam[gDeviceData._registerBank[opcode&0x07]], false);
        (*gDeviceData._accumulator) -= gDeviceData._registerBank[opcode&0x07]  - gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) = cmcs::math::GetBCDCarry(*gDeviceData._accumulator, false);
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetCarry(*gDeviceData._accumulator, false);
        return true;
    }

    return false;
}
bool Op_MUL()
{
    gDeviceData._programCounter+=1;
    unsigned short mul_result = static_cast<unsigned short>(*gDeviceData._accumulator) * (*gDeviceData._b);

    (*gDeviceData._accumulator) = mul_result&0x00FF;
    (*gDeviceData._b) = mul_result>>8;

    gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 0;
    gDeviceData._psw.setPos(device::BIT_PSW_OVERFLOW) = mul_result>255;

    return true;
}
bool Op_CPL()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0xF4: //CPL A
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) = ~(*gDeviceData._accumulator);
        return true;
    case 0xB2: //CPL bit
        gDeviceData._programCounter+=2;
        gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]) = !gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]);
        return true;
    case 0xB3: //CPL C
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = !gDeviceData._psw.setPos(device::BIT_PSW_PARITY);
        return true;
    }

    return false;
}
bool Op_CJNE()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0xB6: //CJNE @R0, #immediate, offset
        gDeviceData._programCounter+=3;

        if ( gDeviceData._dataRam[gDeviceData._registerBank[0]] != gDeviceData._dataFlash[gDeviceData._cursor+1] )
        {
            gDeviceData._programCounter+= static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
        }
        if ( gDeviceData._dataRam[gDeviceData._registerBank[0]] < static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]) )
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 1;
        }
        else
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 0;
        }
        return true;
    case 0xB7: //CJNE @R1, #immediate, offset
        gDeviceData._programCounter+=3;

        if ( gDeviceData._dataRam[gDeviceData._registerBank[1]] != gDeviceData._dataFlash[gDeviceData._cursor+1] )
        {
            gDeviceData._programCounter+= static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
        }
        if ( gDeviceData._dataRam[gDeviceData._registerBank[1]] < static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]) )
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 1;
        }
        else
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 0;
        }
        return true;
    case 0xB4: //CJNE A, #immediate, offset
        gDeviceData._programCounter+=3;

        if ( (*gDeviceData._accumulator) != gDeviceData._dataFlash[gDeviceData._cursor+1] )
        {
            gDeviceData._programCounter+= static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
        }
        if ( (*gDeviceData._accumulator) < static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]) )
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 1;
        }
        else
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 0;
        }
        return true;
    case 0xB5: //CJNE A, direct, offset
        gDeviceData._programCounter+=3;

        if ( (*gDeviceData._accumulator) != gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] )
        {
            gDeviceData._programCounter+= static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
        }
        if ( (*gDeviceData._accumulator) < static_cast<char>(gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]]) )
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 1;
        }
        else
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 0;
        }
        return true;
    }
    if ( (opcode&0xF8)==0xB8 )
    {//CJNE Rn, #immediate, offset
        gDeviceData._programCounter+=3;

        if ( gDeviceData._registerBank[opcode&0x07] != gDeviceData._dataFlash[gDeviceData._cursor+1] )
        {
            gDeviceData._programCounter+= static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
        }
        if ( gDeviceData._registerBank[opcode&0x07] < static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]) )
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 1;
        }
        else
        {
            gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 0;
        }
        return true;
    }

    return false;
}
bool Op_PUSH()
{
    gDeviceData._programCounter+=2;
    *gDeviceData._stackPointer += 1;
    gDeviceData._dataRam[*gDeviceData._stackPointer] = gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]];

    return true;
}
bool Op_CLR()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0xE4: //CLR A
        gDeviceData._programCounter+=1;
        (*gDeviceData._accumulator) = 0;
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    case 0xC2: //CLR bit
        gDeviceData._programCounter+=2;
        gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]) = 0;
        return true;
    case 0xC3: //CLR C
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 0;
        return true;
    }

    return false;
}
bool Op_SWAP()
{
    gDeviceData._programCounter+=1;
    (*gDeviceData._accumulator) = ((*gDeviceData._accumulator)>>4) | ((*gDeviceData._accumulator)<<4);

    return true;
}
bool Op_XCH()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    unsigned char buff;

    switch (opcode)
    {
    case 0xC6: //XCH A, @R0
        gDeviceData._programCounter+=1;
        buff = (*gDeviceData._accumulator);
        (*gDeviceData._accumulator) = gDeviceData._dataRam[gDeviceData._registerBank[0]];
        gDeviceData._dataRam[gDeviceData._registerBank[0]] = buff;

        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    case 0xC7: //XCH A, @R1
        gDeviceData._programCounter+=1;
        buff = (*gDeviceData._accumulator);
        (*gDeviceData._accumulator) = gDeviceData._dataRam[gDeviceData._registerBank[1]];
        gDeviceData._dataRam[gDeviceData._registerBank[1]] = buff;

        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    case 0xC5: //XCH A, direct
        gDeviceData._programCounter+=2;

        buff = (*gDeviceData._accumulator);
        (*gDeviceData._accumulator) = gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]];
        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] = buff;

        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    }
    if ( (opcode&0xF8)==0xC8 )
    {//XCH A, Rn
        gDeviceData._programCounter+=1;

        buff = (*gDeviceData._accumulator);
        (*gDeviceData._accumulator) = gDeviceData._registerBank[opcode&0x07];
        gDeviceData._registerBank[opcode&0x07] = buff;

        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    }

    return false;
}
bool Op_POP()
{
    gDeviceData._programCounter+=2;
    gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] = gDeviceData._dataRam[*gDeviceData._stackPointer];
    *gDeviceData._stackPointer -= 1;

    return true;
}
bool Op_SETB()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0xD2: //SETB bit
        gDeviceData._programCounter+=2;
        gDeviceData._dataBit.getBit(gDeviceData._dataFlash[gDeviceData._cursor+1]) = 1;
        return true;
    case 0xD3: //SETB C
        gDeviceData._programCounter+=1;
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = 1;
        return true;
    }

    return false;
}
bool Op_DA()
{
    gDeviceData._programCounter+=1;
    if ( (((*gDeviceData._accumulator)&0x0F)>9) || gDeviceData._psw.setPos(device::BIT_PSW_AUXCARRY) )
    {
        (*gDeviceData._accumulator) += 6;
    }
    if ( (((*gDeviceData._accumulator)>>4)>9) || gDeviceData._psw.setPos(device::BIT_PSW_PARITY) )
    {
        (*gDeviceData._accumulator) += 0x60;
    }
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);

    return true;
}
bool Op_DJNZ()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    gDeviceData._psw.setPos(device::BIT_PSW_PARITY);

    switch (opcode)
    {
    case 0xD5: //DJNZ direct, offset
        gDeviceData._programCounter+=3;

        gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] -= 1;

        if ( gDeviceData._dataRam[gDeviceData._dataFlash[gDeviceData._cursor+1]] )
        {
            gDeviceData._programCounter += static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+2]);
        }
        return true;
    }
    if ( (opcode&0xF8)==0xD8 )
    {//DJNZ Rn, offset
        gDeviceData._programCounter+=2;

        gDeviceData._registerBank[opcode&0x07] -= 1;

        if ( gDeviceData._registerBank[opcode&0x07] )
        {
            gDeviceData._programCounter+= static_cast<char>(gDeviceData._dataFlash[gDeviceData._cursor+1]);
        }
        return true;
    }

    return false;
}
bool Op_XCHD()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];
    unsigned char buff;

    switch (opcode)
    {
    case 0xD6: //XCHD A, @R0
        gDeviceData._programCounter+=1;
        buff = (*gDeviceData._accumulator);
        (*gDeviceData._accumulator) = ((*gDeviceData._accumulator)&0xF0) | (gDeviceData._dataRam[gDeviceData._registerBank[0]]&0x0F);
        gDeviceData._dataRam[gDeviceData._registerBank[0]] = (gDeviceData._dataRam[gDeviceData._registerBank[0]]&0xF0) | (buff&0x0F);

        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    case 0xD7: //XCHD A, @R1
        gDeviceData._programCounter+=1;
        buff = (*gDeviceData._accumulator);
        (*gDeviceData._accumulator) = ((*gDeviceData._accumulator)&0xF0) | (gDeviceData._dataRam[gDeviceData._registerBank[1]]&0x0F);
        gDeviceData._dataRam[gDeviceData._registerBank[1]] = (gDeviceData._dataRam[gDeviceData._registerBank[1]]&0xF0) | (buff&0x0F);

        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity(*gDeviceData._accumulator);
        return true;
    }

    return false;
}
bool Op_MOVX()
{
    unsigned char opcode = gDeviceData._dataFlash[gDeviceData._cursor];

    #warning Doit faire le truc pour acceder a la memoire externe
    //http://www.keil.com/support/man/docs/is51/is51_movx.htm

    switch (opcode)
    {
    case 0xE0: //MOVX A, @DPTR
        gDeviceData._programCounter+=2;
        (*gDeviceData._accumulator) = gDeviceData._dataXRam[*gDeviceData._dptr];
        gDeviceData._psw.setPos(device::BIT_PSW_PARITY) = cmcs::math::GetParity((*gDeviceData._accumulator));
        return true;

    case 0xF2: //MOVX @R0, A
        gDeviceData._programCounter+=1;
        //gDeviceData._dataRam[gDeviceData._registerBank[0]] = (*gDeviceData._accumulator);
        return true;
    case 0xF3: //MOVX @R1, A
        gDeviceData._programCounter+=1;
        //gDeviceData._dataRam[gDeviceData._registerBank[1]] = (*gDeviceData._accumulator);
        return true;
    case 0xE2: //MOVX A, @R0
        gDeviceData._programCounter+=1;
        //gDeviceData._dataRam[gDeviceData._registerBank[0]] = (*gDeviceData._accumulator);
        return true;
    case 0xE3: //MOVX A, @R1
        gDeviceData._programCounter+=1;
        //gDeviceData._dataRam[gDeviceData._registerBank[1]] = (*gDeviceData._accumulator);
        return true;
    }

    return false;
}

OpcodeInfo _C8051_opcode[256]=
{
    {"NOP", "", Op_NOP},
    {"AJMP", "addr11", Op_AJMP},
    {"LJMP", "addr16", Op_LJMP},
    {"RR", "A", Op_RR},

    {"INC", "A", Op_INC},
    {"INC", "direct", Op_INC},
    {"INC", "@R0", Op_INC},
    {"INC", "@R1", Op_INC},
    {"INC", "R0", Op_INC},
    {"INC", "R1", Op_INC},
    {"INC", "R2", Op_INC},
    {"INC", "R3", Op_INC},
    {"INC", "R4", Op_INC},
    {"INC", "R5", Op_INC},
    {"INC", "R6", Op_INC},
    {"INC", "R7", Op_INC},

    {"JBC", "bit, offset", Op_JBC},
    {"ACALL", "addr11", Op_ACALL},
    {"LCALL", "addr16", Op_LCALL},
    {"RRC", "A", Op_RRC},

    {"DEC", "A", Op_DEC},
    {"DEC", "direct", Op_DEC},
    {"DEC", "@R0", Op_DEC},
    {"DEC", "@R1", Op_DEC},
    {"DEC", "R0", Op_DEC},
    {"DEC", "R1", Op_DEC},
    {"DEC", "R2", Op_DEC},
    {"DEC", "R3", Op_DEC},
    {"DEC", "R4", Op_DEC},
    {"DEC", "R5", Op_DEC},
    {"DEC", "R6", Op_DEC},
    {"DEC", "R7", Op_DEC},

    {"JB", "bit, offset", Op_JB},
    {"AJMP", "addr11", Op_AJMP},
    {"RET", "", Op_RET},
    {"RL", "A", Op_RL},

    {"ADD", "A, #immed", Op_ADD},
    {"ADD", "A, direct", Op_ADD},
    {"ADD", "A, @R0", Op_ADD},
    {"ADD", "A, @R1", Op_ADD},
    {"ADD", "A, R0", Op_ADD},
    {"ADD", "A, R1", Op_ADD},
    {"ADD", "A, R2", Op_ADD},
    {"ADD", "A, R3", Op_ADD},
    {"ADD", "A, R4", Op_ADD},
    {"ADD", "A, R5", Op_ADD},
    {"ADD", "A, R6", Op_ADD},
    {"ADD", "A, R7", Op_ADD},

    {"JNB", "bit, offset", Op_JNB},
    {"ACALL", "addr11", Op_ACALL},
    {"RETI", "", Op_RETI},
    {"RLC", "A", Op_RLC},

    {"ADDC", "A, #immed", Op_ADDC},
    {"ADDC", "A, direct", Op_ADDC},
    {"ADDC", "A, @R0", Op_ADDC},
    {"ADDC", "A, @R1", Op_ADDC},
    {"ADDC", "A, R0", Op_ADDC},
    {"ADDC", "A, R1", Op_ADDC},
    {"ADDC", "A, R2", Op_ADDC},
    {"ADDC", "A, R3", Op_ADDC},
    {"ADDC", "A, R4", Op_ADDC},
    {"ADDC", "A, R5", Op_ADDC},
    {"ADDC", "A, R6", Op_ADDC},
    {"ADDC", "A, R7", Op_ADDC},

    {"JC", "offset", Op_JC},
    {"AJMP", "addr11", Op_AJMP},

    {"ORL", "direct, A", Op_ORL},
    {"ORL", "direct, #immed", Op_ORL},
    {"ORL", "A, #immed", Op_ORL},
    {"ORL", "A, direct", Op_ORL},
    {"ORL", "A, @R0", Op_ORL},
    {"ORL", "A, @R1", Op_ORL},
    {"ORL", "A, R0", Op_ORL},
    {"ORL", "A, R1", Op_ORL},
    {"ORL", "A, R2", Op_ORL},
    {"ORL", "A, R3", Op_ORL},
    {"ORL", "A, R4", Op_ORL},
    {"ORL", "A, R5", Op_ORL},
    {"ORL", "A, R6", Op_ORL},
    {"ORL", "A, R7", Op_ORL},

    {"JNC", "offset", Op_JNC},
    {"ACALL", "addr11", Op_ACALL},

    {"ANL", "direct, A", Op_ANL},
    {"ANL", "direct, #immed", Op_ANL},
    {"ANL", "A, #immed", Op_ANL},
    {"ANL", "A, direct", Op_ANL},
    {"ANL", "A, @R0", Op_ANL},
    {"ANL", "A, @R1", Op_ANL},
    {"ANL", "A, R0", Op_ANL},
    {"ANL", "A, R1", Op_ANL},
    {"ANL", "A, R2", Op_ANL},
    {"ANL", "A, R3", Op_ANL},
    {"ANL", "A, R4", Op_ANL},
    {"ANL", "A, R5", Op_ANL},
    {"ANL", "A, R6", Op_ANL},
    {"ANL", "A, R7", Op_ANL},

    {"JZ", "offset", Op_JZ},
    {"AJMP", "addr11", Op_AJMP},

    {"XRL", "direct, A", Op_XRL},
    {"XRL", "direct, #immed", Op_XRL},
    {"XRL", "A, #immed", Op_XRL},
    {"XRL", "A, direct", Op_XRL},
    {"XRL", "A, @R0", Op_XRL},
    {"XRL", "A, @R1", Op_XRL},
    {"XRL", "A, R0", Op_XRL},
    {"XRL", "A, R1", Op_XRL},
    {"XRL", "A, R2", Op_XRL},
    {"XRL", "A, R3", Op_XRL},
    {"XRL", "A, R4", Op_XRL},
    {"XRL", "A, R5", Op_XRL},
    {"XRL", "A, R6", Op_XRL},
    {"XRL", "A, R7", Op_XRL},

    {"JNZ", "offset", Op_JNZ},
    {"ACALL", "addr11", Op_ACALL},
    {"ORL", "C, bit", Op_ORL},
    {"JMP", "@A+DPTR", Op_JMP},

    {"MOV", "A, #immed", Op_MOV},
    {"MOV", "direct, #immed", Op_MOV},
    {"MOV", "@R0, #immed", Op_MOV},
    {"MOV", "@R1, #immed", Op_MOV},
    {"MOV", "R0, #immed", Op_MOV},
    {"MOV", "R1, #immed", Op_MOV},
    {"MOV", "R2, #immed", Op_MOV},
    {"MOV", "R3, #immed", Op_MOV},
    {"MOV", "R4, #immed", Op_MOV},
    {"MOV", "R5, #immed", Op_MOV},
    {"MOV", "R6, #immed", Op_MOV},
    {"MOV", "R7, #immed", Op_MOV},

    {"SJMP", "offset", Op_SJMP},
    {"AJMP", "addr11", Op_AJMP},
    {"ANL", "C, bit", Op_ANL},
    {"MOVC", "A, @A+PC", Op_MOVC},
    {"DIV", "AB", Op_DIV},

    {"MOV", "direct, direct", Op_MOV},
    {"MOV", "direct, @R0", Op_MOV},
    {"MOV", "direct, @R1", Op_MOV},
    {"MOV", "direct, R0", Op_MOV},
    {"MOV", "direct, R1", Op_MOV},
    {"MOV", "direct, R2", Op_MOV},
    {"MOV", "direct, R3", Op_MOV},
    {"MOV", "direct, R4", Op_MOV},
    {"MOV", "direct, R5", Op_MOV},
    {"MOV", "direct, R6", Op_MOV},
    {"MOV", "direct, R7", Op_MOV},
    {"MOV", "DPTR, #immed", Op_MOV},

    {"ACALL", "addr11", Op_ACALL},

    {"MOV", "bit, C", Op_MOV},
    {"MOVC", "A, @A+DPTR", Op_MOVC},

    {"SUBB", "A, #immed", Op_SUBB},
    {"SUBB", "A, direct", Op_SUBB},
    {"SUBB", "A, @R0", Op_SUBB},
    {"SUBB", "A, @R1", Op_SUBB},
    {"SUBB", "A, R0", Op_SUBB},
    {"SUBB", "A, R1", Op_SUBB},
    {"SUBB", "A, R2", Op_SUBB},
    {"SUBB", "A, R3", Op_SUBB},
    {"SUBB", "A, R4", Op_SUBB},
    {"SUBB", "A, R5", Op_SUBB},
    {"SUBB", "A, R6", Op_SUBB},
    {"SUBB", "A, R7", Op_SUBB},

    {"ORL", "C, /bit", Op_ORL},
    {"AJMP", "addr11", Op_AJMP},
    {"MOV", "C, bit", Op_MOV},
    {"INC", "DPTR", Op_INC},
    {"MUL", "AB", Op_MUL},

    {"NOP", "", Op_NOP}, //{"reserved", "", nullptr},

    {"MOV", "@R0, direct", Op_MOV},
    {"MOV", "@R1, direct", Op_MOV},
    {"MOV", "R0, direct", Op_MOV},
    {"MOV", "R1, direct", Op_MOV},
    {"MOV", "R2, direct", Op_MOV},
    {"MOV", "R3, direct", Op_MOV},
    {"MOV", "R4, direct", Op_MOV},
    {"MOV", "R5, direct", Op_MOV},
    {"MOV", "R6, direct", Op_MOV},
    {"MOV", "R7, direct", Op_MOV},

    {"ANL", "C, /bit", Op_ANL},
    {"ACALL", "addr11", Op_ACALL},
    {"CPL", "bit", Op_CPL},
    {"CPL", "C", Op_CPL},

    {"CJNE", "A, #immed, offset", Op_CJNE},
    {"CJNE", "A, direct, offset", Op_CJNE},
    {"CJNE", "@R0, #immed, offset", Op_CJNE},
    {"CJNE", "@R1, #immed, offset", Op_CJNE},
    {"CJNE", "R0, #immed, offset", Op_CJNE},
    {"CJNE", "R1, #immed, offset", Op_CJNE},
    {"CJNE", "R2, #immed, offset", Op_CJNE},
    {"CJNE", "R3, #immed, offset", Op_CJNE},
    {"CJNE", "R4, #immed, offset", Op_CJNE},
    {"CJNE", "R5, #immed, offset", Op_CJNE},
    {"CJNE", "R6, #immed, offset", Op_CJNE},
    {"CJNE", "R7, #immed, offset", Op_CJNE},

    {"PUSH", "direct", Op_PUSH},
    {"AJMP", "addr11", Op_AJMP},
    {"CLR", "bit", Op_CLR},
    {"CLR", "C", Op_CLR},
    {"SWAP", "A", Op_SWAP},

    {"XCH", "A, direct", Op_XCH},
    {"XCH", "A, @R0", Op_XCH},
    {"XCH", "A, @R1", Op_XCH},
    {"XCH", "A, R0", Op_XCH},
    {"XCH", "A, R1", Op_XCH},
    {"XCH", "A, R2", Op_XCH},
    {"XCH", "A, R3", Op_XCH},
    {"XCH", "A, R4", Op_XCH},
    {"XCH", "A, R5", Op_XCH},
    {"XCH", "A, R6", Op_XCH},
    {"XCH", "A, R7", Op_XCH},

    {"POP", "direct", Op_POP},
    {"ACALL", "addr11", Op_ACALL},
    {"SETB", "bit", Op_SETB},
    {"SETB", "C", Op_SETB},
    {"DA", "A", Op_DA},
    {"DJNZ", "direct, offset", Op_DJNZ},
    {"XCHD", "A, @R0", Op_XCHD},
    {"XCHD", "A, @R1", Op_XCHD},

    {"DJNZ", "R0, offset", Op_DJNZ},
    {"DJNZ", "R1, offset", Op_DJNZ},
    {"DJNZ", "R2, offset", Op_DJNZ},
    {"DJNZ", "R3, offset", Op_DJNZ},
    {"DJNZ", "R4, offset", Op_DJNZ},
    {"DJNZ", "R5, offset", Op_DJNZ},
    {"DJNZ", "R6, offset", Op_DJNZ},
    {"DJNZ", "R7, offset", Op_DJNZ},

    {"MOVX", "A, @DPTR", Op_MOVX},
    {"AJMP", "addr11", Op_AJMP},
    {"MOVX", "A, @R0", Op_MOVX},
    {"MOVX", "A, @R1", Op_MOVX},
    {"CLR", "A", Op_CLR},

    {"MOV", "A, direct", Op_MOV},
    {"MOV", "A, @R0", Op_MOV},
    {"MOV", "A, @R1", Op_MOV},
    {"MOV", "A, R0", Op_MOV},
    {"MOV", "A, R1", Op_MOV},
    {"MOV", "A, R2", Op_MOV},
    {"MOV", "A, R3", Op_MOV},
    {"MOV", "A, R4", Op_MOV},
    {"MOV", "A, R5", Op_MOV},
    {"MOV", "A, R6", Op_MOV},
    {"MOV", "A, R7", Op_MOV},

    {"MOVX", "#DPTR, A", Op_MOVX},
    {"ACALL", "addr11", Op_ACALL},
    {"MOVX", "@R0, A", Op_MOVX},
    {"MOVX", "@R1, A", Op_MOVX},
    {"CPL", "A", Op_CPL},

    {"MOV", "direct, A", Op_MOV},
    {"MOV", "@R0, A", Op_MOV},
    {"MOV", "@R1, A", Op_MOV},
    {"MOV", "R0, A", Op_MOV},
    {"MOV", "R1, A", Op_MOV},
    {"MOV", "R2, A", Op_MOV},
    {"MOV", "R3, A", Op_MOV},
    {"MOV", "R4, A", Op_MOV},
    {"MOV", "R5, A", Op_MOV},
    {"MOV", "R6, A", Op_MOV},
    {"MOV", "R7, A", Op_MOV}
};

}//end cmcs::core::opcode
