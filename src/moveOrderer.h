//
// Created by fabian on 10/18/24.
//

#ifndef CHESSENGINEV2_MOVEORDERER_H
#define CHESSENGINEV2_MOVEORDERER_H


#include "Game.h"
#include "engineConstants.h"

struct moveRank{
    int index;
    int score;
};

class moveOrderer {

private:
    bitboard majorPieces;
    bitboard minorPieces;
    bitboard diagonalChecks;
    bitboard straightChecks;
    bitboard knightChecks;

    int getNudge(move &m);

public:
    std::vector<int> rankMoves(Game &g, std::vector<move>& next, int bestCon);
};


#endif //CHESSENGINEV2_MOVEORDERER_H
