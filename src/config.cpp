#include "config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <stdexcept>

namespace fs = std::filesystem;

static fs::path config_dir() {
    const char* cfg_home = std::getenv("XDG_CONFIG_HOME");
    fs::path base;
    if (cfg_home && cfg_home[0] != '\0') {
        base = cfg_home;
    } else if (const char* home = std::getenv("HOME")) {
        base = fs::path(home) / ".config";
    } else {
        throw std::runtime_error("Cannot determine config directory");
    }
    base /= "wallpaper_rulette";
    fs::create_directories(base);
    return base;
}

static fs::path config_path() {
    return config_dir() / "config.json";
}

void Config::load() {
    auto path = config_path();
    if (!fs::exists(path)) {
        save();
        return;
    }
    try {
        std::ifstream in(path);
        nlohmann::json j;
        in >> j;
        if (j.contains("directory")) {
            directory = j["directory"].get<std::string>();
        }
        if (j.contains("frequency_seconds")) {
            frequency_seconds = j["frequency_seconds"].get<int>();
        }
        if (j.contains("backend")) {
            backend = j["backend"].get<std::string>();
        }
    } catch (...) {
        save();
    }
}

void Config::save() const {
    nlohmann::json j;
    j["directory"] = directory;
    j["frequency_seconds"] = frequency_seconds;
    j["backend"] = backend;
    std::ofstream out(config_path());
    out << j.dump(4) << std::endl;
}
