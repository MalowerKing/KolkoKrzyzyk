cmake_minimum_required(VERSION 3.16)
project(Nowak_Projekt_Hash_Table)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Try multiple methods to find SDL3
find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL3 REQUIRED sdl3)
    else()
        # Manual fallback
        find_path(SDL3_INCLUDE_DIRS SDL3/SDL.h)
        find_library(SDL3_LIBRARIES SDL3)
    endif()
endif()

# Try multiple methods to find SDL3_ttf
find_package(SDL3_ttf QUIET)
if(NOT SDL3_ttf_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL3_TTF REQUIRED SDL3_ttf)
    else()
        # Manual fallback
        find_path(SDL3_TTF_INCLUDE_DIRS SDL3/SDL_ttf.h)
        find_library(SDL3_TTF_LIBRARIES SDL3_ttf)
    endif()
endif()

add_executable(result main.cpp)

# Link libraries - handle both find_package and pkg-config variable names
if(SDL3_ttf_FOUND)
    # Using find_package variables (SDL3 style)
    target_link_libraries(result SDL3::SDL3 SDL3_ttf::SDL3_ttf)
else()
    # Using pkg-config variables or manual fallback
    target_link_libraries(result ${SDL3_LIBRARIES} ${SDL3_TTF_LIBRARIES})
    target_include_directories(result PRIVATE ${SDL3_INCLUDE_DIRS} ${SDL3_TTF_INCLUDE_DIRS})
endif()

set_property(TARGET result PROPERTY CXX_STANDARD 17)