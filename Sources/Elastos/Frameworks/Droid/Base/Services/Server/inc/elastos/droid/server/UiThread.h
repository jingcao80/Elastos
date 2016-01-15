#ifndef __ELASTOS_DROID_SERVER_UITHREAD_H__
#define __ELASTOS_DROID_SERVER_UITHREAD_H__

#include "elastos/droid/server/ServiceThread.h"
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Shared singleton thread for showing UI.  This is a foreground thread, and in
 * additional should not have operations that can take more than a few ms scheduled
 * on it to avoid UI jank.
 */
class UiThread
    : public ServiceThread
{
public:
    static AutoPtr<UiThread> Get();

    static AutoPtr<IHandler> GetHandler();

private:
    static void EnsureThreadLocked();

    UiThread();

public:
    static Object sLock;

private:
    static AutoPtr<UiThread> sInstance;
    static AutoPtr<IHandler> sHandler;

};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_UITHREAD_H__