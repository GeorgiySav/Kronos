#pragma once
#include "utility.h"
#include "Board.h"

namespace KRONOS {

	namespace EVALUATION {

		namespace PARAMS {
			
			struct Material {
				Score value;
				int16_t phase;
				int8_t flags;
				Material() : value({ 0, 0 }), phase(0), flags(0) {}
			};

			static Score pawnPST[64] = {
					Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0),
					Score(2, -8), Score(4, -6), Score(11,  9), Score(18,  5), Score(16, 16), Score(21,  6), Score(9, -6), Score(-3,-18),
					Score(-9, -9), Score(-15, -7), Score(11,-10), Score(15,  5), Score(31,  2), Score(23,  3), Score(6, -8), Score(-20, -5),
					Score(-3,  7), Score(-20,  1), Score(8, -8), Score(19, -2), Score(39,-14), Score(17,-13), Score(2,-11), Score(-5, -6),
					Score(11, 12), Score(-4,  6), Score(-11,  2), Score(2, -6), Score(11, -5), Score(0, -4), Score(-12, 14), Score(5,  9),
					Score(3, 27), Score(-11, 18), Score(-6, 19), Score(22, 29), Score(-8, 30), Score(-5,  9), Score(-14,  8), Score(-11, 14),
					Score(-7, -1), Score(6,-14), Score(-2, 13), Score(-11, 22), Score(4, 24), Score(-14, 17), Score(10,  7), Score(-9,  7),
					Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0), Score(0,  0)
			};

			static Score knightPST[64] = {
				 Score(-175, -96), Score(-92,-65), Score(-74,-49), Score(-73,-21), Score(-73,-21), Score(-74,-49), Score(-92,-65), Score(-175, -96),
				 Score(-77, -67), Score(-41,-54), Score(-27,-18), Score(-15,  8), Score(-15,  8), Score(-27,-18), Score(-41,-54), Score(-77, -67),
				 Score(-61, -40), Score(-17,-27), Score(6, -8), Score(12, 29), Score(12, 29), Score(6, -8), Score(-17,-27), Score(-61, -40),
				 Score(-35, -35), Score(8, -2), Score(40, 13), Score(49, 28), Score(49, 28), Score(40, 13), Score(8, -2), Score(-35, -35),
				 Score(-34, -45), Score(13,-16), Score(44,  9), Score(51, 39), Score(51, 39), Score(44,  9), Score(13,-16), Score(-34, -45),
				 Score(-9, -51), Score(22,-44), Score(58,-16), Score(53, 17), Score(53, 17), Score(58,-16), Score(22,-44), Score(-9, -51),
				 Score(-67, -69), Score(-27,-50), Score(4,-51), Score(37, 12), Score(37, 12), Score(4,-51), Score(-27,-50), Score(-67, -69),
				 Score(-201,-100), Score(-83,-88), Score(-56,-56), Score(-26,-17), Score(-26,-17), Score(-56,-56), Score(-83,-88), Score(-201,-100),
			};

			static Score bishopPST[64] = {
				 Score(-37,-40), Score(-4 ,-21), Score(-6,-26), Score(-16, -8), Score(-16, -8), Score(-6,-26), Score(-4 ,-21), Score(-37,-40),
				 Score(-11,-26), Score(6, -9), Score(13,-12), Score(3,  1), Score(3,  1), Score(13,-12), Score(6, -9), Score(-11,-26),
				 Score(-5 ,-11), Score(15, -1), Score(-4, -1), Score(12,  7), Score(12,  7), Score(-4, -1), Score(15, -1), Score(-5 ,-11),
				 Score(-4 ,-14), Score(8, -4), Score(18,  0), Score(27, 12), Score(27, 12), Score(18,  0), Score(8, -4), Score(-4 ,-14),
				 Score(-8 ,-12), Score(20, -1), Score(15,-10), Score(22, 11), Score(22, 11), Score(15,-10), Score(20, -1), Score(-8 ,-12),
				 Score(-11,-21), Score(4,  4), Score(1,  3), Score(8,  4), Score(8,  4), Score(1,  3), Score(4,  4), Score(-11,-21),
				 Score(-12,-22), Score(-10,-14), Score(4, -1), Score(0,  1), Score(0,  1), Score(4, -1), Score(-10,-14), Score(-12,-22),
				 Score(-34,-32), Score(1,-29), Score(-10,-26), Score(-16,-17), Score(-16,-17), Score(-10,-26), Score(1,-29), Score(-34,-32),
			};


