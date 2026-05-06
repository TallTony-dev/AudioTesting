#include <string>
#include <cstdlib>
#include "pluginloader.hpp"
#include "plugins/include/plugininterface.hpp"
#include <filesystem>
#include <iostream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#endif
#ifdef __APPLE__
#include <dlfcn.h>
#endif

bool PluginLoader::LoadPlugin(const std::string& pluginName, bool clean) {
    std::string pluginRelPath = "./plugins/" + pluginName;
    std::string makeTarget = "all";
    //run makefile and error on fail
    #ifdef _WIN32
    makeTarget = "all";
    #endif
    if (clean) {
        std::system((std::string("make -C ") + pluginRelPath + " clean").c_str());
    }
    std::system((std::string("make -C ") + pluginRelPath + " " + makeTarget).c_str());

    //link the dll and call the constructor from plugininterface
    // Heavy LLM use in this area
    #ifdef __APPLE__
    //load dylib here
    void *handle;
    std::cout << "Loading dylib from path " + (pluginRelPath + "/bin/" + pluginName + ".dylib" + "\n");
    if ((handle = dlopen((pluginRelPath + "/bin/" + pluginName + ".dylib").c_str(), RTLD_NOW)) != NULL) {
        std::cout << "Successfully loaded plugin from " << pluginRelPath << std::endl;
        auto create = (CreateSequenceFn)dlsym(handle, "CreateSequence");
        auto destroy = (DestroySequenceFn)dlsym(handle, "DestroySequence");
        if (!create || !destroy) {
            std::cout << "Plugin missing CreateSequence/DestroySequence at path " << pluginRelPath << std::endl;
            dlclose(handle);
            return false;
        }
        LoadedPlugin plugin;
        plugin.handle = handle;
        plugin.destroy = destroy;
        plugin.sequence = create();
        plugin.sequence->name = pluginName;
        plugin.sequence->Initialize({250,250});
        plugins.push_back(plugin);
    }
    else {
        std::cout << "Failed to load dylib from " << pluginRelPath << std::endl;
        const char* err = dlerror();
        std::cout << "Errors: " << err << std::endl;
        return false;
    }
    return true;
    #endif
    #ifdef _WIN32
    void *handle;
    std::string dllPath = pluginRelPath + "/bin/" + pluginName + ".dll";
    std::cout << "Loading dll from path " + dllPath + "\n";
    if ((handle = (void*)LoadLibraryA(dllPath.c_str())) != NULL) {
        std::cout << "Successfully loaded plugin from " << pluginRelPath << std::endl;
        auto create = (CreateSequenceFn)GetProcAddress((HMODULE)handle, "CreateSequence");
        auto destroy = (DestroySequenceFn)GetProcAddress((HMODULE)handle, "DestroySequence");
        if (!create || !destroy) {
            std::cout << "Plugin missing CreateSequence/DestroySequence at path " << pluginRelPath << std::endl;
            FreeLibrary((HMODULE)handle);
            return false;
        }
        LoadedPlugin plugin;
        plugin.handle = handle;
        plugin.destroy = destroy;
        plugin.sequence = create();
        plugin.sequence->name = pluginName;
        plugin.sequence->Initialize({250,250});
        plugins.push_back(plugin);
        return true;
    }
    else {
        std::cout << "Failed to load dll from " << pluginRelPath << " with error code: " << GetLastError() << std::endl;
        return false;
    }
    #endif
    std::cout << "non mac/windows not yet supported" << std::endl;
    return false;
}
bool PluginLoader::ReloadPlugin(const std::string& pluginName) {
    for (LoadedPlugin plugin : plugins) {
        if (plugin.sequence->name == pluginName) {
            plugin.destroy(plugin.sequence);
            return true;
        }
    }
    //call make clean here
    return false;
}
bool PluginLoader::UnloadPlugin() {
    return false;
}
void PluginLoader::UnloadAll() {

}