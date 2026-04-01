#pragma once

#include <cstdint>

class FillLevelEstimator {
public:
    void startReturnMeasurement(uint32_t nowMs);
    void stopReturnMeasurement(uint32_t nowMs);

    void calibrateLow20(uint32_t durationMs);
    void calibrateFull100(uint32_t durationMs);

    uint8_t estimatePercent();
    bool isCalibrated() const;

    uint32_t getLastReturnDurationMs() const;
    uint8_t getLastEstimatedPercent() const;

private:
    uint32_t calibrationLow20Ms_{0};
    uint32_t calibrationFull100Ms_{0};
    uint32_t measurementStartMs_{0};
    bool measuring_{false};
    uint32_t lastReturnDurationMs_{0};
    uint8_t lastEstimatedPercent_{0};
};
