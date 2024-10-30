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

    //Checking how endgame it is:
    int value = 0;
    value += pieceLocations[ROOK + WHITE].size() * 5;
    value += pieceLocations[ROOK + BLACK].size() * 5;
    value += pieceLocations[QUEEN + WHITE].size() * 9;
    value += pieceLocations[QUEEN + BLACK].size() * 9;
    value += pieceLocations[KNIGHT + WHITE].size() * 3;
    value += pieceLocations[KNIGHT + BLACK].size() * 3;
    value += pieceLocations[BISHOP + WHITE].size() * 3;
    value += pieceLocations[BISHOP + BLACK].size() * 3;

    int endgame = 10 * (value <= 7) + (-0.5 * value + 13) * (value > 7 && value < 26);

    int eval = 0;
    for (int i = 0; i < 6; i++) {
        eval += (pieceLocations[i + g.getWhoToMove()].size() * (eg_value[i] * endgame + mg_value[i] * (10 - endgame)))/10;
        eval -= (pieceLocations[i + g.getWhoNotToMove()].size() * (eg_value[i] * endgame + mg_value[i] * (10 - endgame)))/10;
    }

    for (int i = 0; i < 6; i++) {
        for (coord c: pieceLocations[i + g.getWhoToMove()]) {
            eval += getSquareValue(i + g.getWhoToMove(), c, endgame);
        }
        for (coord c: pieceLocations[i + g.getWhoNotToMove()]) {
            eval -= getSquareValue(i + g.getWhoNotToMove(), c, endgame);
        }
    }

    //Checking for passed pawns
    int whiteEval = 0;
    for(coord c: pieceLocations[WHITE + PAWN]){
        whiteEval += 400 * ((whitePassedPawns[c.x + 8 * c.y] & g.getPieceBoards()[PAWN + BLACK]) == 0);
    }
    int blackEval = 0;
    for(coord c: pieceLocations[BLACK + PAWN]){
        blackEval += 400 * ((blackPassedPawns[c.x + 8 * c.y] & g.getPieceBoards()[PAWN + WHITE]) == 0);
    }
    if(g.getWhoToMove() == WHITE){
        eval += (whiteEval - blackEval)/(14 - endgame);
    } else {
        eval += (blackEval - whiteEval)/(14 - endgame);
    }
/*
    //King pawn distance:
    int xKing = Game::getXCoord(g.getPieceBoards()[KING + g.getWhoToMove()]);
    int yKing = Game::getYCoord(g.getPieceBoards()[KING + g.getWhoToMove()]);
    auto pawnLocation = Game::locatePieces(g.getPieceBoards()[PAWN + g.getWhoToMove()]);
    int distance = 0;
    for(coord c: pawnLocation){
        distance += std::min(abs(xKing - c.x), abs(yKing - c.y));

        //Bonus for pawns protecting other pawns
       eval += pawnProtectionBonus * ((g.getPieceBoards()[g.getWhoToMove() + PAWN] & Game::generateBitboard(c.x + 1, c.y + 1)) != 0);
       eval += pawnProtectionBonus * ((g.getPieceBoards()[g.getWhoToMove() + PAWN] & Game::generateBitboard(c.x - 1, c.y + 1)) != 0);
    }
    eval -= distance * endgame;

    //King pawn distance:
    xKing = Game::getXCoord(g.getPieceBoards()[KING + g.getWhoNotToMove()]);
    yKing = Game::getYCoord(g.getPieceBoards()[KING + g.getWhoNotToMove()]);
    pawnLocation = Game::locatePieces(g.getPieceBoards()[PAWN + g.getWhoNotToMove()]);
    distance = 0;
    for(coord c: pawnLocation){
        distance += std::min(abs(xKing - c.x), abs(yKing - c.y));

        //Bonus for pawns protecting other pawns
       eval -= pawnProtectionBonus * ((g.getPieceBoards()[g.getWhoNotToMove() + PAWN] & Game::generateBitboard(c.x + 1, c.y + 1)) != 0);
       eval -= pawnProtectionBonus * ((g.getPieceBoards()[g.getWhoNotToMove() + PAWN] & Game::generateBitboard(c.x - 1, c.y + 1)) != 0);
    }
    eval += distance * endgame;
    */
    return eval;
}

/*
 * Returns the value a piece gets due to being on a square. the int endgame is how much endgame tables shuld be weighted,
 * with 10 being only endgame and 0 only midgame
 */
