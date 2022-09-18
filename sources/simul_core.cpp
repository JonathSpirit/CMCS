#include "simul_core.hpp"
#include "simul_main.hpp"
#include "simul_ext.hpp"
#include "simul_math.hpp"
#include "simul_C8051_opcode.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <thread>

namespace simul
{
namespace core
{

device::C8051_data gDeviceData;

/*unsigned short HexToUShort(const char* msb_char)
{
    unsigned short result = 0x0000;
    for (unsigned int i=0; i<4; ++i)
    {
        result |= (msb_char[i]>='0') ? ((msb_char[i]>='A') ? static_cast<unsigned short>(msb_char[i]-55)<<(12-4*i) : static_cast<unsigned short>(msb_char[i]-'0')<<(12-4*i)) : 0x0000;
    }
    return result;
}
unsigned char HexToUChar(const char* msb_char)
{
    unsigned char result = 0x00;
    for (unsigned int i=0; i<2; ++i)
    {
        result |= (msb_char[i]>='0') ? ((msb_char[i]>='A') ? static_cast<unsigned char>(msb_char[i]-55)<<(4-4*i) : static_cast<unsigned char>(msb_char[i]-'0')<<(4-4*i)) : 0x00;
    }
    return result;
}*/

bool LoadDevice(const std::string& pathfile)
{
    /*
    std::string gDeviceData._device;
    std::vector<unsigned char> gDeviceData._dataFlash;
    std::vector<unsigned char> gDeviceData._dataRam;
    simul::BitBank<unsigned char> gDeviceData._dataBit;
    std::vector<unsigned char> gDeviceData._dataXRam;
    unsigned short gDeviceData._programCounter; //PC
    unsigned short gDeviceData._cursor;

    unsigned char* gDeviceData._stackPointer; //SP
    unsigned short* gDeviceData._dptr; //DPTR
    unsigned char* gDeviceData._registerBank; //Rn
    unsigned char* gDeviceData._accumulator; //A
    unsigned char* gDeviceData._b; //B
    simul::Bit<unsigned char> gDeviceData._psw; //PSW
    */
    gDeviceData._dataRam.resize(256);
    gDeviceData._dataFlash.resize(16000);
    gDeviceData._dataXRam.resize(4352);

    gDeviceData._dataBit.clear();
    gDeviceData._dataBit.reserve(0x2F-0x20 + 16);
    unsigned int a = 0;
    for (unsigned int i=0x20; i<=0x2F; ++i)
    {
        gDeviceData._dataBit.addData(&gDeviceData._dataRam[i]);
        ++a;
    }
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0x80]); //P0
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0x88]); //TCON
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0x90]); //P1
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0x98]); //SCON0
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xA0]); //P2
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xA8]); //IE
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xB0]); //SPI1CN
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xB8]); //IP
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xC0]); //SMB0CN
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xC8]); //TMR2CN
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xD0]); //PSW
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xD8]); //PCA0CN
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xE0]); //ACC
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xE8]); //ADC0CN
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xF0]); //B
    gDeviceData._dataBit.addData(&gDeviceData._dataRam[0xF8]); //SPI0CN

    gDeviceData._stackPointer = &gDeviceData._dataRam[0x81];
    *gDeviceData._stackPointer = 0x4F;
    gDeviceData._dptr = reinterpret_cast<unsigned short*>(&gDeviceData._dataRam[0x82]); //A voir
    gDeviceData._registerBank = &gDeviceData._dataRam[0];
    gDeviceData._accumulator = &gDeviceData._dataRam[0xE0];
    gDeviceData._b = &gDeviceData._dataRam[0xF0];
    gDeviceData._psw.setData(&gDeviceData._dataRam[0xD0]);

    gDeviceData._programCounter = 0;
    gDeviceData._cursor = 0;

    gDeviceData._device = "F38TEST";
    return true;
}

