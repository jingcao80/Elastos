
#ifndef __ELASTOS_DROID_OS_CHANDLERTHREAD_H__
#define __ELASTOS_DROID_OS_CHANDLERTHREAD_H__

#include "_Elastos_Droid_Os_CHandlerThread.h"
#include "elastos/droid/os/HandlerThread.h"

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Handy class for starting a new thread that has a looper. The looper can then be
 * used to create handler classes. Note that start() must still be called.
 */
CarClass(CHandlerThread)
    , public HandlerThread
{
public:
    CAR_OBJECT_DECL()

    CHandlerThread();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CHANDLERTHREAD_H__
