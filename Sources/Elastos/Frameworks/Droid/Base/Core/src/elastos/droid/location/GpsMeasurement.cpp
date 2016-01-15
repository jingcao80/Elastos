
#include "elastos/droid/location/GpsMeasurement.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

const String GpsMeasurement::TAG("GpsMeasurement");
const Int32 GpsMeasurement::HAS_NO_FLAGS = 0;
const Int32 GpsMeasurement::HAS_SNR = 1 << 0;
const Int32 GpsMeasurement::HAS_ELEVATION = 1 << 1;
const Int32 GpsMeasurement::HAS_ELEVATION_UNCERTAINTY = 1 << 2;
const Int32 GpsMeasurement::HAS_AZIMUTH = 1 << 3;
const Int32 GpsMeasurement::HAS_AZIMUTH_UNCERTAINTY = 1 << 4;
const Int32 GpsMeasurement::HAS_PSEUDORANGE = 1 << 5;
const Int32 GpsMeasurement::HAS_PSEUDORANGE_UNCERTAINTY = 1 << 6;
const Int32 GpsMeasurement::HAS_CODE_PHASE = 1 << 7;
const Int32 GpsMeasurement::HAS_CODE_PHASE_UNCERTAINTY = 1 << 8;
const Int32 GpsMeasurement::HAS_CARRIER_FREQUENCY = 1 << 9;
const Int32 GpsMeasurement::HAS_CARRIER_CYCLES = 1 << 10;
const Int32 GpsMeasurement::HAS_CARRIER_PHASE = 1 << 11;
const Int32 GpsMeasurement::HAS_CARRIER_PHASE_UNCERTAINTY = 1 << 12;
const Int32 GpsMeasurement::HAS_BIT_NUMBER = 1 << 13;
const Int32 GpsMeasurement::HAS_TIME_FROM_LAST_BIT = 1 << 14;
const Int32 GpsMeasurement::HAS_DOPPLER_SHIFT = 1 << 15;
const Int32 GpsMeasurement::HAS_DOPPLER_SHIFT_UNCERTAINTY = 1 << 16;

CAR_INTERFACE_IMPL_2(GpsMeasurement, Object, IGpsMeasurement, IParcelable)

GpsMeasurement::GpsMeasurement()
{
    Initialize();
}

ECode GpsMeasurement::constructor()
{
    return NOERROR;
}

ECode GpsMeasurement::GetFlag(
    /* [out] */ Int32* flag)
{
    VALIDATE_NOT_NULL(flag);
    *flag = mFlags;
    return NOERROR;
}

ECode GpsMeasurement::Set(
    /* [in] */ IGpsMeasurement* measurement)
{
    measurement->GetFlag(&mFlags);
    measurement->GetPrn(&mPrn);
    measurement->GetTimeOffsetInNs(&mTimeOffsetInNs);
    measurement->GetState(&mState);
    measurement->GetReceivedGpsTowInNs(&mReceivedGpsTowInNs);
    measurement->GetReceivedGpsTowUncertaintyInNs(&mReceivedGpsTowUncertaintyInNs);
    measurement->GetCn0InDbHz(&mCn0InDbHz);
    measurement->GetPseudorangeRateInMetersPerSec(&mPseudorangeRateInMetersPerSec);
    measurement->GetPseudorangeRateUncertaintyInMetersPerSec(&mPseudorangeRateUncertaintyInMetersPerSec);
    measurement->GetAccumulatedDeltaRangeState(&mAccumulatedDeltaRangeState);
    measurement->GetAccumulatedDeltaRangeInMeters(&mAccumulatedDeltaRangeInMeters);
    measurement->GetAccumulatedDeltaRangeUncertaintyInMeters(&mAccumulatedDeltaRangeUncertaintyInMeters);
    measurement->GetPseudorangeInMeters(&mPseudorangeInMeters);
    measurement->GetPseudorangeUncertaintyInMeters(&mPseudorangeUncertaintyInMeters);
    measurement->GetCodePhaseInChips(&mCodePhaseInChips);
    measurement->GetCodePhaseUncertaintyInChips(&mCodePhaseUncertaintyInChips);
    measurement->GetCarrierFrequencyInHz(&mCarrierFrequencyInHz);
    measurement->GetCarrierCycles(&mCarrierCycles);
    measurement->GetCarrierPhase(&mCarrierPhase);
    measurement->GetCarrierPhaseUncertainty(&mCarrierPhaseUncertainty);
    measurement->GetLossOfLock(&mLossOfLock);
    measurement->GetBitNumber(&mBitNumber);
    measurement->GetTimeFromLastBitInMs(&mTimeFromLastBitInMs);
    measurement->GetDopplerShiftInHz(&mDopplerShiftInHz);
    measurement->GetDopplerShiftUncertaintyInHz(&mDopplerShiftUncertaintyInHz);
    measurement->GetMultipathIndicator(&mMultipathIndicator);
    measurement->GetSnrInDb(&mSnrInDb);
    measurement->GetElevationInDeg(&mElevationInDeg);
    measurement->GetElevationUncertaintyInDeg(&mElevationUncertaintyInDeg);
    measurement->GetAzimuthInDeg(&mAzimuthInDeg);
    measurement->GetAzimuthUncertaintyInDeg(&mAzimuthUncertaintyInDeg);
    measurement->IsUsedInFix(&mUsedInFix);
    return NOERROR;
}


