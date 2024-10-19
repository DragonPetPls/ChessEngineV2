#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include "Game.h"
#include "Communication.h"
#include "magicBitboards.h"

//#define GAME_DEBUG
//#define MAGIC_DEBUG
//#define ENGINE_DEBUG

#ifdef GAME_DEBUG
int nodesSearched = 0;
int perftSearch(Game &g, int depth, bool printInfo);
#endif

int main() {

#ifndef GAME_DEBUG
#ifndef MAGIC_DEBUG
#ifndef ENGINE_DEBUG
    std::cout << "Yay!" << std::endl;
    Communication c;
    c.startCommunication();
#endif
#endif
#endif

    // Doing all test for the Game Class
#ifdef GAME_DEBUG

    Game g;
    std::string fen[6] = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
                          "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",
                          "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ",
                          "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ",
                          "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "};
    int depth[6] = {5, 4, 5, 4, 4, 4};
    int solution[6] = {4865609, 4085603, 674624, 422333, 2103487, 3894594};

    nodesSearched = 0;
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "testing..." << std::endl;
    //6 Perft test
    for(int i = 0; i < 6; i++){
        g.loadFen(fen[i]);
        int result = perftSearch(g, depth[i], false);
        if(result == solution[i]){
            std::cout << "Test " << i << " successful" << std::endl;
        } else {
            std::cout << "Error in Test " << i << ": Result: " << result << " Correct: " << solution[i] << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << nodesSearched << " nodes were searched." << std::endl;
    std::cout << nodesSearched / elapsed.count() << " nodes per second" << std::endl;

#endif


#ifdef MAGIC_DEBUG
    magicBitboards mb;
    //mb.calculateStraightMagicNumbers();
    //mb.calculateDiagonalMagicNumbers();
#endif


#ifdef ENGINE_DEBUG

    Game g;
    Engine e;

    std::string fen[6] = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                          "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
                          "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",
                          "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ",
                          "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ",
                          "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "};
    int depth[6] = {6, 6, 6, 6, 6, 6};
    e.evalCounter = 0;

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "testing..." << std::endl;
    //6 Perft test
    for(int i = 0; i < 6; i++){
        g.loadFen(fen[i]);
        int eval =  e.search(g, depth[i]);
        std::cout << "Position " << i + 1 << " finished. Eval: " << eval << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "The test took " << elapsed.count() << " seconds" << std::endl;
    std::cout << e.evalCounter << " evaluations performed." << std::endl;


#endif


    return 0;
}

#ifdef GAME_DEBUG

int perftSearch(Game &g, int depth, bool printInfo){
    nodesSearched++;
    //Exit condition
    if(depth == 0){
        return 1;
    }
    if(g.getStatus() != ON_GOING){
        return 0;
    }


    //Continue searching
    std::vector<move> moves = g.getAllPseudoLegalMoves();
    int sum = 0;
    int x;
    for(auto move: moves){
        g.doMove(move);
        if(!g.isPositionLegal()){
            g.undoMove();
            continue;
        }
        x = perftSearch(g, depth - 1, false);
        sum += x;
        if(printInfo){
            //g.printGame();
            g.printMove(move);
            std::cout << " ";
            std::cout << x << std::endl;
        }
        g.undoMove();
    }
    return sum;
}

#endif