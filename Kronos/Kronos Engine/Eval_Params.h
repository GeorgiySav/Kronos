#pragma once
#include "utility.h"

namespace KRONOS {

	namespace EVALUATION {

		namespace PARAMS {

			static Score PAWN_VALUE = { 100, 150 };
			static Score KNIGHT_VALUE = { 370, 400 };
			static Score BISHOP_VALUE = { 400, 450 };
			static Score ROOK_VALUE = { 500, 800 };
			static Score QUEEN_VALUE = { 1000, 1500 };
			static Score KING_VALUE = { 20000, 20000 };

			static Score CONNECTED_PAWN_VALUE =       Score( 8, 8 );
			static Score HALF_CONNECTED_PAWN_VALUE =  Score( 7, 7 );
			static Score DOUBLED_PAWN_VALUE =         Score( -9, -18 );
			static Score ISOLATED_PAWN_VALUE =        Score( -3, -9 );
			static Score HALF_ISOLATED_PAWN_VALUE =   Score( 0, 0 );
			static Score BACKWARD_PAWN_VALUE =        Score( 2, -10 );
			static Score PASSED_ISOLATED_PAWN_VALUE = Score( -20, -20 );
			static Score PASSED_BACKWARD_PAWN_VALUE = Score( -20, -20 );

			static Score BISHOP_ON_KING_RING = Score(24, 0);
			static Score ROOK_ON_KING_RING = Score(16, 0);

			static basic_score kingAttackWeights[] = { 0, 75, 46, 45, 15 };

			CompileTime int tileFile(int tile) {
				return tile % 8;
			}
			CompileTime int tileRank(int tile) {
				return tile / 8;
			}

			static Score pawnPST[64] = {
				Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0),
				Score(  2, -8), Score(  4, -6), Score( 11,  9), Score( 18,  5), Score( 16, 16), Score( 21,  6), Score(  9, -6), Score( -3,-18),
				Score( -9, -9), Score(-15, -7), Score( 11,-10), Score( 15,  5), Score( 31,  2), Score( 23,  3), Score(  6, -8), Score(-20, -5),
				Score( -3,  7), Score(-20,  1), Score(  8, -8), Score( 19, -2), Score( 39,-14), Score( 17,-13), Score(  2,-11), Score( -5, -6),
				Score( 11, 12), Score( -4,  6), Score(-11,  2), Score(  2, -6), Score( 11, -5), Score(  0, -4), Score(-12, 14), Score(  5,  9),
				Score(  3, 27), Score(-11, 18), Score( -6, 19), Score( 22, 29), Score( -8, 30), Score( -5,  9), Score(-14,  8), Score(-11, 14),
				Score( -7, -1), Score(  6,-14), Score( -2, 13), Score(-11, 22), Score(  4, 24), Score(-14, 17), Score( 10,  7), Score( -9,  7),
				Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0), Score(  0,  0)
			};

			static Score knightPST[64] = {
				 Score(-175, -96), Score(-92,-65), Score(-74,-49), Score(-73,-21), Score(-73,-21), Score(-74,-49), Score(-92,-65), Score(-175, -96),
				 Score( -77, -67), Score(-41,-54), Score(-27,-18), Score(-15,  8), Score(-15,  8), Score(-27,-18), Score(-41,-54), Score( -77, -67),
				 Score( -61, -40), Score(-17,-27), Score(  6, -8), Score( 12, 29), Score( 12, 29), Score(  6, -8), Score(-17,-27), Score( -61, -40),
				 Score( -35, -35), Score(  8, -2), Score( 40, 13), Score( 49, 28), Score( 49, 28), Score( 40, 13), Score(  8, -2), Score( -35, -35),
				 Score( -34, -45), Score( 13,-16), Score( 44,  9), Score( 51, 39), Score( 51, 39), Score( 44,  9), Score( 13,-16), Score( -34, -45),
				 Score(  -9, -51), Score( 22,-44), Score( 58,-16), Score( 53, 17), Score( 53, 17), Score( 58,-16), Score( 22,-44), Score(  -9, -51),
				 Score( -67, -69), Score(-27,-50), Score(  4,-51), Score( 37, 12), Score( 37, 12), Score(  4,-51), Score(-27,-50), Score( -67, -69),
				 Score(-201,-100), Score(-83,-88), Score(-56,-56), Score(-26,-17), Score(-26,-17), Score(-56,-56), Score(-83,-88), Score(-201,-100),
			};

