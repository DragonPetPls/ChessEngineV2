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
#include "magicBitboards.h"
#include <iostream>

struct move {
    bitboard startingSquare = 0;
    bitboard finalSquare = 0;
    piece promotion = NONE;
    piece movingPiece = 0;
};


//This struct has all information to also undo a move
struct pastMove {
    move regularMove;
    uint8_t castlingRights = 0;
    piece capturedPiece = 0;
    uint8_t enPassant = 0;
    int counterToDraw = 0;
    int prevEvaluation = 0;
};

//This struct represents a coordinate pair
struct coord {
    int x;
    int y;
};

struct squaresLookup {
    std::vector<bitboard> finalSquares;
    bitboard allSquares;
};


struct GameKey {
    bitboard pieceBoards[12];
    uint8_t castleRights;
    uint8_t whoToMove;
    uint8_t enPassant;

    bool operator==(const GameKey &other) const {
        for (int i = 0; i < 12; ++i) {
            if (pieceBoards[i] != other.pieceBoards[i]) {
                return false;
            }
        }
        return castleRights == other.castleRights &&
               enPassant == other.enPassant &&
               whoToMove == other.whoToMove;
    }
};


class Game {
private:
    uint8_t currentStatus = TBD;

    std::list<uint64_t> pastHashes;

    squaresLookup kingLookup[64];

    void initSquaresLookup();

    bitboard pieceBoards[12];
    uint8_t castleRights;
    uint8_t enPassant;
    color whoToMove;
    color whoNotToMove;

    //This evaluation is updated whenever a move is made to allow for a fast evaluation which can the be refined if it seems worth the time
    int eval = 0;

    std::stack<pastMove> pastMoves;

    std::vector<bitboard> &getKnightFinalSquares(coord location);

    bitboard &getKnightReachableSquares(coord location);

    std::vector<bitboard> &getKingFinalSquares(coord location);

    bitboard &getKingReachableSquares(coord location);

    std::vector<bitboard> generateKnightFinalSquares(coord knightLocation);

    std::vector<bitboard> generateKingFinalSquares(coord kingLocation);

    bool isSquareUnderAttack(coord square, color attackingColor, bitboard hitmap);

    bool isSquareUnderAttack(coord square, color attackingColor);

    bool isMovePlayable();

    int counterToDraw = 0;

public:
    int moveCount = 0;
    magicBitboards magic;
    squaresLookup knightLookup[64];

    Game();

    void printGame();

    void loadStartingPosition();

    void doMove(move m);

    move stringToMove(std::string move);

    void undoMove();

    std::vector<move> getAllPseudoLegalMoves();

    status getStatus();

    bool isKingSafe(color whichKing);

    bool isPositionLegal();

    void loadFen(std::string &fen);

    void printMove(move m);

    static std::vector<coord> locatePieces(bitboard board);

    static int getXCoord(bitboard board);

    static int getYCoord(bitboard board);

    static bitboard generateBitboard(int x, int y);

    const bitboard *getPieceBoards() const;

    uint8_t getCastleRights() const;

    uint8_t getEnPassant() const;

    color getWhoToMove() const;

    color getWhoNotToMove() const;

    int getCounterToDraw() const;

    int getEval() const;

    void setEval(int eval);

    GameKey toKey() const;

    //Necessary for the hashtable
    bool operator==(const Game &other) const;
};


namespace std {
    template<>
    struct hash<GameKey> {
        std::size_t operator()(const GameKey &k) const {
            bitboard hash = 0;
            for (int i = 0; i < 12; i++) {
                hash ^= std::hash<bitboard>{}(k.pieceBoards[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            hash ^= std::hash<uint8_t>{}(k.whoToMove) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<uint8_t>{}(k.castleRights) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<uint8_t>{}(k.enPassant) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            return hash;
        }
    };
}


namespace std {
    template<>
    struct hash<Game> {
        std::size_t operator()(const Game &p) const {
            uint64_t hash = 0;
            for (int i = 0; i < 12; i++) {
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
