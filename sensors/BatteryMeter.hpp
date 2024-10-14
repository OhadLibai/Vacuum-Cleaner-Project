#pragma once

#include <cstddef>

class BatteryMeter {
public:
	virtual ~BatteryMeter() {}
	virtual size_t getBatteryState() const = 0;
};