			static Score bishopPST[64] = {
				 Score(-37,-40), Score(-4 ,-21), Score( -6,-26), Score(-16, -8), Score(-16, -8), Score( -6,-26), Score(-4 ,-21), Score(-37,-40),
				 Score(-11,-26), Score(  6, -9), Score( 13,-12), Score(  3,  1), Score(  3,  1), Score( 13,-12), Score(  6, -9), Score(-11,-26),
				 Score(-5 ,-11), Score( 15, -1), Score( -4, -1), Score( 12,  7), Score( 12,  7), Score( -4, -1), Score( 15, -1), Score(-5 ,-11),
				 Score(-4 ,-14), Score(  8, -4), Score( 18,  0), Score( 27, 12), Score( 27, 12), Score( 18,  0), Score(  8, -4), Score(-4 ,-14),
				 Score(-8 ,-12), Score( 20, -1), Score( 15,-10), Score( 22, 11), Score( 22, 11), Score( 15,-10), Score( 20, -1), Score(-8 ,-12),
				 Score(-11,-21), Score(  4,  4), Score(  1,  3), Score(  8,  4), Score(  8,  4), Score(  1,  3), Score(  4,  4), Score(-11,-21),
				 Score(-12,-22), Score(-10,-14), Score(  4, -1), Score(  0,  1), Score(  0,  1), Score(  4, -1), Score(-10,-14), Score(-12,-22),
				 Score(-34,-32), Score(  1,-29), Score(-10,-26), Score(-16,-17), Score(-16,-17), Score(-10,-26), Score(  1,-29), Score(-34,-32),
			};


			static Score rookPST[64] = {
				 Score(-31, -9), Score(-20,-13), Score(-14,-10), Score(-5, -9), Score(-5, -9),  Score(-14,-10), Score(-20,-13), Score(-31, -9),
				 Score(-21,-12), Score(-13, -9), Score( -8, -1), Score( 6, -2),	Score( 6, -2),  Score( -8, -1),	Score(-13, -9), Score(-21,-12),
				 Score(-25,  6), Score(-11, -8), Score( -1, -2), Score( 3, -6),	Score( 3, -6),  Score( -1, -2),	Score(-11, -8), Score(-25,  6),
				 Score(-13, -6), Score( -5,  1), Score( -4, -9), Score(-6,  7),	Score(-6,  7),  Score( -4, -9),	Score( -5,  1), Score(-13, -6),
				 Score(-27, -5), Score(-15,  8), Score( -4,  7), Score( 3, -6),	Score( 3, -6),  Score( -4,  7),	Score(-15,  8), Score(-27, -5),
				 Score(-22,  6), Score( -2,  1), Score(  6, -7), Score(12, 10),	Score(12, 10),  Score(  6, -7),	Score( -2,  1), Score(-22,  6),
				 Score( -2,  4), Score( 12,  5), Score( 16, 20), Score(18, -5),	Score(18, -5),  Score( 16, 20),	Score( 12,  5), Score( -2,  4),
				 Score(-17, 18), Score(-19,  0), Score( -1, 19), Score( 9, 13),	Score( 9, 13),  Score( -1, 19),	Score(-19,  0), Score(-17, 18),
			};

			static Score queenPST[64] = {
				 Score( 3,-69), Score(-5,-57), Score(-5,-47), Score( 4,-26), Score( 4,-26), Score(-5,-47), Score(-5,-57), Score( 3,-69),
				 Score(-3,-54), Score( 5,-31), Score( 8,-22), Score(12, -4), Score(12, -4),	Score( 8,-22), Score( 5,-31), Score(-3,-54),
				 Score(-3,-39), Score( 6,-18), Score(13, -9), Score( 7,  3), Score( 7,  3),	Score(13, -9), Score( 6,-18), Score(-3,-39),
				 Score( 4,-23), Score( 5, -3), Score( 9, 13), Score( 8, 24), Score( 8, 24),	Score( 9, 13), Score( 5, -3), Score( 4,-23),
				 Score( 0,-29), Score(14, -6), Score(12,  9), Score( 5, 21), Score( 5, 21),	Score(12,  9), Score(14, -6), Score( 0,-29),
				 Score(-4,-38), Score(10,-18), Score( 6,-11), Score( 8,  1), Score( 8,  1),	Score( 6,-11), Score(10,-18), Score(-4,-38),
				 Score(-5,-50), Score( 6,-27), Score(10,-24), Score( 8, -8), Score( 8, -8),	Score(10,-24), Score( 6,-27), Score(-5,-50),
				 Score(-2,-74), Score(-2,-52), Score( 1,-43), Score(-2,-34), Score(-2,-34),	Score( 1,-43), Score(-2,-52), Score(-2,-74),
			};

