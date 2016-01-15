#ifndef __ELASTOS_DROID_SERVER_IOTHREAD_H__
#define __ELASTOS_DROID_SERVER_IOTHREAD_H__

#include "elastos/droid/server/ServiceThread.h"
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Shared singleton I/O thread for the system.  This is a thread for non-background
 * service operations that can potential block briefly on network IO operations
 * (not waiting for data itself, but communicating with network daemons).
 */
class IoThread
    : public ServiceThread
{
public:
    static AutoPtr<IoThread> Get();

    static AutoPtr<IHandler> GetHandler();

private:
    static void EnsureThreadLocked();

    IoThread();

public:
    static Object sLock;

private:
    static AutoPtr<IoThread> sInstance;
    static AutoPtr<IHandler> sHandler;

};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_IOTHREAD_H__