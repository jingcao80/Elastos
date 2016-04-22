#ifndef __ELASTOS_DROID_SERVER_CMASTER_CLEAR_RECEIVER_H__
#define __ELASTOS_DROID_SERVER_CMASTER_CLEAR_RECEIVER_H__

#include "_Elastos_Droid_Server_CMasterClearReceiver.h"
#include "elastos/droid/server/MasterClearReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CMasterClearReceiver)
    , public MasterClearReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CMASTER_CLEAR_RECEIVER_H__
