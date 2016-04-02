
#ifndef __ELASTOS_DROID_NET_CNETWORKINFOHELPER_H__
#define __ELASTOS_DROID_NET_CNETWORKINFOHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Net_CNetworkInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::NetworkInfoDetailedState;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CNetworkInfoHelper)
    , public Singleton
    , public INetworkInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NetworkInfoDetailedStateFromString(
        /* [in] */ const String& str,
        /* [out] */ NetworkInfoDetailedState* state);

    CARAPI NetworkInfoDetailedStateToString(
        /* [in] */ NetworkInfoDetailedState state,
        /* [out] */ String* str);

    CARAPI Index(
        /* [in] */ NetworkInfoDetailedState state,
        /* [out] */ Int32* index);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CNETWORKINFOHELPER_H__
