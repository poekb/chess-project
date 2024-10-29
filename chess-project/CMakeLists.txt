# CMakeList.txt : CMake project for chess-project, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (chessProject "chessProject.c"  "game/gameHandler.h" "game/gameHandler.c" "global.h"  "graphics/boardRenderer.c" "graphics/boardRenderer.h" "graphics/svgLoader.c" "graphics/svgLoader.h" "graphics/textRenderer.c" "graphics/textRenderer.h"     "logic/evaluator.c" "logic/evaluator.h" "logic/pieceMaps.h" "logic/pieceMaps.c" "ai/chessBot.c" "ai/chessBot.h" "core/board.h" "core/board.c" "core/piece.c" "core/piece.h" "core/pieceList.c" "core/pieceList.h" "core/gameState.h" "core/gameState.c" "core/move.c" "core/move.h" "moveGeneration/precompution.c" "moveGeneration/precompution.h" "moveGeneration/moveGenerator.c" "moveGeneration/moveGenerator.h" "ai/moveOrdering.c" "ai/moveOrdering.h" "game/notations.c" "game/notations.h" "graphics/layout.c" "graphics/layout.h" "game/pgn.c" "game/pgn.h" "zobrist/zobristHashing.c" "zobrist/zobristHashing.h" "zobrist/randomGenerator.c" "zobrist/randomGenerator.h" "ai/transpositionTable.c" "ai/transpositionTable.h")

find_package(SDL2)
find_package(SDL2_image)
find_package(SDL2_ttf)

target_link_libraries(chessProject SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

find_package(sdl2-gfx CONFIG REQUIRED)
target_link_libraries(chessProject SDL2::SDL2_gfx)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chessProject PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
