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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDHOSTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDHOSTVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardHostView.h"
#include "elastos/droid/systemui/keyguard/KeyguardViewBase.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"
#include "elastos/droid/widget/RemoteViews.h"
#include "elastos/droid/view/View.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::AppWidget::IAppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::RemoteViews;
using Elastos::Droid::Widget::IRemoteViewsOnClickHandler;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardHostView)
    , public KeyguardViewBase
    , public IKeyguardHostView
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardHostView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CKeyguardHostView* mHost;
    };

    class MyOnDismissAction
        : public Object
        , public IKeyguardHostViewOnDismissAction
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyOnDismissAction")

        CAR_INTERFACE_DECL()

        MyOnDismissAction(
            /* [in] */ IView* view,
            /* [in] */ IPendingIntent* pendingIntent,
            /* [in] */ IIntent* fillInIntent)
            : mView(view)
            , mPendingIntent(pendingIntent)
            , mFillInIntent(fillInIntent)
        {}

        CARAPI OnDismiss(
            /* [out] */ Boolean* succeed);

    private:
        AutoPtr<IView> mView;
        AutoPtr<IPendingIntent> mPendingIntent;
        AutoPtr<IIntent> mFillInIntent;
    };

    class MyOnClickHandler
        : public RemoteViews::RemoteViewsOnClickHandler
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyOnClickHandler")

        MyOnClickHandler(
            /* [in] */ CKeyguardHostView* host);

        CARAPI OnClickHandler(
            /* [in] */ IView* view,
            /* [in] */ IPendingIntent* pending,
            /* [in] */ IIntent* fillInIntent,
            /* [out] */ Boolean* succeed);

    private:
        CKeyguardHostView* mHost;

        // weak reference to the hostView to avoid keeping a live reference
        // due to Binder GC linkages to AppWidgetHost. By the same token,
        // this click handler should not keep references to any large
        // objects.
        AutoPtr<IWeakReference> mKeyguardHostView;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CKeyguardHostView* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CKeyguardHostView* mHost;
    };

    class MyControlCallback
        : public Object
        , public IKeyguardHostViewTransportControlCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyControlCallback(
            /* [in] */ CKeyguardHostView* host)
            : mHost(host)
        {}

        CARAPI UserActivity();

    private:
        CKeyguardHostView* mHost;
    };

    class SavedState
        : public View::BaseSavedState
        , public IKeyguardHostViewSavedState
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::SavedState")

        CAR_INTERFACE_DECL()

        SavedState();

        CARAPI constructor(
            /* [in] */ IParcelable* superState);

        //@Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

    private:
        CARAPI constructor(
            /* [in] */ IParcel* in);

    public:
        Int32 mTransportState;
        Int32 mAppWidgetToShow;
        AutoPtr<IRect> mInsets;

        // public static final Parcelable.Creator<SavedState> CREATOR
        //         = new Parcelable.Creator<SavedState>() {
        //     public SavedState createFromParcel(Parcel in) {
        //         return new SavedState(in);
        //     }

        //     public SavedState[] newArray(int size) {
        //         return new SavedState[size];
        //     }
        // };
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyRunnable2")

        MyRunnable2(
            /* [in] */ CKeyguardHostView* host,
            /* [in] */ Int32 pageToShow)
            : mHost(host)
            , mPageToShow(pageToShow)
        {}

        //@Override
        CARAPI Run();

    private:
        CKeyguardHostView* mHost;
        Int32 mPageToShow;
    };

    class MyUserSwitcherCallback
        : public Object
        , public IKeyguardHostViewUserSwitcherCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyUserSwitcherCallback")

        CAR_INTERFACE_DECL()

        MyUserSwitcherCallback(
            /* [in] */ CKeyguardHostView* host)
            : mHost(host)
        {}

        CARAPI HideSecurityView(
           /* [in] */ Int32 duration);

        CARAPI ShowSecurityView();

        CARAPI ShowUnlockHint();

        CARAPI UserActivity();

    private:
        CKeyguardHostView* mHost;
    };

    class MyKeyguardWidgetPagerCallbacks
        : public Object
        , public IKeyguardWidgetPagerCallbacks
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyKeyguardWidgetPagerCallbacks")

        CAR_INTERFACE_DECL()

        MyKeyguardWidgetPagerCallbacks(
            /* [in] */ CKeyguardHostView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI UserActivity();

        //@Override
        CARAPI OnUserActivityTimeoutChanged();

        //@Override
        CARAPI OnAddView(
            /* [in] */ IView* v);

        //@Override
        CARAPI OnRemoveView(
            /* [in] */ IView* v,
            /* [in] */ Boolean deletePermanently);

        //@Override
        CARAPI OnRemoveViewAnimationCompleted();

    private:
        CKeyguardHostView* mHost;
    };

    class MyCameraWidgetFrameCallbacks
        : public Object
        , public ICameraWidgetFrameCallbacks
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyCameraWidgetFrameCallbacks")

        CAR_INTERFACE_DECL()

        MyCameraWidgetFrameCallbacks(
            /* [in] */ CKeyguardHostView* host)
            : mHost(host)
        {}

        CARAPI OnLaunchingCamera();

        CARAPI OnCameraLaunchedSuccessfully();

        CARAPI OnCameraLaunchedUnsuccessfully();

    private:
        CARAPI_(void) SetSliderHandleAlpha(
            /* [in] */ Float alpha);

    private:
        CKeyguardHostView* mHost;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyRunnable3")

        MyRunnable3(
            /* [in] */ CKeyguardHostView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CKeyguardHostView* mHost;
    };

    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardHostView::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardHostView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnBootCompleted();

        //@Override
        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 userId);

    private:
        CKeyguardHostView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardHostView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI Dismiss(
        /* [in] */ Boolean authenticated,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetLockPatternUtils(
        /* [in] */ ILockPatternUtils* utils);

    CARAPI AddWidget(
        /* [in] */ IAppWidgetHostView* view,
        /* [in] */ int pageIndex);

    //@Override
    CARAPI OnUserSwitching(
        /* [in] */ Boolean switching);

    CARAPI UserActivity();

    CARAPI OnUserActivityTimeoutChanged();

    //@Override
    CARAPI GetUserActivityTimeout(
        /* [in] */ Int64* time);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    CARAPI ClearAppWidgetToShow();

    CARAPI CheckAppWidgetConsistency();

    //@Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    //@Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    CARAPI IsMusicPage(
        /* [in] */ Int32 pageIndex,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI CleanUp();

    CARAPI GoToWidget(
        /* [in] */ Int32 appWidgetId);

    //@Override
    CARAPI OnExternalMotionEvent(
        /* [in] */ IMotionEvent* event);

protected:
    //@Override
    CARAPI OnFinishInflate();

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI FitSystemWindows(
        /* [in] */ IRect* insets,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ShowBouncer(
        /* [in] */ Boolean show);

    //@Override
    CARAPI OnCreateOptions(
        /* [in] */ IBundle* options);

private:
    CARAPI_(void) GetInitialTransportState();

    CARAPI_(void) CleanupAppWidgetIds();

    static CARAPI_(Boolean) Contains(
        /* [in] */ ArrayOf<Int32>* array,
        /* [in] */ Int32 target);

    static CARAPI_(Boolean) IsMusicPlaying(
        /* [in] */ Int32 playbackState);

    CARAPI_(Int32) GetWidgetPosition(
        /* [in] */ Int32 id);

    CARAPI_(void) UpdateAndAddWidgets();

    CARAPI_(void) MaybeEnableAddButton();

    CARAPI_(Boolean) ShouldEnableAddWidget();

    CARAPI_(Int32) GetDisabledFeatures(
        /* [in] */ IDevicePolicyManager* dpm);

    CARAPI_(Boolean) WidgetsDisabled();

    CARAPI_(Boolean) CameraDisabledByDpm();

    CARAPI_(Boolean) AddWidget(
        /* [in] */ Int32 appId,
        /* [in] */ Int32 pageIndex,
        /* [in] */ Boolean updateDbIfFailed);

    CARAPI_(Int32) NumWidgets();

    CARAPI_(void) AddDefaultWidgets();

    /**
     * Create KeyguardTransportControlView on demand.
     * @return
     */
    CARAPI_(AutoPtr<IKeyguardTransportControlView>) GetOrCreateTransportControl();

    CARAPI_(Int32) GetInsertPageIndex();

    CARAPI_(void) AddDefaultStatusWidget(
        /* [in] */ Int32 index);

    CARAPI_(void) AddWidgetsFromSettings();

    CARAPI_(Int32) AllocateIdForDefaultAppWidget();

    CARAPI_(void) SetInsets(
        /* [in] */ IRect* insets);

    CARAPI_(void) ShowAppropriateWidgetPage();

    /**
     * Examines the current state and adds the transport to the widget pager when the state changes.
     *
     * Showing the initial transport and keeping it around is a bit tricky because the signals
     * coming from music players aren't always clear. Here's how the states are handled:
     *
     * {@link TRANSPORT_GONE} means we have no reason to show the transport - remove it if present.
     *
     * {@link TRANSPORT_INVISIBLE} means we have potential to show the transport because a music
     * player is registered but not currently playing music (or we don't know the state yet). The
     * code adds it conditionally on play state.
     *
     * {@link #TRANSPORT_VISIBLE} means a music player is active and transport should be showing.
     *
     * Once the transport is showing, we always show it until keyguard is dismissed. This state is
     * maintained by onSave/RestoreInstanceState(). This state is cleared in
     * {@link KeyguardViewManager#hide} when keyguard is dismissed, which causes the transport to be
     * gone when keyguard is restarted until we get an update with the current state.
     *
     * @param state
     */
    CARAPI_(Boolean) EnsureTransportPresentOrRemoved(
        /* [in] */ Int32 state);

    CARAPI_(AutoPtr<ICameraWidgetFrame>) FindCameraPage();

    CARAPI_(Int32) GetAppropriateWidgetPage(
        /* [in] */ Int32 musicTransportState);

    CARAPI_(void) EnableUserSelectorIfNecessary();

public:
    static Boolean DEBUG;
    static Boolean DEBUGXPORT; // debug music transport control

    // Transport control states.
    static const Int32 TRANSPORT_GONE;
    static const Int32 TRANSPORT_INVISIBLE;
    static const Int32 TRANSPORT_VISIBLE;

    // Found in KeyguardAppWidgetPickActivity.java
    static const Int32 APPWIDGET_HOST_ID;

protected:
    Int32 mFailedAttempts;

    Int32 mClientGeneration;

    Boolean mShowSecurityWhenReturn;

private:
    static const String TAG;

    Int32 mTransportState;

    const Int32 MAX_WIDGETS;

    AutoPtr<IAppWidgetHost> mAppWidgetHost;
    AutoPtr<IAppWidgetManager> mAppWidgetManager;
    AutoPtr<IKeyguardWidgetPager> mAppWidgetContainer;
    // TODO remove transport control references, these don't exist anymore
    AutoPtr<IKeyguardTransportControlView> mTransportControl;
    Int32 mAppWidgetToShow;

    AutoPtr<IKeyguardViewStateManager> mViewStateManager;

    AutoPtr<IRect> mTempRect;
    Int32 mDisabledFeatures;
    Boolean mCameraDisabled;
    Boolean mSafeModeEnabled;
    Boolean mUserSetupCompleted;

    // User for whom this host view was created.  Final because we should never change the
    // id without reconstructing an instance of KeyguardHostView. See note below...
    Int32 mUserId;

    AutoPtr<IKeyguardMultiUserSelectorView> mKeyguardMultiUserSelectorView;

    AutoPtr<IRect> mInsets;

    AutoPtr<IRemoteViewsOnClickHandler> mOnClickHandler;

    AutoPtr<IRunnable> mPostBootCompletedRunnable;

    AutoPtr<MyKeyguardUpdateMonitorCallback> mUpdateMonitorCallbacks;

    AutoPtr<ISlidingChallengeLayout> mSlidingChallengeLayout;
    AutoPtr<IMultiPaneChallengeLayout> mMultiPaneChallengeLayout;

    AutoPtr<IKeyguardWidgetPagerCallbacks> mWidgetCallbacks;

    AutoPtr<ICameraWidgetFrameCallbacks> mCameraWidgetCallbacks;

    AutoPtr<IRunnable> mSwitchPageRunnable;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDHOSTVIEW_H__
