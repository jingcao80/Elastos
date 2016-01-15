#ifndef __ELASTOS_DROID_SERVER_DISPLAY_THREAD_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_THREAD_H__

#include "elastos/droid/server/ServiceThread.h"
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {


/**
 * Shared singleton foreground thread for the system.  This is a thread for
 * operations that affect what's on the display, which needs to have a minimum
 * of latency.  This thread should pretty much only be used by the WindowManager,
 * DisplayManager, and InputManager to perform quick operations in real time.
 */
class DisplayThread
    : public ServiceThread
{
public:
    static AutoPtr<DisplayThread> Get();

    static AutoPtr<IHandler> GetHandler();

private:
    static void EnsureThreadLocked();

    DisplayThread();

private:
    static AutoPtr<DisplayThread> sInstance;
    static AutoPtr<IHandler> sHandler;
    static Object sLock;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_THREAD_H__