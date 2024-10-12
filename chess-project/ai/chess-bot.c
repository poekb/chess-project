#include "chess-bot.h"
#include "../logic/game-handler.h"
#include "../logic/evaluator.h"

int moveCount;

void CalcBestMove(Game* game) {
	moveCount = 0;
	EvalCase bestMove = miniMax(5, game->position,!game->position.turn);
	printf("Moves tested: %d\n", moveCount);
	//printf("%d - %d, %d - %d\n", bestMove.from.rank, bestMove.from.file, bestMove.to.rank, bestMove.to.file);


	movePieceFromTo(bestMove.from, bestMove.to);
}


EvalCase miniMax(int depth, GamePosition position, bool inv) {

	if (depth == 0) {
		moveCount++;
		return (EvalCase) { inv ? -evalBoard(position.board) : evalBoard(position.board) };
	}

	// generate possible positions
	Possibilities* possibilities = NULL;
	for (int rank = 0; rank < 8; rank++) {
		for (int file = 0; file < 8; file++) {
			generatePossibleMoves(position, (Pos) { rank, file }, &possibilities);
		}
	}

	EvalCase result = {
		-1234567
	};
	Possibilities* currentPoss = possibilities;

	while (currentPoss != NULL) {
		EvalCase evalCase = miniMax(depth - 1, currentPoss->possibility.rersultPosition, inv);
		
		if (position.turn)  // When is it min and when is it max ???????
			evalCase.value *= -1;

		if (evalCase.value > result.value) {
			

			result.value = evalCase.value;
			result.from = currentPoss->possibility.source;
			result.to = currentPoss->possibility.target;

		}

		currentPoss = currentPoss->next;
	}

	freePossibilities(possibilities);

	return result;
}