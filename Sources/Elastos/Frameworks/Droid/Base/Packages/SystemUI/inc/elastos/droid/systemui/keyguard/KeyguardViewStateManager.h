
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWSTATEMANAGER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWSTATEMANAGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardViewStateManager
    : public Object
    , public IKeyguardViewStateManager
    , public ISlidingChallengeLayoutOnChallengeScrolledListener
    , public IChallengeLayoutOnBouncerStateChangedListener
{
private:
    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("KeyguardViewStateManager::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ KeyguardViewStateManager* host)
            : mHost(host)
        {}

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        KeyguardViewStateManager* mHost;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("KeyguardViewStateManager::MyAnimatorListenerAdapter2")

        MyAnimatorListenerAdapter2(
            /* [in] */ KeyguardViewStateManager* host)
            : mHost(host)
        {}

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        KeyguardViewStateManager* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("KeyguardViewStateManager::MyRunnable")

        MyRunnable(
            /* [in] */ KeyguardViewStateManager* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        KeyguardViewStateManager* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("KeyguardViewStateManager::MyRunnable2")

        MyRunnable2(
            /* [in] */ KeyguardViewStateManager* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        KeyguardViewStateManager* mHost;
    };

public:
    TO_STRING_IMPL("KeyguardViewStateManager")

    CAR_INTERFACE_DECL()

    KeyguardViewStateManager();

    CARAPI constructor(
        /* [in] */ IKeyguardHostView* hostView);

    CARAPI SetPagedView(
        /* [in] */ IKeyguardWidgetPager* pagedView);

    CARAPI SetChallengeLayout(
        /* [in] */ IChallengeLayout* layout);

    CARAPI IsChallengeShowing(
        /* [out] */ Boolean* result);

    CARAPI IsChallengeOverlapping(
        /* [out] */ Boolean* result);

    CARAPI SetSecurityViewContainer(
        /* [in] */ IKeyguardSecurityView* container);

    CARAPI ShowBouncer(
        /* [in] */  Boolean show);

    CARAPI IsBouncing(
        /* [out] */ Boolean* result);

    CARAPI FadeOutSecurity(
        /* [in] */ Int32 duration);

    CARAPI FadeInSecurity(
        /* [in] */ Int32 duration);

    CARAPI OnPageBeginMoving();

    CARAPI OnPageEndMoving();

    CARAPI OnPageSwitching(
        /* [in] */ IView* newPage,
        /* [in] */ Int32 newPageIndex);

    CARAPI OnPageSwitched(
        /* [in] */ IView* newPage,
        /* [in] */ Int32 newPageIndex);

    CARAPI OnPageBeginWarp();

    CARAPI OnPageEndWarp();

    //@Override
    CARAPI OnScrollStateChanged(
        /* [in] */ Int32 scrollState);

    //@Override
    CARAPI OnScrollPositionChanged(
        /* [in] */ Float scrollPosition,
        /* [in] */ Int32 challengeTop);

    CARAPI ShowUsabilityHints();

    // ChallengeLayout.OnBouncerStateChangedListener
    //@Override
    CARAPI OnBouncerStateChanged(
        /* [in] */ Boolean bouncerActive);

private:
    CARAPI_(void) UpdateEdgeSwiping();

    CARAPI_(Int32) GetChallengeTopRelativeToFrame(
        /* [in] */ IKeyguardWidgetFrame* frame,
        /* [in] */ Int32 top);

    /**
     * Simple method to map a point from one view's coordinates to another's. Note: this method
     * doesn't account for transforms, so if the views will be transformed, this should not be used.
     *
     * @param fromView The view to which the point is relative
     * @param toView The view into which the point should be mapped
     * @param pt The point
     */
    CARAPI_(void) MapPoint(
        /* [in] */ IView* fromView,
        /* [in] */ IView* toView,
        /* [in] */ ArrayOf<Int32>* pt);

    CARAPI_(void) UserActivity();

public:
    AutoPtr<IHandler> mMainQueue;

    Int32 mLastScrollState;

    Int32 mChallengeTop;

private:
    static const String TAG;
    AutoPtr<IKeyguardWidgetPager> mKeyguardWidgetPager;
    AutoPtr<IChallengeLayout> mChallengeLayout;
    AutoPtr<IKeyguardHostView> mKeyguardHostView;
    AutoPtr<ArrayOf<Int32> > mTmpPoint;
    AutoPtr<ArrayOf<Int32> > mTmpLoc;

    AutoPtr<IKeyguardSecurityView> mKeyguardSecurityContainer;
    static const Int32 SCREEN_ON_HINT_DURATION;
    static const Int32 SCREEN_ON_RING_HINT_DELAY;
    static const Boolean SHOW_INITIAL_PAGE_HINTS;

    // Paged view state
    Int32 mPageListeningToSlider;
    Int32 mCurrentPage;
    Int32 mPageIndexOnPageBeginMoving;

    AutoPtr<IAnimatorListener> mPauseListener;

    AutoPtr<IAnimatorListener> mResumeListener;

    AutoPtr<IRunnable> mHideHintsRunnable;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDVIEWSTATEMANAGER_H__