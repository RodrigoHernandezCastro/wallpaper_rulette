#include <iostream>
#include <string>
#include <fstream>
#include "libraries/ini.h"
#include <vector>
#include <cstdlib>
#include <filesystem>
#include <thread>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <random>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
bool running = true;

void signalHandler(int signum) {
    running = false;
}

void daemonize() {
    pid_t pid = fork();

    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(){
    daemonize();

    signal(SIGTERM, signalHandler);

    std::ofstream logfile("/var/log/myapp.log", std::ios_base::app);
    logfile << "Service started." << std::endl;

    const char* homeDir = std::getenv("HOME");

    if(homeDir == nullptr){
        std::cerr << "Error: The environment variable HOME is not defined" << std::endl;
    }

    std::string configPath = std::string(homeDir) + "/.config/waypaper/config.ini";

    mINI::INIFile file(configPath);
    mINI::INIStructure ini;

    std::vector<std::string> extension_options = {".jpg", ".gif", ".png", ".jpeg"};
    std::vector<std::string> file_names;
    std::vector<int> random_numbers;
    std::string location;
    std::string wallpapers;
    std::string location_path;
    std::string wallpapers_path;

    if (file.read(ini)) {
        location    = ini["Settings"]["folder"];
        wallpapers  = ini["Settings"]["wallpaper"];
        if (!location.empty() && location.front() == '~'){
            location.erase(0, 1);
        }
        location_path = std::string(homeDir) + location;
        wallpapers_path = std::string(homeDir) + wallpapers;
        std::cout << "Location: " << location << std::endl;
        std::cout << "Location wallpaper: " << wallpapers << std::endl;
        std::cout << location_path << std::endl;
    } else {
        std::cerr << "Error: Could not open configuration file." << std::endl;
        return 1;
    }

    for (const auto& entry : std::filesystem::directory_iterator(location_path)) {
        std::filesystem::path ext = entry.path().extension();
        std::filesystem::path file_name = entry.path();
        if (std::find(extension_options.begin(), extension_options.end(), ext) == extension_options.end()) {
            std::cout << ext << " is NOT in the vector." << std::endl;
        }else{
            file_names.push_back(file_name.filename());
        }
    }

    std::vector<int> indices(file_names.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), std::mt19937{std::random_device{}()});
    std::size_t idx = 0;

    bool running = true;
    if(location.front() == '~' && location.back() == '/'){
        std::cout << "Nothing to do." << std::endl;
    }else{
        location.insert(0, 1, '~');
        location.push_back('/');
    }
    while(running){
        std::cout << "Starting..." << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(60));

        std::cout << "Done!" << std::endl;

        int pick = indices[idx];

        wallpapers_path = location + file_names[pick];
        ini["Settings"]["wallpaper"] = wallpapers_path;

        if (file.write(ini)) {
            std::cout << "INI file written successfully!" << std::endl;
        } else {
            std::cerr << "Error: Failed to write INI file." << std::endl;
            return 1;
        }
        std::system("waypaper --restore");

        idx++;
        if (idx >= indices.size()) {
            std::shuffle(indices.begin(), indices.end(), std::mt19937{std::random_device{}()});
            idx = 0;
        }
    }

    logfile << "Service stopped." << std::endl;
    logfile.close();
    return 0;
}
