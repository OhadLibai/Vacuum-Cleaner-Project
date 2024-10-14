#pragma once

#include "../common/Direction.hpp"

class WallsSensor {
public:
	virtual ~WallsSensor() {};
	virtual bool isWall(Direction d) const = 0;
};