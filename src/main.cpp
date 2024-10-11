#include <iostream>
#include "GameState.h"

#define GAME_STATE_TESTING

int main() {

#ifndef GAME_STATE_TESTING
    std::cout << "Hello, World!" << std::endl;
#endif

    // Doing all test for the gameState Class
#ifdef GAME_STATE_TESTING
    GameState g;
    g.loadStartingPosition();
    g.printBoard();
    g.isBoardLegit();

    move move = GameState::strToMove("e2e4");
    std::cout << move << std::endl;
    g.doMove(move);
    g.printBoard();
    g.isBoardLegit();
    move = GameState::strToMove("a8e4");
    g.doMove(move);
    g.printBoard();
    g.isBoardLegit();

#endif

    return 0;
}
