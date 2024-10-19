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

#define NONE 13

#define EN_PASSANT_PAWN 12

#define ON_GOING 0
#define CHECKMATE 1
#define DRAW 2

#define TBD 3

#define NUMBER_OF_HASH_KEYS 1000

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

const bitboard WHITE_KING_CASTLE_STARTING_POS = 8;
const bitboard WHITE_KING_SHORT_CASTLE_FINAL_POS = 2;
const bitboard WHITE_KING_LONG_CASTLE_FINAL_POS = 32;
const bitboard BLACK_KING_CASTLE_STARTING_POS = 576460752303423488;
const bitboard BLACK_KING_SHORT_CASTLE_FINAL_POS = 144115188075855872;
const bitboard BLACK_KING_LONG_CASTLE_FINAL_POS = 2305843009213693952;

const bitboard ROWS[8] = {255, 65280, 16711680, 4278190080, 1095216660480, 280375465082880, 71776119061217280, 18374686479671623680};
const bitboard COLLOMS[8] = {9259542123273814144, 4629771061636907072, 2314885530818453536,1157442765409226768,
                             578721382704613384, 289360691352306692, 144680345676153346, 72340172838076673};

const bitboard COLLOM_ITERATION[3] = {COLLOMS[4] | COLLOMS[5] | COLLOMS [6] | COLLOMS[7],// ....||||
                                        COLLOMS[2] | COLLOMS[3] | COLLOMS[6] | COLLOMS[7], // ..||..||
                                          COLLOMS[1] | COLLOMS [3] | COLLOMS[5] | COLLOMS[7]};  // .|.|.|.|

const bitboard ROW_ITERATION[3] = {ROWS[4] | ROWS[5] | ROWS [6] | ROWS[7],// ....||||
                                      ROWS[2] | ROWS[3] | ROWS[6] | ROWS[7], // ..||..||
                                      ROWS[1] | ROWS [3] | ROWS[5] | ROWS[7]};  // .|.|.|.|


const uint64_t STRAIGHT_MAGIC_NUMBERS[64] = {11640913163564921777,
                             9104417478872246957,
                             13904580403335216905,
                             137661858952845636,
                             17952377568531374296,
                             12029000571330520825,
                             14634591788160751959,
                             16416690063608851827,
                             15140030154451786094,
                             16824302587029541517,
                             198415644733268023,
                             15874656163393968272,
                             6169740007303004843,
                             13363820805109891171,
                             10136314829030799537,
                             3715453640721322699,
                             10110270546040508512,
                             4317857459598716986,
                             13769022720963211554,
                             13953322047833281956,
                             14715104238039508394,
                             12595835926490795675,
                             790855008173001218,
                             12973552761084429813,
                             14441159688792496545,
                             16845805298438432420,
                             16253304216880091107,
                             11146079356090330549,
                             75860499954672164,
                             14850198627160959074,
                             6834255339832322594,
                             6593705182544822998,
                             16442616306588199209,
                             12606828037317668655,
                             14206672524430977415,
                             8978290059579164961,
                             8844036003933661906,
                             10720083498600368779,
                             10357491288024521729,
                             12774920802775891439,
                             4989409744875756735,
                             3180588863800316929,
                             8962104813414126589,
                             15685835594317447503,
                             10297540070041085044,
                             7401252306454177426,
                             11674010185832952064,
                             15597605476166662415,
                             12208835686182874698,
                             7635601978782399484,
                             14959996147770661866,
                             17353137308979540101,
                             15071944556564698542,
                             11014750584116254476,
                             4594724931905983782,
                             6858853982046434736,
                             16647454481415240890,
                             16455535553656658271,
                             1242556479752605655,
                             5537812754359001599,
                             1728659312955046062,
                             9571022670003212981,
                             10109280777067390330,
                             18056097919192544078};


const uint64_t DIAGONAL_MAGIC_NUMBER[64] = {7525880806140553291,
                                            15600746685787629685,
                                            2540506736674755542,
                                            4950799268018398462,
                                            16981397316276846049,
                                            18094642388848635838,
                                            2417848897855050097,
                                            17684874659336043538,
                                            17100605267897758706,
                                            5216330591992760082,
                                            11314907978804476518,
                                            14909137331990484295,
                                            9197943155637241592,
                                            5677876272376728784,
                                            12858897938770374418,
                                            1111538277226705857,
                                            13648584420059483355,
                                            11544354206578646261,
                                            12546345023335779176,
                                            1292074350115637587,
                                            12135271567755135657,
                                            336871807419566113,
                                            4194342925509090927,
                                            6904504524272795177,
                                            12398730670336919378,
                                            5177790475993730192,
                                            11814120747283346753,
                                            13449586589342782389,
                                            10698241687502575400,
                                            9609346567741445200,
                                            1703561958199356324,
                                            393606570293131923,
                                            10071758108692051126,
                                            10281499462033237881,
                                            9674585804497995097,
                                            7676754362080663125,
                                            912393720664145059,
                                            6544929289678643077,
                                            12120132405889625539,
                                            450257205533368486,
                                            16811748793407659503,
                                            15901319636302813320,
                                            8829305384202008600,
                                            16020587596246984318,
                                            6481566262988726801,
                                            6109615889925270084,
                                            7270669867829702142,
                                            5195987251366690857,
                                            1593226447417839959,
                                            10428623923903605224,
                                            5358652762819955977,
                                            12169262676172144738,
                                            6194915994767412215,
                                            8576570810658987410,
                                            16479966813631396567,
                                            14081591795599935188,
                                            15306579167372877592,
                                            13338218760500600185,
                                            17039166441174093357,
                                            13185822454495903075,
                                            6740959897408687237,
                                            3843839789642649329,
                                            9792057922310270908,
                                            9380129207171632586};
#endif //CHESSENGINEV2_CONSTANTS_H
