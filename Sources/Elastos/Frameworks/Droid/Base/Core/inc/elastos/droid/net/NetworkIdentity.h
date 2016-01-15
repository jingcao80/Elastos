
#ifndef __ELASTOS_DROID_NET_NETWORKIDENTITY_H__
#define __ELASTOS_DROID_NET_NETWORKIDENTITY_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Network definition that includes strong identity. Analogous to combining
 * {@link NetworkInfo} and an IMSI.
 *
 * @hide
 */
class NetworkIdentity
    : public Object
    , public INetworkIdentity
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 subType,
        /* [in] */ const String& subscriberId,
        /* [in] */ const String& networkId,
        /* [in] */ Boolean roaming);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI GetSubType(
        /* [out] */ Int32* result);

    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    CARAPI GetNetworkId(
        /* [out] */ String* result);

    CARAPI GetRoaming(
        /* [out] */ Boolean* result);

    /**
     * Scrub given IMSI on production builds.
     */
    static CARAPI ScrubSubscriberId(
        /* [in] */ const String& subscriberId,
        /* [out] */ String* result);

    /**
     * Build a {@link NetworkIdentity} from the given {@link NetworkState},
     * assuming that any mobile networks are using the current IMSI.
     */
    static CARAPI BuildNetworkIdentity(
        /* [in] */ IContext* context,
        /* [in] */ INetworkState* state,
        /* [out] */ INetworkIdentity** result);

private:
    Int32 mType;

    Int32 mSubType;

    String mSubscriberId;

    String mNetworkId;

    Boolean mRoaming;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKIDENTITY_H__
