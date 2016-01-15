#ifndef __ELASTOS_DROID_SERVER_RESETTABLETIMEOUT_H__
#define __ELASTOS_DROID_SERVER_RESETTABLETIMEOUT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Core.Server.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Os::IConditionVariable;
using Elastos::Droid::Server::IResettableTimeout;
using Elastos::Core::IThread;
using Elastos::Core::Object;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Utility class that you can call on with a timeout, and get called back
 * after a given time, dealing correctly with restarting the timeout.
 *
 * <p>For example, this class is used by the android.os.Vibrator class.
 */
class ResettableTimeout
    : public Object
    , public IResettableTimeout
{
private:
    class T
        : public Thread
    {
    public:
        T(
            /* [in] */ ResettableTimeout* host);

        CARAPI Run();

    private:
        ResettableTimeout* mHost;
    };

    friend class T;
public:
    CAR_INTERFACE_DECL()

    ResettableTimeout();

    /**
     * Override this do what you need to do when it's starting
     * This is called with the monitor on this method held, so be careful.
     *
     * @param alreadyOn is true if it's currently running
     */
    virtual CARAPI On(
        /* [in] */ Boolean alreadyOn) = 0;

    /**
     * Override this to do what you need to do when it's stopping.
     * This is called with the monitor on this method held, so be careful.
     */
    virtual CARAPI Off() = 0;

    /**
     * Does the following steps.
     * <p>1. Call on()</p>
     * <p>2. Start the timer.</p>
     * <p>3. At the timeout, calls off()<p>
     * <p>If you call this again, the timeout is reset to the new one</p>
     */
    CARAPI Go(
        /* [in] */ Int64 milliseconds);

    /**
     * Cancel the timeout and call off now.
     */
    CARAPI Cancel();

private:
    AutoPtr<IConditionVariable> mLock;

    // turn it off at this time.
    Int64 mOffAt;
    Boolean mOffCalled;

    AutoPtr<IThread> mThread;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_RESETTABLETIMEOUT_H__