
#include "elastos/droid/server/wm/Task.h"
#include "elastos/droid/server/wm/TaskStack.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include "elastos/droid/server/wm/WindowState.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
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
    CArrayList::New((IArrayList**)&mAppTokens);
    mTaskId = wtoken->mGroupId;
    mAppTokens->Add((IObject*)wtoken);
}

AutoPtr<DisplayContent> Task::GetDisplayContent()
{
    return mStack->GetDisplayContent();
}

void Task::AddAppToken(
    /* [in] */ Int32 addPos,
    /* [in] */ AppWindowToken* wtoken)
{
    Int32 lastPos;
    mAppTokens->GetSize(&lastPos);
    for (Int32 pos = 0; pos < lastPos && pos < addPos; ++pos) {
        AutoPtr<IInterface> value;
        mAppTokens->Get(pos, (IInterface**)&value);
        AutoPtr<AppWindowToken> token = (AppWindowToken*)(IObject*)value.Get();
        if (token->mRemoved) {
            // addPos assumes removed tokens are actually gone.
            ++addPos;
        }
    }
    mAppTokens->Add(addPos, (IObject*)wtoken);
    mDeferRemoval = FALSE;
}

Boolean Task::RemoveAppToken(
    /* [in] */ AppWindowToken* wtoken)
{
    Boolean removed;
    mAppTokens->Remove((IObject*)wtoken, &removed);
    Int32 size;
    if (mAppTokens->GetSize(&size), size == 0) {
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
