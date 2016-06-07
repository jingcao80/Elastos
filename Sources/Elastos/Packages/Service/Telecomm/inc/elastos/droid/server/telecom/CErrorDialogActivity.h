
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CERRORDIALOGACTIVITY_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CERRORDIALOGACTIVITY_H__

#include "_Elastos_Droid_Server_Telecom_CErrorDialogActivity.h"
#include "elastos/droid/server/telecom/ErrorDialogActivity.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Used to display an error dialog from within the Telecom service when an outgoing call fails
 */
CarClass(CErrorDialogActivity)
    , public ErrorDialogActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CERRORDIALOGACTIVITY_H__