			static Score rookPST[64] = {
				 Score(-31, -9), Score(-20,-13), Score(-14,-10), Score(-5, -9), Score(-5, -9),  Score(-14,-10), Score(-20,-13), Score(-31, -9),
				 Score(-21,-12), Score(-13, -9), Score(-8, -1), Score(6, -2),	Score(6, -2),  Score(-8, -1),	Score(-13, -9), Score(-21,-12),
				 Score(-25,  6), Score(-11, -8), Score(-1, -2), Score(3, -6),	Score(3, -6),  Score(-1, -2),	Score(-11, -8), Score(-25,  6),
				 Score(-13, -6), Score(-5,  1), Score(-4, -9), Score(-6,  7),	Score(-6,  7),  Score(-4, -9),	Score(-5,  1), Score(-13, -6),
				 Score(-27, -5), Score(-15,  8), Score(-4,  7), Score(3, -6),	Score(3, -6),  Score(-4,  7),	Score(-15,  8), Score(-27, -5),
				 Score(-22,  6), Score(-2,  1), Score(6, -7), Score(12, 10),	Score(12, 10),  Score(6, -7),	Score(-2,  1), Score(-22,  6),
				 Score(-2,  4), Score(12,  5), Score(16, 20), Score(18, -5),	Score(18, -5),  Score(16, 20),	Score(12,  5), Score(-2,  4),
				 Score(-17, 18), Score(-19,  0), Score(-1, 19), Score(9, 13),	Score(9, 13),  Score(-1, 19),	Score(-19,  0), Score(-17, 18),
			};

			static Score queenPST[64] = {
				 Score(3,-69), Score(-5,-57), Score(-5,-47), Score(4,-26), Score(4,-26), Score(-5,-47), Score(-5,-57), Score(3,-69),
				 Score(-3,-54), Score(5,-31), Score(8,-22), Score(12, -4), Score(12, -4),	Score(8,-22), Score(5,-31), Score(-3,-54),
				 Score(-3,-39), Score(6,-18), Score(13, -9), Score(7,  3), Score(7,  3),	Score(13, -9), Score(6,-18), Score(-3,-39),
				 Score(4,-23), Score(5, -3), Score(9, 13), Score(8, 24), Score(8, 24),	Score(9, 13), Score(5, -3), Score(4,-23),
				 Score(0,-29), Score(14, -6), Score(12,  9), Score(5, 21), Score(5, 21),	Score(12,  9), Score(14, -6), Score(0,-29),
				 Score(-4,-38), Score(10,-18), Score(6,-11), Score(8,  1), Score(8,  1),	Score(6,-11), Score(10,-18), Score(-4,-38),
				 Score(-5,-50), Score(6,-27), Score(10,-24), Score(8, -8), Score(8, -8),	Score(10,-24), Score(6,-27), Score(-5,-50),
				 Score(-2,-74), Score(-2,-52), Score(1,-43), Score(-2,-34), Score(-2,-34),	Score(1,-43), Score(-2,-52), Score(-2,-74),
			};

