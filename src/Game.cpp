//
// Created by fabian on 10/13/24.
//

#include <iostream>
#include <random>
#include "Game.h"
#include "Evaluator.h"

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
    currentStatus = ON_GOING;
    moveCount = 0;
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
    counterToDraw = 0;

    historyIndex = -1;

    eval = Evaluator::simpleEval(*this);
}

/*
 * This function executes the given move, it is assumed the move is legal
 */
void Game::doMove(move &m) {
    currentStatus = TBD;
    moveCount++;
    //Storing the current hash
    historyIndex++;
    pastHashes[historyIndex] = std::hash<Game>()(*this);

    //Saving the move in case you might undo it later
    pastMove &past = moveHistory[historyIndex];
    past.regularMove = m;
    past.enPassant = this->enPassant;
    past.castlingRights = this->castleRights;
    past.counterToDraw = counterToDraw;
    past.prevEvaluation = eval;

    //Adjusting evaluation
    eval = -eval - Evaluator::getEvalDifference(m, *this);

    //Increasing the counter for 50 move rule
    counterToDraw++;

    //Removes the piece from its original position
    pieceBoards[whoToMove + m.movingPiece] &= ~m.startingSquare;
    //And places it back on its new square
    pieceBoards[whoToMove + m.movingPiece] |= m.finalSquare;

    //Removing the piece
    past.capturedPiece = 0;
    for (int i = 0; i < 6; i++) {
        past.capturedPiece += (i + 1) * ((pieceBoards[i + whoNotToMove] & m.finalSquare) != 0);
        pieceBoards[i + whoNotToMove] &= ~m.finalSquare;
    }
    past.capturedPiece += (1 + NONE) * (past.capturedPiece == 0);
    past.capturedPiece--;

    //Promotion
    pieceBoards[whoToMove + PAWN] &= ~(m.finalSquare * (m.promotion != NONE)); //Removing the pawn again
    pieceBoards[whoToMove + m.promotion * (m.promotion != NONE)] |=
            m.finalSquare * (m.promotion != NONE); //Placing the promoted piece

    //En passant
    bitboard enPassantCaptureSquare =
            ((m.startingSquare << 1) | (m.startingSquare >> 1)) & ((m.finalSquare >> 8) | (m.finalSquare << 8));
    bitboard prevPawns = pieceBoards[PAWN + whoNotToMove];
    pieceBoards[PAWN + whoNotToMove] &= ~(enPassantCaptureSquare *
                                          ((m.movingPiece == PAWN) && (past.capturedPiece == NONE)));
    past.capturedPiece += (EN_PASSANT_PAWN - past.capturedPiece) * (prevPawns != pieceBoards[PAWN + whoNotToMove]);

    //Castling
    pieceBoards[whoToMove + ROOK] ^=
            ROOK_SHORT_CASTLE_MOVEMENT[whoToMove / BLACK] *   //Dividing by black to get to 1 if BLACK is to move
            (m.movingPiece == KING
             && m.startingSquare & KING_SHORT_CASTLE_BOARD
             && m.finalSquare & KING_SHORT_CASTLE_BOARD);
    pieceBoards[whoToMove + ROOK] ^= ROOK_LONG_CASTLE_MOVEMENT[whoToMove / BLACK] *
                                     (m.movingPiece == KING
                                      && m.startingSquare &
                                         KING_LONG_CASTLE_BOARD
                                      && m.finalSquare &
                                         KING_LONG_CASTLE_BOARD);


    //Adjusting en passant
    enPassant = 0;
    for (int i = 0; i < 8; i++) {
        enPassant |= ((COLLOMS[i] & m.startingSquare) != 0) * (COLLOMS[i]) *
                     (m.movingPiece == PAWN && (m.startingSquare & EN_PASSANT_STARTING_ROWS)
                      && (m.finalSquare & EN_PASSANT_ENDING_ROWS));
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

    //reseting counter to draw if neccesary
    counterToDraw *= 1 * !(past.regularMove.movingPiece == PAWN || past.capturedPiece != NONE);
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

/*
 * Generates a bitboard with the given square selected
 */
bitboard Game::generateBitboard(int x, int y) {
    return ROWS[y] & COLLOMS[x];
}

/*
 * Returns the y-Coord of a piece on a bitboard with exactly one square
 */
int Game::getYCoord(bitboard board) {
    int y = 0;
    y += ((board & ROW_ITERATION[0]) != 0) * 4;
    y += ((board & ROW_ITERATION[1]) != 0) * 2;
    y += ((board & ROW_ITERATION[2]) != 0) * 1;
    return y;
}

/*
 * Undoes the last move
 */
void Game::undoMove() {
    currentStatus = TBD;
    moveCount--;
    //Getting the last move
    pastMove &past = moveHistory[historyIndex];

    //Removing the piece
    pieceBoards[whoNotToMove + past.regularMove.movingPiece] &= ~past.regularMove.finalSquare;

    //Placing it back to the old square;
    pieceBoards[whoNotToMove + past.regularMove.movingPiece] |= past.regularMove.startingSquare;

    //Restoring captured piece, including en passant
    pieceBoards[whoToMove + past.capturedPiece] |=
            past.regularMove.finalSquare * (past.capturedPiece != NONE && past.capturedPiece != EN_PASSANT_PAWN);
    bitboard enPassantCaptureSquare =
            ((past.regularMove.startingSquare << 1) | (past.regularMove.startingSquare >> 1)) &
            ((past.regularMove.finalSquare >> 8) | (past.regularMove.finalSquare << 8));
    pieceBoards[PAWN + whoToMove] |= enPassantCaptureSquare * (past.capturedPiece == EN_PASSANT_PAWN);

    //Undoing castle
    pieceBoards[whoNotToMove + ROOK] ^=
            ROOK_SHORT_CASTLE_MOVEMENT[whoNotToMove / BLACK] * (past.regularMove.movingPiece == KING
                                                                && past.regularMove.startingSquare &
                                                                   KING_SHORT_CASTLE_BOARD
                                                                &&
                                                                past.regularMove.finalSquare & KING_SHORT_CASTLE_BOARD);
    pieceBoards[whoNotToMove + ROOK] ^=
            ROOK_LONG_CASTLE_MOVEMENT[whoNotToMove / BLACK] * (past.regularMove.movingPiece == KING
                                                               &&
                                                               past.regularMove.startingSquare & KING_LONG_CASTLE_BOARD
                                                               &&
                                                               past.regularMove.finalSquare & KING_LONG_CASTLE_BOARD);

    //Undoing promotion
    pieceBoards[whoNotToMove + past.regularMove.promotion] &=
            ~past.regularMove.finalSquare * (past.regularMove.promotion != NONE); //Removing the promoted piece

    //Restoring castle rights
    castleRights = past.castlingRights;

    //Restoring en passant rights
    enPassant = past.enPassant;

    //Switching who is to move
    color x = whoToMove;
    whoToMove = whoNotToMove;
    whoNotToMove = x;

    //Restoring counter till draw
    counterToDraw = past.counterToDraw;

    //Restoring prev Eval
    eval = past.prevEvaluation;

    //Popping the stack
    historyIndex--;
}

/*
 * Locates all pieces on the bitboard
 */
std::vector<coord> Game::locatePieces(bitboard board) {
    std::vector<coord> locations;
    locations.reserve(8);

    for (int x = 0; x < 8; x++) {
        if (COLLOMS[x] & board) {
            for (int y = 0; y < 8; y++) {
                if ((board >> (7 - x + 8 * y)) & 1) {
                    coord c;
                    c.x = x;
                    c.y = y;
                    locations.push_back(c);
                }
            }
        }
    }

    return locations;
}

std::vector<move> Game::getAllPseudoLegalMoves() {
    std::vector<move> pseudoLegalMoves;
    pseudoLegalMoves.reserve(80);

    //Generating hitmaps
    bitboard ownHitmap = 0;
    bitboard enemyHitmap = 0;
    bitboard hitmap = 0;
    for (int i = 0; i < 6; i++) {
        ownHitmap |= pieceBoards[i + whoToMove];
        enemyHitmap |= pieceBoards[i + whoNotToMove];
    }
    hitmap = ownHitmap | enemyHitmap;

    {
        //Generating pawn moves
        auto pawnLocations = locatePieces(pieceBoards[whoToMove + PAWN]);
        int vy = 1 - 2 * (whoToMove == BLACK);
        for (coord &c: pawnLocations) {
            bitboard startingSquare = generateBitboard(c.x, c.y);

            //Generating all finalSquares
            //Regular moves
            bitboard finalSquare = generateBitboard(c.x, c.y + vy);
            if (!(finalSquare & hitmap)) {
                move m;
                m.startingSquare = startingSquare;
                m.finalSquare = finalSquare;
                m.movingPiece = PAWN;

                //Checking for promotion
                if (c.y + vy == 0 || c.y + vy == 7) {
                    m.promotion = QUEEN;
                    pseudoLegalMoves.push_back(m);
                    m.promotion = ROOK;
                    pseudoLegalMoves.push_back(m);
                    m.promotion = BISHOP;
                    pseudoLegalMoves.push_back(m);
                    m.promotion = KNIGHT;
                    pseudoLegalMoves.push_back(m);
                } else {
                    m.promotion = NONE;
                    pseudoLegalMoves.push_back(m);
                }
            }

            //Advancing 2 squares
            if ((c.y == 1 && whoToMove == WHITE) || (c.y == 6 && whoToMove == BLACK)) {
                finalSquare = generateBitboard(c.x, c.y + 2 * vy);
                bitboard middleSquare = generateBitboard(c.x, c.y + vy);
                if (!(finalSquare & hitmap)
                    && !(middleSquare & hitmap)) {
                    move m;
                    m.startingSquare = startingSquare;
                    m.finalSquare = finalSquare;
                    m.promotion = NONE;
                    m.movingPiece = PAWN;
                    pseudoLegalMoves.push_back(m);
                }
            }

            //Captures
            for (int i = 0; i < 2; i++) {
                int vx = 1 - 2 * i;
                if (c.x + vx >= 0 && c.x + vx < 8) {
                    finalSquare = generateBitboard(c.x + vx, c.y + vy);
                    if ((finalSquare & enemyHitmap)
                        || (((enPassant >> (7 - c.x - vx)) == 1) &&
                            ((whoToMove == WHITE && c.y == 4) || (whoToMove == BLACK && c.y == 3)))) {
                        move m;
                        m.startingSquare = startingSquare;
                        m.finalSquare = finalSquare;
                        m.movingPiece = PAWN;

                        //Checking for promotion
                        if (c.y + vy == 0 || c.y + vy == 7) {
                            m.promotion = QUEEN;
                            pseudoLegalMoves.push_back(m);
                            m.promotion = ROOK;
                            pseudoLegalMoves.push_back(m);
                            m.promotion = BISHOP;
                            pseudoLegalMoves.push_back(m);
                            m.promotion = KNIGHT;
                            pseudoLegalMoves.push_back(m);
                        } else {
                            m.promotion = NONE;
                            pseudoLegalMoves.push_back(m);
                        }
                    }
                }
            }
        }
    }

    {
        //Knight moves
        auto knightLocations = locatePieces(pieceBoards[whoToMove + KNIGHT]);
        for (coord c: knightLocations) {
            std::vector<bitboard> finalSquares = getKnightFinalSquares(c);
            bitboard startingSquare = generateBitboard(c.x, c.y);
            //Checking for collision on each final square
            for (bitboard finalSquare: finalSquares) {
                if (!(finalSquare & ownHitmap)) {
                    move m;
                    m.startingSquare = startingSquare;
                    m.finalSquare = finalSquare;
                    m.movingPiece = KNIGHT;
                    m.promotion = NONE;
                    pseudoLegalMoves.push_back(m);
                }
            }
        }
    }

    {
        //Straight sliding pieces
        bitboard slidingPieces = pieceBoards[ROOK + whoToMove] | pieceBoards[QUEEN + whoToMove];
        auto slidingPieceLocations = locatePieces(slidingPieces);
        for (coord c: slidingPieceLocations) {
            std::vector<bitboard> finalSquares = magic.getStraightFinalSquares(c.x, c.y, hitmap);
            bitboard startingSquare = generateBitboard(c.x, c.y);

            //Checking for collision on each final square
            for (bitboard finalSquare: finalSquares) {
                if (!(finalSquare & ownHitmap)) {
                    move m;
                    m.startingSquare = startingSquare;
                    m.finalSquare = finalSquare;
                    if (startingSquare & pieceBoards[ROOK + whoToMove]) {
                        m.movingPiece = ROOK;
                    } else {
                        m.movingPiece = QUEEN;
                    }
                    m.promotion = NONE;
                    pseudoLegalMoves.push_back(m);
                }
            }
        }
    }

    {
        //Diagonal pieces
        bitboard diagonalPieces = pieceBoards[BISHOP + whoToMove] | pieceBoards[QUEEN + whoToMove];
        auto diagonalPieceLocations = locatePieces(diagonalPieces);
        for (coord c: diagonalPieceLocations) {
            std::vector<bitboard> finalSquares = magic.getDiagonalFinalSquares(c.x, c.y, hitmap);
            // generateDiagonalPieceFinalSquares(c, hitmap);
            bitboard startingSquare = generateBitboard(c.x, c.y);

            //Checking for collision on each final square
            for (bitboard finalSquare: finalSquares) {
                if (!(finalSquare & ownHitmap)) {
                    move m;
                    m.startingSquare = startingSquare;
                    m.finalSquare = finalSquare;
                    if (startingSquare & pieceBoards[BISHOP + whoToMove]) {
                        m.movingPiece = BISHOP;
                    } else {
                        m.movingPiece = QUEEN;
                    }
                    m.promotion = NONE;
                    pseudoLegalMoves.push_back(m);
                }
            }
        }
    }

    {
        //King moves
        coord location;
        location.x = getXCoord(pieceBoards[KING + whoToMove]);
        location.y = getYCoord(pieceBoards[KING + whoToMove]);
        auto finalSquares = generateKingFinalSquares(location);
        for (bitboard finalSquare: finalSquares) {
            //Checking for collision
            if (!(finalSquare & ownHitmap)) {
                move m;
                m.startingSquare = pieceBoards[whoToMove + KING];
                m.finalSquare = finalSquare;
                m.movingPiece = KING;
                m.promotion = NONE;
                pseudoLegalMoves.push_back(m);
            }
        }
        //Castle
        if (whoToMove == WHITE) {
            if (castleRights & WHITE_SHORT_CASTLE_RIGHT) {
                coord c1;
                c1.x = 4;
                c1.y = 0;
                coord c2;
                c2.x = 5;
                c2.y = 0;
                coord c3;
                c3.x = 6;
                c3.y = 0;
                //Checking for blocking pieces and attacks
                if (!(generateBitboard(5, 0) & hitmap)
                    && !(generateBitboard(6, 0) & hitmap)
                    && !isSquareUnderAttack(c1, BLACK, hitmap)
                    && !isSquareUnderAttack(c2, BLACK, hitmap)
                    && !isSquareUnderAttack(c3, BLACK, hitmap)) {
                    move m;
                    m.movingPiece = KING;
                    m.startingSquare = WHITE_KING_CASTLE_STARTING_POS;
                    m.finalSquare = WHITE_KING_SHORT_CASTLE_FINAL_POS;
                    m.promotion = NONE;
                    pseudoLegalMoves.push_back(m);
                }

            }
            if (castleRights & WHITE_LONG_CASTLE_RIGHT) {
                coord c1;
                c1.x = 4;
                c1.y = 0;
                coord c2;
                c2.x = 3;
                c2.y = 0;
                coord c3;
                c3.x = 2;
                c3.y = 0;
                //Checking for blocking pieces and attacks
                if (!(generateBitboard(1, 0) & hitmap)
                    && !(generateBitboard(2, 0) & hitmap)
                    && !(generateBitboard(3, 0) & hitmap)
                    && !isSquareUnderAttack(c1, BLACK, hitmap)
                    && !isSquareUnderAttack(c2, BLACK, hitmap)
                    && !isSquareUnderAttack(c3, BLACK, hitmap)) {
                    move m;
                    m.movingPiece = KING;
                    m.startingSquare = WHITE_KING_CASTLE_STARTING_POS;
                    m.finalSquare = WHITE_KING_LONG_CASTLE_FINAL_POS;
                    m.promotion = NONE;
                    pseudoLegalMoves.push_back(m);
                }
            }
        } else {
            if (castleRights & BLACK_SHORT_CASTLE_RIGHT) {
                coord c1;
                c1.x = 4;
                c1.y = 7;
                coord c2;
                c2.x = 5;
                c2.y = 7;
                coord c3;
                c3.x = 6;
                c3.y = 7;
                //Checking for blocking pieces and attacks
                if (!(generateBitboard(5, 7) & hitmap)
                    && !(generateBitboard(6, 7) & hitmap)
                    && !isSquareUnderAttack(c1, WHITE, hitmap)
                    && !isSquareUnderAttack(c2, WHITE, hitmap)
                    && !isSquareUnderAttack(c3, WHITE, hitmap)) {
                    move m;
                    m.movingPiece = KING;
                    m.startingSquare = BLACK_KING_CASTLE_STARTING_POS;
                    m.finalSquare = BLACK_KING_SHORT_CASTLE_FINAL_POS;
                    m.promotion = NONE;
                    pseudoLegalMoves.push_back(m);
                }

            }
            if (castleRights & BLACK_LONG_CASTLE_RIGHT) {
                coord c1;
                c1.x = 4;
                c1.y = 7;
                coord c2;
                c2.x = 3;
                c2.y = 7;
                coord c3;
                c3.x = 2;
                c3.y = 7;
                //Checking for blocking pieces and attacks
                if (!(generateBitboard(1, 7) & hitmap)
                    && !(generateBitboard(2, 7) & hitmap)
                    && !(generateBitboard(3, 7) & hitmap)
                    && !isSquareUnderAttack(c1, WHITE, hitmap)
                    && !isSquareUnderAttack(c2, WHITE, hitmap)
                    && !isSquareUnderAttack(c3, WHITE, hitmap)) {
                    move m;
                    m.movingPiece = KING;
                    m.startingSquare = BLACK_KING_CASTLE_STARTING_POS;
                    m.finalSquare = BLACK_KING_LONG_CASTLE_FINAL_POS;
                    m.promotion = NONE;
                    pseudoLegalMoves.push_back(m);
                }
            }
        }
    }
    return pseudoLegalMoves;
}

/*
 * Generates all finalsquares a knight can reach from the starting square, not checking for collisions
 */
std::vector<bitboard> &Game::getKnightFinalSquares(coord knightLocation) {
    return knightLookup[knightLocation.x + knightLocation.y * 8].finalSquares;
}

bool Game::isSquareUnderAttack(coord square, color attackingColor, bitboard hitmap) {

    //Checking knight moves
    if (getKnightReachableSquares(square) & pieceBoards[attackingColor + KNIGHT]) {
        return true;
    }

    //Checking diagonal attack
    bitboard dangerousSquares = magic.getDiagonalAllFinalSquares(square.x, square.y, hitmap);
    bitboard diagonalPieces = pieceBoards[attackingColor + QUEEN] | pieceBoards[attackingColor + BISHOP];
    if (dangerousSquares & diagonalPieces) {
        return true;
    }


    //Checking sliding attacks
    bitboard dangerousSquare = magic.getStraightAllFinalSquares(square.x, square.y, hitmap);
    bitboard slidingPieces = pieceBoards[attackingColor + QUEEN] | pieceBoards[attackingColor + ROOK];
    if (dangerousSquare & slidingPieces) {
        return true;
    }

    //Checking king attacks
    dangerousSquares = getKingReachableSquares(square);
    if (dangerousSquares & pieceBoards[KING + attackingColor]) {
        return true;
    }

    //Checking pawn attacks
    int vy = -1 * (attackingColor == WHITE) + 1 * (attackingColor == BLACK);
    for (int i = 0; i < 2; i++) {
        int vx = 1 - 2 * i;
        if (vx + square.x > 7 || vx + square.x < 0 || vy + square.y > 7 || vy + square.y < 0) {
            continue;
        }

        if (generateBitboard(square.x + vx, square.y + vy) & pieceBoards[attackingColor + PAWN]) {
            return true;
        }
    }

    return false;
}

bool Game::isSquareUnderAttack(coord square, color attackingColor) {

    //Generating hitmaps
    bitboard ownHitmap = 0;
    bitboard enemyHitmap = 0;
    bitboard hitmap = 0;
    for (int i = 0; i < 6; i++) {
        ownHitmap |= pieceBoards[i + whoToMove];
        enemyHitmap |= pieceBoards[i + whoNotToMove];
    }
    hitmap = ownHitmap | enemyHitmap;

    return isSquareUnderAttack(square, attackingColor, hitmap);
}

bool Game::isKingSafe(color whichKing) {
    coord c;
    c.x = getXCoord(pieceBoards[whichKing + KING]);
    c.y = getYCoord(pieceBoards[whichKing + KING]);
    color otherColor;
    if (whichKing == WHITE) {
        otherColor = BLACK;
    } else {
        otherColor = WHITE;
    }
    return !isSquareUnderAttack(c, otherColor);
}

status Game::getStatus() {

    if (currentStatus != TBD) {
        return currentStatus;
    }

    //Checking 50 move rule
    if (counterToDraw >= 100) {
        currentStatus = DRAW;
        return DRAW;
    }


    //Check for insufficient material
    bool majorPieces;
    majorPieces = pieceBoards[ROOK + WHITE] | pieceBoards[ROOK + BLACK];
    majorPieces = majorPieces | pieceBoards[QUEEN + WHITE] | pieceBoards[QUEEN + BLACK];
    bool pawns = pieceBoards[PAWN + WHITE] | pieceBoards[PAWN + BLACK];

    if (!(majorPieces || pawns)) {
        int whiteMinorPieces = locatePieces(pieceBoards[KNIGHT + WHITE] | pieceBoards[BISHOP + WHITE]).size();
        int blackMinorPieces = locatePieces(pieceBoards[KNIGHT + BLACK] | pieceBoards[BISHOP + BLACK]).size();
        //  printGame();
        if (whiteMinorPieces < 2 && blackMinorPieces < 2) {
            return DRAW;
        } else if (blackMinorPieces == 0 && whiteMinorPieces == 2
                   && locatePieces(pieceBoards[KNIGHT + WHITE]).size() == 2) {
            return DRAW; //only two knights from white
        } else if (whiteMinorPieces == 0 && blackMinorPieces == 2
                   && locatePieces(pieceBoards[KNIGHT + BLACK]).size() == 2) {
            return DRAW; //only two knights from black
        }
    }

/* */
    //Checking 3-fold repetion
    uint64_t hash = std::hash<Game>()(*this);
    int counter = 0;
    for (int i = 0; i <= historyIndex; i++) {
        counter += hash == pastHashes[i];
    }
    if (counter >= 2) {
        currentStatus = DRAW;
        return DRAW;
    }

    //Checking for stalemate
    if (isMovePlayable()) {
        currentStatus = ON_GOING;
        return ON_GOING;
    }

    //Checking for stalemate or checkmate
    bool isCheck = !isKingSafe(whoToMove);
    if (isCheck) {
        currentStatus = CHECKMATE;
        return CHECKMATE;
    } else {
        currentStatus = DRAW;
        return DRAW;
    }
    return 0;
}


/*
 * This function checks if the position is legal, it is used to check if a pseudolegal move is legal
 */
bool Game::isPositionLegal() {
    return isKingSafe(whoNotToMove);
}

bool Game::operator==(const Game &other) const {
    //TODO compare if enpassant allows for an extra move
    if (this->whoToMove != other.whoToMove
        || this->castleRights != other.castleRights
        || this->enPassant != other.enPassant) {
        return false;
    }
    for (int i = 0; i < 12; i++) {
        if (this->pieceBoards[i] != other.pieceBoards[i]) {
            return false;
        }
    }

    return true;
}

const bitboard *Game::getPieceBoards() const {
    return pieceBoards;
}

uint8_t Game::getCastleRights() const {
    return castleRights;
}

uint8_t Game::getEnPassant() const {
    return enPassant;
}

color Game::getWhoToMove() const {
    return whoToMove;
}

color Game::getWhoNotToMove() const {
    return whoNotToMove;
}

int Game::getCounterToDraw() const {
    return counterToDraw;
}


/*
 * Loads a chess position using a fen as an input
 */
void Game::loadFen(std::string &fen) {
    currentStatus = TBD;
    moveCount = 0;

    //deleting all old stuff
    historyIndex = -1;

    for (int i = 0; i < 12; i++) {
        pieceBoards[i] = 0;
    }
    castleRights = 0;
    whoToMove = WHITE;
    whoNotToMove = BLACK;
    enPassant = 0;

    //Setting up
    bitboard pointer = 1;
    pointer = pointer << 63;
    int counter = 0;

    while (pointer != 0) {
        piece p = NONE;
        int skip = 0;
        char c = fen[counter];
        switch (fen[counter]) {
            case 'r':
                p = ROOK + BLACK;
                break;
            case 'n':
                p = KNIGHT + BLACK;
                break;
            case 'b':
                p = BISHOP + BLACK;
                break;
            case 'q':
                p = QUEEN + BLACK;
                break;
            case 'k':
                p = KING + BLACK;
                break;
            case 'p':
                p = PAWN + BLACK;
                break;

            case 'R':
                p = ROOK;
                break;
            case 'N':
                p = KNIGHT;
                break;
            case 'B':
                p = BISHOP;
                break;
            case 'Q':
                p = QUEEN;
                break;
            case 'K':
                p = KING;
                break;
            case 'P':
                p = PAWN;
                break;

                // Handle numbers in FEN (skip empty squares)
            case '1':
                skip = 1;
                break;
            case '2':
                skip = 2;
                break;
            case '3':
                skip = 3;
                break;
            case '4':
                skip = 4;
                break;
            case '5':
                skip = 5;
                break;
            case '6':
                skip = 6;
                break;
            case '7':
                skip = 7;
                break;
            case '8':
                skip = 8;
                break;
            default:
                skip = 0;
                break;  // In case of any unexpected characters
        }


        if (p == NONE) {
            pointer = pointer >> skip;
        } else {
            pieceBoards[p] |= pointer;
            pointer = pointer >> 1;
        }

        counter++;
    }

    bool castle = false;
    castleRights = 0;
    while (counter < fen.size()) {

        if (fen[counter] == 'w') {
            whoToMove = WHITE;
            whoNotToMove = BLACK;
        } else if (fen[counter] == 'b') {
            whoToMove = BLACK;
            whoNotToMove = WHITE;
        } else if (fen[counter] == 'K') {
            castleRights |= WHITE_SHORT_CASTLE_RIGHT;
            castle = true;
        } else if (fen[counter] == 'Q') {
            castleRights |= WHITE_LONG_CASTLE_RIGHT;
            castle = true;
        } else if (fen[counter] == 'k') {
            castleRights |= BLACK_SHORT_CASTLE_RIGHT;
            castle = true;
        } else if (fen[counter] == 'q') {
            castleRights |= BLACK_LONG_CASTLE_RIGHT;
            castle = true;
        } else if (fen[counter] == ' ' && castle) {
            break;
        }
        counter++;
    }
    while (counter < fen.size()) {

        if (fen[counter] == 'a') {
            enPassant = 1 << 7;
        } else if (fen[counter] == 'b') {
            enPassant = 1 << 6;
        } else if (fen[counter] == 'c') {
            enPassant = 1 << 5;
        } else if (fen[counter] == 'd') {
            enPassant = 1 << 4;
        } else if (fen[counter] == 'e') {
            enPassant = 1 << 3;
        } else if (fen[counter] == 'f') {
            enPassant = 1 << 2;
        } else if (fen[counter] == 'g') {
            enPassant = 1 << 1;
        } else if (fen[counter] == 'h') {
            enPassant = 1 << 0;
        }
        counter++;
    }

    eval = Evaluator::simpleEval(*this);
    //printGame();
}

/*
 * Prints a move such as "e2e4"
 */
void Game::printMove(move m) {
    //Generating a hitmaps, hitmaps indicate if a square is occupied or not
    bitboard moveTo = m.finalSquare;
    bitboard movedFrom = m.startingSquare;

    bitboard mb[2] = {movedFrom, moveTo};

    for (int i = 0; i < 2; i++) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {

                if (mb[i] >> (7 - x + 8 * y) == 1) {
                    //Found the square
                    char c = 'a' + x;
                    std::cout << c;
                    std::cout << y + 1;
                }
            }
        }
    }
}

