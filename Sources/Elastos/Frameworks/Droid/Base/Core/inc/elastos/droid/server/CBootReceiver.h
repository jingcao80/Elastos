#ifndef __ELASTOS_DROID_SERVER_CBOOTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_CBOOTRECEIVER_H__

#include "_Elastos_Droid_Server_CBootReceiver.h"
#include "elastos/droid/server/BootReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CBootReceiver)
    , public BootReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBOOTRECEIVER_H__
