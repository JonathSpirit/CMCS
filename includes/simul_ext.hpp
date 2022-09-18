#ifndef _CMCS_SIMUL_EXT_HPP_INCLUDED
#define _CMCS_SIMUL_EXT_HPP_INCLUDED

#include <string>
#include "CMCS_API.hpp"

#define CMCS_EXT_FUNCDEF_INIT "_Z12CMCSlib_InitRN4cmcs13ShareableDataE"
#define CMCS_EXT_FUNCDEF_UNINIT "_Z14CMCSlib_Uninitv"
#define CMCS_EXT_FUNCDEF_UPDATE "_Z14CMCSlib_Updatev"
#define CMCS_EXT_FUNCDEF_RETRIEVE_EXTENSION_IDENTITY "_Z33CMCSlib_retrieveExtensionIdentityv"

namespace cmcs::ext
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

}//end cmcs::ext

#endif // _CMCS_SIMUL_EXT_HPP_INCLUDED
