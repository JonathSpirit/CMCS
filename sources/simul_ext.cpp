#include "simul_ext.hpp"
#include "simul_core.hpp"
#include "simul_main.hpp"

#include <unordered_map>
#include <SDL_loadso.h>

namespace cmcs::ext
{

namespace
{

std::unordered_map<std::string, cmcs::ext::Extension> gExtensions;

}//end

bool LoadExtension(const std::string& filePath, ShareableData& shareableData)
{
    cmcs::ext::Extension extension;
    extension._object = SDL_LoadObject(filePath.c_str());

    if (extension._object == nullptr)
    {
        gTerminalPtr->print(S_PRINT_FORMAT("error", "Can't load the extension : %s\n"), SDL_GetError());
        return false;
    }

    gTerminalPtr->print(S_PRINT_FORMAT("info", "Loading extension at %s ...\n"), filePath.c_str());

    extension._funcRetrieveExtensionIdentity = reinterpret_cast<FunctionRetrieveExtensionIdentity>(SDL_LoadFunction(extension._object, CMCS_EXT_FUNCDEF_RETRIEVE_EXTENSION_IDENTITY));
    if (extension._funcRetrieveExtensionIdentity == nullptr)
    {
        gTerminalPtr->print(S_PRINT_FORMAT("error", "Extension appear to not have a valid \"retrieve extension identity\" function !\n"));
        SDL_UnloadObject(extension._object);
        return false;
    }

    extension._identity = extension._funcRetrieveExtensionIdentity();

    if (extension._identity._name.empty() || extension._identity._author.empty() || extension._identity._version.empty())
    {
        gTerminalPtr->print(S_PRINT_FORMAT("error", "Extension appear to not have a valid identity !\n"));
        SDL_UnloadObject(extension._object);
        return false;
    }

    if (gExtensions.find(extension._identity._name) != gExtensions.end())
    {
        gTerminalPtr->print(S_PRINT_FORMAT("error", "An extension with the same name is already loaded ! (%s)\n"), extension._identity._name.c_str());
        SDL_UnloadObject(extension._object);
        return false;
    }

    extension._funcInit = reinterpret_cast<FunctionInit>(SDL_LoadFunction(extension._object, CMCS_EXT_FUNCDEF_INIT));
    extension._funcUninit = reinterpret_cast<FunctionUninit>(SDL_LoadFunction(extension._object, CMCS_EXT_FUNCDEF_UNINIT));
    extension._funcUpdate = reinterpret_cast<FunctionUpdate>(SDL_LoadFunction(extension._object, CMCS_EXT_FUNCDEF_UPDATE));

    if (extension._funcInit == nullptr || extension._funcUninit == nullptr || extension._funcUpdate == nullptr)
    {
        gTerminalPtr->print(S_PRINT_FORMAT("error", "Extension appear to not have required exported functions !\n"));
        SDL_UnloadObject(extension._object);
        return false;
    }

    gTerminalPtr->print(S_PRINT_FORMAT("info", "Extension identity :\n"));
    gTerminalPtr->print(S_PRINT_FORMAT("info", "\tName : %s\n"), extension._identity._name.c_str());
    gTerminalPtr->print(S_PRINT_FORMAT("info", "\tVersion : %s\n"), extension._identity._version.c_str());
    gTerminalPtr->print(S_PRINT_FORMAT("info", "\tAuthor : %s\n"), extension._identity._author.c_str());

    extension._funcInit(shareableData);

    gExtensions[extension._identity._name] = extension;

    return true;
}

void UpdateAll()
{
    for (auto& extension : gExtensions)
    {
        extension.second._funcUpdate();
    }
}

void FreeAll()
{
    for (auto& extension : gExtensions)
    {
        extension.second._funcUninit();
        SDL_UnloadObject(extension.second._object);
    }

    gExtensions.clear();
}

bool FreeExtension(const std::string& name)
{
    auto it = gExtensions.find(name);

    if (it != gExtensions.end())
    {
        (*it).second._funcUninit();
        SDL_UnloadObject((*it).second._object);

        gExtensions.erase(it);
        return true;
    }
    return false;
}

}//end cmcs::ext
