#include "systray.h"
#include "config.h"
#include "wallpaper_manager.h"

#include <gtk/gtk.h>
#include <libayatana-appindicator/app-indicator.h>

static Config* g_config = nullptr;
static WallpaperManager* g_wm = nullptr;
static SystrayCallbacks g_cb;
static AppIndicator* g_indicator = nullptr;

static GtkWidget* create_menu();

static void on_change_now(GtkMenuItem*, gpointer) {
    if (g_cb.change_now) g_cb.change_now();
}

static void on_select_directory(GtkMenuItem*, gpointer) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Select Wallpaper Directory",
        nullptr,
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        nullptr);

    if (!g_config->directory.empty()) {
        gtk_file_chooser_set_current_folder(
            GTK_FILE_CHOOSER(dialog), g_config->directory.c_str());
    }

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        g_config->directory = folder;
        g_free(folder);
        g_config->save();
        if (g_cb.select_directory) g_cb.select_directory();
    }

    gtk_widget_destroy(dialog);
}

static void on_set_frequency(GtkMenuItem*, gpointer) {
    GtkWidget* dialog = gtk_dialog_new_with_buttons(
        "Set Frequency",
        nullptr,
        GTK_DIALOG_MODAL,
        "_OK", GTK_RESPONSE_OK,
        "_Cancel", GTK_RESPONSE_CANCEL,
        nullptr);

    GtkWidget* content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(box), 12);
    gtk_box_pack_start(GTK_BOX(content), box, FALSE, FALSE, 6);

    GtkWidget* label = gtk_label_new("Minutes between changes:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    int minutes = g_config->frequency_seconds / 60;
    if (minutes < 1) minutes = 1;

    GtkWidget* spin = gtk_spin_button_new_with_range(1, 1440, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), minutes);
    gtk_box_pack_start(GTK_BOX(box), spin, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int mins = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
        g_config->frequency_seconds = mins * 60;
        g_config->save();
        if (g_cb.frequency_changed) g_cb.frequency_changed();
    }

    gtk_widget_destroy(dialog);
}

static void on_backend_selected(GtkCheckMenuItem* item, gpointer) {
    if (!gtk_check_menu_item_get_active(item)) return;

    const gchar* label = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    if (!label) return;

    std::string backend(label);
    if (g_config->backend == backend) return;

    g_config->backend = backend;
    g_config->save();
    if (g_cb.backend_changed) g_cb.backend_changed(backend);
}

static void on_quit(GtkMenuItem*, gpointer) {
    if (g_cb.quit) g_cb.quit();
}

static GtkWidget* create_menu() {
    GtkWidget* menu = gtk_menu_new();

    auto* change = gtk_menu_item_new_with_label("Change Now");
    g_signal_connect(change, "activate", G_CALLBACK(on_change_now), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), change);

    auto* dir_item = gtk_menu_item_new_with_label("Select Directory...");
    g_signal_connect(dir_item, "activate", G_CALLBACK(on_select_directory), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), dir_item);

    auto* freq = gtk_menu_item_new_with_label("Set Frequency...");
    g_signal_connect(freq, "activate", G_CALLBACK(on_set_frequency), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), freq);

    {
        auto* backend_item = gtk_menu_item_new_with_label("Backend");
        auto* backend_menu = gtk_menu_new();
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(backend_item), backend_menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), backend_item);

        GSList* group = nullptr;
        for (const auto& be : WallpaperManager::supported_backends()) {
            auto* radio = gtk_radio_menu_item_new_with_label(group, be.c_str());
            group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(radio));

            if (be == g_config->backend) {
                gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(radio), TRUE);
            }

            g_signal_connect(radio, "toggled", G_CALLBACK(on_backend_selected), nullptr);
            gtk_menu_shell_append(GTK_MENU_SHELL(backend_menu), radio);
        }
    }

    auto* sep = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep);

    auto* quit = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit, "activate", G_CALLBACK(on_quit), nullptr);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit);

    gtk_widget_show_all(menu);
    return menu;
}

void systray_init(int argc, char** argv, Config* config, WallpaperManager* wm,
                  SystrayCallbacks callbacks) {
    g_config = config;
    g_wm = wm;
    g_cb = callbacks;

    gtk_init(&argc, &argv);

    g_indicator = app_indicator_new(
        "wallpaper-rulette",
        "preferences-desktop-wallpaper",
        APP_INDICATOR_CATEGORY_OTHER);

    app_indicator_set_status(g_indicator, APP_INDICATOR_STATUS_ACTIVE);

    GtkWidget* menu = create_menu();
    app_indicator_set_menu(g_indicator, GTK_MENU(menu));
}

void systray_run() {
    gtk_main();
}
