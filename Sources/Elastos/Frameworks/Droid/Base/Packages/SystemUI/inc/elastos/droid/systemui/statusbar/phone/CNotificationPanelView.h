//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNOTIFICATIONPANELVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNOTIFICATIONPANELVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CNotificationPanelView.h"
#include "elastos/droid/systemui/statusbar/phone/PanelView.h"
#include "elastos/droid/systemui/statusbar/phone/KeyguardClockPositionAlgorithm.h"
#include "elastos/droid/systemui/statusbar/phone/KeyguardAffordanceHelper.h"
#include "elastos/droid/systemui/statusbar/phone/SecureCameraLaunchManager.h"
#include "elastos/droid/systemui/statusbar/policy/KeyguardUserSwitcher.h"
#include "elastos/droid/systemui/statusbar/FlingAnimationUtils.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::SystemUI::Qs::IQSPanel;
using Elastos::Droid::SystemUI::StatusBar::Policy::KeyguardUserSwitcher;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardUserSwitcher;
using Elastos::Droid::SystemUI::StatusBar::Stack::INotificationStackScrollLayout;
using Elastos::Droid::SystemUI::StatusBar::Stack::IOnOverscrollTopChangedListener;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::Widget::IGridLayout;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CNotificationPanelView)
    , public PanelView
    , public INotificationPanelView
{
private:
    class AffordanceHelperCallback
        : public Object
        , public IKeyguardAffordanceHelperCallback
    {
    public:
        CAR_INTERFACE_DECL()

        AffordanceHelperCallback(
            /* [in] */ CNotificationPanelView* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnAnimationToSideStarted(
            /* [in] */ Boolean rightPage)
        {
            return mHost->OnAnimationToSideStarted(rightPage);
        }

        // @Override
        CARAPI OnAnimationToSideEnded()
        {
            return mHost->OnAnimationToSideEnded();
        }

        // @Override
        CARAPI GetPageWidth(
            /* [out] */ Float* result)
        {
            return mHost->GetPageWidth(result);
        }

        // @Override
        CARAPI OnSwipingStarted()
        {
            return mHost->OnSwipingStarted();
        }

        // @Override
        CARAPI GetLeftIcon(
            /* [out] */ IKeyguardAffordanceView** view)
        {
            return mHost->GetLeftIcon(view);
        }

        // @Override
        CARAPI GetCenterIcon(
            /* [out] */ IKeyguardAffordanceView** view)
        {
            return mHost->GetCenterIcon(view);
        }

        // @Override
        CARAPI GetRightIcon(
            /* [out] */ IKeyguardAffordanceView** view)
        {
            return mHost->GetRightIcon(view);
        }

        // @Override
        CARAPI GetLeftPreview(
            /* [out] */ IView** view)
        {
            return mHost->GetLeftPreview(view);
        }

        // @Override
        CARAPI GetRightPreview(
            /* [out] */ IView** view)
        {
            return mHost->GetRightPreview(view);
        }

        // @Override
        CARAPI GetAffordanceFalsingFactor(
            /* [out] */ Float* result)
        {
            return mHost->GetAffordanceFalsingFactor(result);
        }

    private:
        CNotificationPanelView* mHost;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationPanelView* mHost;
    };

    class Runnable2: public Runnable
    {
    public:
        Runnable2(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationPanelView* mHost;
    };

    class AnimatorListenerAdapter1: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter1(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CNotificationPanelView* mHost;
    };

    class QsContainerAnimatorUpdaterListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        QsContainerAnimatorUpdaterListener(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);

    private:
        CNotificationPanelView* mHost;
    };

    class QsContainerOnLayoutChangeListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        QsContainerOnLayoutChangeListener(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);

    private:
        CNotificationPanelView* mHost;
    };

    class OnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnPreDrawListener(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        CNotificationPanelView* mHost;
    };

    class KeyguardStatusBarVisibilityRunnable: public Runnable
    {
    public:
        KeyguardStatusBarVisibilityRunnable(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationPanelView* mHost;
    };

    class KeyguardBottomAreaVisibilityRunnable: public Runnable
    {
    public:
        KeyguardBottomAreaVisibilityRunnable(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationPanelView* mHost;
    };

    class HeaderUpdateEverythingRunnable: public Runnable
    {
    public:
        HeaderUpdateEverythingRunnable(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationPanelView* mHost;
    };

    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnClickListener(
            /* [in] */ CNotificationPanelView* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CNotificationPanelView* mHost;
    };

    class ScrollViewListener
        : public Object
        , public IObservableScrollViewListener
    {
    public:
        CAR_INTERFACE_DECL()

        ScrollViewListener(
            /* [in] */ CNotificationPanelView* host);

        CARAPI OnScrollChanged();

        CARAPI OnOverscrolled(
            /* [in] */ Float lastX,
            /* [in] */ Float lastY,
            /* [in] */ Int32 amount);

    private:
        CNotificationPanelView* mHost;
    };

    class HeightChangedListener
        : public Object
        , public IExpandableViewOnHeightChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        HeightChangedListener(
            /* [in] */ CNotificationPanelView* host);

        CARAPI OnHeightChanged(
            /* [in] */ IExpandableView* view);

        CARAPI OnReset(
            /* [in] */ IExpandableView* view);

    private:
        CNotificationPanelView* mHost;
    };

    class TopChangedListener
        : public Object
        , public IOnOverscrollTopChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        TopChangedListener(
            /* [in] */ CNotificationPanelView* host);

        CARAPI OnOverscrollTopChanged(
            /* [in] */ Float amount,
            /* [in] */ Boolean isRubberbanded);

        CARAPI FlingTopOverscroll(
            /* [in] */ Float velocity,
            /* [in] */ Boolean open);

    private:
        CNotificationPanelView* mHost;
    };

    class OnPreDrawListener2
        : public Object
        , public IOnPreDrawListener
    {
    private:
        class AnimatorListenerAdapter2: public AnimatorListenerAdapter
        {
        public:
            AnimatorListenerAdapter2(
                /* [in] */ CNotificationPanelView* host);

            // @Override
            CARAPI OnAnimationEnd(
                /* [in] */ IAnimator* animation);

        private:
            CNotificationPanelView* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        OnPreDrawListener2(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        CNotificationPanelView* mHost;
    };

    class Runnable6: public Runnable
    {
    public:
        Runnable6(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationPanelView* mHost;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CNotificationPanelView* mHost;
    };

    class AnimatorListenerAdapter3: public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter3(
            /* [in] */ CNotificationPanelView* host,
            /* [in] */ IRunnable* onFinishRunnable);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CNotificationPanelView* mHost;
        AutoPtr<IRunnable> mOnFinishRunnable;
    };

    class Runnable7: public Runnable
    {
    public:
        Runnable7(
            /* [in] */ CNotificationPanelView* host);

        // @Override
        CARAPI Run();

    private:
        CNotificationPanelView* mHost;
    };

    class Runnable8: public Runnable
    {
    public:
        Runnable8(
            /* [in] */ CNotificationPanelView* host,
            /* [in] */ IKeyguardAffordanceView* icon);

        // @Override
        CARAPI Run();

    private:
        CNotificationPanelView* mHost;
        AutoPtr<IKeyguardAffordanceView> mIcon;
    };

    class SetBgColorAlphaAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        SetBgColorAlphaAnimatorUpdateListener(
            /* [in] */ IView* target,
            /* [in] */ Int32 r,
            /* [in] */ Int32 g,
            /* [in] */ Int32 b);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<IView> mTarget;
        Int32 mR;
        Int32 mG;
        Int32 mB;
    };

    class SetBgColorAlphaAnimatorListenerAdapter: public AnimatorListenerAdapter
    {
    public:
        SetBgColorAlphaAnimatorListenerAdapter(
            /* [in] */ IView* target);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mTarget;
    };


public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNotificationPanelView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetStatusBar(
        /* [in] */ IPhoneStatusBar* bar);

    CARAPI UpdateResources();

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    CARAPI AnimateToFullShade(
        /* [in] */ Int64 delay);

    CARAPI SetQsExpansionEnabled(
        /* [in] */ Boolean qsExpansionEnabled);

    // @Override
    CARAPI ResetViews();

    CARAPI CloseQs();

    CARAPI AnimateCloseQs();

    CARAPI OpenQs();

    // @Override
    CARAPI_(void) Fling(
        /* [in] */ Float vel,
        /* [in] */ Boolean expand);

    // @Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RequestDisallowInterceptTouchEvent(
        /* [in] */ Boolean disallowIntercept);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnOverscrolled(
        /* [in] */ Float lastTouchX,
        /* [in] */ Float lastTouchY,
        /* [in] */ Int32 amount);

    // @Override
    CARAPI OnOverscrollTopChanged(
        /* [in] */ Float amount,
        /* [in] */ Boolean isRubberbanded);

    // @Override
    CARAPI FlingTopOverscroll(
        /* [in] */ Float velocity,
        /* [in] */ Boolean open);

    CARAPI SetBarState(
        /* [in] */ Int32 statusBarState,
        /* [in] */ Boolean keyguardFadingAway,
        /* [in] */ Boolean goingToFullShade);

    // @Override
    CARAPI InstantExpand();

    // @Override
    CARAPI OnHeightChanged(
        /* [in] */ IExpandableView* view);

    // @Override
    CARAPI OnReset(
        /* [in] */ IExpandableView* view);

    using PanelView::OnScrollChanged;

    // @Override
    CARAPI OnScrollChanged();

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    // @Override
    CARAPI OnAnimationToSideStarted(
        /* [in] */ Boolean rightPage);

    // @Override
    CARAPI OnAnimationToSideEnded();

    // @Override
    CARAPI GetPageWidth(
        /* [out] */ Float* result);

    // @Override
    CARAPI OnSwipingStarted();

    // @Override
    CARAPI GetLeftIcon(
        /* [out] */ IKeyguardAffordanceView** view);

    // @Override
    CARAPI GetCenterIcon(
        /* [out] */ IKeyguardAffordanceView** view);

    // @Override
    CARAPI GetRightIcon(
        /* [out] */ IKeyguardAffordanceView** view);

    // @Override
    CARAPI GetLeftPreview(
        /* [out] */ IView** view);

    // @Override
    CARAPI GetRightPreview(
        /* [out] */ IView** view);

    // @Override
    CARAPI GetAffordanceFalsingFactor(
        /* [out] */ Float* result);

    CARAPI NotifyVisibleChildrenChanged();

    CARAPI IsQsExpanded(
        /* [out] */ Boolean* result);

    CARAPI IsQsDetailShowing(
        /* [out] */ Boolean* result);

    CARAPI CloseQsDetail();

    // @Override
    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    CARAPI IsLaunchTransitionFinished(
        /* [out] */ Boolean* result);

    CARAPI IsLaunchTransitionRunning(
        /* [out] */ Boolean* result);

    CARAPI SetLaunchTransitionEndRunnable(
        /* [in] */ IRunnable* r);

    CARAPI SetEmptyDragAmount(
        /* [in] */ Float amount);

    CARAPI SetDozing(
        /* [in] */ Boolean dozing);

    // @Override
    CARAPI_(Boolean) IsDozing();

    CARAPI SetShadeEmpty(
        /* [in] */ Boolean shadeEmpty);

    CARAPI SetQsScrimEnabled(
        /* [in] */ Boolean qsScrimEnabled);

    CARAPI SetKeyguardUserSwitcher(
        /* [in] */ IKeyguardUserSwitcher* keyguardUserSwitcher);

    CARAPI OnScreenTurnedOn();

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(void) LoadDimens();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI_(Boolean) FlingExpands(
        /* [in] */ Float vel,
        /* [in] */ Float vectorVel);

    // @Override
    CARAPI_(Boolean) HasConflictingGestures();

    // @Override
    CARAPI_(Boolean) IsScrolledToBottom();

    // @Override
    CARAPI_(Int32) GetMaxPanelHeight();

    // @Override
    CARAPI_(void) OnHeightUpdated(
        /* [in] */ Float expandedHeight);

    // @Override
    CARAPI_(Float) GetOverExpansionAmount();

    // @Override
    CARAPI_(Float) GetOverExpansionPixels();

    // @Override
    CARAPI_(void) OnExpandingStarted();

    // @Override
    CARAPI_(void) OnExpandingFinished();

    // @Override
    CARAPI_(void) SetOverExpansion(
        /* [in] */ Float overExpansion,
        /* [in] */ Boolean isPixels);

    // @Override
    CARAPI_(void) OnTrackingStarted();

    // @Override
    CARAPI_(void) OnTrackingStopped(
        /* [in] */ Boolean expand);

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI_(void) OnEdgeClicked(
        /* [in] */ Boolean right);

    // @Override
    CARAPI_(void) StartUnlockHintAnimation();

    // @Override
    CARAPI_(Float) GetPeekHeight();

    // @Override
    CARAPI_(Float) GetCannedFlingDurationFactor();

    // @Override
    CARAPI_(Boolean) FullyExpandedClearAllVisible();

    // @Override
    CARAPI_(Boolean) IsClearAllVisible();

    // @Override
    CARAPI_(Int32) GetClearAllHeight();

    // @Override
    CARAPI_(Boolean) IsTrackingBlocked();

private:
    /**
     * Positions the clock and notifications dynamically depending on how many notifications are
     * showing.
     */
    CARAPI_(void) PositionClockAndNotifications();

    CARAPI_(void) StartClockAnimation(
        /* [in] */ Int32 y);

    CARAPI_(void) UpdateClock(
        /* [in] */ Float alpha,
        /* [in] */ Float scale);

    CARAPI_(void) ResetDownStates(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) FlingQsWithCurrentVelocity();

    CARAPI_(Boolean) FlingExpandsQs(
        /* [in] */ Float vel);

    CARAPI_(Boolean) IsBelowFalsingThreshold();

    CARAPI_(Float) GetQsExpansionFraction();

    CARAPI_(Boolean) IsInQsArea(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) HandleQsDown(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) OnQsTouch(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Int32) GetFalsingThreshold();

    CARAPI_(void) OnQsExpansionStarted();

    CARAPI_(void) OnQsExpansionStarted(
        /* [in] */ Int32 overscrollAmount);

    CARAPI_(void) SetQsExpanded(
        /* [in] */ Boolean expanded);

    CARAPI_(void) AnimateHeaderSlidingIn();

    CARAPI_(void) AnimateKeyguardStatusBarOut();

    CARAPI_(void) SetKeyguardBottomAreaVisibility(
        /* [in] */ Int32 statusBarState,
        /* [in] */ Boolean goingToFullShade);

    CARAPI_(void) SetKeyguardStatusViewVisibility(
        /* [in] */ Int32 statusBarState,
        /* [in] */ Boolean keyguardFadingAway,
        /* [in] */ Boolean goingToFullShade);

    CARAPI_(void) UpdateQsState();

    CARAPI_(void) SetQsExpansion(
        /* [in] */ Float height);

    CARAPI_(String) GetKeyguardOrLockScreenString();

    CARAPI_(void) UpdateNotificationScrim(
        /* [in] */ Float height);

    CARAPI_(Float) GetHeaderExpansionFraction();

    CARAPI_(void) SetQsTranslation(
        /* [in] */ Float height);

    CARAPI_(Float) CalculateQsTopPadding();

    CARAPI_(void) RequestScrollerTopPaddingUpdate(
        /* [in] */ Boolean animate);

    CARAPI_(void) TrackMovement(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) InitVelocityTracker();

    CARAPI_(Float) GetCurrentVelocity();

    CARAPI_(void) CancelAnimation();

    CARAPI_(void) FlingSettings(
        /* [in] */ Float vel,
        /* [in] */ Boolean expand);

    CARAPI_(void) FlingSettings(
        /* [in] */ Float vel,
        /* [in] */ Boolean expand,
        /* [in] */ IRunnable* onFinishRunnable);

    /**
     * @return Whether we should intercept a gesture to open Quick Settings.
     */
    CARAPI_(Boolean) ShouldQuickSettingsIntercept(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float yDiff);

    CARAPI_(Boolean) IsInSettings();

    /**
     * @return a temporary override of {@link #mQsMaxExpansionHeight}, which is needed when
     *         collapsing QS / the panel when QS was scrolled
     */
    CARAPI_(Int32) GetTempQsMaxExpansion();

    CARAPI_(Int32) CalculatePanelHeightShade();

    CARAPI_(Int32) CalculatePanelHeightQsExpanded();

    CARAPI_(Int32) GetScrollViewScrollY();

    CARAPI_(void) UpdateNotificationTranslucency();

    CARAPI_(void) UpdateUnlockIcon();

    /**
     * Hides the header when notifications are colliding with it.
     */
    CARAPI_(void) UpdateHeader();

    CARAPI_(void) UpdateHeaderShade();

    CARAPI_(Float) GetHeaderTranslation();

    CARAPI_(void) UpdateHeaderKeyguard();

    CARAPI_(Float) GetNotificationsTopY();

    CARAPI_(void) SetListening(
        /* [in] */ Boolean listening);

    /**
     * Starts the highlight (making it fully opaque) animation on an icon.
     */
    CARAPI_(void) StartHighlightIconAnimation(
        /* [in] */ IKeyguardAffordanceView* icon);

    static CARAPI_(Float) Interpolate(
        /* [in] */ Float t,
        /* [in] */ Float start,
        /* [in] */ Float end);

    CARAPI_(void) UpdateKeyguardStatusBarVisibility();

    static CARAPI_(void) SetBackgroundColorAlpha(
        /* [in] */ IView* target,
        /* [in] */ Int32 rgb,
        /* [in] */ Int32 targetAlpha,
        /* [in] */ Boolean animate);

    static CARAPI_(Int32) GetBackgroundAlpha(
        /* [in] */ IView* view);

    CARAPI_(void) UpdateEmptyShadeView();


private:
    // Cap and total height of Roboto font. Needs to be adjusted when font for the big clock is
    // changed.
    static const Int32 CAP_HEIGHT;
    static const Int32 FONT_HEIGHT;

    static const Float HEADER_RUBBERBAND_FACTOR;
    static const Float LOCK_ICON_ACTIVE_SCALE;

    static const Int32 DOZE_BACKGROUND_COLOR;
    static const Int32 TAG_KEY_ANIM;
    static const Int64 DOZE_BACKGROUND_ANIM_DURATION;

    AutoPtr<KeyguardAffordanceHelper> mAfforanceHelper;
    AutoPtr<IStatusBarHeaderView> mHeader;
    AutoPtr<KeyguardUserSwitcher> mKeyguardUserSwitcher;
    AutoPtr<IKeyguardStatusBarView> mKeyguardStatusBar;
    AutoPtr<IView> mQsContainer;
    AutoPtr<IQSPanel> mQsPanel;
    AutoPtr<IGridLayout> mKeyguardStatusView;
    AutoPtr<IObservableScrollView> mScrollView;
    AutoPtr<ITextView> mClockView;
    AutoPtr<IView> mReserveNotificationSpace;
    AutoPtr<IView> mQsNavbarScrim;
    AutoPtr<IView> mNotificationContainerParent;
    AutoPtr<INotificationStackScrollLayout> mNotificationStackScroller;
    Int32 mNotificationTopPadding;
    Boolean mAnimateNextTopPaddingChange;

    Int32 mTrackingPointer;
    AutoPtr<IVelocityTracker> mVelocityTracker;
    Boolean mQsTracking;

    /**
     * Handles launching the secure camera properly even when other applications may be using the
     * camera hardware.
     */
    AutoPtr<SecureCameraLaunchManager> mSecureCameraLaunchManager;

    /**
     * If set, the ongoing touch gesture might both trigger the expansion in {@link PanelView} and
     * the expansion for quick settings.
     */
    Boolean mConflictingQsExpansionGesture;

    /**
     * Whether we are currently handling a motion gesture in #onInterceptTouchEvent, but haven't
     * intercepted yet.
     */
    Boolean mIntercepting;
    Boolean mQsExpanded;
    Boolean mQsExpandedWhenExpandingStarted;
    Boolean mQsFullyExpanded;
    Boolean mKeyguardShowing;
    Boolean mDozing;
    Int32 mStatusBarState;
    Float mInitialHeightOnTouch;
    Float mInitialTouchX;
    Float mInitialTouchY;
    Float mLastTouchX;
    Float mLastTouchY;
    Float mQsExpansionHeight;
    Int32 mQsMinExpansionHeight;
    Int32 mQsMaxExpansionHeight;
    Int32 mQsPeekHeight;
    Boolean mStackScrollerOverscrolling;
    Boolean mQsExpansionFromOverscroll;
    Float mLastOverscroll;
    Boolean mQsExpansionEnabled;
    AutoPtr<IValueAnimator> mQsExpansionAnimator;
    AutoPtr<FlingAnimationUtils> mFlingAnimationUtils;
    Int32 mStatusBarMinHeight;
    Boolean mUnlockIconActive;
    Int32 mNotificationsHeaderCollideDistance;
    Int32 mUnlockMoveDistance;
    Float mEmptyDragAmount;

    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    AutoPtr<IInterpolator> mFastOutLinearInterpolator;
    AutoPtr<IObjectAnimator> mClockAnimator;
    Int32 mClockAnimationTarget;
    Int32 mTopPaddingAdjustment;
    AutoPtr<KeyguardClockPositionAlgorithm> mClockPositionAlgorithm;
    AutoPtr<KeyguardClockPositionAlgorithm::Result> mClockPositionResult;
    Boolean mIsExpanding;

    Boolean mBlockTouches;
    Int32 mNotificationScrimWaitDistance;
    Boolean mTwoFingerQsExpand;
    Boolean mTwoFingerQsExpandPossible;

    /**
     * If we are in a panel collapsing motion, we reset scrollY of our scroll view but still
     * need to take this into account in our panel height calculation.
     */
    Int32 mScrollYOverride;
    Boolean mQsAnimatorExpand;
    Boolean mIsLaunchTransitionFinished;
    Boolean mIsLaunchTransitionRunning;
    AutoPtr<IRunnable> mLaunchAnimationEndRunnable;
    Boolean mOnlyAffordanceInThisMotion;
    Boolean mKeyguardStatusViewAnimating;
    Boolean mHeaderAnimatingIn;
    AutoPtr<IObjectAnimator> mQsContainerAnimator;

    Boolean mShadeEmpty;

    Boolean mQsScrimEnabled;
    Boolean mLastAnnouncementWasQuickSettings;
    Boolean mQsTouchAboveFalsingThreshold;
    Int32 mQsFalsingThreshold;

    AutoPtr<IRunnable> mAnimateKeyguardStatusViewInvisibleEndRunnable;
    AutoPtr<IRunnable> mAnimateKeyguardStatusViewVisibleEndRunnable;
    AutoPtr<IAnimatorListener> mAnimateHeaderSlidingInListener;
    AutoPtr<IViewOnLayoutChangeListener> mQsContainerAnimatorUpdater;
    AutoPtr<IOnPreDrawListener> mStartHeaderSlidingIn;
    AutoPtr<IRunnable> mAnimateKeyguardStatusBarInvisibleEndRunnable;
    AutoPtr<IRunnable> mAnimateKeyguardBottomAreaInvisibleEndRunnable;
    AutoPtr<IRunnable> mUpdateHeader;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNOTIFICATIONPANELVIEW_H__
