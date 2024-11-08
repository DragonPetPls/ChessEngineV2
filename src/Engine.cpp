//
// Created by fabian on 10/15/24.
//

#include "Engine.h"
#include "Game.h"
#include "Evaluator.h"
#include  <cmath>
#include <thread>

move Engine::getMove(Game g, int timeLeft, int incTime, int moveTime) {

    int timeInMs;
    if(moveTime == 0) {
        //match time
        recommendedSearchTime = std::max(timeLeft/25, incTime + timeLeft/40);
        timeInMs = std::min(timeLeft, recommendedSearchTime * 3);
    } else {
        //Move time
        timeInMs = moveTime;
        recommendedSearchTime = PLUS_INF;
    }

    searchStartTime = std::chrono::high_resolution_clock::now();
    std::cout << recommendedSearchTime << std::endl;

    //Starting the thread
    keepRunning = true;
    searchCompleted = false;
    std::thread searchThread(&Engine::search, this, g, MAX);

    //Waiting the time
    std::unique_lock<std::mutex> lock(searchMtx);
    if (cv.wait_for(lock, std::chrono::milliseconds(timeInMs - timeTolerance), [this] { return searchCompleted.load(); })) {
        std::cout << "Search was cancelled.\n";
    } else {
        std::cout << "Emergency break out of search\n";
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(timeInMs));

    //Stopping the search
    keepRunning = false;

    if (searchThread.joinable()) {
        searchThread.join();  // Ensures the thread completes execution
    }

    auto next = g.getAllPseudoLegalMoves();
    return next[bestContinuation];

}

void Engine::stopSearch() {

}


/*
 * Performs search
 */
int Engine::search(Game g, int toDepth) {
    if(toDepth != MAX){
        keepRunning = true;
    }

    //hashTable.clear();
    //hashTable.reserve(40000000);

    int depth = 1;
    int score;

    if(g.getStatus() != ON_GOING){
        return Evaluator::evalPosition(g, -MINUS_INF);
    }

    while (keepRunning && (depth <= toDepth)) {
        score = negamax(g, depth, MINUS_INF, PLUS_INF, depth);
        if (keepRunning) {
            bestContinuation = transpositionTable.writeEntry(g, MINUS_INF, PLUS_INF, depth)->bestCon;
            std::cout << "info depth " << depth << " score cp " << score << std::endl;
        }
        if(score > WIN - 10000 || depth > 100){
            break;
        }
        depth++;

        //If we assume that a new search iteration would take to long exit early
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - searchStartTime);
        if(keepRunning && timeElapsed.count() > recommendedTimeDivisor * recommendedSearchTime && toDepth == MAX){
            std::cout << "quit " << timeElapsed.count() << std::endl;
            searchCompleted = true;
            cv.notify_one();
            break;
        }
    }
    return score;
}

/*
 * A standart negamax function with multiple improvements
 */
int Engine::negamax(Game &g, int depth, int alpha, int beta, int toDepth) {
    int nBestCon = UNKNOWN;

    if(!keepRunning){
        return 0;
    }

    //Exit condition
    if (g.getStatus() != ON_GOING) {
        //Position is the end of the game
        node* nodePtr = transpositionTable.readEntry(g, alpha, beta, depth);
        if (nodePtr == nullptr) {
            int score = Evaluator::evalPosition(g, beta);
            setNode(g, score, 0, MINUS_INF, PLUS_INF, true);
            return score;
        }
        return nodePtr->score;
    }

    //Checking existing hashtable if the entry already exists
    node* nodePtrRead = transpositionTable.readEntry(g, alpha, beta, depth);
    if (nodePtrRead != nullptr) {
        node *n = nodePtrRead;

        if (n->depth >= depth
            && n->alpha <= alpha
            && n->beta >= beta) {
            return n->score;
        } else if (n->isOver) {
            return n->score;
        }
        nBestCon = n->bestCon;
        if(n->score < alpha - 100){
            depth--;
        }
    }

    if (depth <= 0) {
        //Exit due to depth
        setNode(g, quiesce(g, alpha, beta), depth, alpha, beta, false);
        return transpositionTable.readEntry(g, alpha, beta, depth)->score;
    }

    //Going though future moves
    auto next = g.getAllPseudoLegalMoves();
    int bestCon = UNKNOWN;
    Evaluator o;
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
    node *n = transpositionTable.writeEntry(g, alpha, beta, depth);
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

    int stand_pat = Evaluator::evalPosition(g, beta);
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
    Evaluator mo{};
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

