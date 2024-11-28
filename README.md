# Chess Project

>## Build process on Windows with `CMake` and `vcpkg`
>
>### First install the required libraries
>
> If you don't use vcpkg, then you can skip this part, but make sure that CMake can find the required dependencies.
>
>```commandline
>vcpkg install sdl2:x64-mingw-dynamic
>vcpkg install sdl2-image:x64-mingw-dynamic
>vcpkg install sdl2-ttf:x64-mingw-dynamic
>vcpkg install sdl2-gfx:x64-mingw-dynamic
>```
>
>### Then use this command to make a CMake profile
>```commandline
>cmake -Bbuild-directory-to-be-created -DCMAKE_BUILD_TYPE=<Debug/Release> -G "Ninja" 
>-DCMAKE_TOOLCHAIN_FILE=<your-vcpkg-directory>\scripts\buildsystems\vcpkg.cmake 
>-DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic
>```
>You should see CMake creating the specified build directory.
>
>### Finally, you can build the app with CMake
>```commandline
>cmake --build build-directory-to-be-created
>```
>Cmake should build an executable in chess-project under your build directory.
>
> This process should be fairly similar on `Linux` and `macOS`, but I haven't tested that yet.

## Chess Engine Structure | `core`

For optimizing the chess bot, a method is needed to efficiently make and undo moves on the board.
This is essential because finding the best move requires navigating
through millions of positions to locate the optimal one.

### Storing Pieces | `piece` Module

The `Piece` type stores a piece in a single byte, with a unique number assigned to each type of piece.

The `PieceType` type does the same but ignores the color of the piece, focusing only on its type.

```c
typedef Uint8 Piece;
typedef Uint8 PieceType;
```

### Storing the Board | `board` Module

For fast moves, it's necessary to store which pieces occupy each square and also which squares
each type of piece occupies. The `square` array is a 64-element 1D representation of the chessboard,
storing the piece on each square.

The `PieceList`s maintain where each piece type is located, separated by color.

The `currentGameState` and `gameStateHistory` structures store irreversible states for move
retraction.

`zobristHash` and `zobristHistory` store the board’s hash, detailed further in the section
on position hashing.

```c
typedef struct Board {
    Piece square[64];
    Uint8 kingSquare[2];
    PieceList Rooks[2];
    PieceList Bishops[2];
    PieceList Queens[2];
    PieceList Knights[2];
    PieceList Pawns[2];
    Uint64 zobristHash;
    Uint64 zobristHistory[10000];
    GameState currentGameState;
    GameState gameStateHistory[10000];
    Uint16 gameStateHistoryCount;
    bool isWhitesMove;
    Uint64 underAttackMap;
    Uint16 fullmoveClock;
} Board;
```

### Lists of Same-Type Pieces | `pieceList` Module

The `PieceList` type stores pieces of the same type in an unordered list, with each piece’s
position in the list.

The `map` assists with locating a piece’s position based on its position on the board.

```c
typedef struct PieceList {
    Uint8 list[10];
    Uint8 count;
    Uint8 map[64];
} PieceList;
```

### Game State | `gameState` Module

- The `enpassantFile` field notes which column a pawn moved double in the previous turn,
- with 255 meaning no such move.

- `castleRights` manages castling rights, and

- `capturedPiece` stores the last captured piece.

- The `halfmoveClock` keeps track of half moves since the last capture or pawn move.

```c
typedef struct GameState {
    Uint8 enpassantFile;
    Uint8 castleRights;
    PieceType capturedPiece;
    Uint8 halfmoveClock;
} GameState;
```

### Storing Moves | `move` Module

Moves are stored in 16 bits for optimization:

- The first 4 bits represent various flags like pawn promotion or castling.
- The next 6 bits represent the starting position.
- The last 6 bits represent the ending position.

```c
typedef Uint16 Move;
```

## Main Functions of the Chess Engine

- `void makeMove(Board* board, Move move);` – Executes a move and saves the current
- state in `gameStateHistory`.
- `void unmakeMove(Board* board, Move move);` – Reverts a move.

### Move Generation | `moveGeneration`

`int generateMoves(Board* boardIn, Move* resultIn, bool onlyAttackIn);`
generates all possible moves for a given position and stores them in `resultIn`,
returning the total count. The `onlyAttackIn` parameter specifies if only attacks should be considered.

## Graphics Renderer | `graphics`

The graphical renderer displays the board with pieces and buttons for move retraction,
saving states, and activating the chess bot.

### Displaying the Board | `boardRender` Module

The board display includes a static background and dynamic piece layout:

- `void renderBoard(SDL_Renderer* renderer, int boardSizeNew, int boardOffsetX, int boardOffsetY);`
  – Renders the board to a texture buffer.
- `void renderStatic(SDL_Renderer* renderer);`
  – Loads the pre-rendered board into the screen buffer.
- `void renderPieces(SDL_Renderer* renderer, Piece board[64]);`
  – Displays pieces.

### Additional UI Rendering | `layout` Module

The rest of the UI (e.g., buttons) is managed here:

- `void recalcUIData();` – Updates the size and position of UI elements if the window is resized.

## Chess Bot Operation | `ai`

The chess bot uses a minimax search algorithm that explores move sequences and evaluates positions to find the best
move.

### Optimizations

Efficient move generation and move-making functions are essential for deep searches. Another critical optimization is
reducing the number of paths checked to find the best move.

#### Alpha-Beta Pruning

Alpha-beta pruning skips paths that are unlikely to yield good moves.

#### Position Hashing | `zobrist`

Zobrist hashing uniquely represents game states using random numbers for each piece’s position and other game
conditions. The hash is used for detecting move repetition and storing evaluated positions in a transposition table.

```c
typedef struct Transposition {
    Uint64 zobrist;
    int eval;
    Move bestMove;
    Uint8 depth;
    Uint8 type;
} Transposition;
```

### Move Ordering | `moveOrdering` Module

Move ordering optimizes alpha-beta pruning by examining the most promising moves first.

### Position Evaluation | `logic`

When the search reaches the desired depth, a basic evaluation assigns values to pieces based on their positions.

### Main Search Function

`int search(int depth, int distFromRoot, int alpha, int beta);` is the main function of the minimax algorithm, returning
the current position’s evaluation.

## Game Management | `game`

Game management functions are in the `gameHandler` module, which also includes the main game loop.

### Saving Positions | `pgn` Module

Games are saved in PGN format with moves recorded in algebraic notation. Initial positions can be specified using FEN
notation.

## User Documentation

### Using the Program

The application has a graphical interface that interacts with the user via mouse input.

### Chessboard

Clicking on a square highlights it, showing possible moves for pieces in blue. Completed moves are shown in green, and a
set of buttons below the board allows move retraction and navigation to the beginning or end of the game.

### Chess Bot

Click the "Start Bot" button to enable the bot, which will think for 2 seconds per move and continue unless disabled.

### Game Save and Load

Buttons on the right side allow game saving and loading.
