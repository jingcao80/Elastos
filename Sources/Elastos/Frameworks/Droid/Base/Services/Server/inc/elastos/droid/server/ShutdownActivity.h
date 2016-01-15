
#ifndef __ELASTOS_DROID_SERVER_SHUTDOWNACTIVITY_H__
#define __ELASTOS_DROID_SERVER_SHUTDOWNACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include <elastos/core/Thread.h>

using Elastos::Core::Thread;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace Server {

class ShutdownActivity
    : public Activity
{
private:
    class LocalThread
        : public ThreadBase
    {
    public:
        LocalThread(
            /* [in] */ ShutdownActivity* host,
            /* [in] */ const String& name)
            : mHost(host)
        {
            Thread::constructor(name);
        }

        CARAPI Run();

    private:
        ShutdownActivity* mHost;
    };

public:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    static const String TAG;
    Boolean mReboot;
    Boolean mConfirm;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_SHUTDOWNACTIVITY_H__
