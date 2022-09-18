#include "simul_core.hpp"
#include "simul_main.hpp"
#include "simul_ext.hpp"
#include "simul_math.hpp"
#include "simul_C8051_opcode.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <thread>

namespace cmcs::core
{

device::C8051_data gDeviceData;

bool LoadDevice(const std::string& filePath)
{
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

FileWriteStats WriteCodeAsm(const std::filesystem::path& filePath, bool replace)
{
    if (!replace)
    {
        if ( std::filesystem::exists(filePath) && std::filesystem::status(filePath).type() == std::filesystem::file_type::regular )
        {
            return FileWriteStats::FW_ALREADY_EXIST;
        }
    }

    std::ofstream hexFile(filePath);
    if (!hexFile)
    {
        return FileWriteStats::FW_ERROR;
    }

    for (std::size_t i=0; i<gDeviceData._dataFlash.size(); ++i)
    {
        hexFile << i << " 0x" << std::hex << static_cast<unsigned int>(gDeviceData._dataFlash[i]) << " ";
        hexFile << std::dec << static_cast<unsigned int>(gDeviceData._dataFlash[i]) << " : " << cmcs::core::opcode::_C8051_opcode[gDeviceData._dataFlash[i]]._opcode << " " << cmcs::core::opcode::_C8051_opcode[gDeviceData._dataFlash[i]]._operands << std::endl;
    }

    hexFile.close();
    return FileWriteStats::FW_OK;
}
bool LoadCodeHex(const std::filesystem::path& filePath)
{///https://en.wikipedia.org/wiki/Intel_HEX
    std::ifstream hexFile(filePath);
    if (!hexFile)
    {
        return false;
    }

    std::size_t globalSize = 0;
    std::string line;
    while ( std::getline(hexFile, line) )
    {
        if ( line.length() < CMCS_CORE_HEX_RECORD_MINIMUM_LENGTH )
        {//The record is too small
            hexFile.close();
            return false;
        }
        if ( line[0] != ':' )
        {//Bad format
            hexFile.close();
            return false;
        }

        //Computing checksum
        uint8_t checksum = 0x00;
        for (std::size_t i=1; i<line.size()-2; i+=2)
        {
            checksum += cmcs::math::HexToU8(line.data()+i);
        }
        checksum = ~checksum+1;
        if (checksum != cmcs::math::HexToU8(line.data()+line.size()-2))
        {//Bad checksum
            hexFile.close();
            return false;
        }

        std::size_t cursor = 1;

        uint8_t byteCount = cmcs::math::HexToU8(line.data()+cursor);
        cursor+=2;

        uint16_t address = cmcs::math::HexToU16(line.data()+cursor);
        cursor+=4;

        auto recordType = static_cast<IntelHexRecordType>(cmcs::math::HexToU8(line.data()+cursor));
        cursor+=2;

        globalSize += byteCount;

        switch ( recordType )
        {
        case IntelHexRecordType::RT_DATA:
            if ( cursor+2+byteCount*2 != line.size() )
            {//Bad size
                hexFile.close();
                return false;
            }
            for (uint8_t i=0; i<byteCount; ++i)
            {
                uint8_t data = cmcs::math::HexToU8(line.data()+cursor);
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
        case IntelHexRecordType::RT_END_OF_FILE:
            hexFile.close();
            return true;
        case IntelHexRecordType::RT_EXTENDED_SEGMENT_ADDRESS:
        case IntelHexRecordType::RT_EXTENDED_LINEAR_ADDRESS:
        case IntelHexRecordType::RT_START_SEGMENT_ADDRESS:
        case IntelHexRecordType::RT_START_LINEAR_ADDRESS:
        default:
            //Unimplemented
            hexFile.close();
            return false;
        }
    }

    //No end of file
    hexFile.close();
    return false;
}
bool LoadCodeBin(const std::filesystem::path& filePath)
{
    std::ifstream binFile(filePath, std::ifstream::in | std::ifstream::binary);
    if (!binFile)
    {
        return false;
    }

    std::size_t address = 0;
    int data;
    data = binFile.get();
    while (data != std::ifstream::traits_type::eof())
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
        std::this_thread::sleep_for(std::chrono::milliseconds{10});
    }
}

bool ExecuteStepCode(bool print)
{
    try
    {
        if (print)
        {
            gTerminalPtr->print(S_PRINT_FORMAT("info", "Executing : %s %s\n"),
                                cmcs::core::opcode::_C8051_opcode[gDeviceData._dataFlash[gDeviceData._cursor]]._opcode.c_str(),
                                cmcs::core::opcode::_C8051_opcode[gDeviceData._dataFlash[gDeviceData._cursor]]._operands.c_str());
        }

        cmcs::core::opcode::_C8051_opcode[gDeviceData._dataFlash[gDeviceData._cursor]]._func();
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

}//end cmcs::core
