//
// Created by fabian on 10/13/24.
//

#ifndef CHESSENGINEV2_GAME_H
#define CHESSENGINEV2_GAME_H

#include <list>
#include <stack>
#include "constants.h"

struct move{
    bitboard startingSquare = 0;
    bitboard finalSquare = 0;
    piece promotion = NONE;
    piece movingPiece = 0;
};


//This struct has all information to also undo a move
struct pastMove{
    move regularMove;
    uint8_t castlingRights = 0;
    piece capturedPiece = 0;
    uint8_t enPassant = 0;
};

class Game {
private:
    bitboard pieceBoards[12];
    uint8_t castleRights;
    uint8_t enPassant;
    color whoToMove;
    color whoNotToMove;

    static int getXCoord(bitboard board);
    static int getYCoord(bitboard board);
    static bitboard generateBitboard(int x, int y);

    std::stack<pastMove> pastMoves;

public:
    void printGame();
    void loadStartingPosition();
    void doMove(move m);
    move stringToMove(std::string move);
    void undoMove();
    std::list<move> getAllLegalMoves();
    status getStatus();
};


#endif //CHESSENGINEV2_GAME_H
