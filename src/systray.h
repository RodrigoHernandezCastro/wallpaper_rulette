#pragma once

#include <functional>
#include <string>

struct Config;
class WallpaperManager;

using VoidCallback = std::function<void()>;
using BackendCallback = std::function<void(const std::string&)>;

struct SystrayCallbacks {
    VoidCallback change_now;
    VoidCallback select_directory;
    VoidCallback frequency_changed;
    BackendCallback backend_changed;
    VoidCallback quit;
};

void systray_init(int argc, char** argv, Config* config, WallpaperManager* wm,
                  SystrayCallbacks callbacks);
void systray_run();
