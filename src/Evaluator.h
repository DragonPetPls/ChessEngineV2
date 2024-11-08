//
// Created by fabian on 10/18/24.
//

#ifndef CHESSENGINEV2_EVALUATOR_H
#define CHESSENGINEV2_EVALUATOR_H

#include "Game.h"
#include "engineConstants.h"
#include "Evaluator.h"

struct moveRank{
    int index;
    int score;
};

class Evaluator {

private:
    static bitboard whitePassedPawns[64];
    static bitboard blackPassedPawns[64];
    bitboard rookBitboard;
    bitboard queenBitboard;
    bitboard knightBitboard;
    bitboard bishopBitboard;
    bitboard pawnBitboard;
    bitboard diagonalChecks;
    bitboard straightChecks;
    bitboard knightChecks;

    int getNudge(move &m, Game &g);
    static int getSquareValue(piece p, coord c, int endgame);
    static int getSquareValue(bitboard square, color pieceColor, piece pieceType);

    static const int* SIMPLE_TABLES[6];

public:
    static void initEvaluator();

    static uint64_t evalCounter;
    std::vector<int> rankMoves(Game &g, std::vector<move>& next, int bestCon);
    std::vector<int> filterQuiesceCandidates(Game &g, std::vector<move>& next);
    std::vector<int> filterNullMoves(Game &g, std::vector<move>& next, int bestCon);
    static int evalPosition(Game &g, int scoreToBeat);

    static int simpleEval(Game &g);
    static int getEvalDifference(move m, Game &g);
};


#endif //CHESSENGINEV2_EVALUATOR_H
