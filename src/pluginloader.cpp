#include <string>
#include <cstdlib>
#include "pluginloader.hpp"
#include "plugins/include/plugininterface.hpp"
#include <libc.h>
#include <filesystem>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <dlfcn.h>
#endif

bool PluginLoader::LoadPlugin(const std::string& pluginName) {
    std::string pluginRelPath = "./plugins/" + pluginName;
    //run makefile and error on fail
    std::system((std::string("make -C ") + pluginRelPath + " all").c_str());

    //link the dll and call the constructor from plugininterface
    
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
        plugin.name = pluginName;
        plugin.sequence->Initialize({200,200});
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
    std::cout << "non mac not yet supported" << std::endl;
    return false;
}
bool PluginLoader::ReloadPlugin(const std::string& pluginName) {
    for (LoadedPlugin plugin : plugins) {
        if (plugin.name == pluginName) {
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