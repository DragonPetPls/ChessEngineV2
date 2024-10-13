//
// Created by fabian on 10/13/24.
//

#ifndef CHESSENGINEV2_CONSTANTS_H
#define CHESSENGINEV2_CONSTANTS_H

#include <cstdint>

#define WHITE_SHORT_CASTLE_RIGHT 1
#define WHITE_LONG_CASTLE_RIGHT 2
#define BLACK_SHORT_CASTLE_RIGHT 4
#define BLACK_LONG_CASTLE_RIGHT 8

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

#define WHITE 0
#define BLACK 6

#define WHITE_PAWN 0
#define WHITE_KNIGHT 1
#define WHITE_BISHOP 2
#define WHITE_ROOK 3
#define WHITE_QUEEN 4
#define WHITE_KING 5
#define BLACK_PAWN 6
#define BLACK_KNIGHT 7
#define BLACK_BISHOP 8
#define BLACK_ROOK 9
#define BLACK_QUEEN 10
#define BLACK_KING 11

typedef uint64_t bitboard;
typedef uint8_t piece;
typedef uint8_t status;
typedef uint8_t color;

const bitboard WHITE_PAWN_STARTING_POS = 65280;
const bitboard WHITE_KNIGHT_STARTING_POS = 66;
const bitboard WHITE_BISHOP_STARTING_POS = 36;
const bitboard WHITE_ROOK_STARTING_POS = 129;
const bitboard WHITE_QUEEN_STARTING_POS = 16;
const bitboard WHITE_KING_STARTING_POS = 8;
const bitboard BLACK_PAWN_STARTING_POS = 71776119061217280;
const bitboard BLACK_KNIGHT_STARTING_POS = 4755801206503243776;
const bitboard BLACK_BISHOP_STARTING_POS = 2594073385365405696;
const bitboard BLACK_ROOK_STARTING_POS = 9295429630892703744;
const bitboard BLACK_QUEEN_STARTING_POS = 1152921504606846976;
const bitboard BLACK_KING_STARTING_POS = 576460752303423488;

#endif //CHESSENGINEV2_CONSTANTS_H
