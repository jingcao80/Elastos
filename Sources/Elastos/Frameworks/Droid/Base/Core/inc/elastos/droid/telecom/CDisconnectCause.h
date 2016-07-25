#ifndef __ELASTOS_DROID_TELECOM_CDISCONNECTCAUSE_H__
#define __ELASTOS_DROID_TELECOM_CDISCONNECTCAUSE_H__

#include "_Elastos_Droid_Telecom_CDisconnectCause.h"
#include "elastos/droid/telecom/DisconnectCause.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CDisconnectCause)
    , public DisconnectCause
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CDISCONNECTCAUSE_H__