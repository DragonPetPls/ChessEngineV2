//
// Created by fabian on 10/15/24.
//

#ifndef CHESSENGINEV2_ENGINE_H
#define CHESSENGINEV2_ENGINE_H


#include "Game.h"

class Engine {
public:
    move getMove(Game g, int timeInMs);
    void stopSearch();

};


#endif //CHESSENGINEV2_ENGINE_H