			static Score kingPST[64] = {
				Score(271,  1), Score(327, 45), Score(271, 85), Score(198, 76),	Score(198, 76),	Score(271, 85),	Score(327, 45),	Score(271,  1),
				Score(278, 53), Score(303,100), Score(234,133), Score(179,135),	Score(179,135),	Score(234,133),	Score(303,100),	Score(278, 53),
				Score(195, 88), Score(258,130), Score(169,169), Score(120,175),	Score(120,175),	Score(169,169),	Score(258,130),	Score(195, 88),
				Score(164,103), Score(190,156), Score(138,172), Score(98,172),	Score(98,172),	Score(138,172),	Score(190,156),	Score(164,103),
				Score(154, 96), Score(179,166), Score(105,199), Score(70,199),	Score(70,199),	Score(105,199),	Score(179,166),	Score(154, 96),
				Score(123, 92), Score(145,172), Score(81,184), Score(31,191),	Score(31,191),	Score(81,184),	Score(145,172),	Score(123, 92),
				Score(88, 47), Score(120,121), Score(65,116), Score(33,131),	Score(33,131),	Score(65,116),	Score(120,121),	Score(88, 47),
				Score(59, 11), Score(89, 59), Score(45, 73), Score(-1, 78),	Score(-1, 78),	Score(45, 73),	Score(89, 59),	Score(59, 11),
			};

			struct Eval_Parameters
			{
				Score PAWN_VALUE = { 100, 154 };
				Score KNIGHT_VALUE = { 376, 412 };
				Score BISHOP_VALUE = { 404, 460 };
				Score ROOK_VALUE = { 506, 821 };
				Score QUEEN_VALUE = { 1006, 1590 };
				Score KING_VALUE = { 20000, 20000 };

				Score CONNECTED_PAWN_VALUE = Score(8, 9);
				Score DOUBLED_PAWN_VALUE = Score(-9, -18);
				Score ISOLATED_PAWN_VALUE = Score(-3, -9);
				Score BACKWARD_PAWN_VALUE = Score(2, -11);
				Score PASSED_ISOLATED_PAWN_VALUE = Score(-21, -21);
				Score PASSED_BACKWARD_PAWN_VALUE = Score(-20, -20);

				Score PASSER_DIST_ALLY[8] = { {0, 0}, {-1, 3}, {2, -5}, {6, -13}, {0, -22}, {-8, -31}, {-13, -27}, {0, 0}, };
				Score PASSER_DIST_ENEMY[8] = { {0, 0}, {-3, 3}, {-4, 6}, {-5, 16}, {-1, 33}, {1, 49}, {14, 42}, {0, 0}, };
				Score PASSER_BONUS[8] = { {0, 0}, {-9, -5}, {-13, 11}, {-6, 2}, {23, 12}, {85, 28}, {121, 92}, {0, 0}, };
				Score UNBLOCKED_PASSER[8] = { {0, 0}, {6, 7}, {2, 1}, {4, 18}, {8, 29}, {19, 67}, {77, 114}, {0, 0}, };
				Score PASSER_SAFE_PUSH[8] = { {0, 0}, {10, -5}, {12, -2}, {0, 10}, {-2, 18}, {22, 36}, {77, 23}, {0, 0}, };
				Score PASSER_SAFE_PROM[8] = { {0, 0}, {-42, 25}, {-48, 14}, {-56, 36}, {-75, 67}, {-38, 129}, {-5, 213}, {0, 0}, };

				basic_score WEAK_TILE = 32;
				basic_score ENEMY_PAWN = -15;
				basic_score QUEEN_SAFE_CHECK = 75;
				basic_score ROOK_SAFE_CHECK = 108;
				basic_score BISHOP_SAFE_CHECK = 54;
				basic_score KNIGHT_SAFE_CHECK = 126;

				basic_score KING_SHELTER_1 = -35;
				basic_score KING_SHELTER_2 = -4;
				basic_score KING_SHELTER_F1 = -57;
				basic_score KING_SHELTER_F2 = -39;
				basic_score KingStorm1 = 122;
				basic_score KingStorm2 = 66;
				
				basic_score ATK_ON_KING_WEIGHT[5] = { 0, 64, 34, 29, 79 };

