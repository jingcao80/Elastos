
#ifndef __ELASTOS_DROID_NET_RSSICURVE_H__
#define __ELASTOS_DROID_NET_RSSICURVE_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A curve defining the network score over a range of RSSI values.
 *
 * <p>For each RSSI bucket, the score may be any byte. Scores have no absolute meaning and are only
 * considered relative to other scores assigned by the same scorer. Networks with no score are all
 * considered equivalent and ranked below any network with a score.
 *
 * <p>For example, consider a curve starting at -110 dBm with a bucket width of 10 and the
 * following buckets: {@code [-20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120]}.
 * This represents a linear curve between -110 dBm and 30 dBm. It scores progressively higher at
 * stronger signal strengths.
 *
 * <p>A network can be assigned a fixed score independent of RSSI by setting
 * {@link #rssiBuckets} to a one-byte array whose element is the fixed score. {@link #start}
 * should be set to the lowest RSSI value at which this fixed score should apply, and
 * {@link #bucketWidth} should be set such that {@code start + bucketWidth} is equal to the
 * highest RSSI value at which this fixed score should apply.
 *
 * <p>Note that RSSI values below -110 dBm or above 30 dBm are unlikely to cause any difference
 * in connectivity behavior from those endpoints. That is, the connectivity framework will treat
 * a network with a -120 dBm signal exactly as it would treat one with a -110 dBm signal.
 * Therefore, graphs which specify scores outside this range may be truncated to this range by
 * the system.
 *
 * @see ScoredNetwork
 * @hide
 */
// @SystemApi
class RssiCurve
    : public Object
    , public IParcelable
    , public IRssiCurve
{
public:
    CAR_INTERFACE_DECL()

    RssiCurve();

    CARAPI constructor();
    /**
     * Construct a new {@link RssiCurve}.
     *
     * @param start the starting dBm of the curve.
     * @param bucketWidth the width of each RSSI bucket, in dBm.
     * @param rssiBuckets the score for each RSSI bucket.
     */
    CARAPI constructor(
        /* [in] */ Int32 start,
        /* [in] */ Int32 bucketWidth,
        /* [in] */ ArrayOf<Byte>* rssiBuckets);

    /**
     * Lookup the score for a given RSSI value.
     *
     * @param rssi The RSSI to lookup. If the RSSI falls below the start of the curve, the score at
     *         the start of the curve will be returned. If it falls after the end of the curve, the
     *         score at the end of the curve will be returned.
     * @return the score for the given RSSI.
     */
    CARAPI LookupScore(
        /* [in] */ Int32 rssi,
        /* [out] */ Byte* result);

    /**
     * Determine if two RSSI curves are defined in the same way.
     *
     * <p>Note that two curves can be equivalent but defined differently, e.g. if one bucket in one
     * curve is split into two buckets in another. For the purpose of this method, these curves are
     * not considered equal to each other.
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /** The starting dBm of the curve. */
    CARAPI GetStart(
        /* [out] */ Int32* result);

    /** The width of each RSSI bucket, in dBm. */
    CARAPI GetBucketWidth(
        /* [out] */ Int32* result);

    /** The score for each RSSI bucket. */
    CARAPI GetRssiBuckets(
        /* [out] */ ArrayOf<Byte>** result);

private:
    /** The starting dBm of the curve. */
    Int32 mStart;

    /** The width of each RSSI bucket, in dBm. */
    Int32 mBucketWidth;

    /** The score for each RSSI bucket. */
    AutoPtr<ArrayOf<Byte> > mRssiBuckets;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_RSSICURVE_H__
