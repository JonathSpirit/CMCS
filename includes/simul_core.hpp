#ifndef _CMCS_SIMUL_CORE_HPP_INCLUDED
#define _CMCS_SIMUL_CORE_HPP_INCLUDED

#include "simul_device.hpp"
#include <string>
#include <filesystem>

#define CMCS_CORE_HEX_RECORD_MINIMUM_LENGTH 11

namespace cmcs::core
{

enum IntelHexRecordType : uint8_t
{
    RT_DATA = 0x00,
    RT_END_OF_FILE = 0x01,
    RT_EXTENDED_SEGMENT_ADDRESS = 0x02,
    RT_START_SEGMENT_ADDRESS = 0x03,
    RT_EXTENDED_LINEAR_ADDRESS = 0x04,
    RT_START_LINEAR_ADDRESS = 0x05,
};

enum class FileWriteStats
{
    FW_OK,
    FW_ERROR,
    FW_ALREADY_EXIST
};

extern device::C8051_data gDeviceData;

bool ExecuteStepCode(bool print);

bool LoadCodeHex(const std::filesystem::path& filePath);
bool LoadCodeBin(const std::filesystem::path& filePath);
bool LoadDevice(const std::string& filePath);

FileWriteStats WriteCodeAsm(const std::filesystem::path& filePath, bool replace);

extern bool _threadRunStepCode;
void ThreadRunStepCode();

}//end cmcs::core

#endif // _CMCS_SIMUL_CORE_HPP_INCLUDED
