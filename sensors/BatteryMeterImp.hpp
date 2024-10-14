#pragma once

#include "BatteryMeter.hpp"


class BatteryMeterImp : public BatteryMeter {

    private:
        size_t batteryLevel;

    public:
        /* Constructors */
        BatteryMeterImp() {};
        BatteryMeterImp(size_t batLevels);

        size_t getBatteryState() const override;

        BatteryMeterImp& operator=(const BatteryMeterImp& other);

        void updateBattery(size_t newLevel);

};