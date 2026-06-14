cmake_minimum_required(VERSION 3.20)

project(wallpaper_rulette
    VERSION 0.2.0
    DESCRIPTION "Systray wallpaper changer"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(AYATANA REQUIRED ayatana-appindicator3-0.1)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/config.cpp
    src/wallpaper_manager.cpp
    src/systray.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${GTK3_INCLUDE_DIRS}
    ${AYATANA_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    ${GTK3_LIBRARIES}
    ${AYATANA_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${GTK3_CFLAGS_OTHER}
    ${AYATANA_CFLAGS_OTHER}
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
