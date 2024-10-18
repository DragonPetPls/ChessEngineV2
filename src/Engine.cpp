//
// Created by fabian on 10/15/24.
//

#include "Engine.h"
#include "Game.h"
#include "moveOrderer.h"
#include  <cmath>
#include <thread>

move Engine::getMove(Game g, int timeInMs) {
    int index;
    int bestScore = MINUS_INF;


    //Starting the thread
    keepRunning = true;
    std::thread searchThread(&Engine::search, this, g, MAX);

    //Waiting the time
    std::this_thread::sleep_for(std::chrono::milliseconds(timeInMs));

    //Stopping the search
    keepRunning = false;

    if (searchThread.joinable()) {
        searchThread.join();  // Ensures the thread completes execution
    }


    /*
    //Getting the best move from the hash table
    for(int i = 0; i < next.size(); i++){
        g.doMove(next[i]);
        if(!g.isPositionLegal()){
            continue;
        }
        if(hashTable.find(g.toKey()) == hashTable.end()){
            if(bestScore == MINUS_INF){
                index = i;
            }
        }

        int score = hashTable[g.toKey()].score;
     */
    auto next = g.getAllPseudoLegalMoves();
    return next[bestContinuation];

}

void Engine::stopSearch() {

}


int Engine::evalPosition(Game &g) {
    evalCounter++;
    switch (g.getStatus()) {
        case DRAW:
            return 0;
        case CHECKMATE:
            return LOSS + g.moveCount;
        default:
            break;
    }

    //Getting piece value
    std::vector<coord> pieceLocations[12];
    for (int i = 0; i < 12; i++) {
        pieceLocations[i] = g.locatePieces(g.getPieceBoards()[i]);
    }

    int eval = 0;
    for (int i = 0; i < 6; i++) {
        eval += pieceLocations[i + g.getWhoToMove()].size() * value[i];
        eval -= pieceLocations[i + g.getWhoNotToMove()].size() * value[i];
    }

    for (int i = 0; i < 6; i++) {
        for (coord c: pieceLocations[i + g.getWhoToMove()]) {
            eval += getSquareValue(i, c);
        }
        for (coord c: pieceLocations[i + g.getWhoNotToMove()]) {
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
        default:
            return 0;
    }


    return 0;
}

int Engine::search(Game g, int toDepth) {
    if(toDepth != MAX){
        keepRunning = true;
    }

    int depth = 1;
    int score;

    while (keepRunning && (depth <= toDepth)) {
        score = negamax(g, depth, MINUS_INF, PLUS_INF);
        if (keepRunning) {
            bestContinuation = hashTable[g.toKey()].bestCon;
            std::cout << "info depth " << depth << " score cp " << score << std::endl;
        }
        depth++;
    }
    return score;
}

int Engine::negamax(Game &g, int depth, int alpha, int beta) {
    int nBestCon = UNKNOWN;

    if(!keepRunning){
        return 0;
    }

    //Exit condition
    if (g.getStatus() != ON_GOING) {
        //Position is the end of the game
        if (hashTable.find(g.toKey()) == hashTable.end()) {
            setNode(g, evalPosition(g), 0, MINUS_INF, PLUS_INF, true);
        }
        return hashTable[g.toKey()].score;
    }

    //Checking existing hashtable if the entry already exists
    if (hashTable.find(g.toKey()) != hashTable.end()) {
        node *n = &hashTable[g.toKey()];

        if (n->depth >= depth
            && n->alpha <= alpha
            && n->beta >= beta) {
            return n->score;
        } else if (n->isOver) {
            g.printGame();
            return n->score;
        }
        nBestCon = n->bestCon;
    }

    if (depth == 0) {
        //Exit due to depth
        setNode(g, quiesce(g, alpha, beta), 0, alpha, beta, false);
        return hashTable[g.toKey()].score;
    }

    //Going though future moves
    auto next = g.getAllPseudoLegalMoves();
    int bestCon = UNKNOWN;
    moveOrderer o;
    auto order = o.rankMoves(g, next, nBestCon);

    int highestScore = LOSS;
    for (int i = 0; i < next.size(); i++) {
        g.doMove(next[order[i]]);
        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }

        int score = -negamax(g, depth - 1, -beta, -alpha);
        //highestScore = std::max(score, highestScore);
        if (highestScore < score) {
            highestScore = score;
            bestCon = order[i];
        }
        if (score > alpha) {
            alpha = score;
        }
        if (score >= beta) {
            g.undoMove();
            break;
        }
        g.undoMove();
    }

    setNode(g, highestScore, depth, alpha, beta, false, bestCon);
    return highestScore;
}

void Engine::setNode(Game &g, int score, int depth, int alpha, int beta, bool isOver, int bestCon) {
    if (hashTable.find(g.toKey()) == hashTable.end()) {
        node n;
        hashTable.insert(std::make_pair(g.toKey(), n));
    }
    node *n = &hashTable[g.toKey()];
    n->depth = depth;
    n->score = score;
    n->alpha = alpha;
    n->beta = beta;
    n->isOver = isOver;
    n->bestCon = bestCon;
}

int Engine::quiesce(Game &g, int alpha, int beta, int depth) {
    int stand_pat = evalPosition(g);
    //We return if the move was worse than the original position
    if(stand_pat > beta){
        return beta;
    }

    if(depth == 0){
        return stand_pat;
    }

    //Checking if the game is over
    if(g.getStatus() != ON_GOING){
        return stand_pat;
    }

    if(alpha < stand_pat){
        alpha = stand_pat;
    }

    auto next = g.getAllPseudoLegalMoves();
    moveOrderer mo;
    auto selection = mo.filterQuiesceCandidates(g, next);
    for(int index: selection){
        g.doMove(next[index]);
        int score = -quiesce(g, -beta, -alpha, depth - 1);
        g.undoMove();
        if( score >= beta )
            return beta;
        if( score > alpha )
            alpha = score;
    }

    return alpha;
}

