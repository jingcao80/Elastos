
#include "elastos/droid/systemui/recents/views/SystemBarScrimViews.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.App.h"
#include "../R.h"

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

SystemBarScrimViews::StartAcitonRunnable::StartAcitonRunnable(
    /* [in] */ IView* view)
    : mView(view)
{
}

ECode SystemBarScrimViews::StartAcitonRunnable::Run()
{
    mView->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

SystemBarScrimViews::SystemBarScrimViews(
    /* [in] */ IActivity* activity,
    /* [in] */ RecentsConfiguration* config)
    : mHasNavBarScrim(FALSE)
    , mShouldAnimateStatusBarScrim(FALSE)
    , mHasStatusBarScrim(FALSE)
    , mShouldAnimateNavBarScrim(FALSE)
{
    mConfig = config;
    activity->FindViewById(R::id::status_bar_scrim, (IView**)&mStatusBarScrimView);
    activity->FindViewById(R::id::nav_bar_scrim, (IView**)&mNavBarScrimView);
}

/**
 * Prepares the scrim views for animating when entering Recents. This will be called before
 * the first draw.
 */
void SystemBarScrimViews::PrepareEnterRecentsAnimation()
{
    mHasNavBarScrim = mConfig->HasNavBarScrim();
    mShouldAnimateNavBarScrim = mConfig->ShouldAnimateNavBarScrim();
    mHasStatusBarScrim = mConfig->HasStatusBarScrim();
    mShouldAnimateStatusBarScrim = mConfig->ShouldAnimateStatusBarScrim();

    mNavBarScrimView->SetVisibility(mHasNavBarScrim && !mShouldAnimateNavBarScrim ?
            IView::VISIBLE : IView::INVISIBLE);
    mStatusBarScrimView->SetVisibility(mHasStatusBarScrim && !mShouldAnimateStatusBarScrim ?
            IView::VISIBLE : IView::INVISIBLE);
}

/**
 * Starts animating the scrim views when entering Recents.
 */
void SystemBarScrimViews::StartEnterRecentsAnimation()
{
    if (mHasStatusBarScrim && mShouldAnimateStatusBarScrim) {
        Int32 h;
        mStatusBarScrimView->GetMeasuredHeight(&h);
        mStatusBarScrimView->SetTranslationY(-h);
        AutoPtr<IViewPropertyAnimator> animate;
        mStatusBarScrimView->Animate((IViewPropertyAnimator**)&animate);
        animate->TranslationY(0);
        animate->SetStartDelay(mConfig->mTaskBarEnterAnimDelay);
        animate->SetDuration(mConfig->mNavBarScrimEnterDuration);
        animate->SetInterpolator(ITimeInterpolator::Probe(mConfig->mQuintOutInterpolator));
        AutoPtr<Runnable> runnable = new StartAcitonRunnable(mStatusBarScrimView);
        animate->WithStartAction(runnable);
        animate->Start();
    }
    if (mHasNavBarScrim && mShouldAnimateNavBarScrim) {
        Int32 h;
        mNavBarScrimView->GetMeasuredHeight(&h);
        mNavBarScrimView->SetTranslationY(h);
        AutoPtr<IViewPropertyAnimator> animate;
        mNavBarScrimView->Animate((IViewPropertyAnimator**)&animate);
        animate->TranslationY(0);
        animate->SetStartDelay(mConfig->mTaskBarEnterAnimDelay);
        animate->SetDuration(mConfig->mNavBarScrimEnterDuration);
        animate->SetInterpolator(ITimeInterpolator::Probe(mConfig->mQuintOutInterpolator));
        AutoPtr<Runnable> runnable = new StartAcitonRunnable(mNavBarScrimView);
        animate->WithStartAction(runnable);
        animate->Start();
    }
}

/**
 * Starts animating the scrim views when leaving Recents (either via launching a task, or
 * going home).
 */
void SystemBarScrimViews::StartExitRecentsAnimation()
{
    if (mHasStatusBarScrim && mShouldAnimateStatusBarScrim) {
        AutoPtr<IViewPropertyAnimator> animate;
        mStatusBarScrimView->Animate((IViewPropertyAnimator**)&animate);
        Int32 h;
        mStatusBarScrimView->GetMeasuredHeight(&h);
        animate->TranslationY(-h);
        animate->SetStartDelay(0);
        animate->SetDuration(mConfig->mTaskBarExitAnimDuration);
        animate->SetInterpolator(ITimeInterpolator::Probe(mConfig->mFastOutSlowInInterpolator));
        animate->Start();
    }
    if (mHasNavBarScrim && mShouldAnimateNavBarScrim) {
        AutoPtr<IViewPropertyAnimator> animate;
        mNavBarScrimView->Animate((IViewPropertyAnimator**)&animate);
        Int32 h;
        mNavBarScrimView->GetMeasuredHeight(&h);
        animate->TranslationY(h);
        animate->SetStartDelay(0);
        animate->SetDuration(mConfig->mTaskBarExitAnimDuration);
        animate->SetInterpolator(ITimeInterpolator::Probe(mConfig->mFastOutSlowInInterpolator));
        animate->Start();
    }
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
