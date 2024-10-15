#include <iostream>
#include <vector>
#include <sstream>
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

    std::string str = "";

    std::vector<std::string> moves;
    std::stringstream ss(str);
    std::string word;

    // Extract words from the stream
    while (ss >> word) {
        moves.push_back(word);
    }

    /*
    for(int i = 0; i < moves.size(); i++){
        g.doMove(g.stringToMove(moves[i]));
        g.printGame();
    }
/* */
    /*
    while (true){
        std::string input;
        std::cin >> input;
        g.doMove(g.stringToMove(input));
        g.printGame();
    }
    */
#endif

    return 0;
}
