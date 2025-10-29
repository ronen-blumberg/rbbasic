#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>

class RBConfig {
private:
    std::map<std::string, std::map<std::string, std::string>> data;
    std::string config_dir;
    
    std::string trim(const std::string& str);
    std::string to_absolute_path(const std::string& path);
    
public:
    RBConfig();
    bool load(const std::string& filename);
    bool save(const std::string& filename);
    
    std::string get(const std::string& section, const std::string& key, const std::string& default_value = "");
    bool get_bool(const std::string& section, const std::string& key, bool default_value = false);
    void set(const std::string& section, const std::string& key, const std::string& value);
    
    std::string get_compiler_root();
    std::string get_mingw_path();
    std::string get_gcc_path();
    std::string get_gpp_path();
    std::string get_sdl2_include_path();
    std::string get_sdl2_lib_path();
    std::string get_sdl2_bin_path();
    std::string get_output_path();
    
    bool validate_paths();
    void create_default_config(const std::string& root_path);
};

#endif
