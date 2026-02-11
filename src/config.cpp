#include "config.h"
#include <algorithm>
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

RBConfig::RBConfig() {
    // Get the directory where rbbasic.exe is located
    #ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exe_path(buffer);
    size_t pos = exe_path.find_last_of("\\/");
    config_dir = exe_path.substr(0, pos);
    #else
    char buffer[4096];
    getcwd(buffer, sizeof(buffer));
    config_dir = buffer;
    #endif
}

std::string RBConfig::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string RBConfig::to_absolute_path(const std::string& path) {
    // If path is already absolute, return it
    if (path.length() >= 2 && path[1] == ':') {
        return path;  // Windows absolute path (C:\...)
    }
    if (path.length() >= 1 && (path[0] == '/' || path[0] == '\\')) {
        return path;  // Unix absolute path or Windows path from root
    }
    
    // Relative path - make it relative to config_dir
    std::string result = config_dir + "\\" + path;
    
    // Convert forward slashes to backslashes on Windows
    #ifdef _WIN32
    std::replace(result.begin(), result.end(), '/', '\\');
    #endif
    
    return result;
}

bool RBConfig::load(const std::string& filename) {
    std::string full_path = to_absolute_path(filename);
    std::ifstream file(full_path);
    
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << full_path << std::endl;
        std::cerr << "Creating default configuration..." << std::endl;
        create_default_config(config_dir);
        return false;
    }
    
    std::string current_section = "";
    std::string line;
    int line_num = 0;
    
    while (std::getline(file, line)) {
        line_num++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Section header
        if (line[0] == '[' && line[line.length()-1] == ']') {
            current_section = line.substr(1, line.length()-2);
            continue;
        }
        
        // Key=Value pair
        size_t equals_pos = line.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = trim(line.substr(0, equals_pos));
            std::string value = trim(line.substr(equals_pos + 1));
            
            if (!current_section.empty() && !key.empty()) {
                data[current_section][key] = value;
            }
        }
    }
    
    file.close();
    return true;
}

bool RBConfig::save(const std::string& filename) {
    std::string full_path = to_absolute_path(filename);
    std::ofstream file(full_path);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not write config file: " << full_path << std::endl;
        return false;
    }
    
    file << "; RB BASIC (Ronen Blumberg BASIC) Configuration File\n";
    file << "; Auto-generated - Edit with care\n\n";
    
    for (const auto& section : data) {
        file << "[" << section.first << "]\n";
        for (const auto& kv : section.second) {
            file << kv.first << "=" << kv.second << "\n";
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

std::string RBConfig::get(const std::string& section, const std::string& key, const std::string& default_value) {
    if (data.find(section) != data.end()) {
        if (data[section].find(key) != data[section].end()) {
            return data[section][key];
        }
    }
    return default_value;
}

bool RBConfig::get_bool(const std::string& section, const std::string& key, bool default_value) {
    std::string value = get(section, key, default_value ? "true" : "false");
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return (value == "true" || value == "1" || value == "yes" || value == "on");
}

void RBConfig::set(const std::string& section, const std::string& key, const std::string& value) {
    data[section][key] = value;
}

std::string RBConfig::get_compiler_root() {
    return to_absolute_path(get("Paths", "CompilerRoot", "."));
}

std::string RBConfig::get_mingw_path() {
    return to_absolute_path(get("Paths", "MinGW32Path", "mingw32"));
}

std::string RBConfig::get_gcc_path() {
    std::string gcc = get("Compiler", "GCC", "mingw32\\bin\\gcc.exe");
    return to_absolute_path(gcc);
}

std::string RBConfig::get_gpp_path() {
    std::string gpp = get("Compiler", "GPP", "mingw32\\bin\\g++.exe");
    return to_absolute_path(gpp);
}

std::string RBConfig::get_sdl2_include_path() {
    std::string inc = get("SDL2", "IncludePath", "lib\\SDL2-mingw32\\i686-w64-mingw32\\include");
    return to_absolute_path(inc);
}

std::string RBConfig::get_sdl2_lib_path() {
    std::string lib = get("SDL2", "LibPath", "lib\\SDL2-mingw32\\i686-w64-mingw32\\lib");
    return to_absolute_path(lib);
}

std::string RBConfig::get_sdl2_bin_path() {
    std::string bin = get("SDL2", "BinPath", "lib\\SDL2-mingw32\\i686-w64-mingw32\\bin");
    return to_absolute_path(bin);
}

std::string RBConfig::get_output_path() {
    return to_absolute_path(get("Paths", "OutputPath", "output"));
}

bool RBConfig::validate_paths() {
    bool all_valid = true;
    
    // Check if GCC exists
    std::string gcc_path = get_gcc_path();
    std::ifstream gcc_test(gcc_path);
    if (!gcc_test.good()) {
        std::cerr << "Error: GCC not found at: " << gcc_path << std::endl;
        all_valid = false;
    }
    gcc_test.close();
    
    // Check if SDL2 include directory exists
    std::string sdl_inc = get_sdl2_include_path();
    #ifdef _WIN32
    DWORD attribs = GetFileAttributesA(sdl_inc.c_str());
    if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
        std::cerr << "Warning: SDL2 include path not found: " << sdl_inc << std::endl;
        std::cerr << "         Graphics features may not work." << std::endl;
    }
    #else
    // On Linux, just check if directory exists
    struct stat sb;
    if (stat(sdl_inc.c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode)) {
        std::cerr << "Warning: SDL2 include path not found: " << sdl_inc << std::endl;
        std::cerr << "         Graphics features may not work." << std::endl;
    }
    #endif
    
    return all_valid;
}

void RBConfig::create_default_config(const std::string& root_path) {
    // Set defaults
    set("Paths", "CompilerRoot", root_path);
    set("Paths", "MinGW32Path", "mingw32");
    set("Paths", "SDL2Path", "lib\\SDL2-mingw32");
    set("Paths", "OutputPath", "output");
    set("Paths", "ExamplesPath", "examples");
    
    set("Compiler", "GCC", "mingw32\\bin\\gcc.exe");
    set("Compiler", "GPP", "mingw32\\bin\\g++.exe");
    set("Compiler", "Optimization", "-O1");
    set("Compiler", "Warnings", "-Wall -Wno-unused-label -Wno-unknown-pragmas");
    
    set("SDL2", "IncludePath", "lib\\SDL2-mingw32\\i686-w64-mingw32\\include");
    set("SDL2", "LibPath", "lib\\SDL2-mingw32\\i686-w64-mingw32\\lib");
    set("SDL2", "BinPath", "lib\\SDL2-mingw32\\i686-w64-mingw32\\bin");
    
    set("Options", "AutoCopySDL", "true");
    set("Options", "CleanupTemp", "false");
    set("Options", "ShowCompileProgress", "true");
    set("Options", "VerboseOutput", "false");
    
    // Save default config
    save("rbc.config");
    
    std::cout << "Created default rbc.config file at: " << root_path << "\\rbc.config" << std::endl;
}
