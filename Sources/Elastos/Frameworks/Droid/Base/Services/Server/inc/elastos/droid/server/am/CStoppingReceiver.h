
#ifndef __ELASTOS_DROID_SERVER_AM_CSTOPPINGRECEIVER_H__
#define __ELASTOS_DROID_SERVER_AM_CSTOPPINGRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Am_CStoppingReceiver.h"
#include "elastos/droid/server/am/UserStartedState.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;

class CActivityManagerService;

CarClass(CStoppingReceiver)
{
public:
    CARAPI constructor(
        /* [in] */ Handle32 host,
        /* [in] */ Handle32 uss,
        /* [in] */ IIntent* shutdownIntent,
        /* [in] */ IIntentReceiver* shutdownReceiver,
        /* [in] */ Int32 userId);

    CARAPI PerformReceive(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

private:
    CActivityManagerService* mHost;
    AutoPtr<UserStartedState> mUss;
    AutoPtr<IIntent> mShutdownIntent;
    AutoPtr<IIntentReceiver> mShutdownReceiver;
    Int32 mUserId;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CSTOPPINGRECEIVER_H__
