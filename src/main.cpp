#include <iostream>
#include <string>
#include <fstream>
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
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

bool running = true;
bool service_running = false;
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

struct ProcessInfo {
    int pid;
    std::string name;
};

std::vector<ProcessInfo> find_processes_by_name(const std::string& target_name) {
    std::vector<ProcessInfo> result;
    for (const auto& entry : std::filesystem::directory_iterator("/proc")){
        if (!entry.is_directory()) continue;

        std::string pid_str = entry.path().filename().string();
        if (!std::all_of(pid_str.begin(), pid_str.end(), ::isdigit)) continue;

        std::ifstream cmdline(entry.path() / "cmdline");
        std::string first_arg;

        if (std::getline(cmdline, first_arg, '\0')){
            std::string exec_name = std::filesystem::path(first_arg).filename().string();

            if (exec_name == target_name){
                result.push_back({std::stoi(pid_str), target_name});
            }
        }
    }
    return result;
}

int main(int argc, const char * argv[]){
    signal(SIGTERM, signalHandler);

    const char* xdg_state_home = std::getenv("XDG_STATE_HOME");
    if (!xdg_state_home) throw std::runtime_error("XDG_STATE_HOME not set");

    std::filesystem::path logDir  = std::filesystem::path(xdg_state_home) / "wallpaper_rulette";
    std::filesystem::path logFile = logDir / "wallpaper_rulette.log";
    std::filesystem::create_directories(logDir);

    std::ofstream fileStream(logFile);

    std::ofstream log(logFile, std::ios::app);
    log << "App started\n";

    std::cout << "Logging to: " << logFile << "\n";

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << std::endl;
        return 1;
    }
    std::vector<std::string> extension_options = {".jpg", ".gif", ".png", ".jpeg"};
    std::vector<std::string> file_names;
    std::vector<int> random_numbers;
    std::string time = argv[1];
    std::string locpath = argv[2];
    std::string backend = argv[3];
    std::string wallpapers_path;
    std::string name_process = "wallpaper_rulette";
    int time_seconds = std::stoi(time);

    if (backend == "waypaper"){
        std::system("waypaper");
    }

    std::vector<ProcessInfo> find_processes_by_name(const std::string& name_process);

    auto processes = find_processes_by_name(name_process);
    pid_t current_pid = getpid();
    log << "Found processes: " << processes.size() << std::endl;
    for (const auto& proc : processes) {
        if (proc.pid == current_pid) {
            continue;
        }
        log << "[PID:  " << proc.pid  << "  ] " << std::endl;
        log << "[Name: " << proc.name << "  ] " << std::endl;

        if (kill(proc.pid, SIGKILL) == 0) {
            log << "Successfully killed leftover process." << std::endl;
        } else {
            log << "Failed to kill process. Error: " << errno << std::endl;
        }
    }

    log << "Time: " << time << std::endl;
    log << "locpath: " << locpath << std::endl;
    log << "backend: " << backend << std::endl;

    for (const auto& entry : std::filesystem::directory_iterator(locpath)) {
        std::filesystem::path ext = entry.path().extension();
        std::filesystem::path file_name = entry.path();
        if (std::find(extension_options.begin(), extension_options.end(), ext) == extension_options.end()) {
            std::cout << ext << " is NOT in the vector." << std::endl;
        }else{
            file_names.push_back(file_name.filename());
        }
    }

    std::random_device rd;
    std::seed_seq seeds{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
    std::mt19937 rng(seeds);

    std::vector<int> indices(file_names.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng);
    std::size_t idx = 0;

    daemonize();

    if (locpath.find(xdg_state_home)){
        if (locpath.front() == '~'){
            locpath.erase(0, 1);
        }else {
            log << "Doesn't have ~ symbol" << std::endl;
        }
    }else {
        log << "Already have home implemented" << std::endl;
    }

    if (locpath.back() == '/'){
        log << "Already have / symbol" << std::endl;
    }else{
        locpath.push_back('/');
    }
    if (service_running == true){
        log << "Already executing process..." << std::endl;
    }else{
        while(running){
            log << "Starting..." << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(time_seconds));

            log << "Done!" << std::endl;

            int pick = indices[idx];
            log << "picIdx: " << pick << std::endl;
            wallpapers_path = locpath + file_names[pick];
            log << "wallpapers_path: " << wallpapers_path << std::endl;
            log << "time: " << time << " locpath: " << locpath << " backend: " << backend << std::endl;

            if (backend == "awww"){
                std::system(("awww img " + wallpapers_path).c_str());
                log << "Started: awww with the wallpaper " << file_names[pick] << std::endl;
            }else if (backend == "swww"){
                std::system(("swww img " + wallpapers_path).c_str());
                log << "Started: swww with the wallpaper " << file_names[pick] << std::endl;
            }else if (backend == "hyprpaper"){
                std::system(("hyprctl hyprpaper reload " + wallpapers_path).c_str());
                log << "Started: hyprpaper with the wallpaper " << file_names[pick] << std::endl;
            }else if (backend == "waypaper"){
                std::system(("waypaper --wallpaper " + wallpapers_path).c_str());
                log << "Started: waypaper with the wallpaper " << file_names[pick] << std::endl;
            }

            idx++;
            if (idx >= indices.size()) {
                std::shuffle(indices.begin(), indices.end(), rng);
                idx = 0;
            }
        }
    }

    log << "Service stopped." << std::endl;
    fileStream.close();
    return 0;
}
