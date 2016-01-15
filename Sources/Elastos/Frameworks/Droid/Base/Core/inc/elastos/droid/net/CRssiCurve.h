
#ifndef __ELASTOS_DROID_NET_CRSSICURVE_H__
#define __ELASTOS_DROID_NET_CRSSICURVE_H__

#include "_Elastos_Droid_Net_CRssiCurve.h"
#include "elastos/droid/net/RssiCurve.h"

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
CarClass(CRssiCurve)
    , public RssiCurve
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CRSSICURVE_H__
