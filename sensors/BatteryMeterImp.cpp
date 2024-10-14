#include "BatteryMeterImp.hpp"

/* constructor */
BatteryMeterImp::BatteryMeterImp(size_t batLevels) : batteryLevel(batLevels) {}


size_t BatteryMeterImp::getBatteryState() const {
    return batteryLevel;
}

void BatteryMeterImp::updateBattery(size_t newLevel) {
    batteryLevel = newLevel;
}


BatteryMeterImp& BatteryMeterImp::operator=(const BatteryMeterImp& other) {
    if (this == &other)
        return *this;
    this->batteryLevel = other.batteryLevel;
    return *this;
}