/*
 * Generates all finalsquares a knight can reach from the starting square, not checking for collisions
 */
std::vector<bitboard> Game::generateKnightFinalSquares(coord knightLocation) {
    std::vector<bitboard> finalSquares;
    int &x = knightLocation.x;
    int &y = knightLocation.y;
    for (int i = 0; i < 8; i++) {
        int vx = 2 * (i == 0 || i == 1) + 1 * (i == 2 || i == 3) - 1 * (i == 4 || i == 5) - 2 * (i == 6 || i == 7);
        int vy = 2 * (i == 2 || i == 4) + 1 * (i == 0 || i == 6) - 1 * (i == 1 || i == 7) - 2 * (i == 3 || i == 5);
        if (x + vx >= 0 && x + vx < 8 && y + vy >= 0 && y + vy < 8) {
            finalSquares.push_back(generateBitboard(x + vx, y + vy));
        }
    }
    return finalSquares;
}

Game::Game() {
    Evaluator::initEvaluator();
    initSquaresLookup();
}


/*
 * Initialises the knight and king lookup table
 */
void Game::initSquaresLookup() {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            coord c;
            c.x = x;
            c.y = y;

            //Knight
            knightLookup[x + 8 * y].finalSquares = generateKnightFinalSquares(c);
            knightLookup[x + 8 * y].allSquares = 0;
            for (bitboard b: knightLookup[x + 8 * y].finalSquares) {
                knightLookup[x + 8 * y].allSquares |= b;
            }

            //king
            kingLookup[x + 8 * y].finalSquares = generateKingFinalSquares(c);
            kingLookup[x + 8 * y].allSquares = 0;
            for (bitboard b: kingLookup[x + 8 * y].finalSquares) {
                kingLookup[x + 8 * y].allSquares |= b;
            }
        }
    }


}

bitboard &Game::getKnightReachableSquares(coord location) {
    return knightLookup[location.x + location.y * 8].allSquares;
}

std::vector<bitboard> Game::generateKingFinalSquares(coord kingLocation) {
    std::vector<bitboard> finalSquares;
    finalSquares.reserve(8);
    int &x = kingLocation.x;
    int &y = kingLocation.y;
    for (int i = 0; i < 8; i++) {
        int vx = 1 * (i < 3) - 1 * (i >= 5);
        int vy = 1 * (i == 0 || i == 3 | i == 5) - 1 * (i == 2 || i == 4 || i == 7);
        if (x + vx >= 0 && x + vx < 8 && y + vy >= 0 && y + vy < 8) {
            bitboard finalSquare = generateBitboard(x + vx, y + vy);
            finalSquares.push_back(finalSquare);
        }
    }
    return finalSquares;
}

bitboard &Game::getKingReachableSquares(coord location) {
    return kingLookup[location.x + 8 * location.y].allSquares;
}

std::vector<bitboard> &Game::getKingFinalSquares(coord location) {
    return kingLookup[location.x + 8 * location.y].finalSquares;
}


GameKey Game::toKey() const {
    GameKey key;
    for (int i = 0; i < 12; ++i) {
        key.pieceBoards[i] = this->pieceBoards[i];
    }
    key.castleRights = this->castleRights;
    key.enPassant = this->enPassant;
    key.whoToMove = this->whoToMove;
    return key;
}

bool Game::isMovePlayable() {

    //Generating hitmaps
    bitboard ownHitmap = 0;
    bitboard enemyHitmap = 0;
    bitboard hitmap = 0;
    for (int i = 0; i < 6; i++) {
        ownHitmap |= pieceBoards[i + whoToMove];
        enemyHitmap |= pieceBoards[i + whoNotToMove];
    }
    hitmap = ownHitmap | enemyHitmap;

    //Knight moves
    auto knightLocations = locatePieces(pieceBoards[whoToMove + KNIGHT]);
    for (coord c: knightLocations) {
        std::vector<bitboard> finalSquares = getKnightFinalSquares(c);
        bitboard startingSquare = generateBitboard(c.x, c.y);
        //Checking for collision on each final square
        for (bitboard finalSquare: finalSquares) {
            if (!(finalSquare & ownHitmap)) {
                move m;
                m.startingSquare = startingSquare;
                m.finalSquare = finalSquare;
                m.movingPiece = KNIGHT;
                m.promotion = NONE;
                //Testing the move
                doMove(m);
                if (isPositionLegal()) {
                    undoMove();
                    return true;
                }
                undoMove();
            }
        }
    }

    //Straight sliding pieces
    bitboard slidingPieces = pieceBoards[ROOK + whoToMove] | pieceBoards[QUEEN + whoToMove];
    auto slidingPieceLocations = locatePieces(slidingPieces);
    for (coord c: slidingPieceLocations) {
        std::vector<bitboard> finalSquares = magic.getStraightFinalSquares(c.x, c.y, hitmap);
        bitboard startingSquare = generateBitboard(c.x, c.y);

        //Checking for collision on each final square
        for (bitboard finalSquare: finalSquares) {
            if (!(finalSquare & ownHitmap)) {
                move m;
                m.startingSquare = startingSquare;
                m.finalSquare = finalSquare;
                if (startingSquare & pieceBoards[ROOK + whoToMove]) {
                    m.movingPiece = ROOK;
                } else {
                    m.movingPiece = QUEEN;
                }
                m.promotion = NONE;
                doMove(m);
                if (isPositionLegal()) {
                    undoMove();
                    return true;
                }
                undoMove();
            }
        }
    }

    //Diagonal pieces
    bitboard diagonalPieces = pieceBoards[BISHOP + whoToMove] | pieceBoards[QUEEN + whoToMove];
    auto diagonalPieceLocations = locatePieces(diagonalPieces);
    for (coord c: diagonalPieceLocations) {
        std::vector<bitboard> finalSquares = magic.getDiagonalFinalSquares(c.x, c.y, hitmap);
        // generateDiagonalPieceFinalSquares(c, hitmap);
        bitboard startingSquare = generateBitboard(c.x, c.y);

        //Checking for collision on each final square
        for (bitboard finalSquare: finalSquares) {
            if (!(finalSquare & ownHitmap)) {
                move m;
                m.startingSquare = startingSquare;
                m.finalSquare = finalSquare;
                if (startingSquare & pieceBoards[BISHOP + whoToMove]) {
                    m.movingPiece = BISHOP;
                } else {
                    m.movingPiece = QUEEN;
                }
                m.promotion = NONE;
                doMove(m);
                if (isPositionLegal()) {
                    undoMove();
                    return true;
                }
                undoMove();
            }
        }
    }

    //King moves, not testing castle because if castle is possible so is a regular move
    coord location;
    location.x = getXCoord(pieceBoards[KING + whoToMove]);
    location.y = getYCoord(pieceBoards[KING + whoToMove]);
    auto finalSquares = generateKingFinalSquares(location);
    for (bitboard finalSquare: finalSquares) {
        //Checking for collision
        if (!(finalSquare & ownHitmap)) {
            move m;
            m.startingSquare = pieceBoards[whoToMove + KING];
            m.finalSquare = finalSquare;
            m.movingPiece = KING;
            m.promotion = NONE;
            doMove(m);
            if (isPositionLegal()) {
                undoMove();
                return true;
            }
            undoMove();
        }
    }

    //Generating pawn moves
    auto pawnLocations = locatePieces(pieceBoards[whoToMove + PAWN]);
    int vy = 1 - 2 * (whoToMove == BLACK);
    for (coord &c: pawnLocations) {
        bitboard startingSquare = generateBitboard(c.x, c.y);

        //Generating all finalSquares
        //Regular moves
        bitboard finalSquare = generateBitboard(c.x, c.y + vy);
        if (!(finalSquare & hitmap)) {
            move m;
            m.startingSquare = startingSquare;
            m.finalSquare = finalSquare;
            m.movingPiece = PAWN;

            //Checking for promotion
            if (c.y + vy == 0 || c.y + vy == 7) {
                m.promotion = QUEEN;
                //different promotions are not relevant for this test
                doMove(m);
                if (isPositionLegal()) {
                    undoMove();
                    return true;
                }
                undoMove();
            } else {
                //Not a promotion
                m.promotion = NONE;
                doMove(m);
                if (isPositionLegal()) {
                    undoMove();
                    return true;
                }
                undoMove();
            }
        }

        //Advancing 2 squares
        if ((c.y == 1 && whoToMove == WHITE) || (c.y == 6 && whoToMove == BLACK)) {
            finalSquare = generateBitboard(c.x, c.y + 2 * vy);
            bitboard middleSquare = generateBitboard(c.x, c.y + vy);
            if (!(finalSquare & hitmap)
                && !(middleSquare & hitmap)) {
                move m;
                m.startingSquare = startingSquare;
                m.finalSquare = finalSquare;
                m.promotion = NONE;
                m.movingPiece = PAWN;
                doMove(m);
                if (isPositionLegal()) {
                    undoMove();
                    return true;
                }
                undoMove();
            }
        }

        //Captures
        for (int i = 0; i < 2; i++) {
            int vx = 1 - 2 * i;
            if (c.x + vx >= 0 && c.x + vx < 8) {
                finalSquare = generateBitboard(c.x + vx, c.y + vy);
                if ((finalSquare & enemyHitmap)
                    || (((enPassant >> (7 - c.x - vx)) == 1) &&
                        ((whoToMove == WHITE && c.y == 4) || (whoToMove == BLACK && c.y == 3)))) {
                    move m;
                    m.startingSquare = startingSquare;
                    m.finalSquare = finalSquare;
                    m.movingPiece = PAWN;

                    //Checking for promotion
                    if (c.y + vy == 0 || c.y + vy == 7) {
                        m.promotion = QUEEN;
                        doMove(m);
                        if (isPositionLegal()) {
                            undoMove();
                            return true;
                        }
                        undoMove();
                    } else {
                        m.promotion = NONE;
                        doMove(m);
                        if (isPositionLegal()) {
                            undoMove();
                            return true;
                        }
                        undoMove();
                    }
                }
            }
        }
    }

    return false;
}

int Game::getEval() const {
    return eval;
}

void Game::setEval(int eval) {
    Game::eval = eval;
}
