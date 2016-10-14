#ifndef __ELASTOS_DROID_OS_REMOTECALLBACKLOCALCALLBACK_H__
#define __ELASTOS_DROID_OS_REMOTECALLBACKLOCALCALLBACK_H__

#include "_Elastos_Droid_Os_CRemoteCallbackLocalCallback.h"
#include "elastos/droid/os/RemoteCallback.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CRemoteCallbackLocalCallback)
    , public RemoteCallback::LocalCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_REMOTECALLBACKLOCALCALLBACK_H__
