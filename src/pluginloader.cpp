#include <string>
#include <cstdlib>
#include "pluginloader.hpp"


bool PluginLoader::LoadPlugin(const std::string& pluginRelPath) {
    //run makefile and error on fail
    std::system(("make -C " + pluginRelPath + " all").c_str());
    //add 
}
bool PluginLoader::ReloadPlugin(const std::string& pluginRelPath) {

}
bool PluginLoader::PluginLoader::UnloadPlugin() {

}
void UnloadAll() {

}