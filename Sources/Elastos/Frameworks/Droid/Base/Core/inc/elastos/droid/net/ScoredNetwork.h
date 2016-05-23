
#ifndef __ELASTOS_DROID_NET_SCOREDNETWORK_H__
#define __ELASTOS_DROID_NET_SCOREDNETWORK_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A network identifier along with a score for the quality of that network.
 *
 * @hide
 */
// @SystemApi
class ScoredNetwork
    : public Object
    , public IParcelable
    , public IScoredNetwork
{
public:
    CAR_INTERFACE_DECL()

    ScoredNetwork();

    CARAPI constructor();
    /**
     * Construct a new {@link ScoredNetwork}.
     *
     * @param networkKey the {@link NetworkKey} uniquely identifying this network.
     * @param rssiCurve the {@link RssiCurve} representing the scores for this network based on the
     *     RSSI. This field is optional, and may be skipped to represent a network which the scorer
     *     has opted not to score at this time. Passing a null value here is strongly preferred to
     *     not returning any {@link ScoredNetwork} for a given {@link NetworkKey} because it
     *     indicates to the system not to request scores for this network in the future, although
     *     the scorer may choose to issue an out-of-band update at any time.
     */
    CARAPI constructor(
        /* [in] */ INetworkKey* networkKey,
        /* [in] */ IRssiCurve* rssiCurve);

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

    /** A {@link NetworkKey} uniquely identifying this network. */
    CARAPI GetNetworkKey(
        /* [out] */ INetworkKey** result);

    /**
     * The {@link RssiCurve} representing the scores for this network based on the RSSI.
     *
     * <p>This field is optional and may be set to null to indicate that no score is available for
     * this network at this time. Such networks, along with networks for which the scorer has not
     * responded, are always prioritized below scored networks, regardless of the score.
     */
    CARAPI GetRssiCurve(
        /* [out] */ IRssiCurve** result);

private:
    /** A {@link NetworkKey} uniquely identifying this network. */
    AutoPtr<INetworkKey> mNetworkKey;

    /**
     * The {@link RssiCurve} representing the scores for this network based on the RSSI.
     *
     * <p>This field is optional and may be set to null to indicate that no score is available for
     * this network at this time. Such networks, along with networks for which the scorer has not
     * responded, are always prioritized below scored networks, regardless of the score.
     */
    AutoPtr<IRssiCurve> mRssiCurve;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_SCOREDNETWORK_H__
