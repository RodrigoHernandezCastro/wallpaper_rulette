#include "config.h"
#include "wallpaper_manager.h"
#include "systray.h"

#include <gtk/gtk.h>

static Config g_config;
static WallpaperManager g_wm;
static guint g_timer_id = 0;

static void apply_random_wallpaper() {
    if (g_wm.empty()) return;
    std::string wp = g_wm.pick_random();
    WallpaperManager::set_wallpaper(wp, g_config.backend);
}

static void change_now() {
    if (g_wm.empty()) {
        g_wm.scan_directory(g_config.directory);
    }
    apply_random_wallpaper();
}

static void on_select_directory() {
    g_wm.scan_directory(g_config.directory);
    apply_random_wallpaper();
}

static void on_frequency_changed() {
    if (g_timer_id > 0) {
        g_source_remove(g_timer_id);
        g_timer_id = 0;
    }
    if (g_config.frequency_seconds > 0) {
        g_timer_id = g_timeout_add_seconds(
            g_config.frequency_seconds,
            [](gpointer) -> gboolean {
                apply_random_wallpaper();
                return G_SOURCE_CONTINUE;
            },
            nullptr);
    }
}

static void on_backend_changed(const std::string& backend) {
    (void)backend;
}

int main(int argc, char* argv[]) {
    g_config.load();

    if (g_config.directory.empty()) {
        const char* home = std::getenv("HOME");
        if (home) {
            g_config.directory = std::string(home) + "/Pictures";
            g_config.save();
        }
    }

    g_wm.scan_directory(g_config.directory);
    apply_random_wallpaper();

    SystrayCallbacks cb;
    cb.change_now = change_now;
    cb.select_directory = on_select_directory;
    cb.frequency_changed = on_frequency_changed;
    cb.backend_changed = on_backend_changed;
    cb.quit = []() { gtk_main_quit(); };

    systray_init(argc, argv, &g_config, &g_wm, cb);

    if (g_config.frequency_seconds > 0 && !g_wm.empty()) {
        g_timer_id = g_timeout_add_seconds(
            g_config.frequency_seconds,
            [](gpointer) -> gboolean {
                apply_random_wallpaper();
                return G_SOURCE_CONTINUE;
            },
            nullptr);
    }

    systray_run();
    return 0;
}
