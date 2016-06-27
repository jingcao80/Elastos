#ifndef __ELASTOS_DROID_SERVER_TELECOM_CTELECOMSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CTELECOMSERVICEIMPL_H__

#include "_Elastos_Droid_Server_Telecom_CTelecomServiceImpl.h"
#include "elastos/droid/server/telecom/TelecomServiceImpl.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CarClass(CTelecomServiceImpl)
    , public TelecomServiceImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CTELECOMSERVICEIMPL_H__
