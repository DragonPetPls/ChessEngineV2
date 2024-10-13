//
// Created by fabian on 10/13/24.
//

#include <iostream>
#include "Game.h"

void Game::printGame() {
    std::cout << "------------------------" << std::endl;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            bool isOccupied = false;
            //Iterating through all pieces
            for (int i = 0; i < 12; i++) {

                //checking if its occupied
                if ((pieceBoards[i] >> (7 - x + 8 * (7 -y))) & 1) {
                    switch (i) {
                        case 0:
                            std::cout << " P ";
                            isOccupied = true;
                            break;
                        case 1:
                            std::cout << " K ";
                            isOccupied = true;
                            break;
                        case 2:
                            std::cout << " B ";
                            isOccupied = true;
                            break;
                        case 3:
                            std::cout << " R ";
                            isOccupied = true;
                            break;
                        case 4:
                            std::cout << " Q ";
                            isOccupied = true;
                            break;
                        case 5:
                            std::cout << " K ";
                            isOccupied = true;
                            break;
                        case 6:
                            std::cout << " p ";
                            isOccupied = true;
                            break;
                        case 7:
                            std::cout << " k ";
                            isOccupied = true;
                            break;
                        case 8:
                            std::cout << " b ";
                            isOccupied = true;
                            break;
                        case 9:
                            std::cout << " r ";
                            isOccupied = true;
                            break;
                        case 10:
                            std::cout << " q ";
                            isOccupied = true;
                            break;
                        case 11:
                            std::cout << " k ";
                            isOccupied = true;
                            break;
                        default:
                            break;
                    }
                }
            }
            if(!isOccupied) {
                std::cout << " . ";
            }

        }
        std::cout << std::endl;
    }
    std::cout << "------------------------" << std::endl;


}


/*
 * loads the default chess starting position
 */
void Game::loadStartingPosition() {
    pieceBoards[WHITE_PAWN] = WHITE_PAWN_STARTING_POS;
    pieceBoards[WHITE_BISHOP] = WHITE_BISHOP_STARTING_POS;
    pieceBoards[WHITE_KNIGHT] = WHITE_KNIGHT_STARTING_POS;
    pieceBoards[WHITE_ROOK] = WHITE_ROOK_STARTING_POS;
    pieceBoards[WHITE_QUEEN] = WHITE_QUEEN_STARTING_POS;
    pieceBoards[WHITE_KING] = WHITE_KING_STARTING_POS;
    pieceBoards[BLACK_PAWN] = BLACK_PAWN_STARTING_POS;
    pieceBoards[BLACK_KNIGHT] = BLACK_KNIGHT_STARTING_POS;
    pieceBoards[BLACK_BISHOP] = BLACK_BISHOP_STARTING_POS;
    pieceBoards[BLACK_ROOK] = BLACK_ROOK_STARTING_POS;
    pieceBoards[BLACK_QUEEN] = BLACK_QUEEN_STARTING_POS;
    pieceBoards[BLACK_KING] = BLACK_KING_STARTING_POS;

    enPassant = 0;
    castleRights = WHITE_SHORT_CASTLE_RIGHT;
    castleRights += WHITE_LONG_CASTLE_RIGHT;
    castleRights += BLACK_SHORT_CASTLE_RIGHT;
    castleRights += BLACK_LONG_CASTLE_RIGHT;
    whoToMove = WHITE;
}
