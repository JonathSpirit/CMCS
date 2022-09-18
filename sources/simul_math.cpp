#include "simul_math.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <algorithm>

namespace simul::math
{

bool GetParity(uint32_t n)
{
    uint8_t counter=0;
    while (n)
    {
        counter += n&0x01;
        n>>=1;
    }
    return counter%2;
}

bool WillOverflow(uint8_t a, uint8_t b, bool operationAddition)
{
    if (operationAddition)
    {//ADD
        uint16_t result = static_cast<uint16_t>(a) + b;
        return result > std::numeric_limits<uint8_t>::max();
    }
    //DEC
    uint16_t result = static_cast<uint16_t>(a) - b;
    return result > std::numeric_limits<uint8_t>::max();
}
bool GetCarry(uint8_t a, bool operationAddition)
{
    return operationAddition ? (a==0xFF) : (a==0x00);
}
bool GetBCDCarry(uint8_t a, bool operationAddition)
{
    return operationAddition ? (a==9) : (a==0);
}

std::optional<uint64_t> HexToInteger(const char* msbChar, std::size_t hexSize)
{
    if (hexSize == 0 || hexSize > 16 || msbChar == nullptr)
    {
        return std::nullopt;
    }

    uint64_t result = 0;

    for (std::size_t i=0; i<hexSize; ++i)
    {
        char c = msbChar[i];

        if (c >= '0' && c <= '9')
        {
            result |= static_cast<uint64_t>(c-'0')<<(4*(hexSize-1)-4*i);
        }
        else if (c >= 'A' && c <= 'F')
        {
            result |= static_cast<uint64_t>(c-'A'+10)<<(4*(hexSize-1)-4*i);
        }
        else if (c >= 'a' && c <= 'f')
        {
            result |= static_cast<uint64_t>(c-'a'+10)<<(4*(hexSize-1)-4*i);
        }
        else
        {
            return std::nullopt;
        }
    }
    return result;
}
uint16_t HexToU16(const char* msbChar)
{
    return HexToInteger(msbChar, 4).value_or(0);
}
uint8_t HexToU8(const char* msbChar)
{
    return HexToInteger(msbChar, 2).value_or(0);
}
uint16_t GetNumFromString(const std::string& str)
{
    if (str.empty())
    {
        return 0;
    }

    //Hex
    if (str.size() > 2)
    {
        if (str[0]=='0' && str[1]=='x')
        {
            std::size_t hexSize = str.size()-2;
            if (hexSize > 4)
            {
                return 0;
            }
            return HexToInteger(str.c_str()+2, hexSize).value_or(0);
        }
    }

    //Decimal
    try
    {
        return std::stoul(str);
    }
    catch(std::exception& e)
    {
        return 0;
    }
}

std::size_t Split(const std::string& str, std::vector<std::string>& output, char delimiter)
{
    output.clear();
    std::istringstream tokenStream{str};
    std::string token;

    while ( std::getline(tokenStream, token, delimiter) )
    {
        output.push_back(std::move(token));
    }
    return output.size();
}

}//end simul::math

