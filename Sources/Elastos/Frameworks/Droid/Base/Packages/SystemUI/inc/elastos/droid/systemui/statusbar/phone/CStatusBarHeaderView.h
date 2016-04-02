
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSTATUSBARHEADERVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSTATUSBARHEADERVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CStatusBarHeaderView.h"
#include "elastos/droid/systemui/statusbar/policy/BatteryController.h"
#include "elastos/droid/systemui/statusbar/policy/NextAlarmController.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/view/ViewOutlineProvider.h>
#include <elastos/droid/widget/RelativeLayout.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlarmClockInfo;
using Elastos::Droid::SystemUI::StatusBar::Policy::BatteryController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBatteryController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBatteryStateChangeCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::INextAlarmController;
using Elastos::Droid::SystemUI::StatusBar::Policy::INextAlarmChangeCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::IUserInfoController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IUserInfoControllerOnUserInfoChangedListener;
using Elastos::Droid::SystemUI::StatusBar::Policy::NextAlarmController;
using Elastos::Droid::SystemUI::Qs::IQSTileDetailAdapter;
using Elastos::Droid::SystemUI::Qs::IQSPanel;
using Elastos::Droid::SystemUI::Qs::IQSPanelCallback;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::View::ViewOutlineProvider;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::RelativeLayout;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * The view to manage the header area in the expanded status bar.
 */