ECode GpsMeasurement::Reset()
{
    Initialize();
    return NOERROR;
}

ECode GpsMeasurement::GetPrn(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPrn;
    return NOERROR;
}

ECode GpsMeasurement::SetPrn(
    /* [in] */ Byte value)
{
    mPrn = value;
    return NOERROR;
}

ECode GpsMeasurement::GetTimeOffsetInNs(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTimeOffsetInNs;
    return NOERROR;
}

ECode GpsMeasurement::SetTimeOffsetInNs(
    /* [in] */ Double value)
{
    mTimeOffsetInNs = value;
    return NOERROR;
}

ECode GpsMeasurement::GetState(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode GpsMeasurement::SetState(
    /* [in] */ Int16 value)
{
    switch (value) {
        case IGpsMeasurement::STATE_UNKNOWN:
        case IGpsMeasurement::STATE_BIT_SYNC:
        case IGpsMeasurement::STATE_CODE_LOCK:
        case IGpsMeasurement::STATE_SUBFRAME_SYNC:
        case IGpsMeasurement::STATE_TOW_DECODED:
            mState = value;
            break;
        default:
            StringBuilder sb("Sanitizing invalid 'sync state': ");
            sb += value;
            Logger::D(TAG, sb.ToString());
            mState = IGpsMeasurement::STATE_UNKNOWN;
            break;
    }
    return NOERROR;
}

ECode GpsMeasurement::GetReceivedGpsTowInNs(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReceivedGpsTowInNs;
    return NOERROR;
}

ECode GpsMeasurement::SetReceivedGpsTowInNs(
    /* [in] */ Int64 value)
{
    mReceivedGpsTowInNs = value;
    return NOERROR;
}

ECode GpsMeasurement::GetReceivedGpsTowUncertaintyInNs(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReceivedGpsTowUncertaintyInNs;
    return NOERROR;
}

ECode GpsMeasurement::SetReceivedGpsTowUncertaintyInNs(
    /* [in] */ Int64 value)
{
    mReceivedGpsTowUncertaintyInNs = value;
    return NOERROR;
}

ECode GpsMeasurement::GetCn0InDbHz(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCn0InDbHz;
    return NOERROR;
}

ECode GpsMeasurement::SetCn0InDbHz(
    /* [in] */ Double value)
{
    mCn0InDbHz = value;
    return NOERROR;
}

ECode GpsMeasurement::GetPseudorangeRateInMetersPerSec(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPseudorangeRateInMetersPerSec;
    return NOERROR;
}

ECode GpsMeasurement::SetPseudorangeRateInMetersPerSec(
    /* [in] */ Double value)
{
    mPseudorangeRateInMetersPerSec = value;
    return NOERROR;
}

ECode GpsMeasurement::GetPseudorangeRateUncertaintyInMetersPerSec(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPseudorangeRateUncertaintyInMetersPerSec;
    return NOERROR;
}

ECode GpsMeasurement::SetPseudorangeRateUncertaintyInMetersPerSec(
    /* [in] */ Double value)
{
    mPseudorangeRateUncertaintyInMetersPerSec = value;
    return NOERROR;
}

ECode GpsMeasurement::GetAccumulatedDeltaRangeState(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccumulatedDeltaRangeState;
    return NOERROR;
}

ECode GpsMeasurement::SetAccumulatedDeltaRangeState(
    /* [in] */ Int16 value)
{
    switch (value) {
        case IGpsMeasurement::ADR_STATE_UNKNOWN:
        case IGpsMeasurement::ADR_STATE_VALID:
        case IGpsMeasurement::ADR_STATE_RESET:
        case IGpsMeasurement::ADR_STATE_CYCLE_SLIP:
            mAccumulatedDeltaRangeState = value;
            break;
        default:
            StringBuilder sb("Sanitizing invalid 'Accumulated Delta Range state': ");
            sb += value;
            Logger::D(TAG, sb.ToString());
            mAccumulatedDeltaRangeState = IGpsMeasurement::ADR_STATE_UNKNOWN;
            break;
    }
    return NOERROR;
}

String GpsMeasurement::GetAccumulatedDeltaRangeStateString()
{
    switch (mAccumulatedDeltaRangeState) {
        case IGpsMeasurement::ADR_STATE_UNKNOWN:
            return String("Unknown");
        case IGpsMeasurement::ADR_STATE_VALID:
            return String("Valid");
        case IGpsMeasurement::ADR_STATE_RESET:
            return String("Reset");
        case IGpsMeasurement::ADR_STATE_CYCLE_SLIP:
            return String("CycleSlip");
        default:
            return String("<Invalid>");
    }
}

ECode GpsMeasurement::GetAccumulatedDeltaRangeInMeters(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccumulatedDeltaRangeInMeters;
    return NOERROR;
}

ECode GpsMeasurement::SetAccumulatedDeltaRangeInMeters(
    /* [in] */ Double value)
{
    mAccumulatedDeltaRangeInMeters = value;
    return NOERROR;
}

ECode GpsMeasurement::GetAccumulatedDeltaRangeUncertaintyInMeters(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccumulatedDeltaRangeUncertaintyInMeters;
    return NOERROR;
}

ECode GpsMeasurement::SetAccumulatedDeltaRangeUncertaintyInMeters(
    /* [in] */ Double value)
{
    mAccumulatedDeltaRangeUncertaintyInMeters = value;
    return NOERROR;
}

ECode GpsMeasurement::HasPseudorangeInMeters(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_PSEUDORANGE);
    return NOERROR;
}

