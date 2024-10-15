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
                if ((pieceBoards[i] >> (7 - x + 8 * (7 - y))) & 1) {
                    switch (i) {
                        case 0:
                            std::cout << " P ";
                            isOccupied = true;
                            break;
                        case 1:
                            std::cout << " N ";
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
                            std::cout << " n ";
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
            if (!isOccupied) {
                std::cout << " . ";
            }

        }
        std::cout << std::endl;
    }
    std::cout << "------------------------" << std::endl;
    //Printing en passant
    for (int i = 0; i < 8; i++) {
        if ((enPassant >> (7 - i)) & 1) {
            std::cout << " | ";
        } else {
            std::cout << "   ";
        }
    }
    std::cout << std::endl;

    if (whoToMove == WHITE) {
        std::cout << "White to move" << std::endl;
    } else {
        std::cout << "Black to move" << std::endl;
    }

    //Printing castle rights
    std::cout << "w: ";
    if (castleRights & WHITE_SHORT_CASTLE_RIGHT) {
        std::cout << "s ";
    }
    if (castleRights & WHITE_LONG_CASTLE_RIGHT) {
        std::cout << "l ";
    }
    std::cout << "| b: ";
    if (castleRights & BLACK_SHORT_CASTLE_RIGHT) {
        std::cout << "s ";
    }
    if (castleRights & BLACK_LONG_CASTLE_RIGHT) {
        std::cout << "l ";
    }
    std::cout << std::endl;
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
    whoNotToMove = BLACK;
}

/*
 * This function executes the given move, it is assumed the move is legal
 */
void Game::doMove(move m) {
    //Saving the move in case you might undo it later
    pastMove past;
    past.regularMove = m;
    past.enPassant = this->enPassant;
    past.castlingRights = this->castleRights;
    past.capturedPiece = NONE;

    //Removes the piece from its original position
    pieceBoards[whoToMove + m.movingPiece] &= ~m.startingSquare;
    //And places it back on its new square
    pieceBoards[whoToMove + m.movingPiece] |= m.finalSquare;

    //Removing the piece
    for (int i = 0; i < 6; i++) {
        if (pieceBoards[i + whoNotToMove] & m.finalSquare) {
            past.capturedPiece = i;
        }
        pieceBoards[i + whoNotToMove] &= ~m.finalSquare;
    }

    //Promotion
    if(m.promotion != NONE){
        pieceBoards[whoToMove + PAWN] &= ~m.finalSquare; //Removing the pawn again
        pieceBoards[whoToMove + m.promotion] |= m.finalSquare; //Placing the promoted piece
    }

    //Capturing en passant
    if (m.movingPiece == PAWN
        && past.capturedPiece == NONE
        && getXCoord(m.startingSquare) != getXCoord(m.finalSquare)) {
        int x = getXCoord(m.finalSquare);
        int y = getYCoord(m.startingSquare);
        pieceBoards[whoNotToMove + PAWN] &= ~generateBitboard(x, y);
        past.capturedPiece = EN_PASSANT_PAWN;
    }

    //Castle
    if (m.movingPiece == KING
        && m.startingSquare & KING_SHORT_CASTLE_BOARD
        && m.finalSquare & KING_SHORT_CASTLE_BOARD) {
        //Moving the rook
        pieceBoards[whoToMove + ROOK] ^= ROOK_SHORT_CASTLE_MOVEMENT[whoToMove /
                                                                    BLACK]; //Dividing by black to get to 1 if BLACK is to move
    } else if (m.movingPiece == KING
               && m.startingSquare & KING_LONG_CASTLE_BOARD
               && m.finalSquare & KING_LONG_CASTLE_BOARD) {
        //Moving the rook
        pieceBoards[whoToMove + ROOK] ^= ROOK_LONG_CASTLE_MOVEMENT[whoToMove /
                                                                   BLACK]; //Dividing by black to get to 1 if BLACK is to move
    }

    //Adjusting en passant
    enPassant = 0;
    if (m.movingPiece == PAWN && (m.startingSquare & EN_PASSANT_STARTING_ROWS)
        && (m.finalSquare & EN_PASSANT_ENDING_ROWS)) {
        for (int i = 0; i < 8; i++) {
            enPassant |= ((COLLOMS[i] & m.startingSquare) != 0) * (COLLOMS[i]);
        }
    } else {
        enPassant = 0;
    }

    //Adjusting Castle rights
    castleRights &= ~(WHITE_SHORT_CASTLE_RIGHT * !(!(WHITE_SHORT_CASTLE_BOARD & (m.finalSquare | m.startingSquare))));
    castleRights &= ~(WHITE_LONG_CASTLE_RIGHT * !(!(WHITE_LONG_CASTLE_BOARD & (m.finalSquare | m.startingSquare))));
    castleRights &= ~(BLACK_SHORT_CASTLE_RIGHT * !(!(BLACK_SHORT_CASTLE_BOARD & (m.finalSquare | m.startingSquare))));
    castleRights &= ~(BLACK_LONG_CASTLE_RIGHT * !(!(BLACK_LONG_CASTLE_BOARD & (m.finalSquare | m.startingSquare))));

    //Switching who is to move
    color x = whoToMove;
    whoToMove = whoNotToMove;
    whoNotToMove = x;

    //Adding the move to last moves
    pastMoves.push(past);
}