			static Score kingPST[64] = {
				Score(271,  1), Score(327, 45), Score(271, 85), Score(198, 76),	Score(198, 76),	Score(271, 85),	Score(327, 45),	Score(271,  1),
			    Score(278, 53), Score(303,100), Score(234,133), Score(179,135),	Score(179,135),	Score(234,133),	Score(303,100),	Score(278, 53),
			    Score(195, 88), Score(258,130), Score(169,169), Score(120,175),	Score(120,175),	Score(169,169),	Score(258,130),	Score(195, 88),
			    Score(164,103), Score(190,156), Score(138,172), Score( 98,172),	Score( 98,172),	Score(138,172),	Score(190,156),	Score(164,103),
			    Score(154, 96), Score(179,166), Score(105,199), Score( 70,199),	Score( 70,199),	Score(105,199),	Score(179,166),	Score(154, 96),
			    Score(123, 92), Score(145,172), Score( 81,184), Score( 31,191),	Score( 31,191),	Score( 81,184),	Score(145,172),	Score(123, 92),
			    Score( 88, 47), Score(120,121), Score( 65,116), Score( 33,131),	Score( 33,131),	Score( 65,116),	Score(120,121),	Score( 88, 47),
			    Score( 59, 11), Score( 89, 59), Score( 45, 73), Score( -1, 78),	Score( -1, 78),	Score( 45, 73),	Score( 89, 59),	Score( 59, 11),
			};

			static Score MOBILITY_BONUS[6][32] = {
					{}, // PAWN
					{   // KNIGHT
						{-41, -65}, {-38, -120}, {-35, -47}, {-30, -23},
						{-14, -15}, { -9,   -1}, { -1,   9}, {  5,  13},
						{ 14,  -2}
					},
					{   // BISHOP
						{-40, -37}, {-40, -47}, {-10, -40}, {-6, -14},
						{  9,   5}, { 18,  19}, { 25,  28}, {26,  34},
						{ 27,  42}, { 28,  41}, { 24,  43}, {62,  17},
						{ 23,  31}, {221, -98}
					},
					{   // ROOK
						{-88, 19}, {-14, -22}, {  1,  1}, { 0, 22},
						{  4, 35}, {  1,  55}, {  4, 56}, {10, 63},
						{ 17, 74}, { 23,  78}, { 19, 87}, {15, 96},
						{ 31, 90}, { 49,  82}, {133, 53}
					},
					{   // QUEEN
						{-662, -587}, {-18, 125}, {-14, 38}, { -9, -20},
						{   6,  -29}, {  5,   4}, { 11, 20}, { 18,  29},
						{  24,   53}, { 32,  52}, { 41, 53}, { 46,  66},
						{  51,   73}, { 49,  75}, { 47, 83}, { 46,  89},
						{  46,   86}, { 36,  92}, { 28, 88}, { 50,  79},
						{  55,   69}, { 67,  53}, { 78, 42}, {126,   6},
						{ 127,   -2}, {115,   6}, {166, 36}, { 84, -12},
						{ 189,  298}
					}
			};

			static basic_score SAFE_CHECK_SCORE[][2] = {
				{}, {805, 1292}, {650, 984}, {1071, 1886}, {730, 1128}
		    };

			static Score BISHOP_PAWN_PENALTY = { 6, 7 };
			static Score ROOK_SEMI_OPEN_FILE_BONUS = { 10, -7 };
			static Score ROOK_OPEN_FILE_BONUS = { 24, 13 };


			static Score FLANK_ATTACKS = Score(8, 0);
			static Score PAWNLESS_FLANK = Score(19, 97);

			static Score THREAT_PAWN_PUSH_VALUE = { 15, 13 };
			static Score THREAT_WEAK_PAWNS_VALUE = { 10, 50 };
			static Score THREAT_PAWNSxMINORS_VALUE = { 70, 30 };
			static Score THREAT_MINORSxMINORS_VALUE = { 30, 40 };
			static Score THREAT_MAJORSxWEAK_MINORS_VALUE = { 30, 80 };
			static Score THREAT_PAWN_MINORSxMAJORS_VALUE = { 40, 30 };
			static Score THREAT_ALLxQUEENS_VALUE = { 30, 20 };
			static Score THREAT_KINGxMINORS_VALUE = { 20, 65 };
			static Score THREAT_KINGxROOKS_VALUE = { -30, 55 };

			static Score PIECE_SPACE = { 3, 3 };
			static Score EMPTY_SPACE = { 5, 0 };

			static basic_score PAWN_PHASE = 0;
			static basic_score KNIGHT_PHASE = 1;
			static basic_score BISHOP_PHASE = 1;
			static basic_score ROOK_PHASE = 2;
			static basic_score QUEEN_PHASE = 4;
			static basic_score TEMPO = 0;
		}

	}

}