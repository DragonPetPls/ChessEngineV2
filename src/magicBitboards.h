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

    uint64_t magicNumbers[64] = {11640913163564921777,
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

private:
    magicSquare straightSquares[64];
    void initStraightSquare(magicSquare* square, int x, int y);
    void calculateStraightSquare(int x, int y);
    bitboard generateBitboard(int x, int y);
    static std::vector<bitboard> generateAllBitboard(int x, int y);
public:
    magicBitboards();
    void calculateStraightMagicNumbers();
    std::vector<bitboard> generateStraightFinalSquares(int x, int y, bitboard hitmap);

    std::vector<bitboard> getStraightFinalSquares(int x, int y, bitboard hitmap);
    bitboard getStraightAllFinalSquares(int x, int y, bitboard hitmap);
};


#endif //CHESSENGINEV2_MAGICBITBOARDS_H
