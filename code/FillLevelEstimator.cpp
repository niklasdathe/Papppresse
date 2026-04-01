#include "FillLevelEstimator.h"

#include "AppConfig.h"

#include <algorithm>

void FillLevelEstimator::startReturnMeasurement(uint32_t nowMs)
{
    measurementStartMs_ = nowMs;
    measuring_ = true;
}

void FillLevelEstimator::stopReturnMeasurement(uint32_t nowMs)
{
    if (!measuring_) {
        return;
    }

    measuring_ = false;
    lastReturnDurationMs_ = nowMs - measurementStartMs_;
}

void FillLevelEstimator::calibrateLow20(uint32_t durationMs)
{
    calibrationLow20Ms_ = durationMs;
}

void FillLevelEstimator::calibrateFull100(uint32_t durationMs)
{
    calibrationFull100Ms_ = durationMs;
}

uint8_t FillLevelEstimator::estimatePercent()
{
    if (!isCalibrated()) {
        lastEstimatedPercent_ = static_cast<uint8_t>(AppConfig::FillLevel::kNotCalibratedPercent);
        return lastEstimatedPercent_;
    }

    if (lastReturnDurationMs_ <= calibrationLow20Ms_) {
        lastEstimatedPercent_ = static_cast<uint8_t>(AppConfig::FillLevel::kLowCalibrationPercent);
        return lastEstimatedPercent_;
    }

    if (lastReturnDurationMs_ >= calibrationFull100Ms_) {
        lastEstimatedPercent_ = static_cast<uint8_t>(AppConfig::FillLevel::kFullPercent);
        return lastEstimatedPercent_;
    }

    const uint32_t span = calibrationFull100Ms_ - calibrationLow20Ms_;
    const uint32_t relative = lastReturnDurationMs_ - calibrationLow20Ms_;
    const uint32_t percentRange = AppConfig::FillLevel::kFullPercent - AppConfig::FillLevel::kLowCalibrationPercent;
    const uint32_t scaled = AppConfig::FillLevel::kLowCalibrationPercent + ((relative * percentRange) / span);

    lastEstimatedPercent_ = static_cast<uint8_t>(std::min<uint32_t>(scaled, AppConfig::FillLevel::kFullPercent));
    return lastEstimatedPercent_;
}

bool FillLevelEstimator::isCalibrated() const
{
    return (calibrationLow20Ms_ > 0) && (calibrationFull100Ms_ > calibrationLow20Ms_);
}

uint32_t FillLevelEstimator::getLastReturnDurationMs() const
{
    return lastReturnDurationMs_;
}

uint8_t FillLevelEstimator::getLastEstimatedPercent() const
{
    return lastEstimatedPercent_;
}