ECode GpsMeasurement::GetPseudorangeInMeters(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPseudorangeInMeters;
    return NOERROR;
}

ECode GpsMeasurement::SetPseudorangeInMeters(
    /* [in] */ Double value)
{
    SetFlag(HAS_PSEUDORANGE);
    mPseudorangeInMeters = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetPseudorangeInMeters()
{
    ResetFlag(HAS_PSEUDORANGE);
    mPseudorangeInMeters = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasPseudorangeUncertaintyInMeters(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_PSEUDORANGE_UNCERTAINTY);
    return NOERROR;
}

ECode GpsMeasurement::GetPseudorangeUncertaintyInMeters(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPseudorangeUncertaintyInMeters;
    return NOERROR;
}

ECode GpsMeasurement::SetPseudorangeUncertaintyInMeters(
    /* [in] */ Double value)
{
    SetFlag(HAS_PSEUDORANGE_UNCERTAINTY);
    mPseudorangeUncertaintyInMeters = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetPseudorangeUncertaintyInMeters()
{
    ResetFlag(HAS_PSEUDORANGE_UNCERTAINTY);
    mPseudorangeUncertaintyInMeters = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasCodePhaseInChips(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_CODE_PHASE);
    return NOERROR;
}

ECode GpsMeasurement::GetCodePhaseInChips(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCodePhaseInChips;
    return NOERROR;
}

ECode GpsMeasurement::SetCodePhaseInChips(
    /* [in] */ Double value)
{
    SetFlag(HAS_CODE_PHASE);
    mCodePhaseInChips = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetCodePhaseInChips()
{
    ResetFlag(HAS_CODE_PHASE);
    mCodePhaseInChips = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasCodePhaseUncertaintyInChips(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_CODE_PHASE_UNCERTAINTY);
    return NOERROR;
}

ECode GpsMeasurement::GetCodePhaseUncertaintyInChips(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCodePhaseUncertaintyInChips;
    return NOERROR;
}

ECode GpsMeasurement::SetCodePhaseUncertaintyInChips(
    /* [in] */ Double value)
{
    SetFlag(HAS_CODE_PHASE_UNCERTAINTY);
    mCodePhaseUncertaintyInChips = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetCodePhaseUncertaintyInChips()
{
    ResetFlag(HAS_CODE_PHASE_UNCERTAINTY);
    mCodePhaseUncertaintyInChips = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasCarrierFrequencyInHz(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_CARRIER_FREQUENCY);
    return NOERROR;
}

ECode GpsMeasurement::GetCarrierFrequencyInHz(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCarrierFrequencyInHz;
    return NOERROR;
}

ECode GpsMeasurement::SetCarrierFrequencyInHz(
    /* [in] */ Float carrierFrequencyInHz)
{
    SetFlag(HAS_CARRIER_FREQUENCY);
    mCarrierFrequencyInHz = carrierFrequencyInHz;
    return NOERROR;
}


ECode GpsMeasurement::ResetCarrierFrequencyInHz()
{
    ResetFlag(HAS_CARRIER_FREQUENCY);
    mCarrierFrequencyInHz = Elastos::Core::Math::FLOAT_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasCarrierCycles(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_CARRIER_CYCLES);
    return NOERROR;
}

ECode GpsMeasurement::GetCarrierCycles(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCarrierCycles;
    return NOERROR;
}

ECode GpsMeasurement::SetCarrierCycles(
    /* [in] */ Int64 value)
{
    SetFlag(HAS_CARRIER_CYCLES);
    mCarrierCycles = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetCarrierCycles()
{
    ResetFlag(HAS_CARRIER_CYCLES);
    mCarrierCycles = Elastos::Core::Math::INT64_MIN_VALUE;
    return NOERROR;
}

ECode GpsMeasurement::HasCarrierPhase(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_CARRIER_PHASE);
    return NOERROR;
}

ECode GpsMeasurement::GetCarrierPhase(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCarrierPhase;
    return NOERROR;
}

ECode GpsMeasurement::SetCarrierPhase(
    /* [in] */ Double value)
{
    SetFlag(HAS_CARRIER_PHASE);
    mCarrierPhase = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetCarrierPhase()
{
    ResetFlag(HAS_CARRIER_PHASE);
    mCarrierPhase = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasCarrierPhaseUncertainty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_CARRIER_PHASE_UNCERTAINTY);
    return NOERROR;
}

ECode GpsMeasurement::GetCarrierPhaseUncertainty(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCarrierPhaseUncertainty;
    return NOERROR;
}

ECode GpsMeasurement::SetCarrierPhaseUncertainty(
    /* [in] */ Double value)
{
    SetFlag(HAS_CARRIER_PHASE_UNCERTAINTY);
    mCarrierPhaseUncertainty = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetCarrierPhaseUncertainty()
{
    ResetFlag(HAS_CARRIER_PHASE_UNCERTAINTY);
    mCarrierPhaseUncertainty = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::GetLossOfLock(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLossOfLock;
    return NOERROR;
}

ECode GpsMeasurement::SetLossOfLock(
    /* [in] */ Byte value)
{
    switch (value) {
        case IGpsMeasurement::LOSS_OF_LOCK_UNKNOWN:
        case IGpsMeasurement::LOSS_OF_LOCK_OK:
        case IGpsMeasurement::LOSS_OF_LOCK_CYCLE_SLIP:
            mLossOfLock = value;
            break;
        default:
            StringBuilder sb("Sanitizing invalid 'loss of lock': ");
            sb += value;
            Logger::D(TAG, sb.ToString());
            mLossOfLock = IGpsMeasurement::LOSS_OF_LOCK_UNKNOWN;
            break;
    }
    return NOERROR;
}

String GpsMeasurement::GetLossOfLockString()
{
    switch (mLossOfLock) {
        case IGpsMeasurement::LOSS_OF_LOCK_UNKNOWN:
            return String("Unknown");
        case IGpsMeasurement::LOSS_OF_LOCK_OK:
            return String("Ok");
        case IGpsMeasurement::LOSS_OF_LOCK_CYCLE_SLIP:
            return String("CycleSlip");
        default:
            return String("<Invalid>");
    }
}

ECode GpsMeasurement::HasBitNumber(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_BIT_NUMBER);
    return NOERROR;
}

ECode GpsMeasurement::GetBitNumber(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBitNumber;
    return NOERROR;
}

ECode GpsMeasurement::SetBitNumber(
    /* [in] */ Int32 bitNumber)
{
    SetFlag(HAS_BIT_NUMBER);
    mBitNumber = bitNumber;
    return NOERROR;
}

ECode GpsMeasurement::ResetBitNumber()
{
    ResetFlag(HAS_BIT_NUMBER);
    mBitNumber = Elastos::Core::Math::INT32_MIN_VALUE;
    return NOERROR;
}

ECode GpsMeasurement::HasTimeFromLastBitInMs(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_TIME_FROM_LAST_BIT);
    return NOERROR;
}

ECode GpsMeasurement::GetTimeFromLastBitInMs(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTimeFromLastBitInMs;
    return NOERROR;
}

ECode GpsMeasurement::SetTimeFromLastBitInMs(
    /* [in] */ Int16 value)
{
    SetFlag(HAS_TIME_FROM_LAST_BIT);
    mTimeFromLastBitInMs = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetTimeFromLastBitInMs()
{
    ResetFlag(HAS_TIME_FROM_LAST_BIT);
    mTimeFromLastBitInMs = Elastos::Core::Math::INT16_MIN_VALUE;
    return NOERROR;
}

ECode GpsMeasurement::HasDopplerShiftInHz(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_DOPPLER_SHIFT);
    return NOERROR;
}

ECode GpsMeasurement::GetDopplerShiftInHz(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDopplerShiftInHz;
    return NOERROR;
}

ECode GpsMeasurement::SetDopplerShiftInHz(
    /* [in] */ Double value)
{
    SetFlag(HAS_DOPPLER_SHIFT);
    mDopplerShiftInHz = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetDopplerShiftInHz()
{
    ResetFlag(HAS_DOPPLER_SHIFT);
    mDopplerShiftInHz = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasDopplerShiftUncertaintyInHz(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet((Int16)HAS_DOPPLER_SHIFT_UNCERTAINTY);
    return NOERROR;
}

ECode GpsMeasurement::GetDopplerShiftUncertaintyInHz(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDopplerShiftUncertaintyInHz;
    return NOERROR;
}

ECode GpsMeasurement::SetDopplerShiftUncertaintyInHz(
    /* [in] */ Double value)
{
    SetFlag((Int16)HAS_DOPPLER_SHIFT_UNCERTAINTY);
    mDopplerShiftUncertaintyInHz = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetDopplerShiftUncertaintyInHz()
{
    ResetFlag((Int16)HAS_DOPPLER_SHIFT_UNCERTAINTY);
    mDopplerShiftUncertaintyInHz = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::GetMultipathIndicator(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMultipathIndicator;
    return NOERROR;
}

ECode GpsMeasurement::SetMultipathIndicator(
    /* [in] */ Byte value)
{
    switch (value) {
        case IGpsMeasurement::MULTIPATH_INDICATOR_UNKNOWN:
        case IGpsMeasurement::MULTIPATH_INDICATOR_DETECTED:
        case IGpsMeasurement::MULTIPATH_INDICATOR_NOT_USED:
            mMultipathIndicator = value;
            break;
        default:
            StringBuilder sb("Sanitizing invalid 'muti-path indicator': ");
            sb += value;
            Logger::D(TAG, sb.ToString());
            mMultipathIndicator = IGpsMeasurement::MULTIPATH_INDICATOR_UNKNOWN;
            break;
    }
    return NOERROR;
}

String GpsMeasurement::GetMultipathIndicatorString() {
    switch(mMultipathIndicator) {
        case IGpsMeasurement::MULTIPATH_INDICATOR_UNKNOWN:
            return String("Unknown");
        case IGpsMeasurement::MULTIPATH_INDICATOR_DETECTED:
            return String("Detected");
        case IGpsMeasurement::MULTIPATH_INDICATOR_NOT_USED:
            return String("NotUsed");
        default:
            return String("<Invalid>");
    }
}

ECode GpsMeasurement::HasSnrInDb(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_SNR);
    return NOERROR;
}

ECode GpsMeasurement::GetSnrInDb(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSnrInDb;
    return NOERROR;
}

ECode GpsMeasurement::SetSnrInDb(
    /* [in] */ Double snrInDb)
{
    SetFlag(HAS_SNR);
    mSnrInDb = snrInDb;
    return NOERROR;
}

ECode GpsMeasurement::ResetSnrInDb()
{
    ResetFlag(HAS_SNR);
    mSnrInDb = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasElevationInDeg(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_ELEVATION);
    return NOERROR;
}

ECode GpsMeasurement::GetElevationInDeg(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mElevationInDeg;
    return NOERROR;
}

ECode GpsMeasurement::SetElevationInDeg(
    /* [in] */ Double elevationInDeg)
{
    SetFlag(HAS_ELEVATION);
    mElevationInDeg = elevationInDeg;
    return NOERROR;
}

ECode GpsMeasurement::ResetElevationInDeg()
{
    ResetFlag(HAS_ELEVATION);
    mElevationInDeg = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasElevationUncertaintyInDeg(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_ELEVATION_UNCERTAINTY);
    return NOERROR;
}

ECode GpsMeasurement::GetElevationUncertaintyInDeg(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mElevationUncertaintyInDeg;
    return NOERROR;
}

ECode GpsMeasurement::SetElevationUncertaintyInDeg(
    /* [in] */ Double value)
{
    SetFlag(HAS_ELEVATION_UNCERTAINTY);
    mElevationUncertaintyInDeg = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetElevationUncertaintyInDeg()
{
    ResetFlag(HAS_ELEVATION_UNCERTAINTY);
    mElevationUncertaintyInDeg = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasAzimuthInDeg(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_AZIMUTH);
    return NOERROR;
}

ECode GpsMeasurement::GetAzimuthInDeg(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAzimuthInDeg;
    return NOERROR;
}

ECode GpsMeasurement::SetAzimuthInDeg(
    /* [in] */ Double value)
{
    SetFlag(HAS_AZIMUTH);
    mAzimuthInDeg = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetAzimuthInDeg()
{
    ResetFlag(HAS_AZIMUTH);
    mAzimuthInDeg = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::HasAzimuthUncertaintyInDeg(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsFlagSet(HAS_AZIMUTH_UNCERTAINTY);
    return NOERROR;
}

ECode GpsMeasurement::GetAzimuthUncertaintyInDeg(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAzimuthUncertaintyInDeg;
    return NOERROR;
}

ECode GpsMeasurement::SetAzimuthUncertaintyInDeg(
    /* [in] */ Double value)
{
    SetFlag(HAS_AZIMUTH_UNCERTAINTY);
    mAzimuthUncertaintyInDeg = value;
    return NOERROR;
}

ECode GpsMeasurement::ResetAzimuthUncertaintyInDeg()
{
    ResetFlag(HAS_AZIMUTH_UNCERTAINTY);
    mAzimuthUncertaintyInDeg = Elastos::Core::Math::DOUBLE_NAN;
    return NOERROR;
}

ECode GpsMeasurement::IsUsedInFix(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUsedInFix;
    return NOERROR;
}

ECode GpsMeasurement::SetUsedInFix(
    /* [in] */ Boolean value)
{
    mUsedInFix = value;
    return NOERROR;
}

ECode GpsMeasurement::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadInt32(&mFlags);
    parcel->ReadByte(&mPrn);
    parcel->ReadDouble(&mTimeOffsetInNs);
    parcel->ReadInt16(&mState);
    parcel->ReadInt64(&mReceivedGpsTowInNs);
    parcel->ReadInt64(&mReceivedGpsTowUncertaintyInNs);
    parcel->ReadDouble(&mCn0InDbHz);
    parcel->ReadDouble(&mPseudorangeRateInMetersPerSec);
    parcel->ReadDouble(&mPseudorangeRateUncertaintyInMetersPerSec);
    parcel->ReadInt16(&mAccumulatedDeltaRangeState);
    parcel->ReadDouble(&mAccumulatedDeltaRangeInMeters);
    parcel->ReadDouble(&mAccumulatedDeltaRangeUncertaintyInMeters);
    parcel->ReadDouble(&mPseudorangeInMeters);
    parcel->ReadDouble(&mPseudorangeUncertaintyInMeters);
    parcel->ReadDouble(&mCodePhaseInChips);
    parcel->ReadDouble(&mCodePhaseUncertaintyInChips);
    parcel->ReadFloat(&mCarrierFrequencyInHz);
    parcel->ReadInt64(&mCarrierCycles);
    parcel->ReadDouble(&mCarrierPhase);
    parcel->ReadDouble(&mCarrierPhaseUncertainty);
    parcel->ReadByte(&mLossOfLock);
    parcel->ReadInt32(&mBitNumber);
    parcel->ReadInt16(&mTimeFromLastBitInMs);
    parcel->ReadDouble(&mDopplerShiftInHz);
    parcel->ReadDouble(&mDopplerShiftUncertaintyInHz);
    parcel->ReadByte(&mMultipathIndicator);
    parcel->ReadDouble(&mSnrInDb);
    parcel->ReadDouble(&mElevationInDeg);
    parcel->ReadDouble(&mElevationUncertaintyInDeg);
    parcel->ReadDouble(&mAzimuthInDeg);
    parcel->ReadDouble(&mAzimuthUncertaintyInDeg);
    Int32 value;
    parcel->ReadInt32(&value);
    mUsedInFix = value != 0;
    return NOERROR;
}

ECode GpsMeasurement::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteInt32(mFlags);
    parcel->WriteByte(mPrn);
    parcel->WriteDouble(mTimeOffsetInNs);
    parcel->WriteInt32(mState);
    parcel->WriteInt64(mReceivedGpsTowInNs);
    parcel->WriteInt64(mReceivedGpsTowUncertaintyInNs);
    parcel->WriteDouble(mCn0InDbHz);
    parcel->WriteDouble(mPseudorangeRateInMetersPerSec);
    parcel->WriteDouble(mPseudorangeRateUncertaintyInMetersPerSec);
    parcel->WriteInt32(mAccumulatedDeltaRangeState);
    parcel->WriteDouble(mAccumulatedDeltaRangeInMeters);
    parcel->WriteDouble(mAccumulatedDeltaRangeUncertaintyInMeters);
    parcel->WriteDouble(mPseudorangeInMeters);
    parcel->WriteDouble(mPseudorangeUncertaintyInMeters);
    parcel->WriteDouble(mCodePhaseInChips);
    parcel->WriteDouble(mCodePhaseUncertaintyInChips);
    parcel->WriteFloat(mCarrierFrequencyInHz);
    parcel->WriteInt64(mCarrierCycles);
    parcel->WriteDouble(mCarrierPhase);
    parcel->WriteDouble(mCarrierPhaseUncertainty);
    parcel->WriteByte(mLossOfLock);
    parcel->WriteInt32(mBitNumber);
    parcel->WriteInt32(mTimeFromLastBitInMs);
    parcel->WriteDouble(mDopplerShiftInHz);
    parcel->WriteDouble(mDopplerShiftUncertaintyInHz);
    parcel->WriteByte(mMultipathIndicator);
    parcel->WriteDouble(mSnrInDb);
    parcel->WriteDouble(mElevationInDeg);
    parcel->WriteDouble(mElevationUncertaintyInDeg);
    parcel->WriteDouble(mAzimuthInDeg);
    parcel->WriteDouble(mAzimuthUncertaintyInDeg);
    parcel->WriteInt32(mUsedInFix ? 1 : 0);
    return NOERROR;
}

ECode GpsMeasurement::ToString(
    /* [out] */ String* strOut)
{
    return E_NOT_IMPLEMENTED;

    #if 0 // TODO: Translate codes below
        final String format = "   %-29s = %s\n";
        final String formatWithUncertainty = "   %-29s = %-25s   %-40s = %s\n";
        StringBuilder builder = new StringBuilder("GpsMeasurement:\n");

        builder.append(String.format(format, "Prn", mPrn));

        builder.append(String.format(format, "TimeOffsetInNs", mTimeOffsetInNs));

        builder.append(String.format(format, "State", getStateString()));

        builder.append(String.format(
                formatWithUncertainty,
                "ReceivedGpsTowInNs",
                mReceivedGpsTowInNs,
                "ReceivedGpsTowUncertaintyInNs",
                mReceivedGpsTowUncertaintyInNs));

        builder.append(String.format(format, "Cn0InDbHz", mCn0InDbHz));

        builder.append(String.format(
                formatWithUncertainty,
                "PseudorangeRateInMetersPerSec",
                mPseudorangeRateInMetersPerSec,
                "PseudorangeRateUncertaintyInMetersPerSec",
                mPseudorangeRateUncertaintyInMetersPerSec));

        builder.append(String.format(
                format,
                "AccumulatedDeltaRangeState",
                getAccumulatedDeltaRangeStateString()));

        builder.append(String.format(
                formatWithUncertainty,
                "AccumulatedDeltaRangeInMeters",
                mAccumulatedDeltaRangeInMeters,
                "AccumulatedDeltaRangeUncertaintyInMeters",
                mAccumulatedDeltaRangeUncertaintyInMeters));

        builder.append(String.format(
                formatWithUncertainty,
                "PseudorangeInMeters",
                hasPseudorangeInMeters() ? mPseudorangeInMeters : null,
                "PseudorangeUncertaintyInMeters",
                hasPseudorangeUncertaintyInMeters() ? mPseudorangeUncertaintyInMeters : null));

        builder.append(String.format(
                formatWithUncertainty,
                "CodePhaseInChips",
                hasCodePhaseInChips() ? mCodePhaseInChips : null,
                "CodePhaseUncertaintyInChips",
                hasCodePhaseUncertaintyInChips() ? mCodePhaseUncertaintyInChips : null));

        builder.append(String.format(
                format,
                "CarrierFrequencyInHz",
                hasCarrierFrequencyInHz() ? mCarrierFrequencyInHz : null));

        builder.append(String.format(
                format,
                "CarrierCycles",
                hasCarrierCycles() ? mCarrierCycles : null));

        builder.append(String.format(
                formatWithUncertainty,
                "CarrierPhase",
                hasCarrierPhase() ? mCarrierPhase : null,
                "CarrierPhaseUncertainty",
                hasCarrierPhaseUncertainty() ? mCarrierPhaseUncertainty : null));

        builder.append(String.format(format, "LossOfLock", getLossOfLockString()));

        builder.append(String.format(
                format,
                "BitNumber",
                hasBitNumber() ? mBitNumber : null));

        builder.append(String.format(
                format,
                "TimeFromLastBitInMs",
                hasTimeFromLastBitInMs() ? mTimeFromLastBitInMs : null));

        builder.append(String.format(
                formatWithUncertainty,
                "DopplerShiftInHz",
                hasDopplerShiftInHz() ? mDopplerShiftInHz : null,
                "DopplerShiftUncertaintyInHz",
                hasDopplerShiftUncertaintyInHz() ? mDopplerShiftUncertaintyInHz : null));

        builder.append(String.format(format, "MultipathIndicator", getMultipathIndicatorString()));

        builder.append(String.format(
                format,
                "SnrInDb",
                hasSnrInDb() ? mSnrInDb : null));

        builder.append(String.format(
                formatWithUncertainty,
                "ElevationInDeg",
                hasElevationInDeg() ? mElevationInDeg : null,
                "ElevationUncertaintyInDeg",
                hasElevationUncertaintyInDeg() ? mElevationUncertaintyInDeg : null));

        builder.append(String.format(
                formatWithUncertainty,
                "AzimuthInDeg",
                hasAzimuthInDeg() ? mAzimuthInDeg : null,
                "AzimuthUncertaintyInDeg",
                hasAzimuthUncertaintyInDeg() ? mAzimuthUncertaintyInDeg : null));

        builder.append(String.format(format, "UsedInFix", mUsedInFix));

        return builder.toString();
    #endif
}

void GpsMeasurement::Initialize()
{
    mFlags = HAS_NO_FLAGS;
    SetPrn(Elastos::Core::Math::BYTE_MIN_VALUE);
    SetTimeOffsetInNs(Elastos::Core::Math::INT64_MIN_VALUE);
    SetState(IGpsMeasurement::STATE_UNKNOWN);
    SetReceivedGpsTowInNs(Elastos::Core::Math::INT64_MIN_VALUE);
    SetReceivedGpsTowUncertaintyInNs(Elastos::Core::Math::INT64_MAX_VALUE);
    SetCn0InDbHz(Elastos::Core::Math::DOUBLE_MIN_VALUE);
    SetPseudorangeRateInMetersPerSec(Elastos::Core::Math::DOUBLE_MIN_VALUE);
    SetPseudorangeRateUncertaintyInMetersPerSec(Elastos::Core::Math::DOUBLE_MIN_VALUE);
    SetAccumulatedDeltaRangeState(IGpsMeasurement::ADR_STATE_UNKNOWN);
    SetAccumulatedDeltaRangeInMeters(Elastos::Core::Math::DOUBLE_MIN_VALUE);
    SetAccumulatedDeltaRangeUncertaintyInMeters(Elastos::Core::Math::DOUBLE_MIN_VALUE);
    ResetPseudorangeInMeters();
    ResetPseudorangeUncertaintyInMeters();
    ResetCodePhaseInChips();
    ResetCodePhaseUncertaintyInChips();
    ResetCarrierFrequencyInHz();
    ResetCarrierCycles();
    ResetCarrierPhase();
    ResetCarrierPhaseUncertainty();
    SetLossOfLock(IGpsMeasurement::LOSS_OF_LOCK_UNKNOWN);
    ResetBitNumber();
    ResetTimeFromLastBitInMs();
    ResetDopplerShiftInHz();
    ResetDopplerShiftUncertaintyInHz();
    SetMultipathIndicator(IGpsMeasurement::MULTIPATH_INDICATOR_UNKNOWN);
    ResetSnrInDb();
    ResetElevationInDeg();
    ResetElevationUncertaintyInDeg();
    ResetAzimuthInDeg();
    ResetAzimuthUncertaintyInDeg();
    SetUsedInFix(FALSE);
}

void GpsMeasurement::SetFlag(
    /* [in] */ Int16 flag)
{
    mFlags |= flag;
}

void GpsMeasurement::ResetFlag(
    /* [in] */ Int16 flag)
{
    mFlags &= ~flag;
}

Boolean GpsMeasurement::IsFlagSet(
    /* [in] */ Int16 flag)
{
    return (mFlags & flag) == flag;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos
