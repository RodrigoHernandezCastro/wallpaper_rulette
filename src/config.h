#pragma once

#include <string>

struct Config {
    std::string directory;
    int frequency_seconds = 300;
    std::string backend = "swww";

    void load();
    void save() const;
};
