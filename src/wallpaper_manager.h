#pragma once

#include <string>
#include <vector>
#include <random>

class WallpaperManager {
public:
    void scan_directory(const std::string& path);
    const std::vector<std::string>& files() const { return file_paths; }
    bool empty() const { return file_paths.empty(); }

    std::string pick_random();
    static void set_wallpaper(const std::string& filepath, const std::string& backend);
    static const std::vector<std::string>& supported_backends();

private:
    std::vector<std::string> file_paths;
    std::mt19937 rng{std::random_device{}()};
    std::vector<size_t> indices;
    size_t idx = 0;
};
