#ifndef __ELASTOS_DROID_SERVER_FGTHREAD_H__
#define __ELASTOS_DROID_SERVER_FGTHREAD_H__

#include "elastos/droid/server/ServiceThread.h"
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Shared singleton foreground thread for the system.  This is a thread for regular
 * foreground service operations, which shouldn't be blocked by anything running in
 * the background.  In particular, the shared background thread could be doing
 * relatively long-running operations like saving state to disk (in addition to
 * simply being a background priority), which can cause operations scheduled on it
 * to be delayed for a user-noticeable amount of time.
 */
class FgThread
    : public ServiceThread
{
public:
    static AutoPtr<FgThread> Get();

    static AutoPtr<IHandler> GetHandler();

private:
    static void EnsureThreadLocked();

    FgThread();

private:
    static AutoPtr<FgThread> sInstance;
    static AutoPtr<IHandler> sHandler;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_FGTHREAD_H__