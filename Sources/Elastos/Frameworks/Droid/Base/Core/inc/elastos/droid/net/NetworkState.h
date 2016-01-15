
#ifndef __ELASTOS_DROID_NET_NETWORKSTATE_H__
#define __ELASTOS_DROID_NET_NETWORKSTATE_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Snapshot of network state.
 *
 * @hide
 */
class NetworkState
    : public Object
    , public IParcelable
    , public INetworkState
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ INetworkInfo* networkInfo,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ INetworkCapabilities* networkCapabilities);

    CARAPI constructor(
        /* [in] */ INetworkInfo* networkInfo,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ const String& subscriberId,
        /* [in] */ const String& networkId);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetNetworkInfo(
        /* [out] */ INetworkInfo** result);

    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** result);

    CARAPI GetNetworkCapabilities(
        /* [out] */ INetworkCapabilities** result);

    /** Currently only used by testing. */
    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    CARAPI GetNetworkId(
        /* [out] */ String* result);

private:
    AutoPtr<INetworkInfo> mNetworkInfo;

    AutoPtr<ILinkProperties> mLinkProperties;

    AutoPtr<INetworkCapabilities> mNetworkCapabilities;

    /** Currently only used by testing. */
    String mSubscriberId;

    String mNetworkId;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKSTATE_H__
