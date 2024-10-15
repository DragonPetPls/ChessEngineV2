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
#define NONE 6

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

#define EN_PASSANT_PAWN 12

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

const bitboard EN_PASSANT_STARTING_ROWS = 71776119061282560;
const bitboard EN_PASSANT_ENDING_ROWS = 1099494850560;

//These boards are intended to check if castle rights are lost
const bitboard WHITE_SHORT_CASTLE_BOARD = 9;    //....|..|
const bitboard WHITE_LONG_CASTLE_BOARD = 136;  //|...|...
const bitboard BLACK_SHORT_CASTLE_BOARD = 648518346341351424;   //....|..|
const bitboard BLACK_LONG_CASTLE_BOARD = 9799832789158199296;   //|...|...

//These boards are intended to check if castle is played
const bitboard KING_SHORT_CASTLE_BOARD = 720575940379279370;   //
const bitboard KING_LONG_CASTLE_BOARD = 2882303761517117480;

const bitboard ROOK_SHORT_CASTLE_MOVEMENT[2] = {5, 360287970189639680};//First entry is for white, second for black
const bitboard ROOK_LONG_CASTLE_MOVEMENT[2] =  {144, 10376293541461622784}; //First entry is for white, second for black

const bitboard ROWS[8] = {255, 65280, 16711680, 4278190080, 1095216660480, 280375465082880, 71776119061217280, 18374686479671623680};
const bitboard COLLOMS[8] = {9259542123273814144, 4629771061636907072, 2314885530818453536,1157442765409226768,
                             578721382704613384, 289360691352306692, 144680345676153346, 72340172838076673};

const bitboard COLLOM_ITERATION[3] = {COLLOMS[4] | COLLOMS[5] | COLLOMS [6] | COLLOMS[7],// ....||||
                                        COLLOMS[2] | COLLOMS[3] | COLLOMS[6] | COLLOMS[7], // ..||..||
                                          COLLOMS[1] | COLLOMS [3] | COLLOMS[5] | COLLOMS[7]};  // .|.|.|.|

const bitboard ROW_ITERATION[3] = {ROWS[4] | ROWS[5] | ROWS [6] | ROWS[7],// ....||||
                                      ROWS[2] | ROWS[3] | ROWS[6] | ROWS[7], // ..||..||
                                      ROWS[1] | ROWS [3] | ROWS[5] | ROWS[7]};  // .|.|.|.|


#endif //CHESSENGINEV2_CONSTANTS_H
