#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CTELECOMMANAGER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CTELECOMMANAGER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CTelecomManager.h"
#include "elastos/droid/telecomm/telecom/TelecomManager.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CTelecomManager)
    , public TelecomManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CTELECOMMANAGER_H__