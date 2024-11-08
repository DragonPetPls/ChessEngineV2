//
// Created by fabian on 11/8/24.
//

#ifndef CHESSENGINEV2_TRANSPOSITIONTABLE_H
#define CHESSENGINEV2_TRANSPOSITIONTABLE_H

#include "engineConstants.h"
#include "Game.h"

class TranspositionTable {
private:

    struct entry{
        node n;
        bool isEmpty;
        GameKey key;
    };

    static const int TABLE_SIZE = 40000000;
    static const int DIFFERENT_POSITION = 13123;
    static const int WIDER_WINDOW = 1345;
    entry* table = new entry[TABLE_SIZE];

public:
    TranspositionTable();
    void clear();

    node* readEntry(Game &game, int alpha, int beta, int depth);
    node* writeEntry(Game &game, int alpha, int beta, int depth);
};


#endif //CHESSENGINEV2_TRANSPOSITIONTABLE_H
