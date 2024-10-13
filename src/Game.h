//
// Created by fabian on 10/13/24.
//

#ifndef CHESSENGINEV2_GAME_H
#define CHESSENGINEV2_GAME_H

#include <list>
#include "constants.h"

struct move{
    bitboard startingSquare;
    bitboard endingSquare;
    piece promotion;
};


//This struct has all information to also undo a move
struct pastMove{
    move regularMove;
    uint8_t castlingRights;
    piece capturedPiece;
    uint8_t enPassant;
};

class Game {
    bitboard pieceBoards[12];
    uint8_t castleRights;
    uint8_t enPassant;
    color whoToMove;

public:
    void printGame();
    void loadStartingPosition();
    void doMove(move m);
    void undoMove();
    std::list<move> getAllLegalMoves();
    status getStatus();



};


#endif //CHESSENGINEV2_GAME_H
