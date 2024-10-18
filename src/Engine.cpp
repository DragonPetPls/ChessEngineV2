//
// Created by fabian on 10/15/24.
//

#include "Engine.h"
#include "Game.h"
#include "moveOrderer.h"
#include  <cmath>

move Engine::getMove(Game g, int timeInMs) {
    auto n = g.getAllPseudoLegalMoves();
    for(auto move: n){
        g.doMove(move);
        if(g.isPositionLegal()){
            return move;
        } else {
            g.undoMove();
        }
    }
}

void Engine::stopSearch() {

}


int Engine::evalPosition(Game &g) {
    evalCounter++;
    switch (g.getStatus()) {
        case DRAW: return 0;
        case CHECKMATE: return LOSS;
        default: break;
    }

    //Getting piece value
    std::vector<coord> pieceLocations[12];
    for(int i = 0; i < 12; i++){
        pieceLocations[i] = g.locatePieces(g.getPieceBoards()[i]);
    }

    int eval = 0;
    for(int i = 0; i < 6; i++){
        eval += pieceLocations[i + g.getWhoToMove()].size() * value[i];
        eval -= pieceLocations[i + g.getWhoNotToMove()].size() * value[i];
    }

    for(int i = 0; i < 6; i++){
        for (coord c: pieceLocations[i + g.getWhoToMove()]){
            eval += getSquareValue(i, c);
        }
        for (coord c: pieceLocations[i + g.getWhoNotToMove()]){
            eval -= getSquareValue(i, c);
        }
    }

    return eval;
}

int Engine::getSquareValue(piece p, coord c) {

    int y = c.y * (p < 6) + (7 - c.y) * (p >= 6);
    int index = (7 - y) * 8 + c.x;
    p = p - 6 * (p > 5);
    switch (p) {
        case PAWN:
            return pawnSquareTable[index];
        case KNIGHT:
            return knightSquareTable[index];
        case BISHOP:
            return bishopSquareTable[index];
        case ROOK:
            return rookSquareTable[index];
        case QUEEN:
            return queenSquareTable[index];
        case KING:
            return kingSquareTable[index];
        default: return 0;
    }


    return 0;
}

int Engine::search(Game g, int toDepth) {
    int depth = 1;
    int score;

    while(keepRunning || ((toDepth != MAX) && depth <= toDepth)){
        score = negamax(g, depth, MINUS_INF, PLUS_INF);
        depth++;
    }
    return score;
}

int Engine::negamax(Game &g, int depth, int alpha, int beta) {
    int nBestCon = UNKNOWN;

    //Checking existing hashtable if the entry already exists
    if(hashTable.find(g.toKey()) != hashTable.end()){
        node *n = &hashTable[g.toKey()];

        if(n->depth >= depth
        && n->alpha <= alpha
        && n->beta >= beta){
            return n->score;
        } else if (n->isOver){
            g.printGame();
            return n->score;
        }
        nBestCon = n->bestCon;
    }

    //Exit condition
    if (g.getStatus() != ON_GOING){
        //Position is the end of the game
        if(hashTable.find(g.toKey()) == hashTable.end()){
            setNode(g, evalPosition(g), 0, MINUS_INF, PLUS_INF, true);
        } else {
            return hashTable[g.toKey()].score;
        }

    } else if(depth == 0){
        //Exit due to depth
        setNode(g, evalPosition(g), 0, alpha, beta, false);
        return hashTable[g.toKey()].score;
    }

    //Going though future moves
    auto next = g.getAllPseudoLegalMoves();
    int bestCon = UNKNOWN;
    moveOrderer o;
    auto order = o.rankMoves(g, next, nBestCon);

    int highestScore = LOSS;
    for(int i = 0; i < next.size(); i++){
        g.doMove(next[order[i]]);
        if(!g.isPositionLegal()){
            g.undoMove();
            continue;
        }

        int score = -negamax(g, depth - 1, -beta, -alpha);
        //highestScore = std::max(score, highestScore);
        if(highestScore < score){
            highestScore = score;
            bestCon = order[i];
        }
        if(score > alpha){
            alpha = score;
        }
        if (score >= beta){
            g.undoMove();
            break;
        }
        g.undoMove();
    }

    setNode(g, highestScore, depth, alpha, beta, false, bestCon);
    return highestScore;
}

void Engine::setNode(Game &g, int score, int depth, int alpha, int beta, bool isOver, int bestCon) {
    if(hashTable.find(g.toKey()) == hashTable.end()){
        node n;
        hashTable.insert(std::make_pair(g.toKey(), n));
    }
    node* n = &hashTable[g.toKey()];
    n->depth = depth;
    n->score = score;
    n->alpha = alpha;
    n->beta = beta;
    n->isOver = isOver;
    n->bestCon = bestCon;
}

