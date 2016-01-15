
#ifndef __ELASTOS_DROID_INTERNAL_NET_CVPNPROFILEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_NET_CVPNPROFILEHELPER_H__

#include "_Elastos_Droid_Internal_Net_CVpnProfileHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CVpnProfileHelper)
    , public Singleton
    , public IVpnProfileHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Decode(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ IVpnProfile** profile);
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CVPNPROFILEHELPER_H__
