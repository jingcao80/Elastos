
#include "elastos/droid/systemui/settings/CurrentUserTracker.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

CurrentUserTracker::CurrentUserTracker(
    /* [in] */ IContext* context)
    : mContext(context)
    , mCurrentUserId(0)
{}

Int32 CurrentUserTracker::GetCurrentUserId()
{
    return mCurrentUserId;
}

ECode CurrentUserTracker::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        Int32 oldUserId = mCurrentUserId;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &mCurrentUserId);
        if (oldUserId != mCurrentUserId) {
            OnUserSwitched(mCurrentUserId);
        }
    }
    return NOERROR;
}

void CurrentUserTracker::StartTracking()
{
    AutoPtr<IActivityManagerHelper> amh;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amh);
    amh->GetCurrentUser(&mCurrentUserId);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(this, filter, (IIntent**)&intent);
}

void CurrentUserTracker::StopTracking()
{
    mContext->UnregisterReceiver(this);
}

Boolean CurrentUserTracker::IsCurrentUserOwner()
{
    return mCurrentUserId == IUserHandle::USER_OWNER;
}

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
