#pragma once
#include "utility.h"
#include "Board.h"

namespace KRONOS {

	namespace EVALUATION {

		namespace PARAMS {
			
			struct Material {
				Score value[2];
				int16_t phase;
				int8_t flags;
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

				Score MOBILITY_BONUS[6] = {
						{}, // PAWN
						Score(7, 7),
						Score(9, 6),
						Score(4, 6),
						Score(3, 5)
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

				int MAT_MUL[6] = { 0,1,9,27,81,243 };

				Material MATERIAL_TABLE[486][486];

				Score IMBALANCE_INTERNAL[6][6] = {
					{ {35, 68}, },
					{ {0, -1}, {0, 0}, },
					{ {-5, -7}, {0, 9}, {0, -6}, },
					{ {-2, 30}, {0, 4}, {6, 7}, {4, -11}, },
					{ {1, -14}, {-1, 5}, {0, 14}, {4, 25}, {-4, -2}, },
					{ {-7, -26}, {0, -4}, {-15, -5}, {-23, 24}, {-56, 37}, {50, -399}, },
				};

				Score IMBALANCE_EXTERNAL[6][6] = {
					{ },
					{ {0, 1}, },
					{ {-1, 4}, {0, 5}, },
					{ {3, 14}, {0, 6}, {-1, -9}, },
					{ {-4, -10}, {-1, 7}, {-4, -11}, {-1, -7}, },
					{ {2, -25}, {2, 40}, {9, 81}, {3, 127}, {-102, 194}, },
				};

				Eval_Parameters();
				~Eval_Parameters();

				void clearParameters();

				Score imbalance(const int pieceCount[2][6], bool side) {
					side = !side;
					Score bonus;
					// Second-degree polynomial material imbalance, by Tord Romstad
					for (int pt1 = 0; pt1 <= 4; ++pt1) {
						if (!pieceCount[side][pt1]) continue;
						Score value = IMBALANCE_INTERNAL[pt1][pt1] * pieceCount[side][pt1];
						for (int pt2 = 0; pt2 < pt1; ++pt2)
							value += IMBALANCE_INTERNAL[pt1][pt2] * pieceCount[side][pt2] + IMBALANCE_EXTERNAL[pt1][pt2] * pieceCount[!side][pt2];
						bonus += value * pieceCount[side][pt1];
					}
					return bonus;
				}

				void initMatTab()
				{
					memset(MATERIAL_TABLE, 0, sizeof(MATERIAL_TABLE));
					for (int wq = 0; wq <= 1; wq++) for (int bq = 0; bq <= 1; bq++)
						for (int wr = 0; wr <= 2; wr++) for (int br = 0; br <= 2; br++)
							for (int wb = 0; wb <= 2; wb++) for (int bb = 0; bb <= 2; bb++)
								for (int wn = 0; wn <= 2; wn++) for (int bn = 0; bn <= 2; bn++)
									for (int wp = 0; wp <= 8; wp++) for (int bp = 0; bp <= 8; bp++) {
										int idx1 = wp * MAT_MUL[PAWN] + wn * MAT_MUL[KNIGHT] + wb * MAT_MUL[BISHOP] + wr * MAT_MUL[ROOK] + wq * MAT_MUL[QUEEN];
										int idx2 = bp * MAT_MUL[PAWN] + bn * MAT_MUL[KNIGHT] + bb * MAT_MUL[BISHOP] + br * MAT_MUL[ROOK] + bq * MAT_MUL[QUEEN];

										Material& mat = MATERIAL_TABLE[idx1][idx2];
										mat.phase = QUEEN_PHASE * (wq + bq) + ROOK_PHASE * (wr + br) + BISHOP_PHASE * (wb + bb) + KNIGHT_PHASE * (wn + bn);
										mat.value[1] = { 0, 0 }; mat.value[0] = { 0, 0 };
										mat.flags = 0;

										int pieceCount[2][6] = {
											{wb > 1, wp, wn, wb, wr, wq},
											{bb > 1, bp, bn, bb, br, bq},
										};

										for (int side = 0; side <= 1; ++side) {
											Score scr;
											scr += PAWN_VALUE * (side == WHITE ? wp : bp);
											scr += KNIGHT_VALUE * (side == WHITE ? wn : bn);
											scr += BISHOP_VALUE * (side == WHITE ? wb : bb);
											scr += ROOK_VALUE * (side == WHITE ? wr : br);
											scr += QUEEN_VALUE * (side == WHITE ? wq : bq);
											mat.value[WHITE] += scr * (side == WHITE ? 1 : -1);
											mat.value[WHITE] += imbalance(pieceCount, side) * (side == WHITE ? 1 : -1);
											scr = SCORE_ZERO;
											scr += PAWN_VALUE * (side == BLACK ? bp : wp);
											scr += KNIGHT_VALUE * (side == BLACK ? bn : wn);
											scr += BISHOP_VALUE * (side == BLACK ? bb : wb);
											scr += ROOK_VALUE * (side == BLACK ? br : wr);
											scr += QUEEN_VALUE * (side == BLACK ? bq : wq);
											mat.value[BLACK] += scr * (side == BLACK ? 1 : -1);
											mat.value[BLACK] += imbalance(pieceCount, side) * (side == BLACK ? 1 : -1);
										}
										

										int wminors = wn + wb;
										int bminors = bn + bb;
										int wmajors = wr + wq;
										int bmajors = br + bq;
										int minors = wminors + bminors;
										int majors = wmajors + bmajors;

										if (wp + bp + minors + majors == 0) mat.flags |= 1;
										if (!wp && !bp) {
											if (majors == 0 && wminors < 2 && bminors < 2) mat.flags |= 1; // minor vs minor
											if (majors == 0 && minors == 2 && (wn == 2 || bn == 2)) mat.flags |= 1; // 2 knights
											if (majors == 1 && wr == 1 && wminors == 0 && bminors == 1) mat.flags |= 2; // rook vs minor
											if (majors == 1 && br == 1 && bminors == 0 && wminors == 1) mat.flags |= 2; // rook vs minor
											if (majors == 2 && wr == 1 && br == 1 && minors < 2) mat.flags |= 2; // rook+minor vs rook
										}
										if (majors == 0 && minors == 2 && wb == 1 && bb == 1) mat.flags |= 4; // possible ocb
									}
				}

				Material& getMaterial(int idx1, int idx2) {
					return MATERIAL_TABLE[idx1][idx2];
				}

				void saveParams();
				void loadParams(std::string filePath);
			};

			
		}

	}

}