
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CACCOUNTSELECTION_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CACCOUNTSELECTION_H__

#include "_Elastos_Droid_Server_Telecom_CAccountSelection.h"
#include "elastos/droid/server/telecom/AccountSelection.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CarClass(CAccountSelection)
    , public AccountSelection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CACCOUNTSELECTION_H__
