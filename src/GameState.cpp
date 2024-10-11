//
// Created by fabian on 10/11/24.
//

#include <iostream>
#include "GameState.h"

/*
 * Prints a simple representation of the board into the consol
 */
void GameState::printBoard() {

    std::cout << "Chessboard: " << std::endl;

    bitboard overlay = 1;
    overlay = overlay << 63;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {

            // Checking which white piece
            if ((overlay & pieceBoards[WHITE][PAWN]) != 0) {
                std::cout << " P ";
            } else if ((overlay & pieceBoards[WHITE][ROOK]) != 0) {
                std::cout << " R ";
            } else if ((overlay & pieceBoards[WHITE][KNIGHT]) != 0) {
                std::cout << " N ";
            } else if ((overlay & pieceBoards[WHITE][BISHOP]) != 0) {
                std::cout << " B ";
            } else if ((overlay & pieceBoards[WHITE][KING]) != 0) {
                std::cout << " K ";
            } else if ((overlay & pieceBoards[WHITE][QUEEN]) != 0) {
                std::cout << " Q ";
            } else if ((overlay & pieceBoards[BLACK][PAWN]) != 0) {
                std::cout << " p ";
            } else if ((overlay & pieceBoards[BLACK][ROOK]) != 0) {
                std::cout << " r ";
            } else if ((overlay & pieceBoards[BLACK][KNIGHT]) != 0) {
                std::cout << " n ";
            } else if ((overlay & pieceBoards[BLACK][BISHOP]) != 0) {
                std::cout << " b ";
            } else if ((overlay & pieceBoards[BLACK][KING]) != 0) {
                std::cout << " k ";
            } else if ((overlay & pieceBoards[BLACK][QUEEN]) != 0) { // Changed here for black queen
                std::cout << " q ";
            } else {
                std::cout << " . ";
            }
            overlay = overlay >> 1;
        }
        std::cout << std::endl;
    }
    std::cout << "En passant: " << (int) enPassant << std::endl;
}


void GameState::loadStartingPosition() {

    //Placing all pieces
    pieceBoards[WHITE][KING] = 8;
    pieceBoards[WHITE][QUEEN] = 16;
    pieceBoards[WHITE][ROOK] = 129;
    pieceBoards[WHITE][BISHOP] = 36;
    pieceBoards[WHITE][KNIGHT] = 66;
    pieceBoards[WHITE][PAWN] = 65280;

    pieceBoards[BLACK][KING] = 576460752303423488;
    pieceBoards[BLACK][QUEEN] = 1152921504606846976;
    pieceBoards[BLACK][ROOK] = 9295429630892703744;
    pieceBoards[BLACK][BISHOP] = 2594073385365405696;
    pieceBoards[BLACK][KNIGHT] = 4755801206503243776;
    pieceBoards[BLACK][PAWN] = 71776119061217280;

    whoToMove = WHITE;
    notWhoToMove = BLACK;

    enPassant = 0;
}

/*
 * This function will change the board setup
 * it does assume that the input move is legal
 */
void GameState::doMove(move move, piece promoteTo) {

    piece movingPiece;

    //Getting the moving piece
    if (pieceBoards[whoToMove][PAWN] & move) {
        movingPiece = PAWN;
    } else if (pieceBoards[whoToMove][ROOK] & move) {
        movingPiece = ROOK;
    } else if (pieceBoards[whoToMove][KNIGHT] & move) {
        movingPiece = KNIGHT;
    } else if (pieceBoards[whoToMove][BISHOP] & move) {
        movingPiece = BISHOP;
    } else if (pieceBoards[whoToMove][KING] & move) {
        movingPiece = KING;
    } else if (pieceBoards[whoToMove][QUEEN] & move) {
        movingPiece = QUEEN;
    }
    pieceBoards[whoToMove][movingPiece] ^= move;

    //Checking if something was captured
    piece capturedPiece = NONE;
    if (pieceBoards[notWhoToMove][PAWN] & move) {
        capturedPiece = PAWN;
    } else if (pieceBoards[notWhoToMove][ROOK] & move) {
        capturedPiece = ROOK;
    } else if (pieceBoards[notWhoToMove][KNIGHT] & move) {
        capturedPiece = KNIGHT;
    } else if (pieceBoards[notWhoToMove][BISHOP] & move) {
        capturedPiece = BISHOP;
    } else if (pieceBoards[notWhoToMove][KING] & move) {
        capturedPiece = KING;
    } else if (pieceBoards[notWhoToMove][QUEEN] & move) {
        capturedPiece = QUEEN;
    }
    if (capturedPiece != NONE) {
        pieceBoards[notWhoToMove][capturedPiece] &= ~move;
    }

    //Checking for special moves:
    //Castle
    if (movingPiece == KING) {
        if (move == shortCastleKing[whoToMove]) {
            //Short castle
            pieceBoards[whoToMove][ROOK] ^= shortCastleRook[whoToMove];
        } else if (move == longCastleKing[whoToMove]) {
            //Long castle
            pieceBoards[whoToMove][ROOK] ^= longCastleRook[whoToMove];
        }
    }

    //Special Pawn moves
    if(movingPiece == PAWN && capturedPiece == NONE){
        if (!(move & ((move << 8) | (move << 16)))){
            //En passant happend
            bitboard capture = enPassant;
            capture = capture << (NEXT_ROW * (4 - whoToMove ));
            pieceBoards[notWhoToMove][PAWN] &= ~capture;
        } else if (!(move & (move << 8)) && (move & (move << 16))){
            //Advanced two squares
            //Setting en passant
            enPassant = move >> (NEXT_ROW * (1 + whoToMove * 3));
        } else {
            enPassant = 0;
        }
    } else {
        enPassant = 0;
    }

    //Switching who to move
    color x = whoToMove;
    whoToMove = notWhoToMove;
    notWhoToMove = x;
}

move GameState::strToMove(std::string str) {

    bitboard from = 1;
    bitboard to = 1;

    switch (str[0]) {
        case 'h':
            break;
        case 'g':
            from = from << 1;
            break;
        case 'f':
            from = from << 2;
            break;
        case 'e':
            from = from << 3;
            break;
        case 'd':
            from = from << 4;
            break;
        case 'c':
            from = from << 5;
            break;
        case 'b':
            from = from << 6;
            break;
        case 'a':
            from = from << 7;
            break;
        default:
            break;
    }
    from = from << (8 * (str[1] - '0') - 8);

    switch (str[2]) {
        case 'h':
            break;
        case 'g':
            to = to << 1;
            break;
        case 'f':
            to = to << 2;
            break;
        case 'e':
            to = to << 3;
            break;
        case 'd':
            to = to << 4;
            break;
        case 'c':
            to = to << 5;
            break;
        case 'b':
            to = to << 6;
            break;
        case 'a':
            to = to << 7;
            break;
        default:
            break;
    }
    to = to << (8 * (str[3] - '0') - 8);

    return to | from;
}

/*
 * A function that exists for testing purposes, it checks if squares a occupied multiple times
 */
bool GameState::isBoardLegit() {
    bool isLegit = true;

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < i; j++) {
            if ((pieceBoards[i / 6][i % 6] & pieceBoards[j / 6][j % 6]) != 0) {
                std::cout << "piece crash, color: " << i / 2 << " piece: " << i % 6
                          << " with color: " << j / 2 << " piece: " << j % 6 << std::endl;
                isLegit = false;
            }
        }
    }
    return isLegit;
}

