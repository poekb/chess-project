
add_executable(chessProject WIN32 "chessProject.c" "game/gameHandler.h" "game/gameHandler.c" "global.h" "graphics/boardRenderer.c" "graphics/boardRenderer.h" "graphics/svgLoader.c" "graphics/svgLoader.h" "graphics/textRenderer.c" "graphics/textRenderer.h" "logic/evaluator.c" "logic/evaluator.h" "logic/pieceMaps.h" "logic/pieceMaps.c" "ai/chessBot.c" "ai/chessBot.h" "core/board.h" "core/board.c" "core/piece.c" "core/piece.h" "core/pieceList.c" "core/pieceList.h" "core/gameState.h" "core/gameState.c" "core/move.c" "core/move.h" "moveGeneration/precompution.c" "moveGeneration/precompution.h" "moveGeneration/moveGenerator.c" "moveGeneration/moveGenerator.h" "ai/moveOrdering.c" "ai/moveOrdering.h" "game/notations.c" "game/notations.h" "graphics/layout.c" "graphics/layout.h" "game/pgn.c" "game/pgn.h" "zobrist/zobristHashing.c" "zobrist/zobristHashing.h" "zobrist/randomGenerator.c" "zobrist/randomGenerator.h" "ai/transpositionTable.c" "ai/transpositionTable.h"
    global.c)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(sdl2-gfx CONFIG REQUIRED)

target_link_libraries(chessProject
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    SDL2_ttf::SDL2_ttf
    SDL2::SDL2_gfx
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET chessProject PROPERTY CXX_STANDARD 20)
endif ()

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

