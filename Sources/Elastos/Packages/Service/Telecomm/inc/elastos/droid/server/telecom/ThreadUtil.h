
#ifndef __ELASTOS_DROID_SERVER_TELECOM_THREADUTIL_H__
#define __ELASTOS_DROID_SERVER_TELECOM_THREADUTIL_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Helper methods to deal with threading related tasks.
 */
class ThreadUtil
    : public Object
{
public:
    /** @return whether this method is being called from the main (UI) thread. */
    static CARAPI IsOnMainThread(
        /* [out] */ Boolean* result);

    /**
     * Checks that this is being executed on the main thread. If not, a message is logged at
     * WTF-level priority.
     */
    static CARAPI CheckOnMainThread();

    /**
     * Checks that this is not being executed on the main thread. If so, a message is logged at
     * WTF-level priority.
     */
    static CARAPI CheckNotOnMainThread();

private:
    ThreadUtil();

private:
    static const String TAG;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_THREADUTIL_H__
