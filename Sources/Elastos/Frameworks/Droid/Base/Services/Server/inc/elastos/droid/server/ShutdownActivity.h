
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
        : public Thread
    {
    public:
        LocalThread(
            /* [in] */ ShutdownActivity* host)
            : mHost(host)
        {
        }

        CARAPI constructor(
            /* [in] */ const String& name)
        {
            return Thread::constructor(name);
        }

        CARAPI Run();

    private:
        ShutdownActivity* mHost;
    };

public:
    ShutdownActivity();

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
