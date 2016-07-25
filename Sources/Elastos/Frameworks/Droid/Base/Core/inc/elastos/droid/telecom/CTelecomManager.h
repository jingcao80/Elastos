#ifndef __ELASTOS_DROID_TELECOM_CTELECOMMANAGER_H__
#define __ELASTOS_DROID_TELECOM_CTELECOMMANAGER_H__

#include "_Elastos_Droid_Telecom_CTelecomManager.h"
#include "elastos/droid/telecom/TelecomManager.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CTelecomManager)
    , public TelecomManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CTELECOMMANAGER_H__