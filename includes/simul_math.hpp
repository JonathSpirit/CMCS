#ifndef _CMCS_SIMUL_MATH_HPP_INCLUDED
#define _CMCS_SIMUL_MATH_HPP_INCLUDED

#include <string>
#include <vector>
#include <optional>

namespace cmcs::math
{

bool GetParity(uint32_t n);
bool WillOverflow(uint8_t a, uint8_t b, bool operationAddition);
bool GetCarry(uint8_t a, bool operationAddition);
bool GetBCDCarry(uint8_t a, bool operationAddition);

std::optional<uint64_t> HexToInteger(const char* msbChar, std::size_t hexSize);
uint16_t HexToU16(const char* msbChar);
uint8_t HexToU8(const char* msbChar);

uint16_t GetNumFromString(const std::string& str);

std::size_t Split(const std::string& str, std::vector<std::string>& output, char delimiter=' ');

}//end cmcs::math

#endif // _CMCS_SIMUL_MATH_HPP_INCLUDED
