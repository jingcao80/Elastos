#ifndef __ELASTOS_DROID_SERVER_TELECOM_CINCALLADAPTER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CINCALLADAPTER_H__

#include "_Elastos_Droid_Server_Telecom_CInCallAdapter.h"
#include "elastos/droid/server/telecom/InCallAdapter.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CarClass(CInCallAdapter)
    , public InCallAdapter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CINCALLADAPTER_H__
