#ifndef _CMCS_API_HPP_INCLUDED
#define _CMCS_API_HPP_INCLUDED

#include "C_shareableData.hpp"
#include "simul_device.hpp"

#define CMCS_API __declspec(dllexport)

#define CMCS_EXT_PRINT_FORMAT(extensionName_, tag_, str_) "\x1b[1;33m[" extensionName_":" tag_"]\x1b[0m " str_

#define CMCS_MAIN_FONT_PATH "data/fonts/arial.ttf"
#define CMCS_MAIN_FONT "default"
#define CMCS_CORE_VERSION "V0.3"
#define CMCS_CORE_MAXPORTS 11

struct ExtensionIdentity
{
    std::string _name;
    std::string _version;
    std::string _author;
};

using FunctionInit = void(*)(cmcs::ShareableData&);
using FunctionRetrieveExtensionIdentity = ExtensionIdentity(*)();
using FunctionUninit = void(*)();
using FunctionUpdate = bool(*)();

#endif // _CMCS_API_HPP_INCLUDED
