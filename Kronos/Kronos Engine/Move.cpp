#include "Move.h"

#include "Move_Generation.h"

namespace KRONOS
{
	
	uint16_t Move::toIntMove() {
		// xxxx xxxx xxxx xxxx
		// xxxx xxxx xx11 1111 to
		// xxxx 1111 11xx xxxx from
		// 1111 xxxx xxxx xxxx promotion
		return (to) | (from << 6) | ((flag & PROMOTION) << 12);
	}

}