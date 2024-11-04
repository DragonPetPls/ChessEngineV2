//
// Created by fabian on 10/18/24.
//

#include <algorithm>
#include "moveOrderer.h"
#include "Game.h"

int moveOrderer::getNudge(move &m, Game &g) {


    int nudge = 0;

    int prevScore = getSquareValue(m.startingSquare, g.getWhoToMove(), m.movingPiece);
    int newScore = getSquareValue(m.finalSquare, g.getWhoToMove(), m.movingPiece);

    nudge += newScore - prevScore;

    //Small bonus for taking minor pieces, bigger bonus for major pieces
    nudge += mg_value[0] * ((m.finalSquare & pawnBitboard) != 0);
    nudge += mg_value[1] * ((m.finalSquare & knightBitboard) != 0);
    nudge += mg_value[2] * ((m.finalSquare & bishopBitboard) != 0);
    nudge += mg_value[3] * ((m.finalSquare & rookBitboard) != 0);
    nudge += mg_value[4] * ((m.finalSquare & queenBitboard) != 0);

    //Checks:
    switch (m.movingPiece) {
        case QUEEN:
            nudge += checkValue * ((m.finalSquare & straightChecks) != 0);
            nudge += checkValue * ((m.finalSquare & diagonalChecks) != 0);
            break;
        case ROOK:
            nudge += checkValue * ((m.finalSquare & straightChecks) != 0);
            break;
        case BISHOP:
            nudge += checkValue * ((m.finalSquare & diagonalChecks) != 0);
            break;
        case KNIGHT:
            nudge += checkValue * ((m.finalSquare & knightChecks) != 0);
            break;
    }

    if(m.promotion != NONE){
        nudge += (m.promotion != NONE) * mg_value[m.promotion] - mg_value[PAWN];
    }

    return nudge;
}

std::vector<int> moveOrderer::rankMoves(Game &g, std::vector<move> &next, int bestCon) {

    //Initialising bitboards
    queenBitboard = g.getPieceBoards()[QUEEN + g.getWhoNotToMove()];
    rookBitboard = g.getPieceBoards()[ROOK + g.getWhoNotToMove()];

    knightBitboard = g.getPieceBoards()[KNIGHT + g.getWhoNotToMove()];
    bishopBitboard = g.getPieceBoards()[BISHOP + g.getWhoNotToMove()];
    pawnBitboard = g.getPieceBoards()[PAWN + g.getWhoNotToMove()];

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
        mr.score = getNudge(next[i], g);
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
    queenBitboard = g.getPieceBoards()[QUEEN + g.getWhoNotToMove()];
    rookBitboard = g.getPieceBoards()[ROOK + g.getWhoNotToMove()];

    knightBitboard = g.getPieceBoards()[KNIGHT + g.getWhoNotToMove()];
    bishopBitboard = g.getPieceBoards()[BISHOP + g.getWhoNotToMove()];
    pawnBitboard = g.getPieceBoards()[PAWN + g.getWhoNotToMove()];

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
            mr.score = getNudge(next[i], g);
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

int moveOrderer::getSquareValue(bitboard square, color pieceColor, piece pieceType) {
    int x = Game::getXCoord(square);
    int y = Game::getYCoord(square);

    y = y * (pieceColor == WHITE) + (7 - y) * (pieceColor == BLACK);
    int index = (7 - y) * 8 + x;

    int value;
    switch (pieceType) {
        case QUEEN:
            value = mg_queen_table[index];
            break;
        case ROOK:
            value = mg_rook_table[index];
            break;
        case BISHOP:
            value = mg_bishop_table[index];
            break;
        case KNIGHT:
            value = mg_knight_table[index];
            break;
        case PAWN:
            value = mg_pawn_table[index];
            break;
        case KING:
            value = mg_king_table[index];
            break;
        default:
            // Handle the case where the piece 'p' is invalid or unrecognized
            value = 0;
            break;
    }

    return value;
}

std::vector<int> moveOrderer::filterNullMoves(Game &g, std::vector<move> &next, int bestCon) {

    //Initialising bitboards
    queenBitboard = g.getPieceBoards()[QUEEN + g.getWhoNotToMove()];
    rookBitboard = g.getPieceBoards()[ROOK + g.getWhoNotToMove()];

    knightBitboard = g.getPieceBoards()[KNIGHT + g.getWhoNotToMove()];
    bishopBitboard = g.getPieceBoards()[BISHOP + g.getWhoNotToMove()];
    pawnBitboard = g.getPieceBoards()[PAWN + g.getWhoNotToMove()];

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
        mr.score = getNudge(next[i], g);
        mr.score += 1000000 * (i == bestCon);
        if(mr.score > 0){
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
