//
// Created by fabian on 10/13/24.
//

#ifndef CHESSENGINEV2_GAME_H
#define CHESSENGINEV2_GAME_H

#include <list>
#include <stack>
#include <queue>
#include <unordered_map>
#include "constants.h"
#include <iostream>

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
    int counterToDraw = 0;
};

//This struct represents a coordinate pair
struct coord{
    int x;
    int y;
};

struct squaresLookup{
    std::vector<bitboard> finalSquares;
    bitboard allSquares;
};

class Game {
private:
    std::list<uint64_t> pastHashes;

    squaresLookup knightLookup[64];
    void initSquaresLookup();

    bitboard pieceBoards[12];
    uint8_t castleRights;
    uint8_t enPassant;
    color whoToMove;
    color whoNotToMove;

    std::stack<pastMove> pastMoves;

    static std::list<coord> locatePieces(bitboard board);

    std::vector<bitboard>& getKnightFinalSquares(coord knightLocation);
    bitboard& getKnightReachableSquares(coord location);
    static std::vector<bitboard> generateSlidingPieceFinalSquares(coord slidingPieceLocations, bitboard hitmap);
    static std::vector<bitboard> generateDiagonalPieceFinalSquares(coord diagonalPieceLocations, bitboard hitmap);

    std::vector<bitboard> generateKnightFinalSquares(coord knightLocation);

    bool isSquareUnderAttack(coord square, color attackingColor, bitboard hitmap);
    bool isSquareUnderAttack(coord square, color attackingColor);

    int counterToDraw = 0;

public:
    Game();
    void printGame();
    void loadStartingPosition();
    void doMove(move m);
    move stringToMove(std::string move);
    void undoMove();
    std::list<move> getAllPseudoLegalMoves();
    status getStatus();
    bool isKingSafe(color whichKing);
    bool isPositionLegal();
    void loadFen(std::string &fen);
    void printMove(move m);

    static int getXCoord(bitboard board);
    static int getYCoord(bitboard board);
    static bitboard generateBitboard(int x, int y);

    const bitboard *getPieceBoards() const;
    uint8_t getCastleRights() const;
    uint8_t getEnPassant() const;
    color getWhoToMove() const;
    color getWhoNotToMove() const;
    int getCounterToDraw() const;

    //Necessary for the hashtable
    bool operator==(const Game& other) const;
};


namespace std {
    template <>
    struct hash<Game> {
        std::size_t operator()(const Game& p) const {

            uint64_t hash = 0;
            for(int i = 0; i < 12; i++){
                hash += (i * 13 * p.getPieceBoards()[i]) % (i * 186125431121 + 1923546);
            }
            hash += 247513841 * p.getWhoToMove();
            hash += 761565351345173 * p.getCastleRights();
            hash += 746275115481 * p.getEnPassant();
            return hash;
        }
    };
}

#endif //CHESSENGINEV2_GAME_H
