//
// Created by fabian on 10/18/24.
//

#include <algorithm>
#include "moveOrderer.h"
#include "Game.h"

int moveOrderer::getNudge(move &m) {

    int nudge = 0;

    //Small bonus for taking minor pieces, bigger bonus for major pieces
    nudge += 30 * ((m.finalSquare & minorPieces) != 0);
    nudge += 60 * ((m.finalSquare & majorPieces) != 0);

    //Checks:
    switch (m.movingPiece) {
        case QUEEN:
            nudge += 15 * ((m.finalSquare & straightChecks) != 0);
            nudge += 15 * ((m.finalSquare & diagonalChecks) != 0);
            break;
        case ROOK:
            nudge += 15 * ((m.finalSquare & straightChecks) != 0);
            break;
        case BISHOP:
            nudge += 15 * ((m.finalSquare & diagonalChecks) != 0);
            break;
        case KNIGHT:
            nudge += 15 * ((m.finalSquare & knightChecks) != 0);
            break;
    }


    //Moving into the centre
    nudge += 5 * ((66229406269440 & m.finalSquare) != 0);

    //Moving from the start rank
    nudge += 1 * ((18446462598732906495 & m.startingSquare) != 0);


    return nudge;
}

std::vector<int> moveOrderer::rankMoves(Game &g, std::vector<move> &next, int bestCon) {

    //Initialising bitboards
    majorPieces = g.getPieceBoards()[QUEEN + g.getWhoNotToMove()];
    majorPieces |= g.getPieceBoards()[ROOK + g.getWhoNotToMove()];

    minorPieces = g.getPieceBoards()[KNIGHT + g.getWhoNotToMove()];
    minorPieces |= g.getPieceBoards()[BISHOP + g.getWhoNotToMove()];

    bitboard hitmap = 0;
    for(int i = 0; i < 12; i++){
        hitmap |= g.getPieceBoards()[i];
    }

    coord enemyKing;
    enemyKing.x = g.getXCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);
    enemyKing.y = g.getYCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);

    diagonalChecks = g.magic.getDiagonalAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    straightChecks = g.magic.getStraightAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    knightChecks = g.knightLookup[enemyKing.x + enemyKing.y * 8].allSquares;

    std::vector<moveRank> order;
    order.reserve(next.size());
    //Getting all scores
    for(int i = 0; i < next.size(); i++){
        moveRank mr;
        mr.score = getNudge(next[i]);
        mr.score += 1000000 * (i == bestCon);
        mr.index = i;
        order.push_back(mr);
    }

    // Sort the vector by the score in ascending order
    std::sort(order.begin(), order.end(), [](const moveRank &a, const moveRank &b) {
        return a.score > b.score;  // compare scores for sorting
    });

    std::vector<int> intOrder;
    intOrder.reserve(order.size());
    for(moveRank mr: order){
        intOrder.push_back(mr.index);
    }

    return intOrder;
}

/*
 * This function returns a vector of index that points to all captures, checks that arent made with pawns and promotions
 */
std::vector<int> moveOrderer::filterQuiesceCandidates(Game &g, std::vector<move> &next) {
    //Initialising bitboards

    bitboard hitmap = 0;
    for(int i = 0; i < 12; i++){
        hitmap |= g.getPieceBoards()[i];
    }

    coord enemyKing;
    enemyKing.x = g.getXCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);
    enemyKing.y = g.getYCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);

    diagonalChecks = g.magic.getDiagonalAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    straightChecks = g.magic.getStraightAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    knightChecks = g.knightLookup[enemyKing.x + enemyKing.y * 8].allSquares;

    bitboard block = diagonalChecks | straightChecks;

    std::vector<moveRank> order;
    order.reserve(next.size());
    //checking all moves
    for(int i = 0; i < next.size(); i++){
        //Checks:
        bool isCheck;

        if(next[i].movingPiece != PAWN && !(next[i].startingSquare & block)){
            //No discovered check or pawn check
            switch(next[i].movingPiece){
                case QUEEN:
                    isCheck = (diagonalChecks | straightChecks) & next[i].finalSquare;
                    break;
                case ROOK:
                    isCheck = straightChecks & next[i].finalSquare;
                    break;
                case BISHOP:
                    isCheck = diagonalChecks & next[i].finalSquare;
                    break;
                case KNIGHT:
                    isCheck = knightChecks & next[i].finalSquare;
                    break;
                default: isCheck = true;
            }
        } else {
            //PAWN or potential discovered check
            g.doMove(next[i]);
            isCheck = !g.isKingSafe(g.getWhoToMove());
            g.undoMove();
        }

        if(isCheck
        || next[i].finalSquare & hitmap
        || next[i].promotion != NONE){
            moveRank mr;
            mr.score = getNudge(next[i]);
            mr.index = i;
            order.push_back(mr);
        }
    }

    // Sort the vector by the score in ascending order
    std::sort(order.begin(), order.end(), [](const moveRank &a, const moveRank &b) {
        return a.score > b.score;  // compare scores for sorting
    });

    std::vector<int> intOrder;
    intOrder.reserve(order.size());
    for(moveRank mr: order){
        intOrder.push_back(mr.index);
    }

    return intOrder;
}
