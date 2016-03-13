
#include "elastos/droid/launcher2/CheckLongPressHelper.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

ECode CheckLongPressHelper::CheckForLongPress::CheckForLongPress(
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
                mView->PerformLongClick(&tmp);
                if (tmp) {
                    mView->SetPressed(FALSE);
                    mHasPerformedLongPress = TRUE;
                }
            }
        }
    }
    return NOERROR;
}

CheckLongPressHelper::CheckLongPressHelper(
    /* [in] */ IView* v)
    : mHasPerformedLongPress(FALSE)
    , mView(v)
{
}

ECode CheckLongPressHelper::PostCheckForLongPress()
{
    mHasPerformedLongPress = FALSE;

    if (mPendingCheckForLongPress == NULL) {
        mPendingCheckForLongPress = new CheckForLongPress();
    }
    return mView->PostDelayed(mPendingCheckForLongPress, LauncherApplication::GetLongPressTimeout());
}

void CheckLongPressHelper::CancelLongPress()
{
    mHasPerformedLongPress = FALSE;
    if (mPendingCheckForLongPress != NULL) {
        mView->RemoveCallbacks(mPendingCheckForLongPress);
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