int Engine::getSquareValue(piece p, coord c, int endgame) {

    int y = c.y * (p < 6) + (7 - c.y) * (p >= 6);
    int index = (7 - y) * 8 + c.x;
    p = p - 6 * (p > 5);

    int endgameValue;
    int midgameValue;
    switch (p) {
        case QUEEN:
            endgameValue = eg_queen_table[index];
            midgameValue = mg_queen_table[index];
            break;
        case ROOK:
            endgameValue = eg_rook_table[index];
            midgameValue = mg_rook_table[index];
            break;
        case BISHOP:
            endgameValue = eg_bishop_table[index];
            midgameValue = mg_bishop_table[index];
            break;
        case KNIGHT:
            endgameValue = eg_knight_table[index];
            midgameValue = mg_knight_table[index];
            break;
        case PAWN:
            endgameValue = eg_pawn_table[index];
            midgameValue = mg_pawn_table[index];
            break;
        case KING:
            endgameValue = eg_king_table[index];
            midgameValue = mg_king_table[index];
            break;
        default:
            // Handle the case where the piece 'p' is invalid or unrecognized
            endgameValue = 0;
            midgameValue = 0;
            break;
    }

    //Weighting differently depending on the endgame factor
    return (endgameValue * endgame + midgameValue * (10 - endgame))/10;
}

int Engine::search(Game g, int toDepth) {
    if(toDepth != MAX){
        keepRunning = true;
    }

    hashTable.clear();
    hashTable.reserve(40000000);

    int depth = 1;
    int score;

    if(g.getStatus() != ON_GOING){
        return evalPosition(g);
    }

    while (keepRunning && (depth <= toDepth)) {
        score = negamax(g, depth, MINUS_INF, PLUS_INF, depth);
        if (keepRunning) {
            bestContinuation = hashTable[g.toKey()].bestCon;
            std::cout << "info depth " << depth << " score cp " << score << std::endl;
        }
        if(score > WIN - 10000 || depth > 100){
            break;
        }
        depth++;
    }
    return score;
}

int Engine::negamax(Game &g, int depth, int alpha, int beta, int toDepth) {
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
            return n->score;
        }
        nBestCon = n->bestCon;
    }

    if (depth <= 0) {
        //Exit due to depth
        setNode(g, quiesce(g, alpha, beta), 0, alpha, beta, false);
        return hashTable[g.toKey()].score;
    }

    //Going though future moves
    auto next = g.getAllPseudoLegalMoves();
    int bestCon = UNKNOWN;
    moveOrderer o;
    auto order = o.rankMoves(g, next, nBestCon);
    bool failedHigh = false;
    int highestScore = LOSS;
    for (int i = 0; i < next.size(); i++) {
        g.doMove(next[order[i]]);
        if (!g.isPositionLegal()) {
            g.undoMove();
            continue;
        }

        //Late move reduction
        int score;
        int depthReduction;
        if((depth < toDepth * 0.65 + 1 && i > 3) && !failedHigh){
            depthReduction = 2;
        } else {
            depthReduction = 1;
        }


        if(i == 0){
            //Principle variation
            score = -negamax(g, depth - depthReduction, -beta, -alpha, toDepth);
        } else {
            //Search with null window
            score = -negamax(g, depth - depthReduction, -alpha - 1, -alpha, toDepth);

            if(score > alpha && score < beta){
                //Search with full window
                score = -negamax(g, depth - depthReduction, -beta, -alpha, toDepth);
            }
        }

        if(i < 4 && score == -alpha){
            failedHigh = true;
        }

/* */

        //highestScore = std::max(score, highestScore);
        if (highestScore < score) {
            highestScore = score;
            bestCon = order[i];
        }
        if (score > alpha) {
            alpha = score;
        }
        if (score >= beta) {
            highestScore = beta;
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
    if(!keepRunning){
        return 0;
    }

    int stand_pat = evalPosition(g);
    //We return if the move was worse than the original position

    if(stand_pat > beta){
        return beta;
    }

    if(depth < -4){
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
        if(!g.isPositionLegal()){
            g.undoMove();
            continue;
        }
        int score = -quiesce(g, -beta, -alpha, depth - 1);
        g.undoMove();
        if( score >= beta ){
            return beta;
        }
        if( score > alpha ) {
            alpha = score;
        }
    }

    return alpha;
}

Engine::Engine() {
    //Initialising passed pawn bitboards
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){

            bitboard colloms = 0;
            if(x == 0){
                colloms |= COLLOMS[0] | COLLOMS[1];
            } else if (x == 7){
                colloms |= COLLOMS[6] | COLLOMS[7];
            } else {
                colloms |= COLLOMS[x - 1] | COLLOMS[x] | COLLOMS[x + 1];
            }

            whitePassedPawns[x +  8 * y] = colloms;
            blackPassedPawns[x + 8 * y] = colloms;

            for(int i = 0; i < 8; i++){
                if(i <= y){
                    whitePassedPawns[x + 8 * y] &= ~ROWS[i];
                }
                if(i >= y){
                    blackPassedPawns[x + 8 * y] &= ~ROWS[i];
                }
            }
        }
    }
}

