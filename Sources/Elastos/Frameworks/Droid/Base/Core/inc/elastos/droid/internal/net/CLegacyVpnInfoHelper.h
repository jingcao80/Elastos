
#ifndef __ELASTOS_DROID_INTERNAL_NET_CLEGACYVPNINFOHELPER_H__
#define __ELASTOS_DROID_INTERNAL_NET_CLEGACYVPNINFOHELPER_H__

#include "_Elastos_Droid_Internal_Net_CLegacyVpnInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::INetworkInfo;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CLegacyVpnInfoHelper)
    , public Singleton
    , public ILegacyVpnInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI StateFromNetworkInfo(
        /* [in] */ INetworkInfo* info,
        /* [out] */ Int32* state);
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CLEGACYVPNINFOHELPER_H__
