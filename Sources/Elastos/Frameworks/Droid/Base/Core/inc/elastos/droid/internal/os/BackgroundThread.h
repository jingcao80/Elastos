
#ifndef __ELASTOS_DROID_INTERNAL_OS_BACKGROUNDTHREAD_H__
#define __ELASTOS_DROID_INTERNAL_OS_BACKGROUNDTHREAD_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/os/HandlerThread.h"

using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * Shared singleton background thread for each process.
 */
class BackgroundThread
    : public HandlerThread
    , public IBackgroundThread
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IBackgroundThread>) GetInstance();

    static CARAPI_(AutoPtr<IHandler>) GetHandler();

private:
    BackgroundThread();

    static CARAPI_(void) EnsureThreadLocked();

private:
    static AutoPtr<BackgroundThread> sInstance;
    static AutoPtr<IHandler> sHandler;
    static Object sLock;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_BACKGROUNDTHREAD_H__
