
#include "elastos/droid/launcher2/CheckLongPressHelper.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

using Elastos::Droid::View::IViewParent;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CheckLongPressHelper::CheckForLongPress::CheckForLongPress(
    /* [in] */ CheckLongPressHelper* host)
    : mHost(host)
{
}

ECode CheckLongPressHelper::CheckForLongPress::Run()
{
    AutoPtr<IViewParent> parent;
    mHost->mView->GetParent((IViewParent**)&parent);
    if (parent != NULL) {
        Boolean res;
        mHost->mView->HasWindowFocus(&res);
        if (res) {
            if (!mHost->mHasPerformedLongPress) {
                Boolean tmp;
                mHost->mView->PerformLongClick(&tmp);
                if (tmp) {
                    mHost->mView->SetPressed(FALSE);
                    mHost->mHasPerformedLongPress = TRUE;
                }
            }
        }
    }
    return NOERROR;
}

CheckLongPressHelper::CheckLongPressHelper(
    /* [in] */ IView* v)
    : mView(v)
    , mHasPerformedLongPress(FALSE)
{
}

ECode CheckLongPressHelper::PostCheckForLongPress()
{
    mHasPerformedLongPress = FALSE;

    if (mPendingCheckForLongPress == NULL) {
        mPendingCheckForLongPress = new CheckForLongPress(this);
    }
    //Boolean res;
    assert(0 && "need class LauncherApplication");
    //return mView->PostDelayed(mPendingCheckForLongPress, LauncherApplication::GetLongPressTimeout(), &res);
    return NOERROR;
}

void CheckLongPressHelper::CancelLongPress()
{
    mHasPerformedLongPress = FALSE;
    if (mPendingCheckForLongPress != NULL) {
        Boolean res;
        mView->RemoveCallbacks(mPendingCheckForLongPress, &res);
        mPendingCheckForLongPress = NULL;
    }
    return;
}

Boolean CheckLongPressHelper::HasPerformedLongPress()
{
    return mHasPerformedLongPress;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos