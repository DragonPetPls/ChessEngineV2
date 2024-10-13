#include <iostream>
#include "Game.h"

#define GAME_DEBUG

int main() {

#ifndef GAME_DEBUG
    std::cout << "Hello, World!" << std::endl;
#endif

    // Doing all test for the Game Class
#ifdef GAME_DEBUG
    Game g;
    g.loadStartingPosition();
    g.printGame();



#endif

    return 0;
}
