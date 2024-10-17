//
// Created by fabian on 10/16/24.
//

#ifndef CHESSENGINEV2_MAGICBITBOARDS_H
#define CHESSENGINEV2_MAGICBITBOARDS_H

#include <cstdint>
#include <vector>
#include <list>

typedef uint64_t bitboard;

struct blocker{
    std::vector<bitboard> finalSquares;
    bitboard allSquares = 0;
    bool isInit = false;
};

struct magicSquare{
    int index;
    uint64_t magicNumber;
    std::vector<blocker> blockers;
    bitboard overlay;
};

class magicBitboards {
private:
    magicSquare straightSquares[64];
    magicSquare diagonalSquares[64];

    //All straight functions
    void initStraightSquare(magicSquare* square, int x, int y);
    void calculateStraightSquare(int x, int y);
    bitboard generateBitboard(int x, int y);
    static std::vector<bitboard> generateAllStraightBitboard(int x, int y);

    //Diagonal functions
    void initDiagonalSquare(magicSquare* square, int x, int y);
    void calculateDiagonalSquare(int x, int y);
    std::vector<bitboard> generateDiagonalPieceFinalSquares(int x, int y, bitboard hitmap);
    static std::vector<bitboard> generateAllDiagonalBitboard(int x, int y);
    static void setBit(bitboard &board, int x, int y, bool value);

public:
    magicBitboards();
    void calculateStraightMagicNumbers();
    std::vector<bitboard> generateStraightFinalSquares(int x, int y, bitboard hitmap);
    void calculateDiagonalMagicNumbers();


    //All straight getter functions
    std::vector<bitboard>& getStraightFinalSquares(int x, int y, bitboard hitmap);
    bitboard getStraightAllFinalSquares(int x, int y, bitboard hitmap);

    //All diagonal getter functions
    std::vector<bitboard>& getDiagonalFinalSquares(int x, int y, bitboard hitmap);
    bitboard getDiagonalAllFinalSquares(int x, int y, bitboard hitmap);


};


#endif //CHESSENGINEV2_MAGICBITBOARDS_H
