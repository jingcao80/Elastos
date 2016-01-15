
#include "elastos/droid/server/am/CShutdownReceiver.h"
#include "elastos/droid/server/am/CActivityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

ECode CShutdownReceiver::constructor(
    /* [in] */ Handle32 host,
    /* [in] */ Handle32 uss)
{
    mHost = (CActivityManagerService*)host;
    mUss = (UserStartedState*)uss;
    return NOERROR;
}

ECode CShutdownReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    mHost->FinishUserStop(mUss);
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos