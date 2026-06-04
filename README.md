# Wallpaper Rulette

A little weight service that works in the background and change your wallpapers

## Main stuff

* **Adaptative:**: It has the ability to use swww, awww, waypaper or hyprpaper; Dependending on your own configuration. 
* **Light weight**: It uses just the right amount of memory to work.
* **Memory leak**: It has code to prevent the code to multiply himself to the infinite if you are not careful.

## Dependencies and requierements

The project is configured to be compiled using **CMake** and depends on the next libraries:

* C++17 (o superior)
* nlohmann/json

## Compilation

To compile the project from the source code using **CMake**, execute the next commands from the root of the repo:

1. Generate the constructor files:
```bash
cmake -S . -B build
```

2. Compile:
```bash
cmake --build build
```

3. Execute the code:
```bash
./build/bin/wallpaper_rulette 5 /home/randy/Imagenes/Wallpapers/ waypaper
```
The execution of the code have 4 parts.
./build/bin/wallpaper_rulette <--- Our executable

5 <--- The timer between wallpapers

/home/randy/Imagenes/Wallpapers/ <--- The path of the folder with your wallpapers

waypaper <--- Your backend

## Author
[Rodrigo Hernandez](https://github.com/RodrigoHernandezCastro)
