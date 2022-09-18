#ifndef _SIMUL_EXT_HPP_INCLUDED
#define _SIMUL_EXT_HPP_INCLUDED

#include <string>
#include "CMCS_API.hpp"

namespace simul::ext
{

struct Extension
{
    ExtensionIdentity _identity;
    void* _object{nullptr};

    FunctionInit _funcInit{nullptr};
    FunctionRetrieveExtensionIdentity _funcRetrieveExtensionIdentity{nullptr};
    FunctionUninit _funcUninit{nullptr};
    FunctionUpdate _funcUpdate{nullptr};
};

bool LoadExtension(const std::string& filePath, ShareableData& shareableData);
void UpdateAll();
void FreeAll();
bool FreeExtension(const std::string& name);

}//end simul::ext

#endif // _SIMUL_EXT_HPP_INCLUDED