CarClass(CStatusBarHeaderView)
    , public RelativeLayout
    , public IStatusBarHeaderView
    , public IViewOnClickListener
    , public IBatteryStateChangeCallback
    , public INextAlarmChangeCallback
{
private:
    /**
     * Captures all layout values (position, visibility) for a certain state. This is used for
     * animations.
     */
    class LayoutValues: public Object
    {
    public:
        LayoutValues();

        CARAPI_(void) Interpoloate(
            /* [in] */ LayoutValues* v1,
            /* [in] */ LayoutValues* v2,
            /* [in] */ Float t);

    public:
        Float mDateExpandedAlpha;
        Float mDateCollapsedAlpha;
        Float mEmergencyCallsOnlyAlpha;
        Float mAlarmStatusAlpha;
        Float mTimeScale;
        Float mClockY;
        Float mDateY;
        Float mAvatarScale;
        Float mAvatarX;
        Float mAvatarY;
        Float mBatteryX;
        Float mBatteryY;
        Float mBatteryLevelAlpha;
        Float mSettingsAlpha;
        Float mSettingsTranslation;
        Float mSignalClusterAlpha;
        Float mSettingsRotation;
    };

    class QsPanelCallback
        : public Object
        , public IQSPanelCallback
    {
    public:
        CAR_INTERFACE_DECL();

        QsPanelCallback(
            /* [in] */ CStatusBarHeaderView* host);

        // @Override
        CARAPI OnToggleStateChanged(
            /* [in] */ Boolean state);

        // @Override
        CARAPI OnShowingDetail(
            /* [in] */ IQSTileDetailAdapter* detail);

        // @Override
        CARAPI OnScanStateChanged(
            /* [in] */ Boolean state);

        CARAPI_(void) HandleToggleStateChanged(
            /* [in] */ Boolean state);

        CARAPI_(void) HandleScanStateChanged(
            /* [in] */ Boolean state);

        CARAPI_(void) HandleShowingDetail(
            /* [in] */ IQSTileDetailAdapter* detail);

        CARAPI_(void) Transition(
            /* [in] */ IView* v,
            /* [in] */ Boolean in);

    private:
        Boolean mScanState;
        CStatusBarHeaderView* mHost;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ QsPanelCallback* host,
            /* [in] */ Boolean state);

        // @Override
        CARAPI Run();

    private:
        QsPanelCallback* mHost;
        Boolean mState;
    };

    class Runnable2: public Runnable
    {
    public:
        Runnable2(
            /* [in] */ QsPanelCallback* host,
            /* [in] */ IQSTileDetailAdapter* detail);

        // @Override
        CARAPI Run();

    private:
        QsPanelCallback* mHost;
        AutoPtr<IQSTileDetailAdapter> mDetail;
    };

    class Runnable3: public Runnable
    {
    public:
        Runnable3(
            /* [in] */ QsPanelCallback* host,
            /* [in] */ Boolean state);

        // @Override
        CARAPI Run();

    private:
        QsPanelCallback* mHost;
        Boolean mState;
    };

    class ClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        ClickListener(
            /* [in] */ CStatusBarHeaderView* host,
            /* [in] */ IQSTileDetailAdapter* detail);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CStatusBarHeaderView* mHost;
        AutoPtr<IQSTileDetailAdapter> mDetail;
    };

    class Runnable4: public Runnable
    {
    public:
        Runnable4(
            /* [in] */ IView* v,
            /* [in] */ Boolean state);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IView> mView;
        Boolean mIn;
    };

    class ClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        ClickListener2(
            /* [in] */ CStatusBarHeaderView* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CStatusBarHeaderView* mHost;
    };

    class OnLayoutChangeListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnLayoutChangeListener(
            /* [in] */ CStatusBarHeaderView* host);

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
        CStatusBarHeaderView* mHost;
    };

    class OutlineProvider: public ViewOutlineProvider
    {
    public:
        OutlineProvider(
            /* [in] */ CStatusBarHeaderView* host);

        // @Override
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);

    private:
        CStatusBarHeaderView* mHost;
    };

    class OnUserInfoChangedListener
        : public Object
        , public IUserInfoControllerOnUserInfoChangedListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnUserInfoChangedListener(
            /* [in] */ CStatusBarHeaderView* host);

        // @Override
        CARAPI OnUserInfoChanged(
            /* [in] */ const String& name,
            /* [in] */ IDrawable* picture);

    private:
        CStatusBarHeaderView* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CStatusBarHeaderView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetActivityStarter(
        /* [in] */ IActivityStarter* activityStarter);

    CARAPI SetBatteryController(
        /* [in] */ IBatteryController* batteryController);

    CARAPI SetNextAlarmController(
        /* [in] */ INextAlarmController* nextAlarmController);

    CARAPI GetCollapsedHeight(
        /* [out] */ Int32* result);

    CARAPI GetExpandedHeight(
        /* [out] */ Int32* result);

    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI SetExpanded(
        /* [in] */ Boolean expanded);

    CARAPI UpdateEverything();

    // @Override
    CARAPI OnBatteryLevelChanged(
        /* [in] */ Int32 level,
        /* [in] */ Boolean pluggedIn,
        /* [in] */ Boolean charging);

    // @Override
    CARAPI OnPowerSaveChanged();

    // @Override
    CARAPI OnNextAlarmChanged(
        /* [in] */ IAlarmClockInfo* nextAlarm);

    CARAPI SetUserInfoController(
        /* [in] */ IUserInfoController* userInfoController);

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI SetExpansion(
        /* [in] */ Float t);

    CARAPI SetQSPanel(
        /* [in] */ IQSPanel* qsp);

    // @Override
    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    CARAPI SetShowEmergencyCallsOnly(
        /* [in] */ Boolean show);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI_(void) DispatchSetPressed(
        /* [in] */ Boolean pressed);

