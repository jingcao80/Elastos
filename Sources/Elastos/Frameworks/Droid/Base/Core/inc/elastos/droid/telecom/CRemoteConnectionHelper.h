#ifndef __ELASTOS_DROID_TELECOM_CREMOTECONNECTIONHELPER_H__
#define __ELASTOS_DROID_TELECOM_CREMOTECONNECTIONHELPER_H__

#include "_Elastos_Droid_Telecom_CRemoteConnectionHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CRemoteConnectionHelper)
    , public Singleton
    , public IRemoteConnectionHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Failure(
        /* [in] */ IDisconnectCause* disconnectCause,
        /* [out] */ IRemoteConnection** result);
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CREMOTECONNECTIONHELPER_H__