/*
 * This function takes a string such as 'e2e4' and returns the matching move.
 * Due to the moving piece attribute it should only be used right before doing the returned move
 */
move Game::stringToMove(std::string moveStr) {

    move move;

    int x0, x1, y0, y1;
    x0 = moveStr[0] - 'a';
    x1 = moveStr[2] - 'a';
    y0 = moveStr[1] - '1';
    y1 = moveStr[3] - '1';

    bitboard startingSquare = 1;
    startingSquare = startingSquare << ((7 - x0) + 8 * y0);
    bitboard finalSquare = 1;
    finalSquare = finalSquare << ((7 - x1) + 8 * y1);

    move.startingSquare = startingSquare;
    move.finalSquare = finalSquare;

    for (int i = 0; i < 12; i++) {
        if (pieceBoards[i] & startingSquare) {
            move.movingPiece = i % 6;
        }
    }

    //Promotion
    if (moveStr.size() == 5) {
        switch (moveStr[4]) {
            case 'p':
                move.promotion = PAWN;
                break;
            case 'n':
                move.promotion = KNIGHT;
                break;
            case 'b':
                move.promotion = BISHOP;
                break;
            case 'r':
                move.promotion = ROOK;
                break;
            case 'q':
                move.promotion = QUEEN;
                break;
            case 'k':
                move.promotion = KING;
                break;
            default:
                break;
        }
    }

    return move;
}


/*
 * Returns the X-Value of a board with exactly one square
 */
int Game::getXCoord(bitboard board) {
    int x = 0;
    x += ((board & COLLOM_ITERATION[0]) != 0) * 4;
    x += ((board & COLLOM_ITERATION[1]) != 0) * 2;
    x += ((board & COLLOM_ITERATION[2]) != 0) * 1;
    return x;
}

bitboard Game::generateBitboard(int x, int y) {
    return ROWS[y] & COLLOMS[x];
}

int Game::getYCoord(bitboard board) {
    int y = 0;
    y += ((board & ROW_ITERATION[0]) != 0) * 4;
    y += ((board & ROW_ITERATION[1]) != 0) * 2;
    y += ((board & ROW_ITERATION[2]) != 0) * 1;
    return y;
}

void Game::undoMove() {
    //Getting the last move
    pastMove &past = pastMoves.top();

    //Removing the piece
    pieceBoards[whoNotToMove + past.regularMove.movingPiece] &= ~past.regularMove.finalSquare;

    //Placing it back to the old square;
    pieceBoards[whoNotToMove + past.regularMove.movingPiece] |= past.regularMove.startingSquare;

    //Restoring captured piece
    if(past.capturedPiece != NONE){
        if(past.capturedPiece != EN_PASSANT_PAWN){
            //Restoring a regular piece
            pieceBoards[whoToMove + past.capturedPiece] |= past.regularMove.finalSquare;
        } else {
            //Restoring a pawn destroyed from en passant
            int x = getXCoord(past.regularMove.finalSquare);
            int y = getYCoord(past.regularMove.startingSquare);
            pieceBoards[whoToMove + PAWN] |= generateBitboard(x, y);
        }
    }

    //Undoing castle
    if (past.regularMove.movingPiece == KING
        && past.regularMove.startingSquare & KING_SHORT_CASTLE_BOARD
        && past.regularMove.finalSquare & KING_SHORT_CASTLE_BOARD) {
        //Moving the rook
        pieceBoards[whoNotToMove + ROOK] ^= ROOK_SHORT_CASTLE_MOVEMENT[whoNotToMove /
                                                                    BLACK]; //Dividing by black to get to 1 if BLACK is to move
    } else if (past.regularMove.movingPiece == KING
               && past.regularMove.startingSquare & KING_LONG_CASTLE_BOARD
               && past.regularMove.finalSquare & KING_LONG_CASTLE_BOARD) {
        //Moving the rook
        pieceBoards[whoNotToMove + ROOK] ^= ROOK_LONG_CASTLE_MOVEMENT[whoNotToMove /
                                                                   BLACK]; //Dividing by black to get to 1 if BLACK is to move
    }

    //Undoing promotion
    if(past.regularMove.promotion != NONE){
        pieceBoards[whoNotToMove + past.regularMove.promotion] &= ~past.regularMove.finalSquare; //Removing the promoted piece
    }

    //Restoring castle rights
    castleRights = past.castlingRights;

    //Restoring en passant rights
    enPassant = past.enPassant;

    //Switching who is to move
    color x = whoToMove;
    whoToMove = whoNotToMove;
    whoNotToMove = x;

    //Popping the stack
    pastMoves.pop();
}
