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
    bitboard rookBitboard;
    bitboard queenBitboard;
    bitboard knightBitboard;
    bitboard bishopBitboard;
    bitboard pawnBitboard;
    bitboard diagonalChecks;
    bitboard straightChecks;
    bitboard knightChecks;

    int getNudge(move &m, Game &g);
    static int getSquareValue(bitboard square, color pieceColor, piece pieceType);

public:
    std::vector<int> rankMoves(Game &g, std::vector<move>& next, int bestCon);
    std::vector<int> filterQuiesceCandidates(Game &g, std::vector<move>& next);
};


#endif //CHESSENGINEV2_MOVEORDERER_H