				Score MOBILITY_BONUS[6][28] = {
						{}, // PAWN
						{ {-82, -78}, {-23, -75}, {-5, -10}, {6, 7}, {13, 14}, {15, 29}, {22, 32}, {28, 34}, {31, 29}, },
						{ {-61, -86}, {-20, -47}, {-12, 3}, {0, 24}, {13, 29}, {21, 39}, {25, 47}, {30, 51}, {31, 56}, {32, 59}, {33, 60}, {42, 53}, {47, 61}, {65, 46}, },
						{ {-99, -149}, {-47, -88}, {-13, -17}, {-2, 5}, {5, 20}, {6, 33}, {6, 42}, {9, 42}, {13, 46}, {17, 52}, {16, 60}, {16, 65}, {14, 71}, {17, 72}, {18, 71}, },
						{ {-2271, -735}, {-736, 771}, {24, -527}, {8, -169}, {4, 49}, {25, 96}, {36, 24}, {39, 63}, {41, 86}, {45, 91}, {50, 93}, {54, 94}, {56, 105}, {59, 103}, {59, 113}, {60, 121}, {57, 133}, {60, 135}, {57, 135}, {53, 148}, {58, 140}, {65, 138}, {66, 126}, {58, 134}, {69, 125}, {118, 50}, {77, 49}, {389, -148}, }
				};
				Score ROOK_ON_7TH = Score(0, 53);
				Score ROOK_SEMI_OPEN_FILE_BONUS = Score(20, 5);
				Score ROOK_OPEN_FILE_BONUS = Score(38, 12);
				
				Score KNIGHT_OUTPOST_BONUS = Score(32, 27);
				Score KNIGHT_X_OUTPOST_BONUS = Score(17, 14);
				Score BISHOP_OUTPOST_BONUS = Score(39, 1);

				Score PST[2][6][64];

				Score BISHOP_PAWN_PENALTY = { 7, 9 };
				Score BISHOP_CENTER_CONTROL = Score(16, 1);

				Score FLANK_ATTACKS = Score(8, 0);
				Score PAWNLESS_FLANK = Score(19, 97);

				Score THREAT_PAWN_PUSH_VALUE = { 16, 13 };
				Score THREAT_WEAK_PAWNS_VALUE = { 7, 53 };
				Score THREAT_PAWNSxMINORS_VALUE = { 68, 30 };
				Score THREAT_MINORSxMINORS_VALUE = { 29, 40 };
				Score THREAT_MAJORSxWEAK_MINORS_VALUE = { 31, 81 };
				Score THREAT_PAWN_MINORSxMAJORS_VALUE = { 39, 29 };
				Score THREAT_ALLxQUEENS_VALUE = { 39, 21 };
				Score THREAT_KINGxMINORS_VALUE = { 22, 67 };
				Score THREAT_KINGxROOKS_VALUE = { -28, 54 };

				Score PIECE_SPACE = { 3, 3 };
				Score EMPTY_SPACE = { 5, 0 };

				basic_score PAWN_PHASE = 0;
				basic_score KNIGHT_PHASE = 2;
				basic_score BISHOP_PHASE = 5;
				basic_score ROOK_PHASE = 9;
				basic_score QUEEN_PHASE = 24;
				basic_score TEMPO = 38;

				//						P     K     B     R     Q
				Material MATERIAL_TABLE[9][9][3][3][3][3][3][3][2][2];

				// Polynomial material balance parameters
				Score Enemy_Coef[6][6] = {
					{ {35, 68}, },
					{ {0, -1}, {0, 0}, },
					{ {-5, -7}, {0, 9}, {0, -6}, },
					{ {-2, 30}, {0, 4}, {6, 7}, {4, -11}, },
					{ {1, -14}, {-1, 5}, {0, 14}, {4, 25}, {-4, -2}, },
					{ {-7, -26}, {0, -4}, {-15, -5}, {-23, 24}, {-56, 37}, {50, -399}, },
				};

				Score Ally_Coef[6][6] = {
					{ },
					{ {0, 1}, },
					{ {-1, 4}, {0, 5}, },
					{ {3, 14}, {0, 6}, {-1, -9}, },
					{ {-4, -10}, {-1, 7}, {-4, -11}, {-1, -7}, },
					{ {2, -25}, {2, 40}, {9, 81}, {3, 127}, {-102, 194}, },
				};

				Eval_Parameters();
				~Eval_Parameters();


