#pragma once
#include <string>
#include "plugins/sequence.hpp"
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
        bool LoadPlugin(const std::string& pluginRelPath);
        bool ReloadPlugin(const std::string& pluginRelPath);
        bool UnloadPlugin();
        void UnloadAll();
};