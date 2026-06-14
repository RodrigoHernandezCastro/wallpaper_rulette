# Wallpaper Rulette

A lightweight system tray app that randomly changes your wallpaper at a configurable interval.

## Features

* **System tray**: Lives in your system tray — no terminal needed
* **Random wallpaper at startup**: Picks a random wallpaper immediately on launch
* **Configurable**: Select directory, change frequency, and pick backend — all from the tray menu
* **Multi-backend**: Supports swww, awww, waypaper, and hyprpaper

## Dependencies

* C++17 or later
* GTK 3
* libayatana-appindicator
* nlohmann/json

## Compilation

```bash
# With Nix (recommended)
nix develop
cmake -S . -B build
cmake --build build

# Run
./build/bin/wallpaper_rulette
```

```bash
# Without Nix (ensure dependencies are installed)
cmake -S . -B build
cmake --build build

# Run
./build/bin/wallpaper_rulette
```

## Usage

The app runs as a system tray icon. Right-click to access the menu:

| Menu Item | Description |
|-----------|-------------|
| **Change Now** | Immediately set a random wallpaper |
| **Select Directory...** | Pick a folder with wallpaper images |
| **Set Frequency...** | Set how often the wallpaper changes (in minutes) |
| **Backend** ▸ | Choose swww, awww, hyprpaper, or waypaper |
| **Quit** | Exit the application |

On first run, the config is created at `$XDG_CONFIG_HOME/wallpaper_rulette/config.json`. The default directory is `~/Pictures`.

## Author

[Rodrigo Hernandez](https://github.com/RodrigoHernandezCastro)
