
#include "elastos/droid/server/am/CStoppingReceiver.h"
#include "elastos/droid/server/am/CActivityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

ECode CStoppingReceiver::constructor(
    /* [in] */ Handle32 host,
    /* [in] */ Handle32 uss,
    /* [in] */ IIntent* shutdownIntent,
    /* [in] */ IIntentReceiver* shutdownReceiver,
    /* [in] */ Int32 userId)
{
    mHost = (CActivityManagerService*)host;
    mUss = (UserStartedState*)uss;
    mShutdownIntent = shutdownIntent;
    mShutdownReceiver = shutdownReceiver;
    mUserId = userId;
    return NOERROR;
}

ECode CStoppingReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    // On to the next.
    {
        AutoLock lock(mHost->mLock);
        if (mUss->mState != UserStartedState::STATE_STOPPING) {
            // Whoops, we are being started back up.  Abort, abort!
            return NOERROR;
        }
        mUss->mState = UserStartedState::STATE_SHUTDOWN;
    }
    Int32 result;
    mHost->BroadcastIntentLocked(NULL, String(NULL), mShutdownIntent,
            String(NULL), mShutdownReceiver, 0, String(NULL), NULL, String(NULL),
            TRUE, FALSE, CActivityManagerService::MY_PID, IProcess::SYSTEM_UID, mUserId, &result);
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
