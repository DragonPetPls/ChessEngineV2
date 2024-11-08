//
// Created by fabian on 11/8/24.
//

#include "TranspositionTable.h"

/*
 * Every entry is set to empty
 */
void TranspositionTable::clear() {
    for(int i = 0; i < TABLE_SIZE; i++){
        table[i].isEmpty = true;
    }
}

TranspositionTable::TranspositionTable() {
    clear();
}

/*
 * Reading an entry, used to check if a position was already searched.
 * Only returns a ptr to a node if the search window is the same/narrower that the entry in the table
 * returns nullptr if the entry does not exist
 */
node* TranspositionTable::readEntry(Game &game, int alpha, int beta, int depth) {
    //Getting the key
    GameKey key = game.toKey();
    unsigned int hash = std::hash<GameKey>()(key) % TABLE_SIZE;

    //getting the entry
    while(true){
        //if the entry is empty
        if(table[hash].isEmpty){
            return nullptr;
        }

        //Hash collision
        if(!(table[hash].key == key)){
            hash += DIFFERENT_POSITION;
            hash %= TABLE_SIZE;
            continue;
        }

        //Checking if the window is right
        if(table[hash].n.beta >= beta
        && table[hash].n.alpha <= alpha
        && table[hash].n.depth >= depth){
            return &table[hash].n;
        } else {
            hash += WIDER_WINDOW;
            hash %= TABLE_SIZE;
        }
    }
}

/*
 * Returns a ptr to an entry that can then be overwritten to safe the data
 */
node *TranspositionTable::writeEntry(Game &game, int alpha, int beta, int depth) {
    //Getting the key
    GameKey key = game.toKey();
    unsigned int hash = std::hash<GameKey>()(key) % TABLE_SIZE;

    //Getting the entry
    while(true){
        //if the entry is empty we create a new entry
        if(table[hash].isEmpty){
            table[hash].key = key;
            table[hash].isEmpty = false;
            return &table[hash].n;
        }

        //Hash collision
        if(!(table[hash].key == key)){
            hash += DIFFERENT_POSITION;
            hash %= TABLE_SIZE;
            continue;
        }

        //Checking if the window is right
        if(table[hash].n.beta <= beta
           && table[hash].n.alpha >= alpha
           && table[hash].n.depth <= depth){
            //New window is even wider than old one
            return &table[hash].n;
        } else {
            hash += WIDER_WINDOW;
            hash %= TABLE_SIZE;
        }
    }
}
