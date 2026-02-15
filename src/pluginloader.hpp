#pragma once
#include <string>
#include "sequence.hpp"
#include "plugins/plugininterface.hpp"
 
//mild llm usage
struct LoadedPlugin {
    std::string name;
    std::string path; //path to .dylib
    void* handle;
    Sequence* sequence;
    DestroySequenceFn destroy;
};
class PluginLoader {
    public:
        std::vector<LoadedPlugin> plugins;
        bool LoadPlugin(const std::string& pluginDir);
        bool ReloadPlugin(const std::string& pluginDir);
        bool UnloadPlugin();
        void UnloadAll();
};