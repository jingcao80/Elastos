
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CTELECOMAPP_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CTELECOMAPP_H__

#include "_Elastos_Droid_Server_Telecom_CTelecomApp.h"
#include "elastos/droid/server/telecom/TelecomApp.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CarClass(CTelecomApp)
    , public TelecomApp
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_TELECOM_CTELECOMAPP_H__
