
#include "elastos/droid/server/wm/Task.h"
#include "elastos/droid/server/wm/TaskStack.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/server/wm/WindowState.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

Task::Task(
    /* [in] */ AppWindowToken* wtoken,
    /* [in] */ TaskStack* stack,
    /* [in] */ Int32 userId)
    : mStack(stack)
    , mTaskId(0)
    , mUserId(userId)
    , mDeferRemoval(FALSE)
{
    mTaskId = wtoken->mGroupId;
    mAppTokens.PushBack(wtoken);
}

AutoPtr<DisplayContent> Task::GetDisplayContent()
{
    return mStack->GetDisplayContent();
}

void Task::AddAppToken(
    /* [in] */ Int32 addPos,
    /* [in] */ AppWindowToken* wtoken)
{
    AppTokenList::Iterator it = mAppTokens.Begin();
    for (Int32 pos = 0; it != mAppTokens.End() && pos < addPos; ++pos, ++it) {
        if ((*it)->mRemoved) {
            // addPos assumes removed tokens are actually gone.
            ++addPos;
        }
    }
    mAppTokens.Insert(addPos, wtoken);
    mDeferRemoval = FALSE;
}

Boolean Task::RemoveAppToken(
    /* [in] */ AppWindowToken* wtoken)
{
    AppTokenList::Iterator it = Find(mAppTokens.Begin(), mAppTokens.End(), AutoPtr<AppWindowToken>(wtoken));
    Boolean removed = FALSE;
    if (it != mAppTokens.End()) {
        mAppTokens.Erase(it);
        removed = TRUE;
    }
    if (mAppTokens.Begin() == mAppTokens.End()) {
        Slogger::D("WM_TASK_REMOVED", "%d removeAppToken: last token", mTaskId);
        // EventLog.writeEvent(com.android.server.EventLogTags.WM_TASK_REMOVED, taskId,
        //         "removeAppToken: last token");
    }
    return removed;
}

ECode Task::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("{taskId=") + StringUtils::ToString(mTaskId)/* + mAppTokens + "}"*/;
    return NOERROR;
}

} // Wm
} // Server
} // Droid
} // Elastos
