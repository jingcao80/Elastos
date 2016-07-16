
#include "elastos/droid/systemui/keyguard/CheckLongPressHelper.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CheckLongPressHelper::CheckForLongPress::Run()
{
    AutoPtr<IViewParent> parent;
    mHost->mView->GetParent((IViewParent**)&parent);
    Boolean res;
    if ((parent != NULL) && (mHost->mView->HasWindowFocus(&res), res)
            && !mHost->mHasPerformedLongPress) {

        if (mView->PerformLongClick(&res), res) {
            mView->SetPressed(FALSE);
            mHasPerformedLongPress = TRUE;
        }
    }
    return NOERROR;
}

CheckLongPressHelper::CheckLongPressHelper(
    /* [in] */ IView* v)
    : mView(v)
    , mHasPerformedLongPress(FALSE)
    , mDownX(0.0)
    , mDownY(0.0)
{
    AutoPtr<IContext> context;
    v->GetContext((IContext**)&context);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> viewConfig;
    helper->Get(context, (IViewConfiguration**)&viewConfig);
    viewConfig->GetScaledTouchSlop(&mScaledTouchSlop);
    helper->GetLongPressTimeout(&mLongPressTimeout);
}

ECode CheckLongPressHelper::PostCheckForLongPress(
    /* [in] */ IMotionEvent* ev)
{
    ev->GetX(&mDownX);
    ev->GetY(&mDownY);
    mHasPerformedLongPress = FALSE;

    if (mPendingCheckForLongPress == NULL) {
        mPendingCheckForLongPress = new CheckForLongPress();
    }
    Boolean res;
    return mView->PostDelayed(IRunnable::Probe(mPendingCheckForLongPress), mLongPressTimeout, &res);
}

ECode CheckLongPressHelper::OnMove(
    /* [in] */ IMotionEvent* ev)
{
    Float x;
    ev->GetX(&x);
    Float y;
    ev->GetY(&y);
    Boolean xMoved = Math::Abs(mDownX - x) > mScaledTouchSlop;
    Boolean yMoved = Math::Abs(mDownY - y) > mScaledTouchSlop;

    if (xMoved || yMoved) {
        CancelLongPress();
    }
    return NOERROR;
}

ECode CheckLongPressHelper::CancelLongPress()
{
    mHasPerformedLongPress = FALSE;
    if (mPendingCheckForLongPress != NULL) {
        mView->RemoveCallbacks(mPendingCheckForLongPress);
        mPendingCheckForLongPress = NULL;
    }
}

ECode CheckLongPressHelper::HasPerformedLongPress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHasPerformedLongPress;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
