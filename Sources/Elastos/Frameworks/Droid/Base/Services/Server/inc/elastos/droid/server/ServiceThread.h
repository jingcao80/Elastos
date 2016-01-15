#ifndef __ELASTOS_DROID_SERVER_SERVICE_THREAD_H__
#define __ELASTOS_DROID_SERVER_SERVICE_THREAD_H__

#include <elastos/droid/os/HandlerThread.h>

using Elastos::Droid::Os::HandlerThread;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Special handler thread that we create for system services that require their own loopers.
 */
class ServiceThread
    : public HandlerThread
{
public:
    ServiceThread(
        /* [in] */ const String& name,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean allowIo);

    // @Override
    CARAPI Run();

private:
    static const String TAG;

    Boolean mAllowIo;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_SERVICE_THREAD_H__