//
// Created by fabian on 10/11/24.
//

#ifndef CHESSENGINEV2_GAMESTATE_H
#define CHESSENGINEV2_GAMESTATE_H


#include <cstdint>

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

#define NONE 6

#define WHITE 0
#define BLACK 1

#define AMOUNT_OF_PIECES 6

#define NEXT_ROW 8

typedef uint64_t bitboard;
typedef uint64_t move;
typedef uint8_t color;
typedef uint8_t piece;

class GameState {

private:

    const move shortCastleKing[2] = {10, 720575940379279360};
    const move shortCastleRook[2] = {5, 360287970189639680};
    const move longCastleKing[2] = {40, 2882303761517117440};
    const move longCastleRook[2] = {144, 10376293541461622784};

    bitboard pieceBoards[2][AMOUNT_OF_PIECES];
    color whoToMove;
    color notWhoToMove;

    uint8_t enPassant; //A bit is set in the row where en passant is possible

public:
    void printBoard();
    void loadStartingPosition();
    void doMove(move move, piece promoteTo = NONE);
    static move strToMove(std::string str);

    bool isBoardLegit();
};


#endif //CHESSENGINEV2_GAMESTATE_H
