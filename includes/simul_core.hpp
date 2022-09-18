#ifndef _CMCS_SIMUL_CORE_HPP_INCLUDED
#define _CMCS_SIMUL_CORE_HPP_INCLUDED

#define KMCS_MAIN_FONT_PATH "data/fonts/arial.ttf"
#define KMCS_MAIN_FONT "default"
#define KMCS_CORE_VERSION "V0.3"
#define KMCS_CORE_MAXPORTS 11

#include "simul_device.hpp"
#include <string>

namespace cmcs::core
{

enum RecordType : uint8_t
{
    RECORDTYPE_DATA = 0,
    RECORDTYPE_END_OF_FILE,
    RECORDTYPE_EXTENDED_SEGMENT_ADDRESS,
    RECORDTYPE_EXTENDED_LINEAR_ADDRESS,
    RECORDTYPE_START_LINEAR_ADDRESS,
};

extern device::C8051_data gDeviceData;

bool ExecuteStepCode(bool print);

bool LoadCodeHex(const std::string& pathfile);
bool LoadCodeBin(const std::string& pathfile);
bool LoadDevice(const std::string& pathfile);

bool WriteCodeAsm(const std::string& pathfile);

extern bool _threadRunStepCode;
void ThreadRunStepCode();

}//end cmcs::core

#endif // _CMCS_SIMUL_CORE_HPP_INCLUDED
