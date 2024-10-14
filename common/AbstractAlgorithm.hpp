#pragma once

#include "../common/Direction.hpp"
#include "../common/Step.hpp"
#include "../sensors/WallsSensor.hpp"
#include "../sensors/DirtSensor.hpp"
#include "../sensors/BatteryMeter.hpp"

class AbstractAlgorithm {
public:
	virtual ~AbstractAlgorithm() {}
	virtual void setMaxSteps(size_t maxSteps) = 0;
	virtual void setWallsSensor(const WallsSensor&) = 0;
	virtual void setDirtSensor(const DirtSensor&) = 0;
	virtual void setBatteryMeter(const BatteryMeter&) = 0;
	
	virtual Step nextStep() = 0;
};