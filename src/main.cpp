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

    //Playing a short opening with captures, a long and a short castle
    std::string moves[18] = {"e2e4", "e7e5", "g1f3", "d7d5", "f1a6", "c8g4", "e1g1", "b8a6",
                             "f3e5", "d8h4", "e5g4", "e8c8", "e4e5", "f7f5", "e5f6", "d5d4",
                             "c2c4", "d4c3"};
    for (int i = 0; i < 18; i++){
        move move = GameState::strToMove(moves[i]);
        std::cout << move << std::endl;
        g.doMove(move);
        g.printBoard();
        g.isBoardLegit();
    }

#endif

    return 0;
}