bool WriteCodeAsm(const std::string& pathfile)
{
    std::ofstream hexFile(pathfile, std::ifstream::out);
    if (!hexFile)
    {
        return false;
    }

    for (unsigned int i=0; i<gDeviceData._dataFlash.size(); ++i)
    {
        hexFile << i << " 0x" << std::hex << (unsigned short)gDeviceData._dataFlash[i] << " ";
        hexFile << std::dec << (unsigned short)gDeviceData._dataFlash[i] << " : " << simul::core::opcode::_C8051_opcode[gDeviceData._dataFlash[i]]._opcode << " " << simul::core::opcode::_C8051_opcode[gDeviceData._dataFlash[i]]._operands << std::endl;
    }

    hexFile.close();
    return true;
}
bool LoadCodeHex(const std::string& pathfile)
{
    std::ifstream hexFile(pathfile, std::ifstream::in);
    if (!hexFile)
    {
        return false;
    }

    unsigned int global_size = 0;
    std::string line;
    while ( std::getline(hexFile, line) )
    {
        if ( line.length() < 11 )
        {//Bad format
            hexFile.close();
            return false;
        }
        if ( line[0] != ':' )
        {//Bad format
            hexFile.close();
            return false;
        }

        unsigned int cursor = 1;
        unsigned char dataSize = simul::math::HexToU8(&line[cursor]);
        cursor+=2;
        unsigned short address = simul::math::HexToU16(&line[cursor]);
        cursor+=4;
        unsigned char recordType = simul::math::HexToU8(&line[cursor]);
        cursor+=2;

        global_size += dataSize;

        switch ( recordType )
        {
        case simul::core::RecordType::RECORDTYPE_DATA:
            if ( cursor+2+dataSize*2 != line.size() )
            {//Bad size
                hexFile.close();
                return false;
            }
            for (unsigned int i=0; i<dataSize; ++i)
            {
                unsigned char data = simul::math::HexToU8(&line[cursor]);
                cursor+=2;
                if ( address >= gDeviceData._dataFlash.size() )
                {//Out of bound
                    hexFile.close();
                    return false;
                }
                gDeviceData._dataFlash[address] = data;
                ++address;
            }
            break;
        case simul::core::RecordType::RECORDTYPE_END_OF_FILE:
            hexFile.close();
            return true;
            break;
        case simul::core::RecordType::RECORDTYPE_EXTENDED_SEGMENT_ADDRESS:
            break;
        case simul::core::RecordType::RECORDTYPE_EXTENDED_LINEAR_ADDRESS:
            break;
        case simul::core::RecordType::RECORDTYPE_START_LINEAR_ADDRESS:
            break;
        default:
            break;
        }
    }

    //bad format
    hexFile.close();
    return false;
}
bool LoadCodeBin(const std::string& pathfile)
{
    std::ifstream binFile(pathfile, std::ifstream::in | std::ifstream::binary);
    if (!binFile)
    {
        return false;
    }

    unsigned int address = 0;
    int data;
    data = binFile.get();
    while ( data >= 0 )
    {
        if ( address >= gDeviceData._dataFlash.size() )
        {//Out of bound
            binFile.close();
            return false;
        }
        gDeviceData._dataFlash[address++] = data;

        data = binFile.get();
    }

    binFile.close();
    return true;
}

bool _threadRunStepCode = false;
void ThreadRunStepCode()
{
    while (_threadRunStepCode)
    {
        ExecuteStepCode(false);
        std::this_thread::sleep_for(std::chrono::milliseconds{20});
    }
}

bool ExecuteStepCode(bool print)
{
    try
    {
        if (print)
        {
            gTerminalPtr->print(S_PRINT_FORMAT("info", "Executing : %s %s\n"),
                                simul::core::opcode::_C8051_opcode[gDeviceData._dataFlash[gDeviceData._cursor]]._opcode.c_str(),
                                simul::core::opcode::_C8051_opcode[gDeviceData._dataFlash[gDeviceData._cursor]]._operands.c_str());
        }

        simul::core::opcode::_C8051_opcode[gDeviceData._dataFlash[gDeviceData._cursor]]._func();
        gDeviceData._cursor = gDeviceData._programCounter;

        if (print)
        {
            gTerminalPtr->print(S_PRINT_FORMAT("info", "Cursor : %u\n"), gDeviceData._cursor);
        }
    }
    catch (std::exception& e)
    {
        gTerminalPtr->print(S_PRINT_FORMAT("error", "An error has just occurred, Cursor : %u\n"), gDeviceData._cursor);
        gTerminalPtr->print(S_PRINT_FORMAT("error", "%s\n"), e.what());
        return false;
    }

    return true;
}

}//end core
}//end simul
