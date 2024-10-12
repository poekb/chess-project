#include "../global.h"

Game* getGame();

void initGame();

bool movePieceFromTo(Pos from, Pos to);
void cleanUpGameHandler();

void fillPossibilityMatrix();
void generatePossibleMoves(GamePosition position, Pos from, Possibilities** end);

Possibilities* containsPossibility(Possibilities* possibilities, Pos move);
void freePossibilities(Possibilities* possibilities);
