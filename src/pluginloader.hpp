#pragma once
#include <string>
#include "plugins/include/sequence.hpp"
#include "plugins/include/plugininterface.hpp"
 
//mild llm usage
struct LoadedPlugin {
    void* handle;
    Sequence* sequence;
    DestroySequenceFn destroy;
};
class PluginLoader {
    public:
        std::vector<LoadedPlugin> plugins;
        bool LoadPlugin(const std::string& pluginName);
        bool ReloadPlugin(const std::string& pluginName);
        bool UnloadPlugin();
        void UnloadAll();
};