private:
    CARAPI_(void) UpdateClockCollapsedMargin();

    CARAPI_(void) RequestCaptureValues();

    CARAPI_(void) LoadDimens();

    CARAPI_(void) UpdateHeights();

    CARAPI_(void) UpdateVisibilities();

    CARAPI_(void) UpdateSignalClusterDetachment();

    CARAPI_(void) ReattachSignalCluster();

    CARAPI_(void) UpdateSystemIconsLayoutParams();

    CARAPI_(void) UpdateListeners();

    CARAPI_(void) UpdateAvatarScale();

    CARAPI_(void) UpdateClockScale();

    CARAPI_(void) UpdateAmPmTranslation();

    CARAPI_(void) UpdateClickTargets();

    CARAPI_(void) UpdateClockLp();

    CARAPI_(void) UpdateMultiUserSwitch();

    CARAPI_(void) UpdateLayoutValues(
        /* [in] */ Float t);

    CARAPI_(void) SetClipping(
        /* [in] */ Float height);

    CARAPI_(void) StartSettingsActivity();

    CARAPI_(void) StartBatteryActivity();

    CARAPI_(void) CaptureLayoutValues(
        /* [in] */ LayoutValues* target);

    CARAPI_(Float) GetAlphaForVisibility(
        /* [in] */ IView* v);

    CARAPI_(void) ApplyAlpha(
        /* [in] */ IView* v,
        /* [in] */ Float alpha);

    CARAPI_(void) ApplyLayoutValues(
        /* [in] */ LayoutValues* values);

private:
    Boolean mExpanded;
    Boolean mListening;

    AutoPtr<IViewGroup> mSystemIconsContainer;
    AutoPtr<IView> mSystemIconsSuperContainer;
    AutoPtr<IView> mDateGroup;
    AutoPtr<IView> mClock;
    AutoPtr<ITextView> mTime;
    AutoPtr<ITextView> mAmPm;
    AutoPtr<IMultiUserSwitch> mMultiUserSwitch;
    AutoPtr<IImageView> mMultiUserAvatar;
    AutoPtr<ITextView> mDateCollapsed;
    AutoPtr<ITextView> mDateExpanded;
    AutoPtr<ILinearLayout> mSystemIcons;
    AutoPtr<IView> mSignalCluster;
    AutoPtr<IView> mSettingsButton;
    AutoPtr<IView> mQsDetailHeader;
    AutoPtr<ITextView> mQsDetailHeaderTitle;
    AutoPtr<ISwitch> mQsDetailHeaderSwitch;
    AutoPtr<IImageView> mQsDetailHeaderProgress;
    AutoPtr<ITextView> mEmergencyCallsOnly;
    AutoPtr<ITextView> mBatteryLevel;
    AutoPtr<ITextView> mAlarmStatus;

    Boolean mShowEmergencyCallsOnly;
    Boolean mAlarmShowing;
    AutoPtr<IAlarmClockInfo> mNextAlarm;

    Int32 mCollapsedHeight;
    Int32 mExpandedHeight;

    Int32 mMultiUserExpandedMargin;
    Int32 mMultiUserCollapsedMargin;

    Int32 mClockMarginBottomExpanded;
    Int32 mClockMarginBottomCollapsed;
    Int32 mMultiUserSwitchWidthCollapsed;
    Int32 mMultiUserSwitchWidthExpanded;

    Int32 mClockCollapsedSize;
    Int32 mClockExpandedSize;

    /**
     * In collapsed QS, the clock and avatar are scaled down a bit post-layout to allow for a nice
     * transition. These values determine that factor.
     */
    Float mClockCollapsedScaleFactor;
    Float mAvatarCollapsedScaleFactor;

    AutoPtr<IActivityStarter> mActivityStarter;
    AutoPtr<IBatteryController> mBatteryController;
    AutoPtr<INextAlarmController> mNextAlarmController;
    AutoPtr<IQSPanel> mQSPanel;


    AutoPtr<IRect> mClipBounds;

    Boolean mCaptureValues;
    Boolean mSignalClusterDetached;
    AutoPtr<LayoutValues> mCollapsedValues;
    AutoPtr<LayoutValues> mExpandedValues;
    AutoPtr<LayoutValues> mCurrentValues;

    Float mCurrentT;
    Boolean mShowingDetail;
    AutoPtr<IQSPanelCallback> mQsPanelCallback;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSTATUSBARHEADERVIEW_H__
