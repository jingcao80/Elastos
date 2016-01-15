
#include "elastos/droid/server/am/AppErrorResult.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

AppErrorResult::AppErrorResult()
    : mHasResult(FALSE)
    , mResult(0)
{}

void AppErrorResult::SetResult(
    /* [in] */ Int32 res)
{
    AutoLock lock(this);
    mHasResult = TRUE;
    mResult = res;
    NotifyAll();
}

Int32 AppErrorResult::GetResult()
{
    AutoLock lock(this);
    while (!mHasResult) {
        Wait();
    }

    return mResult;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
