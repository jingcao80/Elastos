
#ifndef __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYPARCELUTILS_H__
#define __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYPARCELUTILS_H__

#include "_Elastos_Droid_Net_CNetworkStatsHistoryParcelUtils.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Utility methods for interacting with {@link Parcel} structures, mostly
 * dealing with writing partial arrays.
 */
CarClass(CNetworkStatsHistoryParcelUtils)
    , public Singleton
    , public INetworkStatsHistoryParcelUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ReadLongArray(
        /* [in] */ IParcel* in,
        /* [out, callee] */ ArrayOf<Int64>** result);

    CARAPI WriteLongArray(
        /* [in] */ IParcel* out,
        /* [in] */ ArrayOf<Int64>* values,
        /* [in] */ Int32 size);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CNETWORKSTATSHISTORYPARCELUTILS_H__
