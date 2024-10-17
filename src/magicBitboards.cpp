//
// Created by fabian on 10/16/24.
//

#include <cstdlib>
#include <random>
#include <iostream>
#include "magicBitboards.h"
#include "constants.h"

magicBitboards::magicBitboards() {

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            initStraightSquare(&straightSquares[x + 8 * y], x, y);
        }
    }

}

void magicBitboards::initStraightSquare(magicSquare *square, int x, int y) {
    square->index = 50;
    square->magicNumber = magicNumbers[x + 8 * y];
    square->overlay = ROWS[y] ^ COLLOMS[x];
    if(y != 0){
        square->overlay &= ~ROWS[0];
    }
    if(y != 7){
        square->overlay &= ~ROWS[7];
    }
    if(x != 0){
        square->overlay &= ~COLLOMS[0];
    }
    if(x != 7){
        square->overlay &= ~COLLOMS[7];
    }

    std::vector<bitboard> bitboards = generateAllBitboard(x, y);
    std::cout << bitboards.size() << std::endl;
    //Setting each blocker up
    for(int i = 0; i < bitboards.size(); i++){

        uint64_t key = bitboards[i] * square->magicNumber;
        key = key >> square->index;
        if(square->blockers.size() < key + 1){
            square->blockers.resize(key + 1);
        }

        //Checking if the key is already used
        if(square->blockers[key].isInit){
           //Square is already set up
        } else {
            //key is new, init the entry
            square->blockers[key].finalSquares = generateStraightFinalSquares(x, y, bitboards[i]);
            square->blockers[key].isInit = true;
            bitboard allSquares = 0;
            for(bitboard f: square->blockers[key].finalSquares){
                allSquares |= f;
            }
            square->blockers[key].allSquares = allSquares;
        }
    }
    std::cout << "x: " << x << " y: " << y << " is set up" << std::endl;
}


/*
 * This function calculates magic numbers for all straight squares
 */
void magicBitboards::calculateStraightMagicNumbers() {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            calculateStraightSquare(x, y);
        }
    }
}

void magicBitboards::calculateStraightSquare(int x, int y) {
    std::vector<blocker> blockers;
    int index = 50;
    uint64_t magicNumber;

    // Create a random device to seed the generator
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    //Setting up a vector of all bitboards that need to be hashed
    std::vector<bitboard> bitboards = generateAllBitboard(x, y);

    //Actually testing numbers
    bool numberFound = false;
    //Setting up
    while(!numberFound){
        magicNumber = dist(gen);
        bool collision = false;
        blockers.clear();

        //Iterating over all bitboards
        for(int i = 0; i < bitboards.size(); i++){

            uint64_t key = bitboards[i] * magicNumber;
            key = key >> index;
            if(blockers.size() < key + 1){
                blockers.resize(key + 1);
            }

            //Checking if the key is already used
            if(blockers[key].isInit){
                auto fs = generateStraightFinalSquares(x, y, bitboards[i]);
                bitboard allSquares = 0;
                for(bitboard f: fs){
                    allSquares |= f;
                }
                if(blockers[key].allSquares == allSquares){
                    //Same legal moves avialable
                } else {
                    //Hash collision
                    collision = true;
                }
            } else {
                //key is new, init the entry
                blockers[key].finalSquares = generateStraightFinalSquares(x, y, bitboards[i]);
                blockers[key].isInit = true;
                bitboard allSquares = 0;
                for(bitboard f: blockers[key].finalSquares){
                    allSquares |= f;
                }
                blockers[key].allSquares = allSquares;
            }
            if(collision){
                break;
            }
        }

        //Checking if a number was found
        if(!collision){
            //std::cout << "number found: " << magicNumber << " for x: " << x << " y: " << y << std::endl;
            std::cout << magicNumber << ",\n ";
            numberFound = true;
        }
    }


}

/*
* Generates all finalsquares a slidingPiece(Rook, Queens horizontal or vertical moves)
*/
std::list<bitboard> magicBitboards::generateStraightFinalSquares(int x, int y, bitboard hitmap) {

    std::list<bitboard> finalSquares;
    for (int i = 0; i < 4; i++) {
        int vx = 1 * (i == 0) - 1 * (i == 1);
        int vy = 1 * (i == 2) - 1 * (i == 3);

        //Iterating over distances
        for (int distance = 1; distance < 8; distance++) {
            if (x + vx * distance >= 0 && x + vx * distance < 8 && y + vy * distance >= 0 && y + vy * distance < 8) {

                bitboard finalSquare = generateBitboard(x + vx * distance, y + vy * distance);
                finalSquares.push_back(finalSquare);

                //Checking for collisions
                if (hitmap & finalSquare) {
                    break;
                }

            } else {
                break;
            }
        }
    }
    return finalSquares;
}

bitboard magicBitboards::generateBitboard(int x, int y) {
    bitboard b = 1;
    return b << (7 - x + 8 * y);
}

std::vector<bitboard> magicBitboards::generateAllBitboard(int x, int y) {
    //Setting up a vector of all bitboards that need to be hashed
    std::vector<bitboard> bitboards;
    for(uint8_t h = 0; h < 64; h++){
        uint8_t horizontal = h << 1;

        //checking if the x-th bit is not set
        if(((horizontal >> (7 - x)) & 1)){
            continue;
        }

        for (int v = 0; v < 64; ++v) {
            uint8_t vertical = v << 1;
            //checking if the y-th bit is not set
            if(((vertical >> y) & 1)){
                continue;
            }

            //Creating a bitboard
            bitboard b = horizontal;
            b = b << (8 * y);

            for(int i = 0; i < 8; i++){
                if((vertical >> i) & 1){
                    uint64_t a = 1;
                    b |= a << (i * 8 + 7 - x);
                }
            }
            bitboards.push_back(b);
        }
    }
    return bitboards;
}

std::list<bitboard> magicBitboards::getStraightFinalSquares(int x, int y, bitboard hitmap) {
    uint64_t magicNumber = straightSquares[x + 8 * y].magicNumber;
    int index = straightSquares[x + 8 * y].index;
    bitboard  b = hitmap & straightSquares[x + 8 * y].overlay;
    uint64_t key = (b * magicNumber) >> index;
    //Debugging only:
   // blocker block = straightSquares[x + 8 * y].blockers[key];
    return straightSquares[x + 8 * y].blockers[key].finalSquares;
}

bitboard magicBitboards::getStraightAllFinalSquares(int x, int y, bitboard hitmap) {
    uint64_t magicNumber = straightSquares[x + 8 * y].magicNumber;
    int index = straightSquares[x + 8 * y].index;
    bitboard  b = hitmap & straightSquares[x + 8 * y].overlay;
    uint64_t key = (b * magicNumber) >> index;
    //Debugging only:
  //  blocker block = straightSquares[x + 8 * y].blockers[key];
    return straightSquares[x + 8 * y].blockers[key].allSquares;
}
