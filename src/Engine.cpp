//
// Created by fabian on 10/15/24.
//

#include "Engine.h"

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
