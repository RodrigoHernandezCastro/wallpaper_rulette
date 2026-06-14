#include "wallpaper_manager.h"
#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <numeric>

namespace fs = std::filesystem;

static const std::vector<std::string> extensions = {
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".webp"
};

const std::vector<std::string>& WallpaperManager::supported_backends() {
    static const std::vector<std::string> backends = {
        "swww", "awww", "hyprpaper", "waypaper"
    };
    return backends;
}

void WallpaperManager::scan_directory(const std::string& path) {
    file_paths.clear();
    indices.clear();
    idx = 0;

    if (path.empty() || !fs::exists(path) || !fs::is_directory(path)) return;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;
        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (std::find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
            file_paths.push_back(entry.path().string());
        }
    }

    if (file_paths.empty()) return;

    indices.resize(file_paths.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng);
}

std::string WallpaperManager::pick_random() {
    if (indices.empty()) return "";

    if (idx >= indices.size()) {
        std::shuffle(indices.begin(), indices.end(), rng);
        idx = 0;
    }

    return file_paths[indices[idx++]];
}

void WallpaperManager::set_wallpaper(const std::string& filepath, const std::string& backend) {
    if (filepath.empty() || backend.empty()) return;

    std::string cmd;
    if (backend == "awww") {
        cmd = "awww img " + filepath;
    } else if (backend == "swww") {
        cmd = "swww img " + filepath;
    } else if (backend == "hyprpaper") {
        cmd = "hyprctl hyprpaper reload " + filepath;
    } else if (backend == "waypaper") {
        cmd = "waypaper --wallpaper " + filepath;
    } else {
        return;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    std::system(cmd.c_str());
#pragma GCC diagnostic pop
}
