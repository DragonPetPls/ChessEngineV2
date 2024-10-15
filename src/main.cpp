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

    std::string str = "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 e1g1 f8e7 f1e1 b7b5 a4b3 d7d6 c2c3 e8g8 h2h3 c6a5 b3c2 c7c5 d2d4 d8c7 b1d2 a5c6 d4d5 c6a5 d2b3 a5b7 c1e3 c5c4 b3d2 c8d7 f3g5 h7h6 g5f3 b7c5 a1b1 a6a5 d1e2 a5a4 b1d1 a4a3 b2b4 c4b3 a2b3 a3a2 c2d3 f8c8 d1c1 b5b4 c3b4 c5d3 c1c7 d3e1 c7c8 d7c8 e2e1 a2a1q g1h2 a1c3 b4b5 a8a1 e1e2 c3c1 d2f1 c1f1 e2f1 a1f1 f3d2 f1e1 b5b6 e1e2 b3b4 e2d2 e3d2 f6e4 d2e3 e4f6 b4b5 f6d5 h2g3 d5e3 f2e3 e7d8 e3e4 d8b6 g3f3 f7f5 e4f5 c8f5 f3e2 e5e4 h3h4 f5g4";

    std::vector<std::string> moves;
    std::stringstream ss(str);
    std::string word;

    // Extract words from the stream
    while (ss >> word) {
        moves.push_back(word);
    }


    for(int i = 0; i < moves.size(); i++){
        g.doMove(g.stringToMove(moves[i]));
        g.printGame();
    }

    std::cout << "Undoing everything" << std::endl;

    for(int i = 0; i < moves.size(); i++){
        g.undoMove();
        g.printGame();
    }
    /* */
/*
    int counter = 0;
    while (true){
        std::string input;
        std::cin >> input;
        if(input == "break"){
            break;
        }
        g.doMove(g.stringToMove(input));
        g.printGame();
        counter++;
    }
    std::cout << "Undoing everything" << std::endl;
    for(int i = 0; i < counter; i++){
        g.undoMove();
        g.printGame();
    }
    /* */
#endif

    return 0;
}
