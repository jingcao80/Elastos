
#ifndef __ELASTOS_DROID_NET_CNETWORKIDENTITYHELPER_H__
#define __ELASTOS_DROID_NET_CNETWORKIDENTITYHELPER_H__

#include "_Elastos_Droid_Net_CNetworkIdentityHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CNetworkIdentityHelper)
    , public Singleton
    , public INetworkIdentityHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ScrubSubscriberId(
        /* [in] */ const String& subscriberId,
        /* [out] */ String* result);

    CARAPI BuildNetworkIdentity(
        /* [in] */ IContext* context,
        /* [in] */ INetworkState* state,
        /* [out] */ INetworkIdentity** result);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CLINKCAPABILITIESHELPER_H__