				Score imbalance(const int pieceCount[2][6], bool side) {
					Score score;

					// Second-degree polynomial material imbalance, by Tord Romstad
					for (int pc1 = 0; pc1 < 6; pc1++) {
						int pCount = pieceCount[side][pc1];
						if (pCount == 0)
							continue;

						Score v = Ally_Coef[pc1][pc1] * pieceCount[side][pCount];

						for (int pc2 = 0; pc2 < pc1; pc2++) {
							v += Ally_Coef[pc1][pc2] * pieceCount[side][pc2] + Enemy_Coef[pc1][pc2] * pieceCount[!side][pc2];
						}
						score += v;
					}

					return score;
				}

				void initMatTab()
				{					
					for (int WP = 0; WP < 9; WP++) {
						for (int BP = 0; BP < 9; BP++) {
							for (int WB = 0; WB < 3; WB++) {
								for (int BB = 0; BB < 3; BB++) {
									for (int WK = 0; WK < 3; WK++) {
										for (int BK = 0; BK < 3; BK++) {
											for (int WR = 0; WR < 3; WR++) {
												for (int BR = 0; BR < 3; BR++) {
													for (int WQ = 0; WQ < 2; WQ++) {
														for (int BQ = 0; BQ < 2; BQ++) {

															Material& mat = MATERIAL_TABLE[WP][BP][WK][BK][WB][BB][WR][BR][WQ][BQ];
															mat.value = SCORE_ZERO;
															mat.phase = 0;
															mat.flags = 0;

															mat.value += PAWN_VALUE * WP + KNIGHT_VALUE * WK + BISHOP_VALUE * WB + ROOK_VALUE * WR + QUEEN_VALUE * WQ;
															mat.value -= (PAWN_VALUE * BP + KNIGHT_VALUE * BK + BISHOP_VALUE * BB + ROOK_VALUE * BR + QUEEN_VALUE * BQ);

															int pieceCount[][6] = {
																{WB > 1, WP, WK, WB, WR, WQ},
																{BB > 1, BP, BK, BB, BR, BQ}
															};

															mat.value += imbalance(pieceCount, WHITE);
															mat.value -= imbalance(pieceCount, BLACK);

															mat.phase = QUEEN_PHASE * (WQ + BQ) + ROOK_PHASE * (WR + BR) + BISHOP_PHASE * (WB + BB) + KNIGHT_PHASE * (WK + BK);

															int wMinors = WK + WB;
															int wMajors = WR + WQ;
															int bMinors = BK + BB;
															int bMajors = BR + BQ;
															int minors = wMinors + bMinors;
															int majors = wMajors + bMajors;

															if (WP + BP + minors + majors == 0)
																mat.flags |= 1;
															if (!WP && !BP) {
																// minor vs minor
																if (majors == 0 && wMinors == 1 && bMinors == 1)
																	mat.flags |= 1;
																// 2 knights
																if (majors == 0 && minors == 2 && (WK == 2 || BK == 2))
																	mat.flags |= 1;
																// rook vs minor
																if (majors == 1 && WR == 1 && wMinors == 0 && bMinors == 1)
																	mat.flags |= 2;
																// rook vs minor
																if (majors == 1 && BR == 1 && bMinors == 0 && wMinors == 1)
																	mat.flags |= 2;
																// rook and minor vs rook
																if (majors == 2 && WR == 1 && BR == 1 && minors < 2)
																	mat.flags |= 2;
															}
															// possible opposite coloured bishops
															if (majors == 0 && minors == 2 && WB == 1 && BB == 1)
																mat.flags |= 4;

														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}

				bool getMaterial(int WP, int BP, int WK, int BK, int WB, int BB, int WR, int BR, int WQ, int BQ, Material& mat) {
					if (WP < 9 && BP < 9 && WK < 3 && BK < 3 && WB < 3 && BB < 3 && WR < 3 && BR < 3 && WQ < 2 && BQ < 2) {
						mat = MATERIAL_TABLE[WP][BP][WK][BK][WB][BB][WR][BR][WQ][BQ];
						return true;
					}
					return false;
				}

				void saveParams();
				void loadParams(std::string filePath);
			};

			
		}

	}

}