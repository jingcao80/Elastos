
#ifndef __ELASTOS_DROID_LOCATION_GPSCLOCK_H__
#define __ELASTOS_DROID_LOCATION_GPSCLOCK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A class containing a GPS clock timestamp.
 * It represents a measurement of the GPS receiver's clock.
 *
 * @hide
 */
class GpsClock
    : public Object
    , public IGpsClock
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GpsClock();

    CARAPI constructor();

    /**
     * Sets all contents to the values stored in the provided object.
     */
    CARAPI Set(
        /* [in] */ IGpsClock* clock);

    /**
     * Resets all the contents to its original state.
     */
    CARAPI Reset();

    /**
     * Gets the type of time reported by {@link #getTimeInNs()}.
     */
    CARAPI GetType(
        /* [out] */ Byte* result);

    /**
     * Sets the type of time reported.
     */
    CARAPI SetType(
        /* [in] */ Byte value);

    /**
     * Returns true if {@link #getLeapSecond()} is available, false otherwise.
     */
    CARAPI HasLeapSecond(
        /* [out] */ Boolean* result);

    /**
     * Gets the leap second associated with the clock's time.
     * The sign of the value is defined by the following equation:
     *      utc_time_ns = time_ns + (full_bias_ns + bias_ns) - leap_second * 1,000,000,000
     *
     * The value is only available if {@link #hasLeapSecond()} is true.
     */
    CARAPI GetLeapSecond(
        /* [out] */ Int16* result);

    /**
     * Sets the leap second associated with the clock's time.
     */
    CARAPI SetLeapSecond(
        /* [in] */ Int16 leapSecond);

    /**
     * Resets the leap second associated with the clock's time.
     */
    CARAPI ResetLeapSecond();

    /**
     * Gets the GPS receiver internal clock value in nanoseconds.
     * This can be either the 'local hardware clock' value ({@link #TYPE_LOCAL_HW_TIME}), or the
     * current GPS time derived inside GPS receiver ({@link #TYPE_GPS_TIME}).
     * {@link #getType()} defines the time reported.
     *
     * For 'local hardware clock' this value is expected to be monotonically increasing during the
     * reporting session. The real GPS time can be derived by compensating
     * {@link #getFullBiasInNs()} (when it is available) from this value.
     *
     * For 'GPS time' this value is expected to be the best estimation of current GPS time that GPS
     * receiver can achieve. {@link #getTimeUncertaintyInNs()} should be available when GPS time is
     * specified.
     *
     * Sub-nanosecond accuracy can be provided by means of {@link #getBiasInNs()}.
     * The reported time includes {@link #getTimeUncertaintyInNs()}.
     */
    CARAPI GetTimeInNs(
        /* [out] */ Int64* result);

    /**
     * Sets the GPS receiver internal clock in nanoseconds.
     */
    CARAPI SetTimeInNs(
        /* [in] */ Int64 timeInNs);

    /**
     * Returns true if {@link #getTimeUncertaintyInNs()} is available, false otherwise.
     */
    CARAPI HasTimeUncertaintyInNs(
        /* [out] */ Boolean* result);

    /**
     * Gets the clock's time Uncertainty (1-Sigma) in nanoseconds.
     * The uncertainty is represented as an absolute (single sided) value.
     *
     * The value is only available if {@link #hasTimeUncertaintyInNs()} is true.
     */
    CARAPI GetTimeUncertaintyInNs(
        /* [out] */ Double* result);

    /**
     * Sets the clock's Time Uncertainty (1-Sigma) in nanoseconds.
     */
    CARAPI SetTimeUncertaintyInNs(
        /* [in] */ Double timeUncertaintyInNs);

    /**
     * Resets the clock's Time Uncertainty (1-Sigma) in nanoseconds.
     */
    CARAPI ResetTimeUncertaintyInNs();

    /**
     * Returns true if {@link @getFullBiasInNs()} is available, false otherwise.
     */
    CARAPI HasFullBiasInNs(
        /* [out] */ Boolean* result);

    /**
     * Gets the difference between hardware clock ({@link #getTimeInNs()}) inside GPS receiver and
     * the true GPS time since 0000Z, January 6, 1980, in nanoseconds.
     *
     * This value is available if {@link #TYPE_LOCAL_HW_TIME} is set, and GPS receiver has solved
     * the clock for GPS time.
     * {@link #getBiasUncertaintyInNs()} should be used for quality check.
     *
     * The sign of the value is defined by the following equation:
     *      true time (GPS time) = time_ns + (full_bias_ns + bias_ns)
     *
     * The reported full bias includes {@link #getBiasUncertaintyInNs()}.
     * The value is onl available if {@link #hasFullBiasInNs()} is true.
     */
    CARAPI GetFullBiasInNs(
        /* [out] */ Int64* result);

    /**
     * Sets the full bias in nanoseconds.
     */
    CARAPI SetFullBiasInNs(
        /* [in] */ Int64 value);

    /**
     * Resets the full bias in nanoseconds.
     */
    CARAPI ResetFullBiasInNs();

    /**
     * Returns true if {@link #getBiasInNs()} is available, false otherwise.
     */
    CARAPI HasBiasInNs(
        /* [out] */ Boolean* result);

    /**
     * Gets the clock's sub-nanosecond bias.
     * The reported bias includes {@link #getBiasUncertaintyInNs()}.
     *
     * The value is only available if {@link #hasBiasInNs()} is true.
     */
    CARAPI GetBiasInNs(
        /* [out] */ Double* result);

    /**
     * Sets the sub-nanosecond bias.
     */
    CARAPI SetBiasInNs(
        /* [in] */ Double biasInNs);

    /**
     * Resets the clock's Bias in nanoseconds.
     */
    CARAPI ResetBiasInNs();

    /**
     * Returns true if {@link #getBiasUncertaintyInNs()} is available, false otherwise.
     */
    CARAPI HasBiasUncertaintyInNs(
        /* [out] */ Boolean* result);

    /**
     * Gets the clock's Bias Uncertainty (1-Sigma) in nanoseconds.
     *
     * The value is only available if {@link #hasBiasUncertaintyInNs()} is true.
     */
    CARAPI GetBiasUncertaintyInNs(
        /* [out] */ Double* result);

    /**
     * Sets the clock's Bias Uncertainty (1-Sigma) in nanoseconds.
     */
    CARAPI SetBiasUncertaintyInNs(
        /* [in] */ Double biasUncertaintyInNs);

    /**
     * Resets the clock's Bias Uncertainty (1-Sigma) in nanoseconds.
     */
    CARAPI ResetBiasUncertaintyInNs();

    /**
     * Returns true if {@link #getDriftInNsPerSec()} is available, false otherwise.
     */
    CARAPI HasDriftInNsPerSec(
        /* [out] */ Boolean* result);

    /**
     * Gets the clock's Drift in nanoseconds per second.
     * A positive value indicates that the frequency is higher than the nominal frequency.
     * The reported drift includes {@link #getDriftUncertaintyInNsPerSec()}.
     *
     * The value is only available if {@link #hasDriftInNsPerSec()} is true.
     */
    CARAPI GetDriftInNsPerSec(
        /* [out] */ Double* result);

    /**
     * Sets the clock's Drift in nanoseconds per second.
     */
    CARAPI SetDriftInNsPerSec(
        /* [in] */ Double driftInNsPerSec);

    /**
     * Resets the clock's Drift in nanoseconds per second.
     */
    CARAPI ResetDriftInNsPerSec();

    /**
     * Returns true if {@link #getDriftUncertaintyInNsPerSec()} is available, false otherwise.
     */
    CARAPI HasDriftUncertaintyInNsPerSec(
        /* [out] */ Boolean* result);

    /**
     * Gets the clock's Drift Uncertainty (1-Sigma) in nanoseconds per second.
     *
     * The value is only available if {@link #hasDriftUncertaintyInNsPerSec()} is true.
     */
    CARAPI GetDriftUncertaintyInNsPerSec(
        /* [out] */ Double* result);

    /**
     * Sets the clock's Drift Uncertainty (1-Sigma) in nanoseconds per second.
     */
    CARAPI SetDriftUncertaintyInNsPerSec(
        /* [in] */ Double driftUncertaintyInNsPerSec);

    /**
     * Resets the clock's Drift Uncertainty (1-Sigma) in nanoseconds per second.
     */
    CARAPI ResetDriftUncertaintyInNsPerSec();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI GetFlag(
        /* [out] */ Int16* flag);

    //@Override
    CARAPI ToString(
        /* [out] */ String* info);
private:
    /**
     * Gets a string representation of the 'type'.
     * For internal and logging use only.
     */
    CARAPI_(String) GetTypeString();

    CARAPI_(void) Initialize();

    CARAPI_(void) SetFlag(
        /* [in] */ Int16 flag);

    CARAPI_(void) ResetFlag(
        /* [in] */ Int16 flag);

    CARAPI_(Boolean) IsFlagSet(
        /* [in] */ Int16 flag);

private:
    const static String TAG;

public:
    // The following enumerations must be in sync with the values declared in gps.h

    /**
     * The type of the time stored is not available or it is unknown.
     */
    const static Byte TYPE_UNKNOWN;

    /**
     * The source of the time value reported by this class is the 'Local Hardware Clock'.
     */
    const static Byte TYPE_LOCAL_HW_TIME;

    /**
     * The source of the time value reported by this class is the 'GPS time' derived from
     * satellites (epoch = Jan 6, 1980).
     */
    const static Byte TYPE_GPS_TIME;

private:
    const static Int16 HAS_NO_FLAGS;
    const static Int16 HAS_LEAP_SECOND;
    const static Int16 HAS_TIME_UNCERTAINTY;
    const static Int16 HAS_FULL_BIAS;
    const static Int16 HAS_BIAS;
    const static Int16 HAS_BIAS_UNCERTAINTY;
    const static Int16 HAS_DRIFT;
    const static Int16 HAS_DRIFT_UNCERTAINTY;

    // End enumerations in sync with gps.h

    Int16 mFlags;
    Int16 mLeapSecond;
    Byte mType;
    Int64 mTimeInNs;
    Double mTimeUncertaintyInNs;
    Int64 mFullBiasInNs;
    Double mBiasInNs;
    Double mBiasUncertaintyInNs;
    Double mDriftInNsPerSec;
    Double mDriftUncertaintyInNsPerSec;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_LOCATION_GPSCLOCK_H__
