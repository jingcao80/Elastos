
#ifndef __ELASTOS_DROID_SERVER_AM_CSHUTDOWNRECEIVER_H__
#define __ELASTOS_DROID_SERVER_AM_CSHUTDOWNRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Am_CShutdownReceiver.h"
#include "elastos/droid/server/am/UserStartedState.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IIntent;

class CActivityManagerService;

CarClass(CShutdownReceiver)
{
public:
    CARAPI constructor(
        /* [in] */ Handle32 host,
        /* [in] */ Handle32 uss);

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
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CSHUTDOWNRECEIVER_H__
