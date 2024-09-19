# CMakeList.txt : CMake project for chess-project, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (chess-project WIN32 "chess-project.c" "chess-project.h" "board-renderer.c" "board-renderer.h" "svg-loader.c" "svg-loader.h" "text-renderer.c" "text-renderer.h")

find_package(SDL2)
find_package(SDL2_image)
find_package(SDL2_ttf)

target_link_libraries(chess-project SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

find_package(sdl2-gfx CONFIG REQUIRED)
target_link_libraries(chess-project SDL2::SDL2_gfx)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chess-project PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
