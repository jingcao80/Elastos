//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_LOCATION_GPSMEASUREMENT_H__
#define __ELASTOS_DROID_LOCATION_GPSMEASUREMENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A class representing a GPS satellite measurement, containing raw and computed information.
 *
 * @hide
 */
class GpsMeasurement
    : public Object
    , public IGpsMeasurement
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GpsMeasurement();

    CARAPI constructor();

    CARAPI GetFlag(
        /* [out] */ Int32* flag);

    /**
     * Sets all contents to the values stored in the provided object.
     */
    CARAPI Set(
        /* [in] */ IGpsMeasurement* measurement);

    /**
     * Resets all the contents to its original state.
     */
    CARAPI Reset();

    /**
     * Gets the Pseudo-random number (PRN).
     * Range: [1, 32]
     */
    CARAPI GetPrn(
        /* [out] */ Byte* result);

    /**
     * Sets the Pseud-random number (PRN).
     */
    CARAPI SetPrn(
        /* [in] */ Byte value);

    /**
     * Gets the time offset at which the measurement was taken in nanoseconds.
     * The reference receiver's time is specified by {@link GpsClock#getTimeInNs()} and should be
     * interpreted in the same way as indicated by {@link GpsClock#getType()}.
     *
     * The sign of this value is given by the following equation:
     *      measurement time = time_ns + time_offset_ns
     *
     * The value provides an individual time-stamp for the measurement, and allows sub-nanosecond
     * accuracy.
     */
    CARAPI GetTimeOffsetInNs(
        /* [out] */ Double* result);

    /**
     * Sets the time offset at which the measurement was taken in nanoseconds.
     */
    CARAPI SetTimeOffsetInNs(
        /* [in] */ Double value);

    /**
     * Gets per-satellite sync state.
     * It represents the current sync state for the associated satellite.
     *
     * This value helps interpret {@link #getReceivedGpsTowInNs()}.
     */
    CARAPI GetState(
        /* [out] */ Int16* result);

    /**
     * Sets the sync state.
     */
    CARAPI SetState(
        /* [in] */ Int16 value);

    /**
     * Gets the received GPS Time-of-Week at the measurement time, in nanoseconds.
     * The value is relative to the beginning of the current GPS week.
     *
     * Given {@link #getState()} of the GPS receiver, the range of this field can be:
     *      Searching           : [ 0           ]   : {@link #STATE_UNKNOWN} is set
     *      Ranging code lock   : [ 0    1 ms   ]   : {@link #STATE_CODE_LOCK} is set
     *      Bit sync            : [ 0   20 ms   ]   : {@link #STATE_BIT_SYNC} is set
     *      Subframe sync       : [ 0    6 ms   ]   : {@link #STATE_SUBFRAME_SYNC} is set
     *      TOW decoded         : [ 0    1 week ]   : {@link #STATE_TOW_DECODED} is set
     */
    CARAPI GetReceivedGpsTowInNs(
        /* [out] */ Int64* result);

    /**
     * Sets the received GPS time-of-week in nanoseconds.
     */
    CARAPI SetReceivedGpsTowInNs(
        /* [in] */ Int64 value);

    /**
     * Gets the received GPS time-of-week's uncertainty (1-Sigma) in nanoseconds.
     */
    CARAPI GetReceivedGpsTowUncertaintyInNs(
        /* [out] */ Int64* result);

    /**
     * Sets the received GPS time-of-week's uncertainty (1-Sigma) in nanoseconds.
     */
    CARAPI SetReceivedGpsTowUncertaintyInNs(
        /* [in] */ Int64 value);

    /**
     * Gets the Carrier-to-noise density in dB-Hz.
     * Range: [0, 63].
     *
     * The value contains the measured C/N0 for the signal at the antenna input.
     */
    CARAPI GetCn0InDbHz(
        /* [out] */ Double* result);

    /**
     * Sets the carrier-to-noise density in dB-Hz.
     */
    CARAPI SetCn0InDbHz(
        /* [in] */ Double value);

    /**
     * Gets the Pseudorange rate at the timestamp in m/s.
     * The reported value includes {@link #getPseudorangeRateUncertaintyInMetersPerSec()}.
     */
    CARAPI GetPseudorangeRateInMetersPerSec(
        /* [out] */ Double* result);

    /**
     * Sets the pseudorange rate at the timestamp in m/s.
     */
    CARAPI SetPseudorangeRateInMetersPerSec(
        /* [in] */ Double value);

    /**
     * Gets the pseudorange's rate uncertainty (1-Sigma) in m/s.
     * The uncertainty is represented as an absolute (single sided) value.
     */
    CARAPI GetPseudorangeRateUncertaintyInMetersPerSec(
        /* [out] */ Double* result);

    /**
     * Sets the pseudorange's rate uncertainty (1-Sigma) in m/s.
     */
    CARAPI SetPseudorangeRateUncertaintyInMetersPerSec(
        /* [in] */ Double value);

    /**
     * Gets 'Accumulated Delta Range' state.
     * It indicates whether {@link #getAccumulatedDeltaRangeInMeters()} is reset or there is a
     * cycle slip (indicating 'loss of lock').
     */
    CARAPI GetAccumulatedDeltaRangeState(
        /* [out] */ Int16* result);

    /**
     * Sets the 'Accumulated Delta Range' state.
     */
    CARAPI SetAccumulatedDeltaRangeState(
        /* [in] */ Int16 value);

    /**
     * Gets the accumulated delta range since the last channel reset, in meters.
     * The reported value includes {@link #getAccumulatedDeltaRangeUncertaintyInMeters()}.
     *
     * The availability of the value is represented by {@link #getAccumulatedDeltaRangeState()}.
     */
    CARAPI GetAccumulatedDeltaRangeInMeters(
        /* [out] */ Double* result);

    /**
     * Sets the accumulated delta range in meters.
     */
    CARAPI SetAccumulatedDeltaRangeInMeters(
        /* [in] */ Double value);

    /**
     * Gets the accumulated delta range's uncertainty (1-Sigma) in meters.
     * The uncertainty is represented as an absolute (single sided) value.
     */
    CARAPI GetAccumulatedDeltaRangeUncertaintyInMeters(
        /* [out] */ Double* result);

    /**
     * Sets the accumulated delta range's uncertainty (1-sigma) in meters.
     *
     * The availability of the value is represented by {@link #getAccumulatedDeltaRangeState()}.
     */
    CARAPI SetAccumulatedDeltaRangeUncertaintyInMeters(
        /* [in] */ Double value);

    /**
     * Returns true if {@link #getPseudorangeInMeters()} is available, false otherwise.
     */
    CARAPI HasPseudorangeInMeters(
        /* [out] */ Boolean* result);

    /**
     * Gets the best derived pseudorange by the chipset, in meters.
     * The reported pseudorange includes {@link #getPseudorangeUncertaintyInMeters()}.
     *
     * The value is only available if {@link #hasPseudorangeInMeters()} is true.
     */
    CARAPI GetPseudorangeInMeters(
        /* [out] */ Double* result);

    /**
     * Sets the Pseudo-range in meters.
     */
    CARAPI SetPseudorangeInMeters(
        /* [in] */ Double value);

    /**
     * Resets the Pseudo-range in meters.
     */
    CARAPI ResetPseudorangeInMeters();

    /**
     * Returns true if {@link #getPseudorangeUncertaintyInMeters()} is available, false otherwise.
     */
    CARAPI HasPseudorangeUncertaintyInMeters(
        /* [out] */ Boolean* result);

    /**
     * Gets the pseudorange's uncertainty (1-Sigma) in meters.
     * The value contains the 'pseudorange' and 'clock' uncertainty in it.
     * The uncertainty is represented as an absolute (single sided) value.
     *
     * The value is only available if {@link #hasPseudorangeUncertaintyInMeters()} is true.
     */
    CARAPI GetPseudorangeUncertaintyInMeters(
        /* [out] */ Double* result);

    /**
     * Sets the pseudo-range's uncertainty (1-Sigma) in meters.
     */
    CARAPI SetPseudorangeUncertaintyInMeters(
        /* [in] */ Double value);

    /**
     * Resets the pseudo-range's uncertainty (1-Sigma) in meters.
     */
    CARAPI ResetPseudorangeUncertaintyInMeters();

    /**
     * Returns true if {@link #getCodePhaseInChips()} is available, false otherwise.
     */
    CARAPI HasCodePhaseInChips(
        /* [out] */ Boolean* result);

    /**
     * Gets the fraction of the current C/A code cycle.
     * Range: [0, 1023]
     * The reference frequency is given by the value of {@link #getCarrierFrequencyInHz()}.
     * The reported code-phase includes {@link #getCodePhaseUncertaintyInChips()}.
     *
     * The value is only available if {@link #hasCodePhaseInChips()} is true.
     */
    CARAPI GetCodePhaseInChips(
        /* [out] */ Double* result);

    /**
     * Sets the Code-phase in chips.
     */
    CARAPI SetCodePhaseInChips(
        /* [in] */ Double value);

    /**
     * Resets the Code-phase in chips.
     */
    CARAPI ResetCodePhaseInChips();

    /**
     * Returns true if {@link #getCodePhaseUncertaintyInChips()} is available, false otherwise.
     */
    CARAPI HasCodePhaseUncertaintyInChips(
        /* [out] */ Boolean* result);

    /**
     * Gets the code-phase's uncertainty (1-Sigma) as a fraction of chips.
     * The uncertainty is represented as an absolute (single sided) value.
     *
     * The value is only available if {@link #hasCodePhaseUncertaintyInChips()} is true.
     */
    CARAPI GetCodePhaseUncertaintyInChips(
        /* [out] */ Double* result);

    /**
     * Sets the Code-phase's uncertainty (1-Sigma) in fractions of chips.
     */
    CARAPI SetCodePhaseUncertaintyInChips(
        /* [in] */ Double value);

    /**
     * Resets the Code-phase's uncertainty (1-Sigma) in fractions of chips.
     */
    CARAPI ResetCodePhaseUncertaintyInChips();

    /**
     * Returns true if {@link #getCarrierFrequencyInHz()} is available, false otherwise.
     */
    CARAPI HasCarrierFrequencyInHz(
        /* [out] */ Boolean* result);

    /**
     * Gets the carrier frequency at which codes and messages are modulated, it can be L1 or L2.
     * If the field is not set, the carrier frequency corresponds to L1.
     *
     * The value is only available if {@link #hasCarrierFrequencyInHz()} is true.
     */
    CARAPI GetCarrierFrequencyInHz(
        /* [out] */ Float* result);

    /**
     * Sets the Carrier frequency (L1 or L2) in Hz.
     */
    CARAPI SetCarrierFrequencyInHz(
        /* [in] */ Float carrierFrequencyInHz);

    /**
     * Resets the Carrier frequency (L1 or L2) in Hz.
     */
    CARAPI ResetCarrierFrequencyInHz();

    /**
     * Returns true if {@link #getCarrierCycles()} is available, false otherwise.
     */
    CARAPI HasCarrierCycles(
        /* [out] */ Boolean* result);

    /**
     * The number of full carrier cycles between the satellite and the receiver.
     * The reference frequency is given by the value of {@link #getCarrierFrequencyInHz()}.
     *
     * The value is only available if {@link #hasCarrierCycles()} is true.
     */
    CARAPI GetCarrierCycles(
        /* [out] */ Int64* result);

    /**
     * Sets the number of full carrier cycles between the satellite and the receiver.
     */
    CARAPI SetCarrierCycles(
        /* [in] */ Int64 value);

    /**
     * Resets the number of full carrier cycles between the satellite and the receiver.
     */
    CARAPI ResetCarrierCycles();

    /**
     * Returns true if {@link #getCarrierPhase()} is available, false otherwise.
     */
    CARAPI HasCarrierPhase(
        /* [out] */ Boolean* result);

    /**
     * Gets the RF phase detected by the receiver.
     * Range: [0.0, 1.0].
     * This is usually the fractional part of the complete carrier phase measurement.
     *
     * The reference frequency is given by the value of {@link #getCarrierFrequencyInHz()}.
     * The reported carrier-phase includes {@link #getCarrierPhaseUncertainty()}.
     *
     * The value is only available if {@link #hasCarrierPhase()} is true.
     */
    CARAPI GetCarrierPhase(
        /* [out] */ Double* result);

    /**
     * Sets the RF phase detected by the receiver.
     */
    CARAPI SetCarrierPhase(
        /* [in] */ Double value);

    /**
     * Resets the RF phase detected by the receiver.
     */
    CARAPI ResetCarrierPhase();

    /**
     * Returns true if {@link #getCarrierPhaseUncertainty()} is available, false otherwise.
     */
    CARAPI HasCarrierPhaseUncertainty(
        /* [out] */ Boolean* result);

    /**
     * Gets the carrier-phase's uncertainty (1-Sigma).
     * The uncertainty is represented as an absolute (single sided) value.
     *
     * The value is only available if {@link #hasCarrierPhaseUncertainty()} is true.
     */
    CARAPI GetCarrierPhaseUncertainty(
        /* [out] */ Double* result);

    /**
     * Sets the Carrier-phase's uncertainty (1-Sigma) in cycles.
     */
    CARAPI SetCarrierPhaseUncertainty(
        /* [in] */ Double value);

    /**
     * Resets the Carrier-phase's uncertainty (1-Sigma) in cycles.
     */
    CARAPI ResetCarrierPhaseUncertainty();

    /**
     * Gets a value indicating the 'loss of lock' state of the event.
     */
    CARAPI GetLossOfLock(
        /* [out] */ Byte* result);

    /**
     * Sets the 'loss of lock' status.
     */
    CARAPI SetLossOfLock(
        /* [in] */ Byte value);

    /**
     * Returns true if {@link #getBitNumber()} is available, false otherwise.
     */
    CARAPI HasBitNumber(
        /* [out] */ Boolean* result);

    /**
     * Gets the number of GPS bits transmitted since Sat-Sun midnight (GPS week).
     *
     * The value is only available if {@link #hasBitNumber()} is true.
     */
    CARAPI GetBitNumber(
        /* [out] */ Int32* result);

    /**
     * Sets the bit number within the broadcast frame.
     */
    CARAPI SetBitNumber(
        /* [in] */ Int32 bitNumber);

    /**
     * Resets the bit number within the broadcast frame.
     */
    CARAPI ResetBitNumber();

    /**
     * Returns true if {@link #getTimeFromLastBitInMs()} is available, false otherwise.
     */
    CARAPI HasTimeFromLastBitInMs(
        /* [out] */ Boolean* result);

    /**
     * Gets the elapsed time since the last received bit in milliseconds.
     * Range: [0, 20].
     *
     * The value is only available if {@link #hasTimeFromLastBitInMs()} is true.
     */
    CARAPI GetTimeFromLastBitInMs(
        /* [out] */ Int16* result);

    /**
     * Sets the elapsed time since the last received bit in milliseconds.
     */
    CARAPI SetTimeFromLastBitInMs(
        /* [in] */ Int16 value);

    /**
     * Resets the elapsed time since the last received bit in milliseconds.
     */
    CARAPI ResetTimeFromLastBitInMs();

    /**
     * Returns true if {@link #getDopplerShiftInHz()} is available, false otherwise.
     */
    CARAPI HasDopplerShiftInHz(
        /* [out] */ Boolean* result);

    /**
     * Gets the Doppler Shift in Hz.
     * A positive value indicates that the SV is moving toward the receiver.
     *
     * The reference frequency is given by the value of {@link #getCarrierFrequencyInHz()}.
     * The reported doppler shift includes {@link #getDopplerShiftUncertaintyInHz()}.
     *
     * The value is only available if {@link #hasDopplerShiftInHz()} is true.
     */
    CARAPI GetDopplerShiftInHz(
        /* [out] */ Double* result);

    /**
     * Sets the Doppler shift in Hz.
     */
    CARAPI SetDopplerShiftInHz(
        /* [in] */ Double value);

    /**
     * Resets the Doppler shift in Hz.
     */
    CARAPI ResetDopplerShiftInHz();

    /**
     * Returns true if {@link #getDopplerShiftUncertaintyInHz()} is available, false otherwise.
     */
    CARAPI HasDopplerShiftUncertaintyInHz(
        /* [out] */ Boolean* result);

    /**
     * Gets the Doppler's Shift uncertainty (1-Sigma) in Hz.
     * The uncertainty is represented as an absolute (single sided) value.
     *
     * The value is only available if {@link #hasDopplerShiftUncertaintyInHz()} is true.
     */
    CARAPI GetDopplerShiftUncertaintyInHz(
        /* [out] */ Double* result);

    /**
     * Sets the Doppler's shift uncertainty (1-Sigma) in Hz.
     */
    CARAPI SetDopplerShiftUncertaintyInHz(
        /* [in] */ Double value);

    /**
     * Resets the Doppler's shift uncertainty (1-Sigma) in Hz.
     */
    CARAPI ResetDopplerShiftUncertaintyInHz();

    /**
     * Gets a value indicating the 'multipath' state of the event.
     */
    CARAPI GetMultipathIndicator(
        /* [out] */ Byte* result);

    /**
     * Sets the 'multi-path' indicator.
     */
    CARAPI SetMultipathIndicator(
        /* [in] */ Byte value);

    /**
     * Returns true if {@link #getSnrInDb()} is available, false otherwise.
     */
    CARAPI HasSnrInDb(
        /* [out] */ Boolean* result);

    /**
         * Gets the Signal-to-Noise ratio (SNR) in dB.
         *
         * The value is only available if {@link #hasSnrInDb()} is true.
         */
    CARAPI GetSnrInDb(
        /* [out] */ Double* result);

    /**
         * Sets the Signal-to-noise ratio (SNR) in dB.
         */
    CARAPI SetSnrInDb(
        /* [in] */ Double snrInDb);

    /**
         * Resets the Signal-to-noise ratio (SNR) in dB.
         */
    CARAPI ResetSnrInDb();

    /**
         * Returns true if {@link #getElevationInDeg()} is available, false otherwise.
         */
    CARAPI HasElevationInDeg(
        /* [out] */ Boolean* result);

    /**
     * Gets the Elevation in degrees.
     * Range: [-90, 90]
     * The reported elevation includes {@link #getElevationUncertaintyInDeg()}.
     *
     * The value is only available if {@link #hasElevationInDeg()} is true.
     */
    CARAPI GetElevationInDeg(
        /* [out] */ Double* result);

    /**
     * Sets the Elevation in degrees.
     */
    CARAPI SetElevationInDeg(
        /* [in] */ Double elevationInDeg);

    /**
         * Resets the Elevation in degrees.
         */
    CARAPI ResetElevationInDeg();

    /**
         * Returns true if {@link #getElevationUncertaintyInDeg()} is available, false otherwise.
         */
    CARAPI HasElevationUncertaintyInDeg(
        /* [out] */ Boolean* result);

    /**
         * Gets the elevation's uncertainty (1-Sigma) in degrees.
         * Range: [0, 90]
         *
         * The uncertainty is represented as an absolute (single sided) value.
         *
         * The value is only available if {@link #hasElevationUncertaintyInDeg()} is true.
         */
    CARAPI GetElevationUncertaintyInDeg(
        /* [out] */ Double* result);

    /**
         * Sets the elevation's uncertainty (1-Sigma) in degrees.
         */
    CARAPI SetElevationUncertaintyInDeg(
        /* [in] */ Double value);

    /**
         * Resets the elevation's uncertainty (1-Sigma) in degrees.
         */
    CARAPI ResetElevationUncertaintyInDeg();

    /**
         * Returns true if {@link #getAzimuthInDeg()} is available, false otherwise.
         */
    CARAPI HasAzimuthInDeg(
        /* [out] */ Boolean* result);

    /**
         * Gets the azimuth in degrees.
         * Range: [0, 360).
         *
         * The reported azimuth includes {@link #getAzimuthUncertaintyInDeg()}.
         *
         * The value is only available if {@link #hasAzimuthInDeg()} is true.
         */
    CARAPI GetAzimuthInDeg(
        /* [out] */ Double* result);

    /**
         * Sets the Azimuth in degrees.
         */
    CARAPI SetAzimuthInDeg(
        /* [in] */ Double value);

    /**
         * Resets the Azimuth in degrees.
         */
    CARAPI ResetAzimuthInDeg();

    /**
         * Returns true if {@link #getAzimuthUncertaintyInDeg()} is available, false otherwise.
         */
    CARAPI HasAzimuthUncertaintyInDeg(
        /* [out] */ Boolean* result);

    /**
         * Gets the azimuth's uncertainty (1-Sigma) in degrees.
         * Range: [0, 180].
         *
         * The uncertainty is represented as an absolute (single sided) value.
         *
         * The value is only available if {@link #hasAzimuthUncertaintyInDeg()} is true.
         */
    CARAPI GetAzimuthUncertaintyInDeg(
        /* [out] */ Double* result);

    /**
         * Sets the Azimuth's uncertainty (1-Sigma) in degrees.
         */
    CARAPI SetAzimuthUncertaintyInDeg(
        /* [in] */ Double value);

    /**
         * Resets the Azimuth's uncertainty (1-Sigma) in degrees.
         */
    CARAPI ResetAzimuthUncertaintyInDeg();

    /**
         * Gets a flag indicating whether the GPS represented by the measurement was used for computing
         * the most recent fix.
         *
         * @return A non-null value if the data is available, null otherwise.
         */
    CARAPI IsUsedInFix(
        /* [out] */ Boolean* result);

    /**
         * Sets the Used-in-Fix flag.
         */
    CARAPI SetUsedInFix(
        /* [in] */ Boolean value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    //@Override
    CARAPI ToString(
        /* [out] */ String* strOut);

private:
    /**
     * Gets a string representation of the 'Accumulated Delta Range state'.
     * For internal and logging use only.
     */
    CARAPI_(String) GetAccumulatedDeltaRangeStateString();

    /**
     * Gets a string representation of the 'loss of lock'.
     * For internal and logging use only.
     */
    CARAPI_(String) GetLossOfLockString();

    /**
     * Gets a string representation of the 'multi-path indicator'.
     * For internal and logging use only.
     */
    CARAPI_(String) GetMultipathIndicatorString();

    CARAPI_(void) Initialize();

    CARAPI_(void) SetFlag(
        /* [in] */ Int32 flag);

    CARAPI_(void) ResetFlag(
        /* [in] */ Int32 flag);

    CARAPI_(Boolean) IsFlagSet(
        /* [in] */ Int32 flag);

private:
    const static String TAG;

    Int32 mFlags;
    Byte mPrn;
    Double mTimeOffsetInNs;
    Int16 mState;
    Int64 mReceivedGpsTowInNs;
    Int64 mReceivedGpsTowUncertaintyInNs;
    Double mCn0InDbHz;
    Double mPseudorangeRateInMetersPerSec;
    Double mPseudorangeRateUncertaintyInMetersPerSec;
    Int16 mAccumulatedDeltaRangeState;
    Double mAccumulatedDeltaRangeInMeters;
    Double mAccumulatedDeltaRangeUncertaintyInMeters;
    Double mPseudorangeInMeters;
    Double mPseudorangeUncertaintyInMeters;
    Double mCodePhaseInChips;
    Double mCodePhaseUncertaintyInChips;
    Float mCarrierFrequencyInHz;
    Int64 mCarrierCycles;
    Double mCarrierPhase;
    Double mCarrierPhaseUncertainty;
    Byte mLossOfLock;
    Int32 mBitNumber;
    Int16 mTimeFromLastBitInMs;
    Double mDopplerShiftInHz;
    Double mDopplerShiftUncertaintyInHz;
    Byte mMultipathIndicator;
    Double mSnrInDb;
    Double mElevationInDeg;
    Double mElevationUncertaintyInDeg;
    Double mAzimuthInDeg;
    Double mAzimuthUncertaintyInDeg;
    Boolean mUsedInFix;

    // The following enumerations must be in sync with the values declared in gps.h

    const static Int32 HAS_NO_FLAGS;
    const static Int32 HAS_SNR;
    const static Int32 HAS_ELEVATION;
    const static Int32 HAS_ELEVATION_UNCERTAINTY;
    const static Int32 HAS_AZIMUTH;
    const static Int32 HAS_AZIMUTH_UNCERTAINTY;
    const static Int32 HAS_PSEUDORANGE;
    const static Int32 HAS_PSEUDORANGE_UNCERTAINTY;
    const static Int32 HAS_CODE_PHASE;
    const static Int32 HAS_CODE_PHASE_UNCERTAINTY;
    const static Int32 HAS_CARRIER_FREQUENCY;
    const static Int32 HAS_CARRIER_CYCLES;
    const static Int32 HAS_CARRIER_PHASE;
    const static Int32 HAS_CARRIER_PHASE_UNCERTAINTY;
    const static Int32 HAS_BIT_NUMBER;
    const static Int32 HAS_TIME_FROM_LAST_BIT;
    const static Int32 HAS_DOPPLER_SHIFT;
    const static Int32 HAS_DOPPLER_SHIFT_UNCERTAINTY;

    // End enumerations in sync with gps.h

};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_GPSMEASUREMENT_H__
