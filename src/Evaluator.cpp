//
// Created by fabian on 10/18/24.
//

#include <algorithm>
#include "Evaluator.h"
#include "Game.h"

uint64_t Evaluator::evalCounter;
bitboard Evaluator::whitePassedPawns[64];
bitboard Evaluator::blackPassedPawns[64];
const int *Evaluator::SIMPLE_TABLES[6] = {SIMPLE_PAWN_TABLE, SIMPLE_KNIGHT_TABLE, SIMPLE_BISHOP_TABLE, SIMPLE_ROOK_TABLE, SIMPLE_QUEEN_TABLE, SIMPLE_KING_TABLE};

int Evaluator::getNudge(move &m, Game &g) {


    int nudge = 0;

    int prevScore = getSquareValue(m.startingSquare, g.getWhoToMove(), m.movingPiece);
    int newScore = getSquareValue(m.finalSquare, g.getWhoToMove(), m.movingPiece);

    nudge += newScore - prevScore;

    //Small bonus for taking minor pieces, bigger bonus for major pieces
    nudge += mg_value[0] * ((m.finalSquare & pawnBitboard) != 0);
    nudge += mg_value[1] * ((m.finalSquare & knightBitboard) != 0);
    nudge += mg_value[2] * ((m.finalSquare & bishopBitboard) != 0);
    nudge += mg_value[3] * ((m.finalSquare & rookBitboard) != 0);
    nudge += mg_value[4] * ((m.finalSquare & queenBitboard) != 0);

    //Checks:
    switch (m.movingPiece) {
        case QUEEN:
            nudge += checkValue * ((m.finalSquare & straightChecks) != 0);
            nudge += checkValue * ((m.finalSquare & diagonalChecks) != 0);
            break;
        case ROOK:
            nudge += checkValue * ((m.finalSquare & straightChecks) != 0);
            break;
        case BISHOP:
            nudge += checkValue * ((m.finalSquare & diagonalChecks) != 0);
            break;
        case KNIGHT:
            nudge += checkValue * ((m.finalSquare & knightChecks) != 0);
            break;
    }

    if(m.promotion != NONE){
        nudge += (m.promotion != NONE) * mg_value[m.promotion] - mg_value[PAWN];
    }

    return nudge;
}

std::vector<int> Evaluator::rankMoves(Game &g, std::vector<move> &next, int bestCon) {

    //Initialising bitboards
    queenBitboard = g.getPieceBoards()[QUEEN + g.getWhoNotToMove()];
    rookBitboard = g.getPieceBoards()[ROOK + g.getWhoNotToMove()];

    knightBitboard = g.getPieceBoards()[KNIGHT + g.getWhoNotToMove()];
    bishopBitboard = g.getPieceBoards()[BISHOP + g.getWhoNotToMove()];
    pawnBitboard = g.getPieceBoards()[PAWN + g.getWhoNotToMove()];

    bitboard hitmap = 0;
    for(int i = 0; i < 12; i++){
        hitmap |= g.getPieceBoards()[i];
    }

    coord enemyKing{};
    enemyKing.x = Game::getXCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);
    enemyKing.y = Game::getYCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);

    diagonalChecks = g.magic.getDiagonalAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    straightChecks = g.magic.getStraightAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    knightChecks = g.knightLookup[enemyKing.x + enemyKing.y * 8].allSquares;

    std::vector<moveRank> order;
    order.reserve(next.size());
    //Getting all scores
    for(int i = 0; i < next.size(); i++){
        moveRank mr{};
        mr.score = getNudge(next[i], g);
        mr.score += 1000000 * (i == bestCon);
        mr.index = i;
        order.push_back(mr);
    }

    // Sort the vector by the score in ascending order
    std::sort(order.begin(), order.end(), [](const moveRank &a, const moveRank &b) {
        return a.score > b.score;  // compare scores for sorting
    });

    std::vector<int> intOrder;
    intOrder.reserve(order.size());
    for(moveRank mr: order){
        intOrder.push_back(mr.index);
    }

    return intOrder;
}

/*
 * This function returns a vector of index that points to all captures, checks that arent made with pawns and promotions
 */
std::vector<int> Evaluator::filterQuiesceCandidates(Game &g, std::vector<move> &next) {
    //Initialising bitboards
    queenBitboard = g.getPieceBoards()[QUEEN + g.getWhoNotToMove()];
    rookBitboard = g.getPieceBoards()[ROOK + g.getWhoNotToMove()];

    knightBitboard = g.getPieceBoards()[KNIGHT + g.getWhoNotToMove()];
    bishopBitboard = g.getPieceBoards()[BISHOP + g.getWhoNotToMove()];
    pawnBitboard = g.getPieceBoards()[PAWN + g.getWhoNotToMove()];

    bitboard hitmap = 0;
    for(int i = 0; i < 12; i++){
        hitmap |= g.getPieceBoards()[i];
    }

    coord enemyKing;
    enemyKing.x = g.getXCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);
    enemyKing.y = g.getYCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);

    diagonalChecks = g.magic.getDiagonalAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    straightChecks = g.magic.getStraightAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    knightChecks = g.knightLookup[enemyKing.x + enemyKing.y * 8].allSquares;

    bitboard block = diagonalChecks | straightChecks;

    std::vector<moveRank> order;
    order.reserve(next.size());
    //checking all moves
    for(int i = 0; i < next.size(); i++){
        //Checks:
        bool isCheck;

        if(next[i].movingPiece != PAWN && !(next[i].startingSquare & block)){
            //No discovered check or pawn check
            switch(next[i].movingPiece){
                case QUEEN:
                    isCheck = (diagonalChecks | straightChecks) & next[i].finalSquare;
                    break;
                case ROOK:
                    isCheck = straightChecks & next[i].finalSquare;
                    break;
                case BISHOP:
                    isCheck = diagonalChecks & next[i].finalSquare;
                    break;
                case KNIGHT:
                    isCheck = knightChecks & next[i].finalSquare;
                    break;
                default: isCheck = true;
            }
        } else {
            //PAWN or potential discovered check
            g.doMove(next[i]);
            isCheck = !g.isKingSafe(g.getWhoToMove());
            g.undoMove();
        }

        if(isCheck
           || next[i].finalSquare & hitmap
           || next[i].promotion != NONE){
            moveRank mr;
            mr.score = getNudge(next[i], g);
            mr.index = i;
            order.push_back(mr);
        }
    }

    // Sort the vector by the score in ascending order
    std::sort(order.begin(), order.end(), [](const moveRank &a, const moveRank &b) {
        return a.score > b.score;  // compare scores for sorting
    });

    std::vector<int> intOrder;
    intOrder.reserve(order.size());
    for(moveRank mr: order){
        intOrder.push_back(mr.index);
    }

    return intOrder;
}

int Evaluator::getSquareValue(piece p, coord c, int endgame) {

    int y = c.y * (p < 6) + (7 - c.y) * (p >= 6);
    int index = (7 - y) * 8 + c.x;
    p = p - 6 * (p > 5);

    int endgameValue;
    int midgameValue;
    switch (p) {
        case QUEEN:
            endgameValue = eg_queen_table[index];
            midgameValue = mg_queen_table[index];
            break;
        case ROOK:
            endgameValue = eg_rook_table[index];
            midgameValue = mg_rook_table[index];
            break;
        case BISHOP:
            endgameValue = eg_bishop_table[index];
            midgameValue = mg_bishop_table[index];
            break;
        case KNIGHT:
            endgameValue = eg_knight_table[index];
            midgameValue = mg_knight_table[index];
            break;
        case PAWN:
            endgameValue = eg_pawn_table[index];
            midgameValue = mg_pawn_table[index];
            break;
        case KING:
            endgameValue = eg_king_table[index];
            midgameValue = mg_king_table[index];
            break;
        default:
            // Handle the case where the piece 'p' is invalid or unrecognized
            endgameValue = 0;
            midgameValue = 0;
            break;
    }

    //Weighting differently depending on the endgame factor
    return (endgameValue * endgame + midgameValue * (10 - endgame))/10;
}

std::vector<int> Evaluator::filterNullMoves(Game &g, std::vector<move> &next, int bestCon) {

    //Initialising bitboards
    queenBitboard = g.getPieceBoards()[QUEEN + g.getWhoNotToMove()];
    rookBitboard = g.getPieceBoards()[ROOK + g.getWhoNotToMove()];

    knightBitboard = g.getPieceBoards()[KNIGHT + g.getWhoNotToMove()];
    bishopBitboard = g.getPieceBoards()[BISHOP + g.getWhoNotToMove()];
    pawnBitboard = g.getPieceBoards()[PAWN + g.getWhoNotToMove()];

    bitboard hitmap = 0;
    for(int i = 0; i < 12; i++){
        hitmap |= g.getPieceBoards()[i];
    }

    coord enemyKing{};
    enemyKing.x = Game::getXCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);
    enemyKing.y = Game::getYCoord(g.getPieceBoards()[g.getWhoNotToMove() + KING]);

    diagonalChecks = g.magic.getDiagonalAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    straightChecks = g.magic.getStraightAllFinalSquares(enemyKing.x, enemyKing.y, hitmap);
    knightChecks = g.knightLookup[enemyKing.x + enemyKing.y * 8].allSquares;

    std::vector<moveRank> order;
    order.reserve(next.size());
    //Getting all scores
    for(int i = 0; i < next.size(); i++){
        moveRank mr{};
        mr.score = getNudge(next[i], g);
        mr.score += 1000000 * (i == bestCon);
        if(mr.score > 0){
            mr.index = i;
            order.push_back(mr);
        }
    }

    // Sort the vector by the score in ascending order
    std::sort(order.begin(), order.end(), [](const moveRank &a, const moveRank &b) {
        return a.score > b.score;  // compare scores for sorting
    });

    std::vector<int> intOrder;
    intOrder.reserve(order.size());
    for(moveRank mr: order){
        intOrder.push_back(mr.index);
    }
    return intOrder;
}


int Evaluator::evalPosition(Game &g, int scoreToBeat) {
    evalCounter++;
    switch (g.getStatus()) {
        case DRAW:
            return 0;
        case CHECKMATE:
            return LOSS + g.moveCount;
        default:
            break;
    }

    //return g.getEval();
    if((scoreToBeat + 200 < g.getEval())){
        Evaluator::evalCounter--;
        return g.getEval();
    }

    //Getting piece value
    std::vector<coord> pieceLocations[12];
    for (int i = 0; i < 12; i++) {
        pieceLocations[i] = Game::locatePieces(g.getPieceBoards()[i]);
    }

    //Checking how endgame it is:
    int value = 0;
    value += (int) pieceLocations[ROOK + WHITE].size() * 5;
    value += (int) pieceLocations[ROOK + BLACK].size() * 5;
    value += (int) pieceLocations[QUEEN + WHITE].size() * 9;
    value += (int) pieceLocations[QUEEN + BLACK].size() * 9;
    value += (int) pieceLocations[KNIGHT + WHITE].size() * 3;
    value += (int) pieceLocations[KNIGHT + BLACK].size() * 3;
    value += (int) pieceLocations[BISHOP + WHITE].size() * 3;
    value += (int) pieceLocations[BISHOP + BLACK].size() * 3;

    int endgame = 10 * (value <= 7) + (-0.5 * value + 13) * (value > 7 && value < 26);

    int eval = 0;
    for (int i = 0; i < 6; i++) {
        eval += (int) (pieceLocations[i + g.getWhoToMove()].size() * (eg_value[i] * endgame + mg_value[i] * (10 - endgame)))/10;
        eval -= (int) (pieceLocations[i + g.getWhoNotToMove()].size() * (eg_value[i] * endgame + mg_value[i] * (10 - endgame)))/10;
    }

    for (int i = 0; i < 6; i++) {
        for (coord c: pieceLocations[i + g.getWhoToMove()]) {
            eval += getSquareValue(i + g.getWhoToMove(), c, endgame);
        }
        for (coord c: pieceLocations[i + g.getWhoNotToMove()]) {
            eval -= getSquareValue(i + g.getWhoNotToMove(), c, endgame);
        }
    }


    //Checking for passed pawns
    int whiteEval = 0;
    for(coord c: pieceLocations[WHITE + PAWN]){
        whiteEval += 20 * c.y * ((whitePassedPawns[c.x + 8 * c.y] & g.getPieceBoards()[PAWN + BLACK]) == 0);
    }
    int blackEval = 0;
    for(coord c: pieceLocations[BLACK + PAWN]){
        blackEval += 20 * (7 - c.y) * ((blackPassedPawns[c.x + 8 * c.y] & g.getPieceBoards()[PAWN + WHITE]) == 0);
    }
    if(g.getWhoToMove() == WHITE){
        eval += (whiteEval - blackEval)/(14 - endgame);
    } else {
        eval += (blackEval - whiteEval)/(14 - endgame);
    }

    //King pawn distance:
    int xKing = Game::getXCoord(g.getPieceBoards()[KING + g.getWhoToMove()]);
    int yKing = Game::getYCoord(g.getPieceBoards()[KING + g.getWhoToMove()]);
    auto pawnLocation = Game::locatePieces(g.getPieceBoards()[PAWN + g.getWhoToMove()]);
    int distance = 0;
    for(coord c: pawnLocation){
        distance += std::min(abs(xKing - c.x), abs(yKing - c.y));

        //Bonus for pawns protecting other pawns
       eval += PAWN_PROTECTION_BONUS * ((g.getPieceBoards()[g.getWhoToMove() + PAWN] & Game::generateBitboard(c.x + 1, c.y + 1)) != 0);
       eval += PAWN_PROTECTION_BONUS * ((g.getPieceBoards()[g.getWhoToMove() + PAWN] & Game::generateBitboard(c.x - 1, c.y + 1)) != 0);
    }
    eval -= distance * endgame;

    //King pawn distance:
    xKing = Game::getXCoord(g.getPieceBoards()[KING + g.getWhoNotToMove()]);
    yKing = Game::getYCoord(g.getPieceBoards()[KING + g.getWhoNotToMove()]);
    pawnLocation = Game::locatePieces(g.getPieceBoards()[PAWN + g.getWhoNotToMove()]);
    distance = 0;
    for(coord c: pawnLocation){
        distance += std::min(abs(xKing - c.x), abs(yKing - c.y));

        //Bonus for pawns protecting other pawns
       eval -= PAWN_PROTECTION_BONUS * ((g.getPieceBoards()[g.getWhoNotToMove() + PAWN] & Game::generateBitboard(c.x + 1, c.y + 1)) != 0);
       eval -= PAWN_PROTECTION_BONUS * ((g.getPieceBoards()[g.getWhoNotToMove() + PAWN] & Game::generateBitboard(c.x - 1, c.y + 1)) != 0);
    }
    eval += distance * endgame;

    //std::cout << "Eval " << eval << " g: " << g.getEval() << " " << (int) g.getWhoToMove() << std::endl;
    return eval;
}

/*
 * A simple evaluation function, used when the game sets up a new position. Not for the static evaluation at the end of the search
 */
int Evaluator::simpleEval(Game &g) {
    switch (g.getStatus()) {
        case DRAW:
            return 0;
        case CHECKMATE:
            return LOSS + g.moveCount;
        default:
            break;
    }

    //locating all pieces exept the king
    std::vector<coord> pieceLocations[12];
    for (int i = 0; i < 5; i++) {
        pieceLocations[i] = Game::locatePieces(g.getPieceBoards()[i]);
        pieceLocations[i + BLACK] = Game::locatePieces(g.getPieceBoards()[i + BLACK]);
    }

    int eval = 0;
    eval += SIMPLE_PAWN_VALUE * ((int) (pieceLocations[WHITE + PAWN].size() - pieceLocations[BLACK + PAWN].size()));
    eval += SIMPLE_KNIGHT_VALUE * ((int) (pieceLocations[WHITE + KNIGHT].size() - pieceLocations[BLACK + KNIGHT].size()));
    eval += SIMPLE_BISHOP_VALUE * ((int) (pieceLocations[WHITE + BISHOP].size() - pieceLocations[BLACK + BISHOP].size()));
    eval += SIMPLE_ROOK_VALUE * ((int) (pieceLocations[WHITE + ROOK].size() - pieceLocations[BLACK + ROOK].size()));
    eval += SIMPLE_QUEEN_VALUE * ((int) (pieceLocations[WHITE + QUEEN].size() - pieceLocations[BLACK + QUEEN].size()));


    //Getting piece square values
    //Pawns
    for(auto l: pieceLocations[PAWN + WHITE]){
        eval += SIMPLE_PAWN_TABLE[l.x + 8 * (7 - l.y)];
    }
    for(auto l: pieceLocations[PAWN + BLACK]){
        eval -= SIMPLE_PAWN_TABLE[l.x + 8 * l.y];
    }

    //knight
    for(auto l: pieceLocations[KNIGHT + WHITE]){
        eval += SIMPLE_KNIGHT_TABLE[l.x + 8 * (7 - l.y)];
    }
    for(auto l: pieceLocations[KNIGHT + BLACK]){
        eval -= SIMPLE_KNIGHT_TABLE[l.x + 8 * l.y];
    }

    //bishop
    for(auto l: pieceLocations[BISHOP + WHITE]){
        eval += SIMPLE_BISHOP_TABLE[l.x + 8 * (7 - l.y)];
    }
    for(auto l: pieceLocations[BISHOP + BLACK]){
        eval -= SIMPLE_BISHOP_TABLE[l.x + 8 * l.y];
    }

    //rook
    for(auto l: pieceLocations[ROOK + WHITE]){
        eval += SIMPLE_ROOK_TABLE[l.x + 8 * (7 - l.y)];
    }
    for(auto l: pieceLocations[ROOK + BLACK]){
        eval -= SIMPLE_ROOK_TABLE[l.x + 8 * l.y];
    }

    //queen
    for(auto l: pieceLocations[QUEEN + WHITE]){
        eval += SIMPLE_QUEEN_TABLE[l.x + 8 * (7 - l.y)];
    }
    for(auto l: pieceLocations[QUEEN + BLACK]){
        eval -= SIMPLE_QUEEN_TABLE[l.x + 8 * l.y];
    }

    eval *= (g.getWhoToMove() == WHITE) - 1 * (g.getWhoToMove() == BLACK);

    return eval;
}

/*
 * Returns the eval Difference in a move according to the simple evaluation,
 * returns a high score if the move is good for the player playing the move
 */
int Evaluator::getEvalDifference(move m, Game &g) {

    int oldEval = 0;
    int newEval = 0;
    int fromX = Game::getXCoord(m.startingSquare);
    int fromY = Game::getYCoord(m.startingSquare);
    int toX = Game::getXCoord(m.finalSquare);
    int toY = Game::getYCoord(m.finalSquare);

    toY = (7 - toY) * (g.getWhoToMove() == WHITE) + toY * (g.getWhoToMove() == BLACK);
    fromY = (7 - fromY) * (g.getWhoToMove() == WHITE) + fromY * (g.getWhoToMove() == BLACK);
    oldEval = SIMPLE_TABLES[m.movingPiece][7 - fromX + 8 * fromY];
    newEval = SIMPLE_TABLES[m.movingPiece][7 - toX + 8 * toY];

    //Promotions
    newEval += (m.promotion != NONE) * (SIMPLE_VALUES[m.promotion] + SIMPLE_TABLES[m.promotion][7 - toX + 8 * toY] - SIMPLE_VALUES[PAWN] - newEval);

    //Checking captures
    for(int i = 0; i < 5; i++){
        newEval += SIMPLE_VALUES[i] * ((g.getPieceBoards()[i + g.getWhoNotToMove()] & m.finalSquare) != 0);
    }

    //Rook movements from castling can be ignored as they do not change eval

    return newEval - oldEval;
}

int Evaluator::getSquareValue(bitboard square, color pieceColor, piece pieceType) {
    piece p = pieceType + pieceColor;
    coord c{Game::getXCoord(square), Game::getYCoord(square)};
    return getSquareValue(p, c, 5);
}

void Evaluator::initEvaluator() {
//Initialising passed pawn bitboards
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){

            bitboard colloms = 0;
            if(x == 0){
                colloms |= COLLOMS[0] | COLLOMS[1];
            } else if (x == 7){
                colloms |= COLLOMS[6] | COLLOMS[7];
            } else {
                colloms |= COLLOMS[x - 1] | COLLOMS[x] | COLLOMS[x + 1];
            }

            whitePassedPawns[x +  8 * y] = colloms;
            blackPassedPawns[x + 8 * y] = colloms;

            for(int i = 0; i < 8; i++){
                if(i <= y){
                    whitePassedPawns[x + 8 * y] &= ~ROWS[i];
                }
                if(i >= y){
                    blackPassedPawns[x + 8 * y] &= ~ROWS[i];
                }
            }
        }
    }
}


