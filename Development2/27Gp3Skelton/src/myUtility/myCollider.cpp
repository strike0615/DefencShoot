#pragma once

#include "myCollider.h"
#include "../../../lib/gplib.h"

namespace myLib
{
	bool checkRoundToRound(roundCollider& col1, roundCollider& col2)
	{
		float distance = gplib::math::Distance2(col1.posX(), col1.posY(), col2.posX(), col2.posY());

		if (distance < (col1.radius() + col2.radius())) return true;
		else return false;
	}
}