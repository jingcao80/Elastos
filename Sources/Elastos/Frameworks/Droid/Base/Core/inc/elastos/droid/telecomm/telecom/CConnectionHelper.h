#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONHELPER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONHELPER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CConnectionHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CConnectionHelper)
    , public Singleton
    , public IConnectionHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI CreateFailedConnection(
        /* [in] */ IDisconnectCause* disconnectCause,
        /* [out] */ IConnection** result);

    CARAPI CreateCanceledConnection(
        /* [out] */ IConnection** result);
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CCONNECTIONHELPER_H__