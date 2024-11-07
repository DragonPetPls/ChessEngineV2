//
// Created by fabian on 10/15/24.
//

#ifndef CHESSENGINEV2_ENGINE_H
#define CHESSENGINEV2_ENGINE_H


#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "Game.h"
#include "engineConstants.h"

class Engine {
private:
    bitboard whitePassedPawns[64];
    bitboard blackPassedPawns[64];
    std::atomic<bool> keepRunning;

    int bestContinuation;
    std::chrono::time_point<std::chrono::system_clock> searchStartTime;
    int recommendedSearchTime;
    std::atomic<bool> searchCompleted;
    std::mutex searchMtx;
    std::condition_variable cv;

    int negamax(Game &g, int depth, int alpha, int beta, int toDepth);
    std::unordered_map<GameKey, node> hashTable;
    void setNode(Game &g, int score, int depth, int alpha = MINUS_INF, int beta = PLUS_INF, bool isOver = false, int bestCon = UNKNOWN);
    int quiesce(Game &g, int alpha, int beta, int depth = 0);


public:
    Engine();
    int evalCounter = 0;
    int search(Game g, int toDepth = MAX);

    move getMove(Game g, int timeLeft, int incTime, int moveTime);
    void stopSearch();
};


#endif //CHESSENGINEV2_ENGINE_H
