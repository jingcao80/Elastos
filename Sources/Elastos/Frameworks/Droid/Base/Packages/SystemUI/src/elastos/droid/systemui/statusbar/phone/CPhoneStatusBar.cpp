
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBarHeadsUpObserver.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBarUserSetupObserver.h"
#include "elastos/droid/systemui/statusbar/phone/CFastColorDrawable.h"
#include "elastos/droid/systemui/statusbar/phone/CNotificationPanelView.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBarReceiver.h"
#include "elastos/droid/systemui/statusbar/phone/BarTransitions.h"
#include "elastos/droid/systemui/statusbar/phone/CStatusBarWindowView.h"
#include "elastos/droid/systemui/statusbar/phone/QSTileHost.h"
#include "elastos/droid/systemui/statusbar/policy/AccessibilityController.h"
#include "elastos/droid/systemui/statusbar/policy/KeyguardUserSwitcher.h"
#include "elastos/droid/systemui/statusbar/policy/PreviewInflater.h"
#include "elastos/droid/systemui/statusbar/stack/StackScrollState.h"
#include "elastos/droid/systemui/statusbar/CStatusBarIconView.h"
#include "elastos/droid/systemui/doze/DozeLog.h"
#include "elastos/droid/systemui/FontSizeUtils.h"
#include "../R.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.InputMethodService.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/R.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::CStatusBarManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IStatusBarManagerHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::InputMethodService::IInputMethodService;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::Session::CMediaController;
using Elastos::Droid::Media::Session::IMediaSessionToken;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_OVER;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::EIID_IViewOnFocusChangeListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IGLES20CanvasHelper;
using Elastos::Droid::View::CGLES20CanvasHelper;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::Animation::CPathInterpolator;
using Elastos::Droid::View::Animation::EIID_IAnimationAnimationListener;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::SystemUI::Keyguard::EIID_IKeyguardHostViewOnDismissAction;
using Elastos::Droid::SystemUI::FontSizeUtils;
using Elastos::Droid::SystemUI::Doze::DozeLog;
using Elastos::Droid::SystemUI::Doze::EIID_IDozeHost;
using Elastos::Droid::SystemUI::Keyguard::EIID_IKeyguardViewMediator;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardViewMediator;
using Elastos::Droid::SystemUI::Qs::EIID_IQSTileHostCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::AccessibilityController;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IBatteryStateChangeCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyButtonView;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkControllerImplSignalCluster;
using Elastos::Droid::SystemUI::StatusBar::Policy::KeyguardUserSwitcher;
using Elastos::Droid::SystemUI::StatusBar::Policy::PreviewInflater;
using Elastos::Droid::SystemUI::StatusBar::Stack::EIID_IOnChildLocationsChangedListener;
using Elastos::Droid::SystemUI::StatusBar::Stack::StackScrollState;
using Elastos::Droid::SystemUI::StatusBar::Stack::IStackScrollAlgorithm;
using Elastos::Droid::SystemUI::Volume::EIID_IVolumeComponent;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

static AutoPtr<IInterpolator> InitInterpolator(
    /* [in] */ Float f1,
    /* [in] */ Float f2,
    /* [in] */ Float f3,
    /* [in] */ Float f4)
{
    AutoPtr<IInterpolator> obj;
    CPathInterpolator::New(f1, f2, f3, f4, (IInterpolator**)&obj);
    return obj;
}

static AutoPtr<IAudioAttributes> InitAudioAttributes()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    AutoPtr<IAudioAttributes> attrs;
    builder->Build((IAudioAttributes**)&attrs);
    return attrs;
}

static const String TAG("CPhoneStatusBar");
const Boolean CPhoneStatusBar::DEBUG = FALSE;
const Boolean CPhoneStatusBar::SPEW = FALSE;
const Boolean CPhoneStatusBar::DUMPTRUCK = TRUE; // extra dumpsys info
const Boolean CPhoneStatusBar::DEBUG_GESTURES = FALSE;
const Boolean CPhoneStatusBar::DEBUG_MEDIA = FALSE;
const Boolean CPhoneStatusBar::DEBUG_MEDIA_FAKE_ARTWORK = FALSE;
const Boolean CPhoneStatusBar::DEBUG_WINDOW_STATE = FALSE;
const Boolean CPhoneStatusBar::CHATTY = DEBUG;
const String CPhoneStatusBar::ACTION_STATUSBAR_START("com.android.internal.policy.statusbar.START");
const Boolean CPhoneStatusBar::SHOW_LOCKSCREEN_MEDIA_ARTWORK = TRUE;
const Int32 CPhoneStatusBar::FADE_KEYGUARD_START_DELAY = 100;
const Int32 CPhoneStatusBar::FADE_KEYGUARD_DURATION = 300;
AutoPtr<IInterpolator> CPhoneStatusBar::ALPHA_IN = InitInterpolator(0.4f, 0.f, 1.f, 1.f);
AutoPtr<IInterpolator> CPhoneStatusBar::ALPHA_OUT = InitInterpolator(0.f, 0.f, 0.8f, 1.f);
const Int32 CPhoneStatusBar::MSG_OPEN_NOTIFICATION_PANEL = 1000;
const Int32 CPhoneStatusBar::MSG_CLOSE_PANELS = 1001;
const Int32 CPhoneStatusBar::MSG_OPEN_SETTINGS_PANEL = 1002;
const Boolean CPhoneStatusBar::CLOSE_PANEL_WHEN_EMPTIED = TRUE;
const Int32 CPhoneStatusBar::NOTIFICATION_PRIORITY_MULTIPLIER = 10;
const Int32 CPhoneStatusBar::HIDE_ICONS_BELOW_SCORE = INotification::PRIORITY_LOW * NOTIFICATION_PRIORITY_MULTIPLIER;
const Int32 CPhoneStatusBar::STATUS_OR_NAV_TRANSIENT = IView::STATUS_BAR_TRANSIENT | IView::NAVIGATION_BAR_TRANSIENT;
const Int64 CPhoneStatusBar::AUTOHIDE_TIMEOUT_MS = 3000;
const Int32 CPhoneStatusBar::VISIBILITY_REPORT_MIN_DELAY_MS = 500;
const Int32 CPhoneStatusBar::HINT_RESET_DELAY_MS = 1200;
const Int32 CPhoneStatusBar::LOCK_TO_APP_GESTURE_TOLERENCE = 200;
AutoPtr<IAudioAttributes> CPhoneStatusBar::VIBRATION_ATTRIBUTES = InitAudioAttributes();
const Int32 CPhoneStatusBar::VISIBLE_LOCATIONS = StackScrollState::ViewState::LOCATION_FIRST_CARD
            | StackScrollState::ViewState::LOCATION_TOP_STACK_PEEKING
            | StackScrollState::ViewState::LOCATION_MAIN_AREA
            | StackScrollState::ViewState::LOCATION_BOTTOM_STACK_PEEKING;

//=================================================================================
// CPhoneStatusBarUserSetupObserver
//=================================================================================
CAR_OBJECT_IMPL(CPhoneStatusBarUserSetupObserver)

ECode CPhoneStatusBarUserSetupObserver::constructor(
    /* [in] */ IPhoneStatusBar* host,
    /* [in] */ IHandler* handler)
{
    mHost = (CPhoneStatusBar*)host;
    return ContentObserver::constructor(handler);
}

ECode CPhoneStatusBarUserSetupObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 v = 0;
    Elastos::Droid::Provider::Settings::Secure::GetInt32ForUser(cr,
            ISettingsSecure::USER_SETUP_COMPLETE,
            0 /*default */,
            mHost->mCurrentUserId, &v);
    Boolean userSetup = 0 != v;
    userSetup = TRUE;
    if (CPhoneStatusBar::MULTIUSER_DEBUG) {
        Logger::D(TAG, "User setup changed: selfChange=%d userSetup=%d mUserSetup=%d",
            selfChange, userSetup, mHost->mUserSetup);
    }

    if (userSetup != mHost->mUserSetup) {
        mHost->mUserSetup = userSetup;
        if (!mHost->mUserSetup && mHost->mStatusBarView != NULL) {
            mHost->AnimateCollapseQuickSettings();
        }
    }
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBarHeadsUpObserver
//=================================================================================
CAR_OBJECT_IMPL(CPhoneStatusBarHeadsUpObserver)

ECode CPhoneStatusBarHeadsUpObserver::constructor(
    /* [in] */ IPhoneStatusBar* host,
    /* [in] */ IHandler* handler)
{
    mHost = (CPhoneStatusBar*)host;
    return ContentObserver::constructor(handler);
}

ECode CPhoneStatusBarHeadsUpObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Boolean wasUsing = mHost->mUseHeadsUp;
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 v = 0;
    Elastos::Droid::Provider::Settings::Global::GetInt32(
            cr, ISettingsGlobal::HEADS_UP_NOTIFICATIONS_ENABLED,
            ISettingsGlobal::HEADS_UP_OFF, &v);
    mHost->mUseHeadsUp = CPhoneStatusBar::ENABLE_HEADS_UP && !mHost->mDisableNotificationAlerts
            && ISettingsGlobal::HEADS_UP_OFF != v;

    Elastos::Droid::Provider::Settings::Global::GetInt32(
            cr, CPhoneStatusBar::SETTING_HEADS_UP_TICKER, 0, &v);
    mHost->mHeadsUpTicker = mHost->mUseHeadsUp && 0 != v;
    Logger::D(TAG, "heads up is %s", (mHost->mUseHeadsUp ? "enabled" : "disabled"));
    if (wasUsing != mHost->mUseHeadsUp) {
        if (!mHost->mUseHeadsUp) {
            Logger::D(TAG, "dismissing any existing heads up notification on disable event");
            mHost->SetHeadsUpVisibility(FALSE);
            mHost->mHeadsUpNotificationView->ReleaseResources();
            mHost->RemoveHeadsUpView();
        }
        else {
            mHost->AddHeadsUpView();
        }
    }
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::Autohide
//=================================================================================
CPhoneStatusBar::Autohide::Autohide(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::Autohide::Run()
{
    Int32 requested = mHost->mSystemUiVisibility & ~STATUS_OR_NAV_TRANSIENT;
    if (mHost->mSystemUiVisibility != requested) {
        mHost->NotifyUiVisibilityChanged(requested);
    }
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::MediaListener
//=================================================================================
CPhoneStatusBar::MediaListener::MediaListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::MediaListener::OnPlaybackStateChanged(
    /* [in] */ IPlaybackState* state)
{
    MediaControllerCallback::OnPlaybackStateChanged(state);
    if (DEBUG_MEDIA) Logger::V(TAG, "DEBUG_MEDIA: onPlaybackStateChanged: %s", TO_CSTR(state));
    return NOERROR;
}

ECode CPhoneStatusBar::MediaListener::OnMetadataChanged(
    /* [in] */ IMediaMetadata* metadata)
{
    MediaControllerCallback::OnMetadataChanged(metadata);
    if (DEBUG_MEDIA) Logger::V(TAG, "DEBUG_MEDIA: onMetadataChanged: %s", TO_CSTR(metadata));
    mHost->mMediaMetadata = metadata;
    mHost->UpdateMediaMetaData(TRUE);
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::OnChildLocationsChangedListener
//=================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::OnChildLocationsChangedListener, Object, IOnChildLocationsChangedListener)
CPhoneStatusBar::OnChildLocationsChangedListener::OnChildLocationsChangedListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::OnChildLocationsChangedListener::OnChildLocationsChanged(
    /* [in] */ INotificationStackScrollLayout* stackScrollLayout)
{
    mHost->UserActivity();
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::NotificationLocationsChangedListener
//=================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::NotificationLocationsChangedListener, Object, IOnChildLocationsChangedListener)

CPhoneStatusBar::NotificationLocationsChangedListener::NotificationLocationsChangedListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::NotificationLocationsChangedListener::OnChildLocationsChanged(
    /* [in] */ INotificationStackScrollLayout* stackScrollLayout)
{
    Boolean tmp = FALSE;
    if (mHost->mHandler->HasCallbacks(mHost->mVisibilityReporter, &tmp), tmp) {
        // Visibilities will be reported when the existing
        // callback is executed.
        return NOERROR;
    }
    // Calculate when we're allowed to run the visibility
    // reporter. Note that this timestamp might already have
    // passed. That's OK, the callback will just be executed
    // ASAP.
    Int64 nextReportUptimeMs = mHost->mLastVisibilityReportUptimeMs + VISIBILITY_REPORT_MIN_DELAY_MS;
    mHost->mHandler->PostAtTime(mHost->mVisibilityReporter, nextReportUptimeMs, &tmp);
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::VisibilityReporter
//=================================================================================
CPhoneStatusBar::VisibilityReporter::VisibilityReporter(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{
    CArrayList::New((IArrayList**)&mTmpNewlyVisibleNotifications);
    CArrayList::New((IArrayList**)&mTmpCurrentlyVisibleNotifications);
}

ECode CPhoneStatusBar::VisibilityReporter::Run()
{
    mHost->mLastVisibilityReportUptimeMs = SystemClock::GetUptimeMillis();

    // 1. Loop over mNotificationData entries:
    //   A. Keep list of visible notifications.
    //   B. Keep list of previously hidden, now visible notifications.
    // 2. Compute no-longer visible notifications by removing currently
    //    visible notifications from the set of previously visible
    //    notifications.
    // 3. Report newly visible and no-longer visible notifications.
    // 4. Keep currently visible notifications for next report.
    AutoPtr<IArrayList> activeNotifications;  /*<INotificationDataEntry*/
    mHost->mNotificationData->GetActiveNotifications((IArrayList**)&activeNotifications);
    Int32 N = 0;
    activeNotifications->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        activeNotifications->Get(i, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> entry = INotificationDataEntry::Probe(obj);
        AutoPtr<IStatusBarNotification> notification;
        entry->GetNotification((IStatusBarNotification**)&notification);
        String key;
        notification->GetKey(&key);
        Boolean previouslyVisible = FALSE;
        AutoPtr<ICharSequence> k;
        CString::New(key, (ICharSequence**)&k);
        mHost->mCurrentlyVisibleNotifications->Contains(k, &previouslyVisible);

        AutoPtr<IExpandableNotificationRow> row;
        entry->GetRow((IExpandableNotificationRow**)&row);
        Int32 v = 0;
        mHost->mStackScroller->GetChildLocation(IView::Probe(row), &v);
        Boolean currentlyVisible = (v & VISIBLE_LOCATIONS) != 0;
        if (currentlyVisible) {
            // Build new set of visible notifications.
            mTmpCurrentlyVisibleNotifications->Add(k);
        }
        if (!previouslyVisible && currentlyVisible) {
            mTmpNewlyVisibleNotifications->Add(k);
        }
    }
    AutoPtr<IArraySet> noLongerVisibleNotifications = mHost->mCurrentlyVisibleNotifications;  /*<String*/
    noLongerVisibleNotifications->RemoveAll(ICollection::Probe(mTmpCurrentlyVisibleNotifications));

    mHost->LogNotificationVisibilityChanges(ICollection::Probe(mTmpNewlyVisibleNotifications),
        ICollection::Probe(noLongerVisibleNotifications));

    mHost->mCurrentlyVisibleNotifications->Clear();
    mHost->mCurrentlyVisibleNotifications->AddAll(ICollection::Probe(mTmpCurrentlyVisibleNotifications));

    mTmpNewlyVisibleNotifications->Clear();
    mTmpCurrentlyVisibleNotifications->Clear();
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::OverflowClickListener
//=================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::OverflowClickListener, Object, IViewOnClickListener)

CPhoneStatusBar::OverflowClickListener::OverflowClickListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::OverflowClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->GoToLockedShade(NULL);
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::RecentsClickListener
//=================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::RecentsClickListener, Object, IViewOnClickListener)

CPhoneStatusBar::RecentsClickListener::RecentsClickListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::RecentsClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->AwakenDreams();
    mHost->ToggleRecentApps();
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::LongPressBackRecentsListener
//=================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::LongPressBackRecentsListener, Object, IViewOnLongClickListener)

CPhoneStatusBar::LongPressBackRecentsListener::LongPressBackRecentsListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::LongPressBackRecentsListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->HandleLongPressBackRecents(v);
    *result = TRUE;
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::ShowSearchPanelRunnable
//=================================================================================
CPhoneStatusBar::ShowSearchPanelRunnable::ShowSearchPanelRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::ShowSearchPanelRunnable::Run()
{
    mHost->ShowSearchPanel();
    mHost->AwakenDreams();
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::HomeActionListener
//=================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::HomeActionListener, Object, IViewOnTouchListener)

CPhoneStatusBar::HomeActionListener::HomeActionListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::HomeActionListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action = 0;
    event->GetAction(&action);
    switch(action) {
        case IMotionEvent::ACTION_DOWN:
        if (!mHost->ShouldDisableNavbarGestures()) {
            mHost->mHandler->RemoveCallbacks(mHost->mShowSearchPanel);
            Boolean tmp = FALSE;
            mHost->mHandler->PostDelayed(mHost->mShowSearchPanel, mHost->mShowSearchHoldoff, &tmp);
        }
    break;

    case IMotionEvent::ACTION_UP:
    case IMotionEvent::ACTION_CANCEL:
        mHost->mHandler->RemoveCallbacks(mHost->mShowSearchPanel);
        mHost->AwakenDreams();
    break;
    }
    *result = FALSE;
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::HideBackdropFront
//=================================================================================
CPhoneStatusBar::HideBackdropFront::HideBackdropFront(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::HideBackdropFront::Run()
{
    if (DEBUG_MEDIA) {
        Logger::V(TAG, "DEBUG_MEDIA: removing fade layer");
    }
    IView::Probe(mHost->mBackdropFront)->SetVisibility(IView::INVISIBLE);
    AutoPtr<IViewPropertyAnimator> vp;
    IView::Probe(mHost->mBackdropFront)->Animate((IViewPropertyAnimator**)&vp);
    vp->Cancel();
    mHost->mBackdropFront->SetImageDrawable(NULL);
    return NOERROR;
}

//=================================================================================
// CPhoneStatusBar::H
//=================================================================================
CPhoneStatusBar::H::H(
    /* [in] */ CPhoneStatusBar* host)
    : BaseStatusBar::H(host)
    , mHost(host)
{}

ECode CPhoneStatusBar::H::HandleMessage(
    /* [in] */ IMessage* m)
{
    BaseStatusBar::H::HandleMessage(m);
    Int32 what = 0;
    m->GetWhat(&what);
    switch (what) {
        case MSG_OPEN_NOTIFICATION_PANEL:
            mHost->AnimateExpandNotificationsPanel();
            break;
        case MSG_OPEN_SETTINGS_PANEL:
            mHost->AnimateExpandSettingsPanel();
            break;
        case MSG_CLOSE_PANELS:
            mHost->AnimateCollapsePanels();
            break;
        case BaseStatusBar::MSG_SHOW_HEADS_UP:
            mHost->SetHeadsUpVisibility(TRUE);
            break;
        case BaseStatusBar::MSG_DECAY_HEADS_UP:
            mHost->mHeadsUpNotificationView->ReleaseResources();
            mHost->SetHeadsUpVisibility(FALSE);
            break;
        case BaseStatusBar::MSG_HIDE_HEADS_UP:
            mHost->mHeadsUpNotificationView->ReleaseResources();
            mHost->SetHeadsUpVisibility(FALSE);
            break;
        case BaseStatusBar::MSG_ESCALATE_HEADS_UP:
            mHost->EscalateHeadsUp();
            mHost->SetHeadsUpVisibility(FALSE);
            break;
    }
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::FocusChangeListener
//==========================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::FocusChangeListener, Object, IViewOnFocusChangeListener)

ECode CPhoneStatusBar::FocusChangeListener::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    // Because 'v' is a ViewGroup, all its children will be (un)selected
    // too, which allows marqueeing to work.
    v->SetSelected(hasFocus);
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::AnimateCollapsePanelsRunnable
//==========================================================================================
CPhoneStatusBar::AnimateCollapsePanelsRunnable::AnimateCollapsePanelsRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::AnimateCollapsePanelsRunnable::Run()
{
    mHost->AnimateCollapsePanels();
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::CheckBarModesRunnable
//==========================================================================================
CPhoneStatusBar::CheckBarModesRunnable::CheckBarModesRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::CheckBarModesRunnable::Run()
{
    mHost->CheckBarModes();
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::MyTicker
//==========================================================================================
CPhoneStatusBar::MyTicker::MyTicker(
    /* [in] */ CPhoneStatusBar* host,
    /* [in] */ IContext* context,
    /* [in] */ IView* sb)
    : mHost(host)
{
    Ticker::constructor(context, sb);
    if (!mHost->mTickerEnabled) {
        Logger::W(TAG, "MyTicker instantiated with mTickerEnabled=FALSE");
    }
}

ECode CPhoneStatusBar::MyTicker::TickerStarting()
{
    if (!mHost->mTickerEnabled) return NOERROR;
    mHost->mTicking = TRUE;
    IView::Probe(mHost->mStatusBarContents)->SetVisibility(IView::GONE);
    mHost->mTickerView->SetVisibility(IView::VISIBLE);
    mHost->mTickerView->StartAnimation(mHost->LoadAnim(Elastos::Droid::R::anim::push_up_in, NULL));
    IView::Probe(mHost->mStatusBarContents)->StartAnimation(
            mHost->LoadAnim(Elastos::Droid::R::anim::push_up_out, NULL));
    return NOERROR;
}

ECode CPhoneStatusBar::MyTicker::TickerDone()
{
    if (!mHost->mTickerEnabled) return NOERROR;
    IView::Probe(mHost->mStatusBarContents)->SetVisibility(IView::VISIBLE);
    mHost->mTickerView->SetVisibility(IView::GONE);
    IView::Probe(mHost->mStatusBarContents)->StartAnimation(
            mHost->LoadAnim(Elastos::Droid::R::anim::push_down_in, NULL));
    mHost->mTickerView->StartAnimation(mHost->LoadAnim(Elastos::Droid::R::anim::push_down_out,
                mHost->mTickingDoneListener));
    return NOERROR;
}

ECode CPhoneStatusBar::MyTicker::TickerHalting()
{
    if (!mHost->mTickerEnabled) return NOERROR;
    Int32 v = 0;
    if ((IView::Probe(mHost->mStatusBarContents)->GetVisibility(&v), v) != IView::VISIBLE) {
        IView::Probe(mHost->mStatusBarContents)->SetVisibility(IView::VISIBLE);
        IView::Probe(mHost->mStatusBarContents)->StartAnimation(
                mHost->LoadAnim(Elastos::Droid::R::anim::fade_in, NULL));
    }
    mHost->mTickerView->SetVisibility(IView::GONE);
    // we do not animate the ticker away at this point, just get rid of it (b/6992707)
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::TickingDoneListener
//==========================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::TickingDoneListener, Object, IAnimationAnimationListener)

CPhoneStatusBar::TickingDoneListener::TickingDoneListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::TickingDoneListener::OnAnimationEnd(
    /* [in] */ IAnimation* animation)
{
    mHost->mTicking = FALSE;
    return NOERROR;
}

ECode CPhoneStatusBar::TickingDoneListener::OnAnimationRepeat(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

ECode CPhoneStatusBar::TickingDoneListener::OnAnimationStart(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::StartTracing
//==========================================================================================
CPhoneStatusBar::StartTracing::StartTracing(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::StartTracing::Run()
{
    mHost->Vibrate();
    SystemClock::Sleep(250);
    Logger::D(TAG, "startTracing");
    assert(0 && "TODO");
    // android.os.Debug.StartMethodTracing("/data/statusbar-traces/trace");
    Boolean tmp = FALSE;
    mHost->mHandler->PostDelayed(mHost->mStopTracing, 10000, &tmp);
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::StopTracing
//==========================================================================================
CPhoneStatusBar::StopTracing::StopTracing(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::StopTracing::Run()
{
    mHost->Vibrate();
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::ShadeUpdates
//==========================================================================================
CPhoneStatusBar::ShadeUpdates::ShadeUpdates(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{
    CArraySet::New((IArraySet**)&mVisibleNotifications);
    CArraySet::New((IArraySet**)&mNewVisibleNotifications);
}

ECode CPhoneStatusBar::ShadeUpdates::Check()
{
    mNewVisibleNotifications->Clear();
    AutoPtr<IArrayList> activeNotifications;  /*<INotificationDataEntry*/
    mHost->mNotificationData->GetActiveNotifications((IArrayList**)&activeNotifications);
    Int32 size = 0, v = 0;
    activeNotifications->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        activeNotifications->Get(i, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> entry = INotificationDataEntry::Probe(obj);

        AutoPtr<IExpandableNotificationRow> row;
        entry->GetRow((IExpandableNotificationRow**)&row);
        const Boolean visible = row != NULL
                && (IView::Probe(row)->GetVisibility(&v), v) == IView::VISIBLE;
        if (visible) {
            AutoPtr<IStatusBarNotification> notification;
            entry->GetNotification((IStatusBarNotification**)&notification);
            Int64 t = 0;
            notification->GetPostTime(&t);
            String key;
            entry->GetKey(&key);
            AutoPtr<ICharSequence> k;
            CString::New(key + StringUtils::ToString(t), (ICharSequence**)&k);
            mNewVisibleNotifications->Add(k);
        }
    }
    Boolean updates = FALSE;
    mVisibleNotifications->ContainsAll(ICollection::Probe(mNewVisibleNotifications), &updates);
    updates = !updates;
    mVisibleNotifications->Clear();
    mVisibleNotifications->AddAll(mNewVisibleNotifications);

    // We have new notifications
    if (updates && mHost->mDozeServiceHost != NULL) {
        mHost->mDozeServiceHost->FireNewNotifications();
    }
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::DozeServiceHost::DozeServiceHostHandler
//==========================================================================================
const Int32 CPhoneStatusBar::DozeServiceHost::DozeServiceHostHandler::MSG_START_DOZING = 1;
const Int32 CPhoneStatusBar::DozeServiceHost::DozeServiceHostHandler::MSG_PULSE_WHILE_DOZING = 2;
const Int32 CPhoneStatusBar::DozeServiceHost::DozeServiceHostHandler::MSG_STOP_DOZING = 3;

CPhoneStatusBar::DozeServiceHost::DozeServiceHostHandler::DozeServiceHostHandler(
    /* [in] */ DozeServiceHost* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CPhoneStatusBar::DozeServiceHost::DozeServiceHostHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_START_DOZING: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleStartDozing(IRunnable::Probe(obj));
            break;
        }
        case MSG_PULSE_WHILE_DOZING: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandlePulseWhileDozing(IDozeHostPulseCallback::Probe(obj));
            break;
        }
        case MSG_STOP_DOZING:
            mHost->HandleStopDozing();
            break;
    }
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::DozeServiceHost
//==========================================================================================
const Int64 CPhoneStatusBar::DozeServiceHost::PROCESSING_TIME = 500;

CAR_INTERFACE_IMPL(CPhoneStatusBar::DozeServiceHost, Object, IDozeHost)

CPhoneStatusBar::DozeServiceHost::DozeServiceHost(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{
    CArrayList::New((IArrayList**)&mCallbacks);
    mHandler = new DozeServiceHostHandler(this);
}

ECode CPhoneStatusBar::DozeServiceHost::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    Int32 size = 0;
    mCallbacks->GetSize(&size);
    *str = String("PSB.DozeServiceHost[mCallbacks=") + StringUtils::ToString(size) + String("]");
    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::FirePowerSaveChanged(
    /* [in] */ Boolean active)
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> callback;
        ator->GetNext((IInterface**)&callback);
        IDozeHostCallback::Probe(callback)->OnPowerSaveChanged(active);
    }

    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::FireBuzzBeepBlinked()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> callback;
        ator->GetNext((IInterface**)&callback);
        IDozeHostCallback::Probe(callback)->OnBuzzBeepBlinked();
    }
    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::FireNotificationLight(
    /* [in] */ Boolean on)
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> callback;
        ator->GetNext((IInterface**)&callback);
        IDozeHostCallback::Probe(callback)->OnNotificationLight(on);
    }

    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::FireNewNotifications()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> callback;
        ator->GetNext((IInterface**)&callback);
        IDozeHostCallback::Probe(callback)->OnNewNotifications();
    }

    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::AddCallback(
    /* [in] */ /*@NonNull*/ IDozeHostCallback* callback)
{
    mCallbacks->Add(callback);
    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::RemoveCallback(
    /* [in] */ /*@NonNull*/ IDozeHostCallback* callback)
{
    mCallbacks->Remove(callback);
    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::StartDozing(
    /* [in] */ /*@NonNull*/ IRunnable* ready)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(DozeServiceHostHandler::MSG_START_DOZING, ready, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::PulseWhileDozing(
    /* [in] */ /*@NonNull*/ IDozeHostPulseCallback* callback)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(DozeServiceHostHandler::MSG_PULSE_WHILE_DOZING, callback, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::StopDozing()
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(DozeServiceHostHandler::MSG_STOP_DOZING, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStatusBar::DozeServiceHost::IsPowerSaveActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    *result = mHost->mBatteryController != NULL && (mHost->mBatteryController->IsPowerSave(&tmp), tmp);
    return NOERROR;
}

void CPhoneStatusBar::DozeServiceHost::HandleStartDozing(
    /* [in] */ /*@NonNull*/ IRunnable* ready)
{
    if (!mHost->mDozing) {
        mHost->mDozing = TRUE;
        DozeLog::TraceDozing(mHost->mContext, mHost->mDozing);
        mHost->UpdateDozingState();
    }
    ready->Run();
}

void CPhoneStatusBar::DozeServiceHost::HandlePulseWhileDozing(
    /* [in] */ /*@NonNull*/ IDozeHostPulseCallback* callback)
{
    mHost->mScrimController->Pulse(callback);
}

void CPhoneStatusBar::DozeServiceHost::HandleStopDozing()
{
    if (mHost->mDozing) {
        mHost->mDozing = FALSE;
        DozeLog::TraceDozing(mHost->mContext, mHost->mDozing);
        mHost->UpdateDozingState();
    }
}

//==========================================================================================
// CPhoneStatusBar::StatusBarWindowOnTouchListener
//==========================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::StatusBarWindowOnTouchListener, Object, IViewOnTouchListener)

CPhoneStatusBar::StatusBarWindowOnTouchListener::StatusBarWindowOnTouchListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::StatusBarWindowOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->CheckUserAutohide(v, event);
    Int32 action;
    if ((event->GetAction(&action), action) == IMotionEvent::ACTION_DOWN) {
        if (mHost->mExpandedVisible) {
            mHost->AnimateCollapsePanels();
        }
    }
    return IView::Probe(mHost->mStatusBarWindow)->OnTouchEvent(event, result);
}

//==========================================================================================
// CPhoneStatusBar::VerticalChangedListener
//==========================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::VerticalChangedListener, Object, IOnVerticalChangedListener)

CPhoneStatusBar::VerticalChangedListener::VerticalChangedListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::VerticalChangedListener::OnVerticalChanged(
    /* [in] */ Boolean isVertical)
{
    if (mHost->mSearchPanelView != NULL) {
        mHost->mSearchPanelView->SetHorizontal(isVertical);
    }
    mHost->mNotificationPanel->SetQsScrimEnabled(!isVertical);
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::NavigationBarViewOnTouchListener
//==========================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::NavigationBarViewOnTouchListener, Object, IViewOnTouchListener)

CPhoneStatusBar::NavigationBarViewOnTouchListener::NavigationBarViewOnTouchListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::NavigationBarViewOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->CheckUserAutohide(v, event);
    *result = FALSE;
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::DismissViewOnClickListener
//==========================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::DismissViewOnClickListener, Object, IViewOnClickListener)

CPhoneStatusBar::DismissViewOnClickListener::DismissViewOnClickListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::DismissViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->ClearAllNotifications();
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::BatteryStateChangeCallback
//==========================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::BatteryStateChangeCallback, Object, IBatteryStateChangeCallback)

CPhoneStatusBar::BatteryStateChangeCallback::BatteryStateChangeCallback(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::BatteryStateChangeCallback::OnPowerSaveChanged()
{
    Boolean tmp = FALSE;
    mHost->mHandler->Post(mHost->mCheckBarModes, &tmp);
    if (mHost->mDozeServiceHost != NULL) {
        mHost->mBatteryController->IsPowerSave(&tmp);
        mHost->mDozeServiceHost->FirePowerSaveChanged(tmp);
    }
    return NOERROR;
}

ECode CPhoneStatusBar::BatteryStateChangeCallback::OnBatteryLevelChanged(
    /* [in] */ Int32 level,
    /* [in] */ Boolean pluggedIn,
    /* [in] */ Boolean charging)
{
    // noop
    return NOERROR;
}

//==========================================================================================
// CPhoneStatusBar::HostCallback
//==========================================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::HostCallback, Object, IQSTileHostCallback)

CPhoneStatusBar::HostCallback::HostCallback(
    /* [in] */ CPhoneStatusBar* host,
    /* [in] */ IQSTileHost* qsh)
    : mHost(host)
    , mQsh(qsh)
{}

ECode CPhoneStatusBar::HostCallback::OnTilesChanged()
{
    AutoPtr<ICollection> c;
    mQsh->GetTiles((ICollection**)&c);
    mHost->mQSPanel->SetTiles(c);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::ClearAllNotificationRunnable
//==========================================================================
CPhoneStatusBar::ClearAllNotificationRunnable::ClearAllNotificationRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::ClearAllNotificationRunnable::Run()
{
    mHost->mBarService->OnClearAllNotifications(mHost->mCurrentUserId);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::CollapsePanelRunnable
//==========================================================================
CPhoneStatusBar::CollapsePanelRunnable::CollapsePanelRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::CollapsePanelRunnable::Run()
{
    AutoPtr<DismissStackScrollerRunnable> run = new DismissStackScrollerRunnable(mHost);
    Boolean tmp = FALSE;
    IView::Probe(mHost->mStackScroller)->Post(run, &tmp);
    mHost->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::DismissStackScrollerRunnable
//==========================================================================
CPhoneStatusBar::DismissStackScrollerRunnable::DismissStackScrollerRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::DismissStackScrollerRunnable::Run()
{
    mHost->mStackScroller->SetDismissAllInProgress(FALSE);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::OnPreDrawListener
//==========================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::OnPreDrawListener, Object, IOnPreDrawListener)

CPhoneStatusBar::OnPreDrawListener::OnPreDrawListener(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::OnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mHost->mDrawCount == 1) {
        AutoPtr<IViewTreeObserver> vto;
        IView::Probe(mHost->mStatusBarView)->GetViewTreeObserver((IViewTreeObserver**)&vto);
        vto->RemoveOnPreDrawListener(this);
        AutoPtr<IGLES20CanvasHelper> helper;
        CGLES20CanvasHelper::AcquireSingleton((IGLES20CanvasHelper**)&helper);
        helper->SetProperty(String("extraRasterBucket"),
            StringUtils::ToString(IStackScrollAlgorithm::DIMMED_SCALE));

        AutoPtr<IResources> res;
        mHost->mContext->GetResources((IResources**)&res);
        Int32 collapseSize, expandedSize;
        res->GetDimensionPixelSize(R::dimen::qs_time_collapsed_size, &collapseSize);
        res->GetDimensionPixelSize(R::dimen::qs_time_expanded_size, &expandedSize);
        Float value = collapseSize * 1.0f / expandedSize;
        helper->SetProperty(String("extraRasterBucket"), StringUtils::ToString(value));
    }
    mHost->mDrawCount++;
    *result = TRUE;
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::UpdateNotificationShadeRunnable
//==========================================================================
CPhoneStatusBar::UpdateNotificationShadeRunnable::UpdateNotificationShadeRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::UpdateNotificationShadeRunnable::Run()
{
    mHost->UpdateNotificationShade();
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::CarrierLabelVisibleAnimatorListenerAdapter
//==========================================================================
CPhoneStatusBar::CarrierLabelVisibleAnimatorListenerAdapter::CarrierLabelVisibleAnimatorListenerAdapter(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::CarrierLabelVisibleAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mHost->mCarrierLabelVisible) { // race
        IView::Probe(mHost->mCarrierLabel)->SetVisibility(IView::INVISIBLE);
        IView::Probe(mHost->mCarrierLabel)->SetAlpha(0.f);
    }
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::ViewGoneAnimatorListenerAdapter
//==========================================================================
CPhoneStatusBar::ViewGoneAnimatorListenerAdapter::ViewGoneAnimatorListenerAdapter(
    /* [in] */ IView* view)
    : mView(view)
{}

ECode CPhoneStatusBar::ViewGoneAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mView->SetVisibility(IView::GONE);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::BackdropGoneRunnable
//==========================================================================
CPhoneStatusBar::BackdropGoneRunnable::BackdropGoneRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::BackdropGoneRunnable::Run()
{
    IView::Probe(mHost->mBackdrop)->SetVisibility(IView::GONE);
    AutoPtr<IViewPropertyAnimator> vpa;
    IView::Probe(mHost->mBackdropFront)->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Cancel();

    vpa = NULL;
    IView::Probe(mHost->mBackdropBack)->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Cancel();
    Boolean tmp = FALSE;
    mHost->mHandler->Post(mHost->mHideBackdropFront, &tmp);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::ViewInvisibleRunnable
//==========================================================================
CPhoneStatusBar::ViewInvisibleRunnable::ViewInvisibleRunnable(
    /* [in] */ IView* view)
    : mView(view)
{}

ECode CPhoneStatusBar::ViewInvisibleRunnable::Run()
{
    mView->SetVisibility(IView::INVISIBLE);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::ChangeVisibilityAndResetAnimatorListenerAdapter
//==========================================================================
CPhoneStatusBar::ChangeVisibilityAndResetAnimatorListenerAdapter::ChangeVisibilityAndResetAnimatorListenerAdapter(
    /* [in] */ IView* view,
    /* [in] */ IViewPropertyAnimator* a,
    /* [in] */ Int32 vis)
    : mView(view)
    , mA(a)
    , mVis(vis)
{}

ECode CPhoneStatusBar::ChangeVisibilityAndResetAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mView->SetVisibility(mVis);
    mA->SetListener(NULL); // oneshot
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::ChangeVisibilityAnimatorListenerAdapter
//==========================================================================
CPhoneStatusBar::ChangeVisibilityAnimatorListenerAdapter::ChangeVisibilityAnimatorListenerAdapter(
    /* [in] */ IView* view,
    /* [in] */ Int32 vis)
    : mView(view)
    , mVis(vis)
{}

ECode CPhoneStatusBar::ChangeVisibilityAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mView->SetVisibility(mVis);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::DebugLoacationRunnable
//==========================================================================
CPhoneStatusBar::DebugLoacationRunnable::DebugLoacationRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::DebugLoacationRunnable::Run()
{
    IView::Probe(mHost->mStatusBarView)->GetLocationOnScreen(mHost->mAbsPos);
    Int32 w = 0, h = 0;
    mHost->GetStatusBarHeight(&h);
    IView::Probe(mHost->mStatusBarView)->GetWidth(&w);
    Logger::D(TAG, "mStatusBarView: ----- (%d, %d)%dx%d",
        (*(mHost->mAbsPos))[0], (*(mHost->mAbsPos))[1], w, h);
    // mHost->mStatusBarView.debug();
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::DismissKeyguardViewManagerRunnable
//==========================================================================
CPhoneStatusBar::DismissKeyguardViewManagerRunnable::DismissKeyguardViewManagerRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::DismissKeyguardViewManagerRunnable::Run()
{
    mHost->mStatusBarKeyguardViewManager->Dismiss();
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::StartSettingsActivityRunnable
//==========================================================================
CPhoneStatusBar::StartSettingsActivityRunnable::StartSettingsActivityRunnable(
    /* [in] */ CPhoneStatusBar* host,
    /* [in] */ IIntent* intent)
    : mHost(host)
    , mIntent(intent)
{}

ECode CPhoneStatusBar::StartSettingsActivityRunnable::Run()
{
    mHost->HandleStartSettingsActivity(mIntent, TRUE /*onlyProvisioned*/);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::EndLaunchTransitionRunnable
//==========================================================================
CPhoneStatusBar::EndLaunchTransitionRunnable::EndLaunchTransitionRunnable(
    /* [in] */ CPhoneStatusBar* host)
    : mHost(host)
{}

ECode CPhoneStatusBar::EndLaunchTransitionRunnable::Run()
{
    IView::Probe(mHost->mNotificationPanel)->SetAlpha(1);
    if (mHost->mLaunchTransitionEndRunnable != NULL) {
        mHost->mLaunchTransitionEndRunnable->Run();
    }
    mHost->mLaunchTransitionEndRunnable = NULL;
    mHost->mLaunchTransitionFadingAway = FALSE;
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::FadeKeyguardRunnable
//==========================================================================
CPhoneStatusBar::FadeKeyguardRunnable::FadeKeyguardRunnable(
    /* [in] */ CPhoneStatusBar* host,
    /* [in] */ IRunnable* beforeFading)
    : mHost(host)
    , mBeforeFading(beforeFading)
{}

ECode CPhoneStatusBar::FadeKeyguardRunnable::Run()
{
    mHost->mLaunchTransitionFadingAway = TRUE;
    if (mBeforeFading != NULL) {
        mBeforeFading->Run();
    }
    IView::Probe(mHost->mNotificationPanel)->SetAlpha(1);
    AutoPtr<IViewPropertyAnimator> vpa;
    IView::Probe(mHost->mNotificationPanel)->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Alpha(0);
    vpa->SetStartDelay(FADE_KEYGUARD_START_DELAY);
    vpa->SetDuration(FADE_KEYGUARD_DURATION);
    vpa->WithLayer();

    AutoPtr<EndLaunchTransitionRunnable> run = new EndLaunchTransitionRunnable(mHost);
    vpa->WithEndAction(run);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::KeyguardHostViewOnDismissAction
//==========================================================================
CAR_INTERFACE_IMPL(CPhoneStatusBar::KeyguardHostViewOnDismissAction, Object, IKeyguardHostViewOnDismissAction)

CPhoneStatusBar::KeyguardHostViewOnDismissAction::KeyguardHostViewOnDismissAction(
    /* [in] */ CPhoneStatusBar* host,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean afterKeyguardGone,
    /* [in] */ Boolean keyguardShowing,
    /* [in] */ Boolean dismissShade)
    : mHost(host)
    , mIntent(intent)
    , mAfterKeyguardGone(afterKeyguardGone)
    , mKeyguardShowing(keyguardShowing)
    , mDismissShade(dismissShade)
{}

ECode CPhoneStatusBar::KeyguardHostViewOnDismissAction::OnDismiss(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IRunnable> runnable = new StartActivityRunnable(
        mHost, mIntent, mAfterKeyguardGone, mKeyguardShowing);
    AsyncTask::Execute(runnable);

    if (mDismissShade) {
        mHost->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE, TRUE /* force */);
    }
    *result = TRUE;
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar::StartActivityRunnable
//==========================================================================
CPhoneStatusBar::StartActivityRunnable::StartActivityRunnable(
    /* [in] */ CPhoneStatusBar* host,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean afterKeyguardGone,
    /* [in] */ Boolean keyguardShowing)
    : mHost(host)
    , mIntent(intent)
    , mAfterKeyguardGone(afterKeyguardGone)
    , mKeyguardShowing(keyguardShowing)
{}

ECode CPhoneStatusBar::StartActivityRunnable::Run()
{
    Logger::I(TAG, " >> StartActivityRunnable::Run() start activity: %s", TO_CSTR(mIntent));
    if (mKeyguardShowing && !mAfterKeyguardGone) {
        ActivityManagerNative::GetDefault()->KeyguardWaitingForActivityDrawn();
    }

    AutoPtr<IUserHandle> user;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&user);
    mIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    mHost->mContext->StartActivityAsUser(mIntent, user);
    mHost->OverrideActivityPendingAppTransition(mKeyguardShowing && !mAfterKeyguardGone);
    return NOERROR;
}

//==========================================================================
// CPhoneStatusBar
//==========================================================================

CAR_OBJECT_IMPL(CPhoneStatusBar)

CAR_INTERFACE_IMPL_4(CPhoneStatusBar, BaseStatusBar, IPhoneStatusBar, \
    IDemoMode, IDragDownCallback, IActivityStarter);

CPhoneStatusBar::CPhoneStatusBar()
    : mNaturalBarHeight(-1)
    , mIconSize(-1)
    , mIconHPadding(-1)
    , mStatusBarWindowState(IStatusBarManager::WINDOW_STATE_SHOWING)
    , mScreenOnComingFromTouch(FALSE)
    , mPixelFormat(0)
    , mNotificationPanelGravity(0)
    , mNotificationPanelMarginBottomPx(0)
    , mNotificationPanelMinHeightFrac(0)
    , mLeaveOpenOnKeyguardHide(FALSE)
    , mKeyguardFadingAway(FALSE)
    , mKeyguardFadingAwayDelay(0)
    , mKeyguardFadingAwayDuration(0)
    , mKeyguardMaxNotificationCount(0)
    , mCarrierLabelVisible(FALSE)
    , mCarrierLabelHeight(0)
    , mStatusBarHeaderHeight(0)
    , mShowCarrierInPanel(FALSE)
    , mExpandedVisible(FALSE)
    , mNavigationBarWindowState(IStatusBarManager::WINDOW_STATE_SHOWING)
    , mTrackingPosition(0)
    , mTickerEnabled(FALSE)
    , mTicking(FALSE)
    , mEdgeBorder(0)
    , mTracking(FALSE)
    , mDisabled(0)
    , mSystemUiVisibility(IView::SYSTEM_UI_FLAG_VISIBLE)
    , mNavigationIconHints(0)
    , mUserSetup(FALSE)
    , mInteractingWindows(0)
    , mAutohideSuspended(FALSE)
    , mStatusBarMode(0)
    , mNavigationBarMode(0)
    , mScreenOn(FALSE)
    , mScreenOnFromKeyguard(FALSE)
    , mVisible(FALSE)
    , mWaitingForKeyguardExit(FALSE)
    , mDozing(FALSE)
    , mScrimSrcModeEnabled(FALSE)
    , mDisabledUnmodified(0)
    , mLastVisibilityReportUptimeMs(0)
    , mDrawCount(0)
    , mLaunchTransitionFadingAway(FALSE)
    , mLastLockToAppLongPress(0)
    , mShowSearchHoldoff(0)
    , FLIP_DURATION_OUT(125)
    , FLIP_DURATION_IN(225)
    , FLIP_DURATION(FLIP_DURATION_IN + FLIP_DURATION_OUT)
    , mDemoModeAllowed(FALSE)
    , mDemoMode(FALSE)
{
}

ECode CPhoneStatusBar::constructor()
{
    BaseStatusBar::constructor();

    CPoint::New((IPoint**)&mCurrentDisplaySize);
    mPositionTmp = ArrayOf<Int32>::Alloc(2);
    mAbsPos = ArrayOf<Int32>::Alloc(2);
    CArrayList::New((IArrayList**)&mPostCollapseRunnables);
    CDisplayMetrics::New((IDisplayMetrics**)&mDisplayMetrics);
    mGestureRec = DEBUG_GESTURES
            ? new GestureRecorder(String("/sdcard/statusbar_gestures.dat"))
            : NULL;
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    CPhoneStatusBarUserSetupObserver::New(this, handler, (IContentObserver**)&mUserSetupObserver);
    CPhoneStatusBarHeadsUpObserver::New(this, handler, (IContentObserver**)&mHeadsUpObserver);
    mAutohide = new Autohide(this);
    CLinearInterpolator::New((IInterpolator**)&mLinearInterpolator);
    CAccelerateDecelerateInterpolator::New((IInterpolator**)&mBackdropInterpolator);
    CPorterDuffXfermode::New(PorterDuffMode_SRC, (IPorterDuffXfermode**)&mSrcXferMode);
    CPorterDuffXfermode::New(PorterDuffMode_SRC_OVER, (IPorterDuffXfermode**)&mSrcOverXferMode);
    mMediaListener = new MediaListener(this);
    mOnChildLocationsChangedListener = new OnChildLocationsChangedListener(this);
    CArraySet::New((IArraySet**)&mCurrentlyVisibleNotifications);
    mShadeUpdates = new ShadeUpdates(this);
    mNotificationLocationsChangedListener = new NotificationLocationsChangedListener(this);
    mVisibilityReporter = new VisibilityReporter(this);
    mOverflowClickListener = new OverflowClickListener(this);
    mRecentsClickListener = new RecentsClickListener(this);
    mLongPressBackRecentsListener = new LongPressBackRecentsListener(this);
    mShowSearchPanel = new ShowSearchPanelRunnable(this);
    mHomeActionListener = new HomeActionListener(this);
    mHideBackdropFront = new HideBackdropFront(this);
    mFocusChangeListener = new FocusChangeListener();
    mAnimateCollapsePanels = new AnimateCollapsePanelsRunnable(this);
    CAccelerateInterpolator::New((ITimeInterpolator**)&mAccelerateInterpolator);
    CDecelerateInterpolator::New((ITimeInterpolator**)&mDecelerateInterpolator);
    mCheckBarModes = new CheckBarModesRunnable(this);
    mTickingDoneListener = new TickingDoneListener(this);
    CPhoneStatusBarReceiver::New(this, (IBroadcastReceiver**)&mBroadcastReceiver);
    mStartTracing = new StartTracing(this);
    mStopTracing = new StopTracing(this);
    return NOERROR;
}

ECode CPhoneStatusBar::Start()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    IWindowManager::Probe(obj)->GetDefaultDisplay((IDisplay**)&mDisplay);
    UpdateDisplaySize();

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetBoolean(R::bool_::config_status_bar_scrim_behind_use_src, &mScrimSrcModeEnabled);
    BaseStatusBar::Start(); // calls createAndAddWindows()

    obj = NULL;
    mContext->GetSystemService(IContext::MEDIA_SESSION_SERVICE, (IInterface**)&obj);
    mMediaSessionManager = IMediaSessionManager::Probe(obj);
    // TODO: use MediaSessionManager.SessionListener to hook us up to future updates
    // in session state

    AddNavigationBar();

    // Lastly, call to the icon policy to install/update all the icons.
    mIconPolicy = new PhoneStatusBarPolicy(mContext, mCastController);
    mSettingsObserver->OnChange(FALSE); // set up

    mHeadsUpObserver->OnChange(TRUE); // set up
    if (ENABLE_HEADS_UP) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IUri> uri;
        Elastos::Droid::Provider::Settings::Global::GetUriFor(
                ISettingsGlobal::HEADS_UP_NOTIFICATIONS_ENABLED, (IUri**)&uri);
        cr->RegisterContentObserver(uri, TRUE, mHeadsUpObserver);

        uri = NULL;
        Elastos::Droid::Provider::Settings::Global::GetUriFor(SETTING_HEADS_UP_TICKER, (IUri**)&uri);
        cr->RegisterContentObserver(uri, TRUE, mHeadsUpObserver);
    }
    mUnlockMethodCache = UnlockMethodCache::GetInstance(mContext);
    StartKeyguard();

    mDozeServiceHost = new DozeServiceHost(this);
    PutComponent(String("EIID_IDozeHost"), mDozeServiceHost->Probe(EIID_IInterface));
    PutComponent(String("EIID_IPhoneStatusBar"), TO_IINTERFACE(this));

    SetControllerUsers();

    NotifyUserAboutHiddenNotifications();
    return NOERROR;
}

// ================================================================================
// Constructing the view
// ================================================================================
AutoPtr<IPhoneStatusBarView> CPhoneStatusBar::MakeStatusBarView()
{
    AutoPtr<IContext> context = mContext;

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    UpdateDisplaySize(); // populates mDisplayMetrics
    UpdateResources();

    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::status_bar_icon_size, &mIconSize);

    AutoPtr<IView> statusBarWindow;
    Elastos::Droid::View::View::Inflate(context, R::layout::super_status_bar, NULL, (IView**)&statusBarWindow);
    mStatusBarWindow = IStatusBarWindowView::Probe(statusBarWindow);
    ((CStatusBarWindowView*)mStatusBarWindow.Get())->mService = this;

    AutoPtr<StatusBarWindowOnTouchListener> listener = new StatusBarWindowOnTouchListener(this);
    statusBarWindow->SetOnTouchListener(listener);

    AutoPtr<IView> statusBarView;
    statusBarWindow->FindViewById(R::id::status_bar, (IView**)&statusBarView);
    mStatusBarView = IPhoneStatusBarView::Probe(statusBarView);
    mStatusBarView->SetBar(this);

    AutoPtr<IView> view;
    statusBarWindow->FindViewById(R::id::panel_holder, (IView**)&view);
    AutoPtr<IPanelHolder> holder = IPanelHolder::Probe(view);
    IPanelBar::Probe(mStatusBarView)->SetPanelHolder(holder);

    view = NULL;
    statusBarWindow->FindViewById(R::id::notification_panel, (IView**)&view);
    mNotificationPanel = INotificationPanelView::Probe(view);
    mNotificationPanel->SetStatusBar(this);

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Boolean tmp = FALSE;
    if (helper->IsHighEndGfx(&tmp), !tmp) {
        statusBarWindow->SetBackground(NULL);

        Int32 c = 0;
        res->GetColor(R::color::notification_panel_solid_background, &c);
        AutoPtr<IDrawable> d;
        CFastColorDrawable::New(c, (IDrawable**)&d);
        IView::Probe(mNotificationPanel)->SetBackground(d);
    }
    if (ENABLE_HEADS_UP) {
        view = NULL;
        Elastos::Droid::View::View::Inflate(context, R::layout::heads_up, NULL, (IView**)&view);
        mHeadsUpNotificationView = IHeadsUpNotificationView::Probe(view);
        IView::Probe(mHeadsUpNotificationView)->SetVisibility(IView::GONE);
        mHeadsUpNotificationView->SetBar(this);
    }
    if (MULTIUSER_DEBUG) {
        view = NULL;
        IView::Probe(mNotificationPanel)->FindViewById(R::id::header_debug_info, (IView**)&view);
        mNotificationPanelDebugText = ITextView::Probe(view);
        IView::Probe(mNotificationPanelDebugText)->SetVisibility(IView::VISIBLE);
    }

    UpdateShowSearchHoldoff();

    {
        Boolean showNav = FALSE;
        mWindowManagerService->HasNavigationBar(&showNav);
        if (DEBUG) Logger::V(TAG, "hasNavigationBar=%d", showNav);
        if (showNav) {
            view = NULL;
            Elastos::Droid::View::View::Inflate(context, R::layout::navigation_bar, NULL, (IView**)&view);
            mNavigationBarView = INavigationBarView::Probe(view);

            mNavigationBarView->SetDisabledFlags(mDisabled);
            mNavigationBarView->SetBar(this);

            AutoPtr<VerticalChangedListener> cl = new VerticalChangedListener(this);
            mNavigationBarView->SetOnVerticalChangedListener(cl);

            AutoPtr<NavigationBarViewOnTouchListener> tl = new NavigationBarViewOnTouchListener(this);
            IView::Probe(mNavigationBarView)->SetOnTouchListener(tl);
        }
    } /*catch (RemoteException ex) {
        // no window manager? good luck with that
    }*/

    // figure out which pixel-format to use for the status bar.
    mPixelFormat = IPixelFormat::OPAQUE;

    view = NULL;
    statusBarView->FindViewById(R::id::system_icon_area, (IView**)&view);
    mSystemIconArea = ILinearLayout::Probe(view);
    view = NULL;
    statusBarView->FindViewById(R::id::system_icons, (IView**)&view);
    mSystemIcons = ILinearLayout::Probe(view);
    view = NULL;
    statusBarView->FindViewById(R::id::statusIcons, (IView**)&view);
    mStatusIcons = ILinearLayout::Probe(view);
    statusBarView->FindViewById(R::id::notification_icon_area_inner, (IView**)&mNotificationIconArea);

    view = NULL;
    statusBarView->FindViewById(R::id::notificationIcons, (IView**)&view);
    mNotificationIcons = IIconMerger::Probe(view);
    statusBarView->FindViewById(R::id::moreIcon, (IView**)&mMoreIcon);
    mNotificationIcons->SetOverflowIndicator(mMoreIcon);

    view = NULL;
    statusBarView->FindViewById(R::id::status_bar_contents, (IView**)&view);
    mStatusBarContents = ILinearLayout::Probe(view);

    view = NULL;
    statusBarWindow->FindViewById(R::id::notification_stack_scroller, (IView**)&view);
    mStackScroller = INotificationStackScrollLayout::Probe(view);
    mStackScroller->SetLongPressListener(GetNotificationLongClicker());
    mStackScroller->SetPhoneStatusBar(this);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);

    view = NULL;
    inflater->Inflate(R::layout::status_bar_notification_keyguard_overflow,
        IViewGroup::Probe(mStackScroller), FALSE, (IView**)&view);
    mKeyguardIconOverflowContainer = INotificationOverflowContainer::Probe(view);
    IActivatableNotificationView::Probe(mKeyguardIconOverflowContainer)->SetOnActivatedListener(this);
    view->SetOnClickListener(mOverflowClickListener);
    IViewGroup::Probe(mStackScroller)->AddView(view);

    view = NULL;
    inflater->Inflate(R::layout::status_bar_notification_speed_bump,
            IViewGroup::Probe(mStackScroller), FALSE, (IView**)&view);
    AutoPtr<ISpeedBumpView> speedBump = ISpeedBumpView::Probe(view);
    mStackScroller->SetSpeedBumpView(speedBump);

    view = NULL;
    inflater->Inflate(R::layout::status_bar_no_notifications,
            IViewGroup::Probe(mStackScroller), FALSE, (IView**)&view);
    mEmptyShadeView = IEmptyShadeView::Probe(view);
    mStackScroller->SetEmptyShadeView(mEmptyShadeView);

    view = NULL;
    inflater->Inflate(R::layout::status_bar_notification_dismiss_all,
            IViewGroup::Probe(mStackScroller), FALSE, (IView**)&view);
    mDismissView = IDismissView::Probe(view);

    AutoPtr<DismissViewOnClickListener> cl1 = new DismissViewOnClickListener(this);
    mDismissView->SetOnButtonClickListener(cl1);
    mStackScroller->SetDismissView(mDismissView);
    mExpandedContents = IView::Probe(mStackScroller);

    view = NULL;
    statusBarWindow->FindViewById(R::id::backdrop, (IView**)&view);
    mBackdrop = IBackDropView::Probe(view);

    view = NULL;
    IView::Probe(mBackdrop)->FindViewById(R::id::backdrop_front, (IView**)&view);
    mBackdropFront = IImageView::Probe(view);

    view = NULL;
    IView::Probe(mBackdrop)->FindViewById(R::id::backdrop_back, (IView**)&view);
    mBackdropBack = IImageView::Probe(view);

    view = NULL;
    statusBarWindow->FindViewById(R::id::scrim_behind, (IView**)&view);
    AutoPtr<IScrimView> scrimBehind = IScrimView::Probe(view);
    view = NULL;
    statusBarWindow->FindViewById(R::id::scrim_in_front, (IView**)&view);
    AutoPtr<IScrimView> scrimInFront = IScrimView::Probe(view);
    mScrimController = new ScrimController(scrimBehind, scrimInFront, mScrimSrcModeEnabled);
    mScrimController->SetBackDropView(mBackdrop);
    mStatusBarView->SetScrimController(mScrimController);

    view = NULL;
    statusBarWindow->FindViewById(R::id::header, (IView**)&view);
    mHeader = IStatusBarHeaderView::Probe(view);
    mHeader->SetActivityStarter(this);

    view = NULL;
    statusBarWindow->FindViewById(R::id::keyguard_header, (IView**)&view);
    mKeyguardStatusBar = IKeyguardStatusBarView::Probe(view);

    view = NULL;
    IView::Probe(mKeyguardStatusBar)->FindViewById(R::id::statusIcons, (IView**)&view);
    mStatusIconsKeyguard = ILinearLayout::Probe(view);
    statusBarWindow->FindViewById(R::id::keyguard_status_view, (IView**)&mKeyguardStatusView);

    view = NULL;
    statusBarWindow->FindViewById(R::id::keyguard_bottom_area, (IView**)&view);
    mKeyguardBottomArea = IKeyguardBottomAreaView::Probe(view);
    mKeyguardBottomArea->SetActivityStarter(this);

    view = NULL;
    statusBarWindow->FindViewById(R::id::keyguard_indication_text, (IView**)&view);
    mKeyguardIndicationController = new KeyguardIndicationController();
    mKeyguardIndicationController->constructor(mContext, IKeyguardIndicationTextView::Probe(view));
    mKeyguardBottomArea->SetKeyguardIndicationController(mKeyguardIndicationController);

    res->GetBoolean(R::bool_::enable_ticker, &mTickerEnabled);
    if (mTickerEnabled) {
        view = NULL;
        statusBarView->FindViewById(R::id::ticker_stub, (IView**)&view);
        AutoPtr<IViewStub> tickerStub = IViewStub::Probe(view);
        if (tickerStub != NULL) {
            tickerStub->Inflate((IView**)&mTickerView);
            mTicker = new MyTicker(this, context, statusBarView);

            view = NULL;
            statusBarView->FindViewById(R::id::tickerText, (IView**)&view);
            AutoPtr<ITickerView> tickerView = ITickerView::Probe(view);
            tickerView->SetTicker((ITicker*)mTicker->Probe(EIID_ITicker));
        }
    }

    res->GetDimensionPixelSize(R::dimen::status_bar_edge_ignore, &mEdgeBorder);

    // set the inital view visibility
    SetAreThereNotifications();

    // Other icons
    mLocationController = new LocationControllerImpl(); // will post a notification
    mLocationController->constructor(mContext);
    mBatteryController = new BatteryController(mContext);

    AutoPtr<BatteryStateChangeCallback> bcc = new BatteryStateChangeCallback(this);
    mBatteryController->AddStateChangedCallback(bcc);
    mNetworkController = new NetworkControllerImpl(mContext);
    mHotspotController = new HotspotControllerImpl(mContext);
    mBluetoothController = new BluetoothControllerImpl(mContext);
    mSecurityController = new SecurityControllerImpl(mContext);
    if (res->GetBoolean(R::bool_::config_showRotationLock, &tmp), tmp) {
        mRotationLockController = new RotationLockControllerImpl(mContext);
    }
    mUserInfoController = new UserInfoController(mContext);
    mVolumeComponent = IVolumeComponent::Probe(GetComponent(String("EIID_IVolumeComponent")/*.class*/));
    mVolumeComponent->GetZenController((IZenModeController**)&mZenModeController);
    mCastController = new CastControllerImpl(mContext);

    view = NULL;
    statusBarView->FindViewById(R::id::signal_cluster, (IView**)&view);
    AutoPtr<ISignalClusterView> signalCluster = ISignalClusterView::Probe(view);

    view = NULL;
    IView::Probe(mKeyguardStatusBar)->FindViewById(R::id::signal_cluster, (IView**)&view);
    AutoPtr<ISignalClusterView> signalClusterKeyguard = ISignalClusterView::Probe(view);

    view = NULL;
    IView::Probe(mHeader)->FindViewById(R::id::signal_cluster, (IView**)&view);
    AutoPtr<ISignalClusterView> signalClusterQs = ISignalClusterView::Probe(view);
    mNetworkController->AddSignalCluster(INetworkControllerImplSignalCluster::Probe(signalCluster));
    mNetworkController->AddSignalCluster(INetworkControllerImplSignalCluster::Probe(signalClusterKeyguard));
    mNetworkController->AddSignalCluster(INetworkControllerImplSignalCluster::Probe(signalClusterQs));
    signalCluster->SetSecurityController(mSecurityController);
    signalCluster->SetNetworkController(mNetworkController);
    signalClusterKeyguard->SetSecurityController(mSecurityController);
    signalClusterKeyguard->SetNetworkController(mNetworkController);
    signalClusterQs->SetSecurityController(mSecurityController);
    signalClusterQs->SetNetworkController(mNetworkController);
    Boolean isAPhone = FALSE;
    mNetworkController->HasVoiceCallingFeature(&isAPhone);
    if (isAPhone) {
        mNetworkController->AddEmergencyLabelView(mHeader);
    }

    view = NULL;
    statusBarWindow->FindViewById(R::id::carrier_label, (IView**)&view);
    mCarrierLabel = ITextView::Probe(view);
    mShowCarrierInPanel = (mCarrierLabel != NULL);
    if (DEBUG) {
        AutoPtr<ICharSequence> t;
        mCarrierLabel->GetText((ICharSequence**)&t);
        Logger::V(TAG, "carrierlabel=%s show=%d", TO_CSTR(t), mShowCarrierInPanel);
    }
    if (mShowCarrierInPanel) {
        IView::Probe(mCarrierLabel)->SetVisibility(mCarrierLabelVisible ? IView::VISIBLE : IView::INVISIBLE);

        // for mobile devices, we always show mobile connection info here (SPN/PLMN)
        // for other devices, we show whatever network is connected
        if (mNetworkController->HasMobileDataFeature(&tmp), tmp) {
            mNetworkController->AddMobileLabelView(mCarrierLabel);
        }
        else {
            mNetworkController->AddCombinedLabelView(mCarrierLabel);
        }

        // set up the dynamic hide/show of the label
        // TODO: uncomment, handle this for the Stack scroller aswell
//                ((NotificationRowLayout) mStackScroller)
// .setOnSizeChangedListener(new OnSizeChangedListener() {
//                @Override
//                public void onSizeChanged(View view, Int32 w, Int32 h, Int32 oldw, Int32 oldh) {
//                    UpdateCarrierLabelVisibility(FALSE);
    }

    mFlashlightController = new FlashlightController(mContext);
    mKeyguardBottomArea->SetFlashlightController(mFlashlightController);
    mKeyguardBottomArea->SetPhoneStatusBar(this);
    mAccessibilityController = new AccessibilityController(mContext);
    mKeyguardBottomArea->SetAccessibilityController(mAccessibilityController);
    mNextAlarmController = new NextAlarmController(mContext);
    mKeyguardMonitor = new KeyguardMonitor();
    mUserSwitcherController = new UserSwitcherController();
    mUserSwitcherController->constructor(mContext, mKeyguardMonitor);

    view = NULL;
    statusBarWindow->FindViewById(R::id::keyguard_user_switcher, (IView**)&view);
    mKeyguardUserSwitcher = new KeyguardUserSwitcher(mContext, IViewStub::Probe(view),
            mKeyguardStatusBar, mNotificationPanel, mUserSwitcherController);

    // Set up the quick settings tile panel
    view = NULL;
    statusBarWindow->FindViewById(R::id::quick_settings_panel, (IView**)&view);
    mQSPanel = IQSPanel::Probe(view);
    if (mQSPanel != NULL) {
        AutoPtr<IQSTileHost> qsh = new QSTileHost(mContext, this,
                mBluetoothController, mLocationController, mRotationLockController,
                mNetworkController, mZenModeController, mHotspotController,
                mCastController, mFlashlightController,
                mUserSwitcherController, mKeyguardMonitor,
                mSecurityController);
        mQSPanel->SetHost((IPhoneQSTileHost*)qsh->Probe(EIID_IPhoneQSTileHost));

        AutoPtr<ICollection> c;
        qsh->GetTiles((ICollection**)&c);
        mQSPanel->SetTiles(c);

        mBrightnessMirrorController = new BrightnessMirrorController();
        mBrightnessMirrorController->constructor(mStatusBarWindow);
        mQSPanel->SetBrightnessMirror(mBrightnessMirrorController);
        mHeader->SetQSPanel(mQSPanel);

        AutoPtr<HostCallback> hc = new HostCallback(this, qsh);
        qsh->SetCallback(hc);
    }

    // User info. Trigger first load.
    mHeader->SetUserInfoController(mUserInfoController);
    mKeyguardStatusBar->SetUserInfoController(mUserInfoController);
    mUserInfoController->ReloadUserInfo();

    mHeader->SetBatteryController(mBatteryController);

    view = NULL;
    statusBarView->FindViewById(R::id::battery, (IView**)&view);
    IBatteryMeterView::Probe(view)->SetBatteryController(mBatteryController);
    mKeyguardStatusBar->SetBatteryController(mBatteryController);
    mHeader->SetNextAlarmController(mNextAlarmController);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);

    AutoPtr<IIntent> i;
    CIntent::New((pm->IsScreenOn(&tmp), tmp) ? IIntent::ACTION_SCREEN_ON : IIntent::ACTION_SCREEN_OFF
            , (IIntent**)&i);
    mBroadcastReceiver->OnReceive(mContext, i);

    // receive broadcasts
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IIntent::ACTION_SCREEN_ON);
    if (DEBUG_MEDIA_FAKE_ARTWORK) {
        filter->AddAction(String("fake_artwork"));
    }
    filter->AddAction(ACTION_DEMO);
    i = NULL;
    context->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&i);

    // listen for USER_SETUP_COMPLETE setting (per-user)
    ResetUserSetupObserver();

    StartGlyphRasterizeHack();
    return mStatusBarView;
}

void CPhoneStatusBar::ClearAllNotifications()
{
    // animate-swipe all dismissable notifications, then animate the shade closed
    Int32 numChildren = 0;
    IViewGroup::Probe(mStackScroller)->GetChildCount(&numChildren);

    Boolean tmp = FALSE;
    AutoPtr<IArrayList> viewsToHide;  /*<View*/
    CArrayList::New(numChildren, (IArrayList**)&viewsToHide);
    for (Int32 i = 0; i < numChildren; i++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mStackScroller)->GetChildAt(i, (IView**)&child);
        if (mStackScroller->CanChildBeDismissed(child, &tmp), tmp) {
            Int32 v = 0;
            if ((child->GetVisibility(&v), v) == IView::VISIBLE) {
                viewsToHide->Add(child);
            }
        }
    }

    if (viewsToHide->IsEmpty(&tmp), tmp) {
        AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
        return;
    }

    AutoPtr<ClearAllNotificationRunnable> run = new ClearAllNotificationRunnable(this);
    AddPostCollapseAction(run);

    PerformDismissAllAnimations(viewsToHide);
}

void CPhoneStatusBar::PerformDismissAllAnimations(
    /* [in] */ IArrayList/*<View>*/* hideAnimatedList)
{
    AutoPtr<CollapsePanelRunnable> animationFinishAction = new CollapsePanelRunnable(this);

    // let's disable our normal animations
    mStackScroller->SetDismissAllInProgress(TRUE);

    // Decrease the delay for every row we animate to give the sense of
    // accelerating the swipes
    Int32 rowDelayDecrement = 10;
    Int32 currentDelay = 140;
    Int32 totalDelay = 0;
    Int32 numItems = 0;
    hideAnimatedList->GetSize(&numItems);
    for (Int32 i = 0; i < numItems; i++) {
        AutoPtr<IInterface> obj;
        hideAnimatedList->Get(i, (IInterface**)&obj);
        AutoPtr<IView> view = IView::Probe(obj);
        AutoPtr<IRunnable> endRunnable;
        if (i == numItems - 1) {
            endRunnable = animationFinishAction;
        }
        mStackScroller->DismissViewAnimated(view, endRunnable, totalDelay, 260);
        currentDelay = Elastos::Core::Math::Max(50, currentDelay - rowDelayDecrement);
        totalDelay += currentDelay;
    }
}

/**
 * Hack to improve glyph rasterization for scaled text views.
 */
void CPhoneStatusBar::StartGlyphRasterizeHack()
{
    AutoPtr<IViewTreeObserver> vto;
    IView::Probe(mStatusBarView)->GetViewTreeObserver((IViewTreeObserver**)&vto);

    AutoPtr<OnPreDrawListener> dl = new OnPreDrawListener(this);
    vto->AddOnPreDrawListener(dl);
}

void CPhoneStatusBar::SetZenMode(
    /* [in] */ Int32 mode)
{
    BaseStatusBar::SetZenMode(mode);
    if (mIconPolicy != NULL) {
        mIconPolicy->SetZenMode(mode);
    }
}

void CPhoneStatusBar::StartKeyguard()
{
    AutoPtr<IKeyguardViewMediator> keyguardViewMediator =
        IKeyguardViewMediator::Probe(GetComponent(String("EIID_IKeyguardViewMediator")));
    keyguardViewMediator->RegisterStatusBar(this,
        IViewGroup::Probe(mStatusBarWindow), mStatusBarWindowManager, mScrimController,
        (IStatusBarKeyguardViewManager**)&mStatusBarKeyguardViewManager);
    keyguardViewMediator->GetViewMediatorCallback((IViewMediatorCallback**)&mKeyguardViewMediatorCallback);
}

AutoPtr<IView> CPhoneStatusBar::GetStatusBarView()
{
    return IView::Probe(mStatusBarView);
}

ECode CPhoneStatusBar::GetStatusBarWindow(
    /* [out] */ IStatusBarWindowView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mStatusBarWindow;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

AutoPtr<IWindowManagerLayoutParams> CPhoneStatusBar::GetSearchLayoutParams(
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    Boolean opaque = FALSE;
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL,
            IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM
            | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            (opaque ? IPixelFormat::OPAQUE : IPixelFormat::TRANSLUCENT),
            (IWindowManagerLayoutParams**)&lp);

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Boolean tmp = FALSE;
    if (helper->IsHighEndGfx(&tmp), tmp) {
        Int32 flags = 0;
        lp->GetFlags(&flags);
        lp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED);
    }
    lp->SetGravity(IGravity::BOTTOM | IGravity::START);

    AutoPtr<ICharSequence> t;
    CString::New(String("SearchPanel"), (ICharSequence**)&t);
    lp->SetTitle(t);
    lp->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNCHANGED
        | IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_NOTHING);
    return lp;
}

void CPhoneStatusBar::UpdateSearchPanel()
{
    BaseStatusBar::UpdateSearchPanel();
    if (mNavigationBarView != NULL) {
        mNavigationBarView->SetDelegateView(IView::Probe(mSearchPanelView));
    }
}

ECode CPhoneStatusBar::ShowSearchPanel()
{
    BaseStatusBar::ShowSearchPanel();
    mHandler->RemoveCallbacks(mShowSearchPanel);

    // we want to freeze the sysui state wherever it is
    IView::Probe(mSearchPanelView)->SetSystemUiVisibility(mSystemUiVisibility);

    if (mNavigationBarView != NULL) {
        AutoPtr<IViewGroupLayoutParams> vp;
        IView::Probe(mNavigationBarView)->GetLayoutParams((IViewGroupLayoutParams**)&vp);
        AutoPtr<IWindowManagerLayoutParams> lp = IWindowManagerLayoutParams::Probe(vp);

        Int32 flags = 0;
        lp->GetFlags(&flags);
        lp->SetFlags(flags & ~IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL);
        IViewManager::Probe(mWindowManager)->UpdateViewLayout(IView::Probe(mNavigationBarView)
                , IViewGroupLayoutParams::Probe(lp));
    }
    return NOERROR;
}

ECode CPhoneStatusBar::HideSearchPanel()
{
    BaseStatusBar::HideSearchPanel();
    if (mNavigationBarView != NULL) {
        AutoPtr<IViewGroupLayoutParams> vp;
        IView::Probe(mNavigationBarView)->GetLayoutParams((IViewGroupLayoutParams**)&vp);
        AutoPtr<IWindowManagerLayoutParams> lp = IWindowManagerLayoutParams::Probe(vp);

        Int32 flags = 0;
        lp->GetFlags(&flags);
        lp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL);
        IViewManager::Probe(mWindowManager)->UpdateViewLayout(IView::Probe(mNavigationBarView)
                , IViewGroupLayoutParams::Probe(lp));
    }
    return NOERROR;
}

ECode CPhoneStatusBar::GetStatusBarHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mNaturalBarHeight < 0) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetDimensionPixelSize(Elastos::Droid::R::dimen::status_bar_height, &mNaturalBarHeight);
    }
    *result = mNaturalBarHeight;
    return NOERROR;
}

void CPhoneStatusBar::AwakenDreams()
{
    if (mDreamManager != NULL) {
        mDreamManager->Awaken();
    }
}

void CPhoneStatusBar::PrepareNavigationBarView()
{
    mNavigationBarView->Reorient();

    AutoPtr<IView> view;
    mNavigationBarView->GetRecentsButton((IView**)&view);
    view->SetOnClickListener(mRecentsClickListener);
    view->SetOnTouchListener(mRecentsPreloadOnTouchListener);
    view->SetLongClickable(TRUE);
    view->SetOnLongClickListener(mLongPressBackRecentsListener);

    view = NULL;
    mNavigationBarView->GetBackButton((IView**)&view);
    view->SetLongClickable(TRUE);
    view->SetOnLongClickListener(mLongPressBackRecentsListener);

    view = NULL;
    mNavigationBarView->GetHomeButton((IView**)&view);
    view->SetOnTouchListener(mHomeActionListener);
    UpdateSearchPanel();
}

// For small-screen devices (read: phones) that lack hardware navigation buttons
void CPhoneStatusBar::AddNavigationBar()
{
    if (DEBUG) Logger::V(TAG, "AddNavigationBar: about to add %s", TO_CSTR(mNavigationBarView));
    if (mNavigationBarView == NULL) return;

    PrepareNavigationBarView();

    AutoPtr<IWindowManagerLayoutParams> wp = GetNavigationBarLayoutParams();
    IViewManager::Probe(mWindowManager)->AddView(IView::Probe(mNavigationBarView),
        IViewGroupLayoutParams::Probe(wp));
}

void CPhoneStatusBar::RepositionNavigationBar()
{
    Boolean tmp = FALSE;
    if (mNavigationBarView == NULL || (IView::Probe(mNavigationBarView)->IsAttachedToWindow(&tmp), !tmp)) return;

    PrepareNavigationBarView();

    AutoPtr<IWindowManagerLayoutParams> wp = GetNavigationBarLayoutParams();
    IViewManager::Probe(mWindowManager)->UpdateViewLayout(IView::Probe(mNavigationBarView), IViewGroupLayoutParams::Probe(wp));
}

void CPhoneStatusBar::NotifyNavigationBarScreenOn(
    /* [in] */ Boolean screenOn)
{
    if (mNavigationBarView == NULL) return;
    mNavigationBarView->NotifyScreenOn(screenOn);
}

AutoPtr<IWindowManagerLayoutParams> CPhoneStatusBar::GetNavigationBarLayoutParams()
{
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR,
                0
                | IWindowManagerLayoutParams::FLAG_TOUCHABLE_WHEN_WAKING
                | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
                | IWindowManagerLayoutParams::FLAG_WATCH_OUTSIDE_TOUCH
                | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);
    // this will allow the navbar to run in an overlay on devices that support this
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Boolean tmp = FALSE;
    if (helper->IsHighEndGfx(&tmp), !tmp) {
        Int32 flags = 0;
        lp->GetFlags(&flags);
        lp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED);
    }

    AutoPtr<ICharSequence> t;
    CString::New(String("NavigationBar"), (ICharSequence**)&t);
    lp->SetTitle(t);
    lp->SetWindowAnimations(0);
    return lp;
}

void CPhoneStatusBar::AddHeadsUpView()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 headsUpHeight = 0;
    res->GetDimensionPixelSize(R::dimen::heads_up_window_height, &headsUpHeight);
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT, headsUpHeight,
            IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL, // above the status bar!
            IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
                | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS
                | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
                | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM
                | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
            IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);


    Int32 flags = 0;
    lp->GetFlags(&flags);
    lp->SetFlags(flags | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED);
    lp->SetGravity(IGravity::TOP);

    AutoPtr<ICharSequence> t;
    CString::New(String("Heads Up"), (ICharSequence**)&t);
    lp->SetTitle(t);

    String name;
    mContext->GetPackageName(&name);
    lp->SetPackageName(name);
    lp->SetWindowAnimations(R::style::Animation_StatusBar_HeadsUp);

    IViewManager::Probe(mWindowManager)->AddView(IView::Probe(mHeadsUpNotificationView),
            IViewGroupLayoutParams::Probe(lp));
}

void CPhoneStatusBar::RemoveHeadsUpView()
{
    IViewManager::Probe(mWindowManager)->RemoveView(IView::Probe(mHeadsUpNotificationView));
}

ECode CPhoneStatusBar::RefreshAllStatusBarIcons()
{
    RefreshAllIconsForLayout(mStatusIcons);
    RefreshAllIconsForLayout(mStatusIconsKeyguard);
    RefreshAllIconsForLayout(ILinearLayout::Probe(mNotificationIcons));
    return NOERROR;
}

void CPhoneStatusBar::RefreshAllIconsForLayout(
    /* [in] */ ILinearLayout* ll)
{
    Int32 count = 0;
    IViewGroup::Probe(ll)->GetChildCount(&count);
    for (Int32 n = 0; n < count; n++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(ll)->GetChildAt(n, (IView**)&child);
        if (IStatusBarIconView::Probe(child)) {
            IStatusBarIconView::Probe(child)->UpdateDrawable();
        }
    }
}

ECode CPhoneStatusBar::AddIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* icon)
{
    if (SPEW) Logger::D(TAG, "addIcon slot=%s index=%d viewIndex=%d icon=%s",
            slot.string(), index, viewIndex, TO_CSTR(icon));
    AutoPtr<IStatusBarIconView> view;
    CStatusBarIconView::New(mContext, slot, NULL, (IStatusBarIconView**)&view);
    Boolean tmp = FALSE;
    view->Set(icon, &tmp);
    AutoPtr<IViewGroupLayoutParams> vp;
    CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, mIconSize,
            (IViewGroupLayoutParams**)&vp);
    IViewGroup::Probe(mStatusIcons)->AddView(IView::Probe(view), viewIndex, vp);

    view = NULL;
    CStatusBarIconView::New(mContext, slot, NULL, (IStatusBarIconView**)&view);
    view->Set(icon, &tmp);

    vp = NULL;
    CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, mIconSize,
            (IViewGroupLayoutParams**)&vp);
    IViewGroup::Probe(mStatusIconsKeyguard)->AddView(IView::Probe(view), viewIndex, vp);
    return NOERROR;
}

ECode CPhoneStatusBar::UpdateIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* old,
    /* [in] */ IStatusBarIcon* icon)
{
    if (SPEW) Logger::D(TAG, "updateIcon slot=%s index=%d viewIndex=%d old=%s icon=%s",
             slot.string(), index, viewIndex, TO_CSTR(old), TO_CSTR(icon));

    AutoPtr<IView> v;
    IViewGroup::Probe(mStatusIcons)->GetChildAt(viewIndex, (IView**)&v);
    AutoPtr<IStatusBarIconView> view = IStatusBarIconView::Probe(v);
    Boolean tmp = FALSE;
    view->Set(icon, &tmp);

    v = NULL;
    IViewGroup::Probe(mStatusIconsKeyguard)->GetChildAt(viewIndex, (IView**)&v);
    view = IStatusBarIconView::Probe(v);
    view->Set(icon, &tmp);
    return NOERROR;
}

ECode CPhoneStatusBar::RemoveIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex)
{
    if (SPEW) Logger::D(TAG, "removeIcon slot=%s index=%d viewIndex=%d", slot.string(), index, viewIndex);
    IViewGroup::Probe(mStatusIcons)->RemoveViewAt(viewIndex);
    IViewGroup::Probe(mStatusIconsKeyguard)->RemoveViewAt(viewIndex);
    return NOERROR;
}

ECode CPhoneStatusBar::GetCurrentUserHandle(
    /* [out] */ IUserHandle** uh)
{
    VALIDATE_NOT_NULL(uh);
    return CUserHandle::New(mCurrentUserId, uh);
}

ECode CPhoneStatusBar::AddNotification(
    /* [in] */ IStatusBarNotification* notification,
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
    if (DEBUG) {
        String key;
        notification->GetKey(&key);
        Logger::D(TAG, "addNotification key=%s", key.string());
    }
    if (mUseHeadsUp && ShouldInterrupt(notification)) {
        if (DEBUG) Logger::D(TAG, "launching notification in heads up mode");
        AutoPtr<INotificationDataEntry> interruptionCandidate = new NotificationData::Entry(notification, NULL);
        AutoPtr<IViewGroup> holder;
        mHeadsUpNotificationView->GetHolder((IViewGroup**)&holder);
        if (InflateViewsForHeadsUp(interruptionCandidate, holder)) {
            // 1. Populate mHeadsUpNotificationView
            Boolean tmp = FALSE;
            mHeadsUpNotificationView->ShowNotification(interruptionCandidate, &tmp);

            // do not show the notification in the shade, yet.
            return NOERROR;
        }
    }

    AutoPtr<INotificationDataEntry> shadeEntry = CreateNotificationViews(notification);
    if (shadeEntry == NULL) {
        return NOERROR;
    }

    AutoPtr<INotification> n;
    notification->GetNotification((INotification**)&n);
    AutoPtr<IPendingIntent> pi;
    n->GetFullScreenIntent((IPendingIntent**)&pi);
    if (pi != NULL) {
        // Stop screensaver if the notification has a full-screen intent.
        // (like an incoming phone call)
        AwakenDreams();

        // not immersive & a full-screen alert should be shown
        if (DEBUG) Logger::D(TAG, "Notification has fullScreenIntent; sending fullScreenIntent");
        // try {
        pi->Send();
        // } catch (PendingIntent.CanceledException e) {
        // }
    }
    else {
        // usual case: status bar visible & not immersive

        // show the ticker if there isn't already a heads up
        AutoPtr<INotificationDataEntry> e;
        mHeadsUpNotificationView->GetEntry((INotificationDataEntry**)&e);
        if (e == NULL) {
            Tick(notification, TRUE);
        }
    }
    AddNotificationViews(shadeEntry, ranking);
    // Recalculate the position of the sliding windows and the titles.
    SetAreThereNotifications();
    UpdateExpandedViewPos(EXPANDED_LEAVE_ALONE);
    return NOERROR;
}

ECode CPhoneStatusBar::DisplayNotificationFromHeadsUp(
    /* [in] */ IStatusBarNotification* notification)
{
    AutoPtr<INotificationDataEntry> shadeEntry = CreateNotificationViews(notification);
    if (shadeEntry == NULL) {
        return NOERROR;
    }
    shadeEntry->SetInterruption();

    AddNotificationViews(shadeEntry, NULL);
    // Recalculate the position of the sliding windows and the titles.
    SetAreThereNotifications();
    UpdateExpandedViewPos(EXPANDED_LEAVE_ALONE);
    return NOERROR;
}

ECode CPhoneStatusBar::ResetHeadsUpDecayTimer()
{
    mHandler->RemoveMessages(MSG_DECAY_HEADS_UP);
    Boolean tmp = FALSE;
    if (mUseHeadsUp && mHeadsUpNotificationDecay > 0
            && (mHeadsUpNotificationView->IsClearable(&tmp), tmp)) {
        mHandler->SendEmptyMessageDelayed(MSG_DECAY_HEADS_UP, mHeadsUpNotificationDecay, &tmp);
    }
    return NOERROR;
}

ECode CPhoneStatusBar::ScheduleHeadsUpOpen()
{
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(MSG_SHOW_HEADS_UP, &tmp);
    return NOERROR;
}

ECode CPhoneStatusBar::ScheduleHeadsUpClose()
{
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(MSG_HIDE_HEADS_UP, &tmp);
    return NOERROR;
}

ECode CPhoneStatusBar::ScheduleHeadsUpEscalation()
{
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(MSG_ESCALATE_HEADS_UP, &tmp);
    return NOERROR;
}

ECode CPhoneStatusBar::UpdateNotificationRanking(
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
    mNotificationData->UpdateRanking(ranking);
    UpdateNotifications();
    return NOERROR;
}

ECode CPhoneStatusBar::RemoveNotification(
    /* [in] */ const String& key,
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
    if (ENABLE_HEADS_UP) {
        AutoPtr<INotificationDataEntry> e;
        mHeadsUpNotificationView->GetEntry((INotificationDataEntry**)&e);
        if (e != NULL) {
            AutoPtr<IStatusBarNotification> n;
            e->GetNotification((IStatusBarNotification**)&n);
            String k;
            if (key.Equals((n->GetKey(&k), k))) {
                mHeadsUpNotificationView->Clear();
            }
        }
    }

    AutoPtr<IStatusBarNotification> old = RemoveNotificationViews(key, ranking);
    if (SPEW) Logger::D(TAG, "removeNotification key=%s old=%s", key.string(), TO_CSTR(old));

    if (old != NULL) {
        // Cancel the ticker if it's still running
        if (mTickerEnabled) {
            mTicker->RemoveEntry(old);
        }

        // Recalculate the position of the sliding windows and the titles.
        UpdateExpandedViewPos(EXPANDED_LEAVE_ALONE);

        Boolean tmp = FALSE;
        if (CLOSE_PANEL_WHEN_EMPTIED && (HasActiveNotifications(&tmp), !tmp)
                && (IPanelView::Probe(mNotificationPanel)->IsTracking(&tmp), !tmp)
                && (mNotificationPanel->IsQsExpanded(&tmp), !tmp)) {
            if (mState == IStatusBarState::SHADE) {
                AnimateCollapsePanels();
            }
            else if (mState == IStatusBarState::SHADE_LOCKED) {
                GoToKeyguard();
            }
        }
    }
    SetAreThereNotifications();
    return NOERROR;
}

void CPhoneStatusBar::RefreshLayout(
    /* [in] */ Int32 layoutDirection)
{
    if (mNavigationBarView != NULL) {
        IView::Probe(mNavigationBarView)->SetLayoutDirection(layoutDirection);
    }
    RefreshAllStatusBarIcons();
}

void CPhoneStatusBar::UpdateShowSearchHoldoff()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(R::integer::config_show_search_delay, &mShowSearchHoldoff);
}

void CPhoneStatusBar::UpdateNotificationShade()
{
    if (mStackScroller == NULL) return;

    // Do not modify the notifications during collapse.
    Boolean tmp = FALSE;
    if (IsCollapsing(&tmp), tmp) {
        AutoPtr<UpdateNotificationShadeRunnable> run = new UpdateNotificationShadeRunnable(this);
        AddPostCollapseAction(run);
        return;
    }

    AutoPtr<IArrayList> activeNotifications;  /*<INotificationDataEntry*/
    mNotificationData->GetActiveNotifications((IArrayList**)&activeNotifications);
    Int32 size = 0;
    activeNotifications->GetSize(&size);
    AutoPtr<IArrayList> toShow;  /*<IExpandableNotificationRow*/
    CArrayList::New(size, (IArrayList**)&toShow);
    Int32 N = size;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        activeNotifications->Get(i, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> ent = INotificationDataEntry::Probe(obj);

        AutoPtr<IStatusBarNotification> notification;
        ent->GetNotification((IStatusBarNotification**)&notification);
        AutoPtr<INotification> n;
        notification->GetNotification((INotification**)&n);
        Int32 vis = 0;
        n->GetVisibility(&vis);

        // Display public version of the notification if we need to redact.
        Int32 id = 0;
        notification->GetUserId(&id);
        Boolean hideSensitive = FALSE;
        UserAllowsPrivateNotificationsInPublic(id, &hideSensitive);
        hideSensitive = !hideSensitive;
        Boolean sensitiveNote = vis == INotification::VISIBILITY_PRIVATE;
        String key;
        notification->GetKey(&key);
        Boolean sensitivePackage = PackageHasVisibilityOverride(key);
        Boolean sensitive = (sensitiveNote && hideSensitive) || sensitivePackage;
        Boolean showingPublic = sensitive && (IsLockscreenPublicMode(&tmp), tmp);

        AutoPtr<IExpandableNotificationRow> row;
        ent->GetRow((IExpandableNotificationRow**)&row);
        row->SetSensitive(sensitive);

        Boolean autoRedacted = FALSE, legacy = FALSE;
        ent->GetAutoRedacted(&autoRedacted);
        ent->GetLegacy(&legacy);
        if (autoRedacted && legacy) {
            // TODO: Also fade this? Or, maybe easier (and better), provide a dark redacted form
            // for legacy auto redacted notifications.
            if (showingPublic) {
                IActivatableNotificationView::Probe(row)->SetShowingLegacyBackground(FALSE);
            }
            else {
                IActivatableNotificationView::Probe(row)->SetShowingLegacyBackground(TRUE);
            }
        }
        toShow->Add(row);
    }

    AutoPtr<IArrayList> toRemove;  /*<View*/
    CArrayList::New((IArrayList**)&toRemove);
    Int32 count = 0;
    IViewGroup::Probe(mStackScroller)->GetChildCount(&count);
    for (Int32 i = 0; i< count; i++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mStackScroller)->GetChildAt(i, (IView**)&child);
        if ((toShow->Contains(child, &tmp), !tmp) && IExpandableNotificationRow::Probe(child)) {
            toRemove->Add(child);
        }
    }

    AutoPtr<IIterator> ator;
    toRemove->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> remove;
        ator->GetNext((IInterface**)&remove);
        IViewGroup::Probe(mStackScroller)->RemoveView(IView::Probe(remove));
    }

    toShow->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        toShow->Get(i, (IInterface**)&obj);
        AutoPtr<IView> v = IView::Probe(obj);
        AutoPtr<IViewParent> vp;
        v->GetParent((IViewParent**)&vp);
        if (vp == NULL) {
            IViewGroup::Probe(mStackScroller)->AddView(v);
        }
    }

    // So after all this work notifications still aren't sorted correctly.
    // Let's do that now by advancing through toShow and mStackScroller in
    // lock-step, making sure mStackScroller matches what we see in toShow.
    Int32 j = 0;
    IViewGroup::Probe(mStackScroller)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mStackScroller)->GetChildAt(i, (IView**)&child);
        if (IExpandableNotificationRow::Probe(child) == NULL) {
            // We don't care about non-notification views.
            continue;
        }

        AutoPtr<IInterface> obj;
        toShow->Get(j, (IInterface**)&obj);
        if (child.Get() == IView::Probe(obj)) {
            // Everything is well, advance both lists.
            j++;
            continue;
        }

        // Oops, wrong notification at this position. Put the right one
        // here and advance both lists.
        mStackScroller->ChangeViewPosition(IView::Probe(obj), i);
        j++;
    }
    UpdateRowStates();
    UpdateSpeedbump();
    UpdateClearAll();
    UpdateEmptyShadeView();

    // Disable QS if device not provisioned.
    // If the user switcher is simple then disable QS during setup because
    // the user intends to use the lock screen user switcher, QS in not needed.
    Boolean is = FALSE;
    mNotificationPanel->SetQsExpansionEnabled((IsDeviceProvisioned(&tmp), tmp)
            && ((mUserSwitcherController->IsSimpleUserSwitcher(&is), !is) || mUserSetup));
    mShadeUpdates->Check();
}

Boolean CPhoneStatusBar::PackageHasVisibilityOverride(
    /* [in] */ const String& key)
{
    Int32 v = 0;
    mNotificationData->GetVisibilityOverride(key, &v);
    return v != INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE;
}

void CPhoneStatusBar::UpdateClearAll()
{
    Boolean has = FALSE;
    Boolean showDismissView = mState != IStatusBarState::KEYGUARD
            && (mNotificationData->HasActiveClearableNotifications(&has), has);
    mStackScroller->UpdateDismissView(showDismissView);
}

void CPhoneStatusBar::UpdateEmptyShadeView()
{
    Boolean showEmptyShade = FALSE;
    if (mState != IStatusBarState::KEYGUARD) {
        AutoPtr<IArrayList> list;
        mNotificationData->GetActiveNotifications((IArrayList**)&list);
        Int32 size = 0;
        list->GetSize(&size);
        showEmptyShade = size == 0;
    }
    mNotificationPanel->SetShadeEmpty(showEmptyShade);
}

void CPhoneStatusBar::UpdateSpeedbump()
{
    Int32 speedbumpIndex = -1;
    Int32 currentIndex = 0;
    AutoPtr<IArrayList> activeNotifications;  /*<INotificationDataEntry*/
    mNotificationData->GetActiveNotifications((IArrayList**)&activeNotifications);
    Int32 N = 0;
    activeNotifications->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        activeNotifications->Get(i, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> entry = INotificationDataEntry::Probe(obj);

        AutoPtr<IExpandableNotificationRow> row;
        entry->GetRow((IExpandableNotificationRow**)&row);
        Int32 v = 0;
        if ((IView::Probe(row)->GetVisibility(&v), v) != IView::GONE) {
            String key;
            entry->GetKey(&key);
            Boolean tmp = FALSE;
            if (mNotificationData->IsAmbient(key, &tmp), tmp) {
                speedbumpIndex = currentIndex;
                break;
            }
        }
        currentIndex++;
    }
    mStackScroller->UpdateSpeedBumpIndex(speedbumpIndex);
}

void CPhoneStatusBar::UpdateNotifications()
{
    // TODO: Move this into updateNotificationIcons()?
    if (mNotificationIcons == NULL) return;

    mNotificationData->FilterAndSort();

    UpdateNotificationShade();
    UpdateNotificationIcons();
}

void CPhoneStatusBar::UpdateNotificationIcons()
{
    AutoPtr<ILinearLayoutLayoutParams> params;
    CLinearLayoutLayoutParams::New(mIconSize + 2 * mIconHPadding, mNaturalBarHeight
            , (ILinearLayoutLayoutParams**)&params);

    AutoPtr<IArrayList> activeNotifications;  /*<INotificationDataEntry*/
    mNotificationData->GetActiveNotifications((IArrayList**)&activeNotifications);
    Int32 N = 0;
    activeNotifications->GetSize(&N);
    AutoPtr<IArrayList> toShow;  /*<StatusBarIconView*/
    CArrayList::New(N, (IArrayList**)&toShow);

    // Filter out notifications with low scores.
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        activeNotifications->Get(i, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> ent = INotificationDataEntry::Probe(obj);
        AutoPtr<IStatusBarNotification> notification;
        ent->GetNotification((IStatusBarNotification**)&notification);
        Int32 score = 0;
        notification->GetScore(&score);
        if (score < HIDE_ICONS_BELOW_SCORE &&
                NotificationData::ShowNotificationEvenIfUnprovisioned(notification)) {
            continue;
        }

        AutoPtr<IStatusBarIconView> icon;
        ent->GetIcon((IStatusBarIconView**)&icon);
        toShow->Add(icon);
    }

    if (DEBUG) {
        Int32 size = 0;
        toShow->GetSize(&size);
        Logger::D(TAG, "refreshing icons: %d notifications, mNotificationIcons=%s", size, TO_CSTR(mNotificationIcons));
    }

    AutoPtr<IArrayList> toRemove;  /*<View*/
    CArrayList::New((IArrayList**)&toRemove);
    Int32 count = 0;
    IViewGroup::Probe(mNotificationIcons)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mNotificationIcons)->GetChildAt(i, (IView**)&child);
        Boolean tmp = FALSE;
        if (toShow->Contains(child, &tmp), !tmp) {
            toRemove->Add(child);
        }
    }

    Int32 toRemoveCount = 0;
    toRemove->GetSize(&toRemoveCount);
    for (Int32 i = 0; i < toRemoveCount; i++) {
        AutoPtr<IInterface> obj;
        toRemove->Get(i, (IInterface**)&obj);
        IViewGroup::Probe(mNotificationIcons)->RemoveView(IView::Probe(obj));
    }

    toShow->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        toShow->Get(i, (IInterface**)&obj);
        AutoPtr<IView> v = IView::Probe(obj);
        AutoPtr<IViewParent> vp;
        v->GetParent((IViewParent**)&vp);
        if (vp == NULL) {
            IViewGroup::Probe(mNotificationIcons)->AddView(v, i, IViewGroupLayoutParams::Probe(params));
        }
    }

    // Resort notification icons
    Int32 childCount = 0;
    IViewGroup::Probe(mNotificationIcons)->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> actual;
        IViewGroup::Probe(mNotificationIcons)->GetChildAt(i, (IView**)&actual);

        AutoPtr<IInterface> obj;
        toShow->Get(i, (IInterface**)&obj);
        AutoPtr<IStatusBarIconView> expected = IStatusBarIconView::Probe(obj);
        if (actual.Get() == IView::Probe(expected)) {
            continue;
        }
        IViewGroup::Probe(mNotificationIcons)->RemoveView(IView::Probe(expected));
        IViewGroup::Probe(mNotificationIcons)->AddView(IView::Probe(expected), i);
    }
}

void CPhoneStatusBar::UpdateRowStates()
{
    BaseStatusBar::UpdateRowStates();
    mNotificationPanel->NotifyVisibleChildrenChanged();
}

void CPhoneStatusBar::UpdateCarrierLabelVisibility(
    /* [in] */ Boolean force)
{
    // TODO: Handle this for the notification stack scroller as well
    if (!mShowCarrierInPanel) return;
    // The idea here is to only show the carrier label when there is enough room to see it,
    // i.e. when there aren't enough notifications to fill the panel.
    Int32 height = 0;
    IView::Probe(mStackScroller)->GetHeight(&height);
    if (SPEW) {
        Logger::D(TAG, "stackScrollerh=%d scrollh=%d carrierh=%d", height, height, mCarrierLabelHeight);
    }

    // Emergency calls only is shown in the expanded header now.
    Int32 v = 0, v2 = 0;
    const Boolean emergencyCallsShownElsewhere = TRUE;
    Boolean tmp = FALSE;
    const Boolean makeVisible =
        !(emergencyCallsShownElsewhere && (mNetworkController->IsEmergencyOnly(&tmp), tmp))
        && height < ((IView::Probe(mNotificationPanel)->GetHeight(&v), v)
                - mCarrierLabelHeight - mStatusBarHeaderHeight)
        && (IView::Probe(mStackScroller)->GetVisibility(&v2), v2) == IView::VISIBLE
        && mState != IStatusBarState::KEYGUARD;

    if (force || mCarrierLabelVisible != makeVisible) {
        mCarrierLabelVisible = makeVisible;
        if (DEBUG) {
            Logger::D(TAG, "making carrier label %s", (makeVisible ? "visible" : "invisible"));
        }

        AutoPtr<IViewPropertyAnimator> vpa;
        IView::Probe(mCarrierLabel)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        if (makeVisible) {
            IView::Probe(mCarrierLabel)->SetVisibility(IView::VISIBLE);
        }

        vpa = NULL;
        IView::Probe(mCarrierLabel)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Alpha(makeVisible ? 1.f : 0.f);
        //.setStartDelay(makeVisible ? 500 : 0)
        //.setDuration(makeVisible ? 750 : 100)
        vpa->SetDuration(150);
        AutoPtr<CarrierLabelVisibleAnimatorListenerAdapter> listener = new CarrierLabelVisibleAnimatorListenerAdapter(this);
        vpa->SetListener(makeVisible ? NULL : listener);
        vpa->Start();
    }
}

void CPhoneStatusBar::SetAreThereNotifications()
{
    Boolean tmp = FALSE;
    if (SPEW) {
        Boolean clearable = (HasActiveNotifications(&tmp), tmp) &&
                (mNotificationData->HasActiveClearableNotifications(&tmp), tmp);
        AutoPtr<IArrayList> activeNotifications;
        mNotificationData->GetActiveNotifications((IArrayList**)&activeNotifications);
        Int32 size = 0;
        activeNotifications->GetSize(&size);
        Logger::D(TAG, "SetAreThereNotifications: N=%d any=%d clearable=%d", size,
                (HasActiveNotifications(&tmp), tmp), clearable);
    }

    AutoPtr<IView> nlo;
    IView::Probe(mStatusBarView)->FindViewById(R::id::notification_lights_out, (IView**)&nlo);
    Boolean showDot = (HasActiveNotifications(&tmp), tmp) && !AreLightsOn();

    Float v = 0;
    if (showDot != ((nlo->GetAlpha(&v), v) == 1.0f)) {
        if (showDot) {
            nlo->SetAlpha(0.f);
            nlo->SetVisibility(IView::VISIBLE);
        }

        AutoPtr<IViewPropertyAnimator> vpa;
        nlo->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Alpha(showDot ? 1 : 0);
        vpa->SetDuration(showDot ? 750 : 250);
        AutoPtr<ITimeInterpolator> ti;
        CAccelerateInterpolator::New(2.0f, (ITimeInterpolator**)&ti);
        vpa->SetInterpolator(ti);

        AutoPtr<ViewGoneAnimatorListenerAdapter> listener = new ViewGoneAnimatorListenerAdapter(nlo);
        vpa->SetListener(showDot ? NULL : listener);
        vpa->Start();
    }

    FindAndUpdateMediaNotifications();

    UpdateCarrierLabelVisibility(FALSE);
}

ECode CPhoneStatusBar::FindAndUpdateMediaNotifications()
{
    Boolean metaDataChanged = FALSE;

    {
        AutoLock syncLock(mNotificationData);
        AutoPtr<IArrayList> activeNotifications;  /*<INotificationDataEntry*/
        mNotificationData->GetActiveNotifications((IArrayList**)&activeNotifications);
        Int32 N = 0;
        activeNotifications->GetSize(&N);
        AutoPtr<INotificationDataEntry> mediaNotification;
        AutoPtr<IMediaController> controller;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            activeNotifications->Get(i, (IInterface**)&obj);
            AutoPtr<INotificationDataEntry> entry = INotificationDataEntry::Probe(obj);
            Boolean tmp = FALSE;
            if (IsMediaNotification(entry, &tmp), tmp) {
                AutoPtr<IStatusBarNotification> notification;
                entry->GetNotification((IStatusBarNotification**)&notification);
                AutoPtr<INotification> n;
                notification->GetNotification((INotification**)&n);
                AutoPtr<IBundle> extras;
                n->GetExtras((IBundle**)&extras);
                AutoPtr<IParcelable> p;
                extras->GetParcelable(INotification::EXTRA_MEDIA_SESSION, (IParcelable**)&p);
                AutoPtr<IMediaSessionToken> token = IMediaSessionToken::Probe(p);
                if (token != NULL) {
                    CMediaController::New(mContext, token, (IMediaController**)&controller);
                    if (controller != NULL) {
                        // we've got a live one, here
                        mediaNotification = entry;
                    }
                }
            }
        }

        if (mediaNotification == NULL) {
            // Still nothing? OK, let's just look for live media sessions and see if they match
            // one of our notifications. This will catch apps that aren't (yet!) using media
            // notifications.
            if (mMediaSessionManager != NULL) {
                Logger::D(TAG, "TODO :[Need MediaSessionService.]");
                // AutoPtr<IList> sessions;  /*<MediaController*/
                // mMediaSessionManager->GetActiveSessionsForUser(
                //         NULL, IUserHandle::USER_ALL, (IList**)&sessions);

                // AutoPtr<IIterator> ator;
                // sessions->GetIterator((IIterator**)&ator);
                // Boolean has = FALSE;
                // while (ator->HasNext(&has), has) {
                //     AutoPtr<IInterface> obj;
                //     ator->GetNext((IInterface**)&obj);
                //     AutoPtr<IMediaController> aController = IMediaController::Probe(obj);

                //     if (aController == NULL) continue;
                //     AutoPtr<IPlaybackState> state;
                //     aController->GetPlaybackState((IPlaybackState**)&state);
                //     if (state == NULL) continue;

                //     Int32 s = 0;
                //     switch (state->GetState(&s), s) {
                //         case IPlaybackState::STATE_STOPPED:
                //         case IPlaybackState::STATE_ERROR:
                //             continue;
                //         default: {
                //             // now to see if we have one like this
                //             String pkg;
                //             aController->GetPackageName(&pkg);

                //             for (Int32 i = 0; i < N; i++) {
                //                 AutoPtr<IInterface> o;
                //                 activeNotifications->Get(i, (IInterface**)&o);
                //                 AutoPtr<INotificationDataEntry> entry = INotificationDataEntry::Probe(o);
                //                 AutoPtr<IStatusBarNotification> notification;
                //                 entry->GetNotification((IStatusBarNotification**)&notification);

                //                 String v;
                //                 notification->GetPackageName(&v);
                //                 if (v.Equals(pkg)) {
                //                     notification->GetKey(&v);
                //                     if (DEBUG_MEDIA) {
                //                         Logger::V(TAG, "DEBUG_MEDIA: found controller matching %s"
                //                             , v.string());
                //                     }
                //                     controller = aController;
                //                     mediaNotification = entry;
                //                     break;
                //                 }
                //             }
                //         }
                //     }
                // }
            }
        }

        if (!SameSessions(mMediaController, controller)) {
            // We have a new media session
            if (mMediaController != NULL) {
                // something old was playing
                Logger::V(TAG, "DEBUG_MEDIA: Disconnecting from old controller: %s", TO_CSTR(mMediaController));
                mMediaController->UnregisterCallback(mMediaListener);
            }
            mMediaController = controller;

            if (mMediaController != NULL) {
                mMediaController->RegisterCallback(mMediaListener);
                mMediaController->GetMetadata((IMediaMetadata**)&mMediaMetadata);
                if (DEBUG_MEDIA) {
                    Logger::V(TAG, "DEBUG_MEDIA: insert listener, receive metadata: %s", TO_CSTR(mMediaMetadata));
                }

                String notificationKey;
                if (mediaNotification != NULL) {
                    AutoPtr<IStatusBarNotification> notification;
                    mediaNotification->GetNotification((IStatusBarNotification**)&notification);
                    notification->GetKey(&notificationKey);
                }

                if (notificationKey == NULL || !notificationKey.Equals(mMediaNotificationKey)) {
                    // we have a new notification!
                    if (DEBUG_MEDIA) {
                        Logger::V(TAG, "DEBUG_MEDIA: Found new media notification: key=%s controller=%s"
                                , notificationKey.string(), TO_CSTR(controller));
                    }
                    mMediaNotificationKey = notificationKey;
                }
            }
            else {
                mMediaMetadata = NULL;
                mMediaNotificationKey = NULL;
            }

            metaDataChanged = TRUE;
        }
        else {
            // Media session unchanged
            if (DEBUG_MEDIA) {
                Logger::V(TAG, "DEBUG_MEDIA: Continuing media notification: key=%s", mMediaNotificationKey.string());
            }
        }
    }

    UpdateMediaMetaData(metaDataChanged);
    return NOERROR;
}

Boolean CPhoneStatusBar::SameSessions(
    /* [in] */ IMediaController* a,
    /* [in] */ IMediaController* b)
{
    if (a == b) return TRUE;
    if (a == NULL) return FALSE;
    Boolean tmp = FALSE;
    a->ControlsSameSession(b, &tmp);
    return tmp;
}

/**
 * Refresh or remove lockscreen artwork from media metadata.
 */
ECode CPhoneStatusBar::UpdateMediaMetaData(
    /* [in] */ Boolean metaDataChanged)
{
    if (!SHOW_LOCKSCREEN_MEDIA_ARTWORK) return NOERROR;

    if (mBackdrop == NULL) return NOERROR; // called too early

    if (DEBUG_MEDIA) {
        Logger::V(TAG, "DEBUG_MEDIA: updating album art for notification %s metadata=%s metaDataChanged=%d state=%d"
            , mMediaNotificationKey.string(), TO_CSTR(mMediaMetadata), metaDataChanged, mState);
    }

    AutoPtr<IBitmap> artworkBitmap;
    if (mMediaMetadata != NULL) {
        mMediaMetadata->GetBitmap(IMediaMetadata::METADATA_KEY_ART, (IBitmap**)&artworkBitmap);
        if (artworkBitmap == NULL) {
            mMediaMetadata->GetBitmap(IMediaMetadata::METADATA_KEY_ALBUM_ART, (IBitmap**)&artworkBitmap);
            // might still be NULL
        }
    }

    const Boolean hasArtwork = artworkBitmap != NULL;

    if ((hasArtwork || DEBUG_MEDIA_FAKE_ARTWORK)
            && (mState == IStatusBarState::KEYGUARD || mState == IStatusBarState::SHADE_LOCKED)) {
        // time to show some art!
        Int32 v = 0;
        if ((IView::Probe(mBackdrop)->GetVisibility(&v), v) != IView::VISIBLE) {
            IView::Probe(mBackdrop)->SetVisibility(IView::VISIBLE);

            AutoPtr<IViewPropertyAnimator> vta;
            IView::Probe(mBackdrop)->Animate((IViewPropertyAnimator**)&vta);
            vta->Alpha(1.f);
            metaDataChanged = TRUE;
            if (DEBUG_MEDIA) {
                Logger::V(TAG, "DEBUG_MEDIA: Fading in album artwork");
            }
        }
        if (metaDataChanged) {
            AutoPtr<IDrawable> drawable;
            mBackdropBack->GetDrawable((IDrawable**)&drawable);
            if (drawable != NULL) {
                mBackdropFront->SetImageDrawable(drawable);
                if (mScrimSrcModeEnabled) {
                    AutoPtr<IDrawable> d;
                    mBackdropFront->GetDrawable((IDrawable**)&d);
                    d->Mutate();
                    d->SetXfermode(IXfermode::Probe(mSrcOverXferMode));
                }
                mBackdropFront->SetAlpha(1.f);
                IView::Probe(mBackdropFront)->SetVisibility(IView::VISIBLE);
            }
            else {
                IView::Probe(mBackdropFront)->SetVisibility(IView::INVISIBLE);
            }

            if (DEBUG_MEDIA_FAKE_ARTWORK) {
                const Int32 c = 0xFF000000 | (Int32)(Elastos::Core::Math::Random() * 0xFFFFFF);
                Logger::V(TAG, "DEBUG_MEDIA: setting new color: 0x%08x", c);
                IView::Probe(mBackdropBack)->SetBackgroundColor(0xFFFFFFFF);
                AutoPtr<IDrawable> dr;
                CColorDrawable::New(c, (IDrawable**)&dr);
                mBackdropBack->SetImageDrawable(dr);
            }
            else {
                mBackdropBack->SetImageBitmap(artworkBitmap);
            }
            if (mScrimSrcModeEnabled) {
                drawable->Mutate();
                drawable->SetXfermode(IXfermode::Probe(mSrcXferMode));
            }

            if ((IView::Probe(mBackdropFront)->GetVisibility(&v), v) == IView::VISIBLE) {
                AutoPtr<IDrawable> d;
                mBackdropFront->GetDrawable((IDrawable**)&d);
                if (DEBUG_MEDIA) {
                    Logger::V(TAG, "DEBUG_MEDIA: Crossfading album artwork from %s to %s"
                            , TO_CSTR(d), TO_CSTR(drawable));
                }

                AutoPtr<IViewPropertyAnimator> vpa;
                IView::Probe(mBackdropFront)->Animate((IViewPropertyAnimator**)&vpa);
                vpa->SetDuration(250);
                vpa->Alpha(0.f);
                vpa->WithEndAction(mHideBackdropFront);
            }
        }
    }
    else {
        // need to hide the album art, either because we are unlocked or because
        // the metadata isn't there to support it
        Int32 v = 0;
        if ((IView::Probe(mBackdrop)->GetVisibility(&v), v) != IView::GONE) {
            if (DEBUG_MEDIA) {
                Logger::V(TAG, "DEBUG_MEDIA: Fading out album artwork");
            }
            AutoPtr<IViewPropertyAnimator> vpa;
            IView::Probe(mBackdrop)->Animate((IViewPropertyAnimator**)&vpa);
            vpa->Alpha(0.f);
            vpa->SetInterpolator(ITimeInterpolator::Probe(mBackdropInterpolator));
            vpa->SetDuration(300);
            vpa->SetStartDelay(0);
            AutoPtr<BackdropGoneRunnable> run = new BackdropGoneRunnable(this);
            vpa->WithEndAction(run);
            if (mKeyguardFadingAway) {
                vpa = NULL;
                IView::Probe(mBackdrop)->Animate((IViewPropertyAnimator**)&vpa);
                // Make it disappear faster, as the focus should be on the activity behind.
                vpa->SetDuration(mKeyguardFadingAwayDuration / 2);
                vpa->SetStartDelay(mKeyguardFadingAwayDelay);
                vpa->SetInterpolator(ITimeInterpolator::Probe(mLinearInterpolator));
                vpa->Start();
            }
        }
    }
    return NOERROR;
}

ECode CPhoneStatusBar::ShowClock(
    /* [in] */ Boolean show)
{
    if (mStatusBarView == NULL) return NOERROR;
    AutoPtr<IView> clock;
    IView::Probe(mStatusBarView)->FindViewById(R::id::clock, (IView**)&clock);
    if (clock != NULL) {
        clock->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    }
    return NOERROR;
}

Int32 CPhoneStatusBar::AdjustDisableFlags(
    /* [in] */ Int32 state)
{
    if (!mLaunchTransitionFadingAway
            && (mExpandedVisible || mBouncerShowing || mWaitingForKeyguardExit)) {
        state |= IStatusBarManager::DISABLE_NOTIFICATION_ICONS;
        state |= IStatusBarManager::DISABLE_SYSTEM_INFO;
    }
    return state;
}

/**
 * State is one or more of the DISABLE constants from StatusBarManager.
 */
ECode CPhoneStatusBar::Disable(
    /* [in] */ Int32 state,
    /* [in] */ Boolean animate)
{
    mDisabledUnmodified = state;
    state = AdjustDisableFlags(state);
    const Int32 old = mDisabled;
    const Int32 diff = state ^ old;
    mDisabled = state;

    if (DEBUG) {
        Logger::D(TAG, "disable: 0x%08x -> 0x%08x (diff: 0x%08x)", old, state, diff);
    }

    StringBuilder flagdbg;
    flagdbg.Append("disable: < ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_EXPAND) != 0) ? "EXPAND" : "expand");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_EXPAND) != 0) ? "* " : " ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_NOTIFICATION_ICONS) != 0) ? "ICONS" : "icons");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_NOTIFICATION_ICONS) != 0) ? "* " : " ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_NOTIFICATION_ALERTS) != 0) ? "ALERTS" : "alerts");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_NOTIFICATION_ALERTS) != 0) ? "* " : " ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_SYSTEM_INFO) != 0) ? "SYSTEM_INFO" : "system_info");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_SYSTEM_INFO) != 0) ? "* " : " ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_BACK) != 0) ? "BACK" : "back");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_BACK) != 0) ? "* " : " ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_HOME) != 0) ? "HOME" : "home");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_HOME) != 0) ? "* " : " ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_RECENT) != 0) ? "RECENT" : "recent");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_RECENT) != 0) ? "* " : " ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_CLOCK) != 0) ? "CLOCK" : "clock");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_CLOCK) != 0) ? "* " : " ");
    flagdbg.Append(((state & IStatusBarManager::DISABLE_SEARCH) != 0) ? "SEARCH" : "search");
    flagdbg.Append(((diff  & IStatusBarManager::DISABLE_SEARCH) != 0) ? "* " : " ");
    flagdbg.Append(">");
    Logger::D(TAG, "%s", flagdbg.ToString().string());

    if ((diff & IStatusBarManager::DISABLE_SYSTEM_INFO) != 0) {
        AutoPtr<IViewPropertyAnimator> vpa;
        IView::Probe(mSystemIconArea)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        if ((state & IStatusBarManager::DISABLE_SYSTEM_INFO) != 0) {
            AnimateStatusBarHide(IView::Probe(mSystemIconArea), animate);
        }
        else {
            AnimateStatusBarShow(IView::Probe(mSystemIconArea), animate);
        }
    }

    if ((diff & IStatusBarManager::DISABLE_CLOCK) != 0) {
        Boolean show = (state & IStatusBarManager::DISABLE_CLOCK) == 0;
        ShowClock(show);
    }
    if ((diff & IStatusBarManager::DISABLE_EXPAND) != 0) {
        if ((state & IStatusBarManager::DISABLE_EXPAND) != 0) {
            AnimateCollapsePanels();
        }
    }

    if ((diff & (IStatusBarManager::DISABLE_HOME
                    | IStatusBarManager::DISABLE_RECENT
                    | IStatusBarManager::DISABLE_BACK
                    | IStatusBarManager::DISABLE_SEARCH)) != 0) {
        // the nav bar will take care of these
        if (mNavigationBarView != NULL) mNavigationBarView->SetDisabledFlags(state);

        if ((state & IStatusBarManager::DISABLE_RECENT) != 0) {
            // close recents if it's visible
            mHandler->RemoveMessages(MSG_HIDE_RECENT_APPS);
            Boolean tmp = FALSE;
            mHandler->SendEmptyMessage(MSG_HIDE_RECENT_APPS, &tmp);
        }
    }

    if ((diff & IStatusBarManager::DISABLE_NOTIFICATION_ICONS) != 0) {
        if ((state & IStatusBarManager::DISABLE_NOTIFICATION_ICONS) != 0) {
            if (mTicking) {
                HaltTicker();
            }
            AnimateStatusBarHide(mNotificationIconArea, animate);
        }
        else {
            AnimateStatusBarShow(mNotificationIconArea, animate);
        }
    }

    if ((diff & IStatusBarManager::DISABLE_NOTIFICATION_ALERTS) != 0) {
        mDisableNotificationAlerts =
                (state & IStatusBarManager::DISABLE_NOTIFICATION_ALERTS) != 0;
        mHeadsUpObserver->OnChange(TRUE);
    }
    return NOERROR;
}

/**
 * Animates {@code v}, a view that is part of the status bar, out.
 */
void CPhoneStatusBar::AnimateStatusBarHide(
    /* [in] */ IView* v,
    /* [in] */ Boolean animate)
{
    AutoPtr<IViewPropertyAnimator> vpa;
    v->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Cancel();
    if (!animate) {
        v->SetAlpha(0.f);
        v->SetVisibility(IView::INVISIBLE);
        return;
    }

    vpa = NULL;
    v->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Alpha(0.f);
    vpa->SetDuration(160);
    vpa->SetStartDelay(0);
    vpa->SetInterpolator(ITimeInterpolator::Probe(ALPHA_OUT));
    AutoPtr<ViewInvisibleRunnable> run = new ViewInvisibleRunnable(v);
    vpa->WithEndAction(run);
}

/**
 * Animates {@code v}, a view that is part of the status bar, in.
 */
void CPhoneStatusBar::AnimateStatusBarShow(
    /* [in] */ IView* v,
    /* [in] */ Boolean animate)
{
    AutoPtr<IViewPropertyAnimator> vpa;
    v->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Cancel();
    v->SetVisibility(IView::VISIBLE);
    if (!animate) {
        v->SetAlpha(1.f);
        return;
    }

    vpa = NULL;
    v->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Alpha(1.f);
    vpa->SetDuration(320);
    vpa->SetInterpolator(ITimeInterpolator::Probe(ALPHA_IN));
    vpa->SetStartDelay(50);

    // We need to clean up any pending end action from AnimateStatusBarHide if we call
    // both hide and show in the same frame before the animation actually gets started.
    // cancel() doesn't really remove the end action.
    vpa->WithEndAction(NULL);

    // Synchronize the motion with the Keyguard fading if necessary.
    if (mKeyguardFadingAway) {
        vpa = NULL;
        v->Animate((IViewPropertyAnimator**)&vpa);
        vpa->SetDuration(mKeyguardFadingAwayDuration);
        vpa->SetInterpolator(ITimeInterpolator::Probe(mLinearOutSlowIn));
        vpa->SetStartDelay(mKeyguardFadingAwayDelay);
        vpa->Start();
    }
}

AutoPtr<BaseStatusBar::H> CPhoneStatusBar::CreateHandler()
{
    return new CPhoneStatusBar::H(this);
}

ECode CPhoneStatusBar::StartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean dismissShade)
{
    StartActivityDismissingKeyguard(intent, FALSE, dismissShade);
    return NOERROR;
}

ECode CPhoneStatusBar::GetScrimController(
    /* [out] */ IScrimController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScrimController;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPhoneStatusBar::SetQsExpanded(
    /* [in] */ Boolean expanded)
{
    mStatusBarWindowManager->SetQsExpanded(expanded);
    return NOERROR;
}

ECode CPhoneStatusBar::IsGoingToNotificationShade(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLeaveOpenOnKeyguardHide;
    return NOERROR;
}

ECode CPhoneStatusBar::IsQsExpanded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mNotificationPanel->IsQsExpanded(result);
}

ECode CPhoneStatusBar::IsScreenOnComingFromTouch(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScreenOnComingFromTouch;
    return NOERROR;
}

ECode CPhoneStatusBar::IsFalsingThresholdNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 v = 0;
    Boolean onKeyguard = (GetBarState(&v), v) == IStatusBarState::KEYGUARD;
    Boolean isMethodInsecure = mUnlockMethodCache->IsMethodInsecure();
    *result = onKeyguard && (isMethodInsecure || mDozing || mScreenOnComingFromTouch);
    return NOERROR;
}

ECode CPhoneStatusBar::IsDozing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDozing;
    return NOERROR;
}

// @Override  // NotificationData.Environment
ECode CPhoneStatusBar::GetCurrentMediaNotificationKey(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMediaNotificationKey;
    return NOERROR;
}

ECode CPhoneStatusBar::IsScrimSrcModeEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScrimSrcModeEnabled;
    return NOERROR;
}

/**  if the interrupting notification had a fullscreen intent, fire it now.  */
void CPhoneStatusBar::EscalateHeadsUp()
{
    AutoPtr<INotificationDataEntry> entry;
    mHeadsUpNotificationView->GetEntry((INotificationDataEntry**)&entry);
    if (entry != NULL) {
        AutoPtr<IStatusBarNotification> sbn;
        entry->GetNotification((IStatusBarNotification**)&sbn);
        mHeadsUpNotificationView->ReleaseResources();
        AutoPtr<INotification> notification;
        sbn->GetNotification((INotification**)&notification);
        AutoPtr<IPendingIntent> fullScreenIntent;
        notification->GetFullScreenIntent((IPendingIntent**)&fullScreenIntent);
        if (fullScreenIntent != NULL) {
            if (DEBUG) {
                Logger::D(TAG, "converting a heads up to fullScreen");
            }
            // try {
            fullScreenIntent->Send();
            // } catch (PendingIntent.CanceledException e) {
            // }
        }
    }
}

ECode CPhoneStatusBar::PanelsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mDisabled & IStatusBarManager::DISABLE_EXPAND) == 0;
    return NOERROR;
}

ECode CPhoneStatusBar::MakeExpandedVisible(
    /* [in] */ Boolean force)
{
    if (SPEW) Logger::D(TAG, "Make expanded visible: expanded visible=%d", mExpandedVisible);
    Boolean tmp = FALSE;
    if (!force && (mExpandedVisible || (PanelsEnabled(&tmp), !tmp))) {
        return NOERROR;
    }

    mExpandedVisible = TRUE;
    if (mNavigationBarView != NULL) {
        mNavigationBarView->SetSlippery(TRUE);
    }

    UpdateCarrierLabelVisibility(TRUE);

    UpdateExpandedViewPos(EXPANDED_LEAVE_ALONE);

    // Expand the window to encompass the full screen in anticipation of the drag.
    // This is only possible to do atomically because the status bar is at the top of the screen!
    mStatusBarWindowManager->SetStatusBarExpanded(TRUE);
    IView::Probe(mStatusBarView)->SetFocusable(FALSE);

    VisibilityChanged(TRUE);
    mWaitingForKeyguardExit = FALSE;
    Disable(mDisabledUnmodified, !force /* animate */);
    SetInteracting(IStatusBarManager::WINDOW_STATUS_BAR, TRUE);
    return NOERROR;
}

ECode CPhoneStatusBar::AnimateCollapsePanels()
{
    AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
    return NOERROR;
}

ECode CPhoneStatusBar::PostAnimateCollapsePanels()
{
    Boolean tmp = FALSE;
    return mHandler->Post(mAnimateCollapsePanels, &tmp);
}

ECode CPhoneStatusBar::AnimateCollapsePanels(
    /* [in] */ Int32 flags)
{
    AnimateCollapsePanels(flags, FALSE /* force */);
    return NOERROR;
}

ECode CPhoneStatusBar::AnimateCollapsePanels(
    /* [in] */ Int32 flags,
    /* [in] */ Boolean force)
{
    if (!force &&
            (mState == IStatusBarState::KEYGUARD || mState == IStatusBarState::SHADE_LOCKED)) {
        RunPostCollapseRunnables();
        return NOERROR;
    }
    if (SPEW) {
        Logger::D(TAG, "animateCollapse(): mExpandedVisible=%d flags=%d" , mExpandedVisible, flags);
    }

    Boolean tmp = FALSE;
    if ((flags & ICommandQueue::FLAG_EXCLUDE_RECENTS_PANEL) == 0) {
        if (mHandler->HasMessages(MSG_HIDE_RECENT_APPS, &tmp), !tmp) {
            mHandler->RemoveMessages(MSG_HIDE_RECENT_APPS);
            mHandler->SendEmptyMessage(MSG_HIDE_RECENT_APPS, &tmp);
        }
    }

    if ((flags & ICommandQueue::FLAG_EXCLUDE_SEARCH_PANEL) == 0) {
        mHandler->RemoveMessages(MSG_CLOSE_SEARCH_PANEL);
        mHandler->SendEmptyMessage(MSG_CLOSE_SEARCH_PANEL, &tmp);
    }

    if (mStatusBarWindow != NULL) {
        // release focus immediately to kick off focus change transition
        mStatusBarWindowManager->SetStatusBarFocusable(FALSE);

        mStatusBarWindow->CancelExpandHelper();
        IPanelBar::Probe(mStatusBarView)->CollapseAllPanels(TRUE);
    }
    return NOERROR;
}

void CPhoneStatusBar::RunPostCollapseRunnables()
{
    Int32 size = 0;
    mPostCollapseRunnables->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mPostCollapseRunnables->Get(i, (IInterface**)&obj);
        IRunnable::Probe(obj)->Run();
    }
    mPostCollapseRunnables->Clear();
}

ECode CPhoneStatusBar::SetVisibilityWhenDone(
    /* [in] */ IViewPropertyAnimator* a,
    /* [in] */ IView* v,
    /* [in] */ Int32 vis,
    /* [out] */ IViewPropertyAnimator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ChangeVisibilityAndResetAnimatorListenerAdapter> al = new ChangeVisibilityAndResetAnimatorListenerAdapter(v, a, vis);
    a->SetListener(al);
    *result = a;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPhoneStatusBar::SetVisibilityWhenDone(
    /* [in] */ IAnimator* a,
    /* [in] */ IView* v,
    /* [in] */ Int32 vis,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ChangeVisibilityAnimatorListenerAdapter> al = new ChangeVisibilityAnimatorListenerAdapter(v, vis);
    a->AddListener(al);
    *result = a;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPhoneStatusBar::Interpolator(
    /* [in] */ ITimeInterpolator* ti,
    /* [in] */ IAnimator* a,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result);
    a->SetInterpolator(ti);
    *result = a;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPhoneStatusBar::StartDelay(
    /* [in] */ Int32 d,
    /* [in] */ IAnimator* a,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result);
    a->SetStartDelay(d);
    *result = a;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPhoneStatusBar::Start(
    /* [in] */ IAnimator* a,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result);
    a->Start();
    *result = a;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPhoneStatusBar::AnimateExpandNotificationsPanel()
{
    if (SPEW) Logger::D(TAG, "animateExpand: mExpandedVisible=%d", mExpandedVisible);
    Boolean tmp = FALSE;
    if (PanelsEnabled(&tmp), !tmp) {
        return NOERROR;
    }

    IPanelView::Probe(mNotificationPanel)->Expand();

    if (FALSE) PostStartTracing();
    return NOERROR;
}

ECode CPhoneStatusBar::AnimateExpandSettingsPanel()
{
    if (SPEW) Logger::D(TAG, "animateExpand: mExpandedVisible=%d", mExpandedVisible);
    Boolean tmp = FALSE;
    if (PanelsEnabled(&tmp), !tmp) {
        return NOERROR;
    }

    // Settings are not available in setup
    if (!mUserSetup) return NOERROR;

    IPanelView::Probe(mNotificationPanel)->Expand();
    mNotificationPanel->OpenQs();

    if (FALSE) PostStartTracing();
    return NOERROR;
}

ECode CPhoneStatusBar::AnimateCollapseQuickSettings()
{
    if (mState == IStatusBarState::SHADE) {
        IPanelBar::Probe(mStatusBarView)->CollapseAllPanels(TRUE);
    }
    return NOERROR;
}

ECode CPhoneStatusBar::MakeExpandedInvisible()
{
    if (SPEW) Logger::D(TAG, "makeExpandedInvisible: mExpandedVisible=%d mExpandedVisible=%d"
            , mExpandedVisible , mExpandedVisible);

    if (!mExpandedVisible || mStatusBarWindow == NULL) {
        return NOERROR;
    }

    // Ensure the panel is fully collapsed (just in case; bug 6765842, 7260868)
    IPanelBar::Probe(mStatusBarView)->CollapseAllPanels(/*animate=*/ FALSE);

    // reset things to their proper state
    if (mScrollViewAnim != NULL) {
        mScrollViewAnim->Cancel();
    }

    if (mClearButtonAnim != NULL) {
        mClearButtonAnim->Cancel();
    }

    IView::Probe(mStackScroller)->SetVisibility(IView::VISIBLE);
    IView::Probe(mNotificationPanel)->SetVisibility(IView::GONE);

    mNotificationPanel->CloseQs();

    mExpandedVisible = FALSE;
    if (mNavigationBarView != NULL) {
        mNavigationBarView->SetSlippery(FALSE);
    }
    VisibilityChanged(FALSE);

    // Shrink the window to the size of the status bar only
    mStatusBarWindowManager->SetStatusBarExpanded(FALSE);
    IView::Probe(mStatusBarView)->SetFocusable(TRUE);

    // Close any "App info" popups that might have snuck on-screen
    DismissPopups();

    RunPostCollapseRunnables();
    SetInteracting(IStatusBarManager::WINDOW_STATUS_BAR, FALSE);
    ShowBouncer();
    Disable(mDisabledUnmodified, TRUE /* animate */);
    return NOERROR;
}

ECode CPhoneStatusBar::InterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DEBUG_GESTURES) {
        Int32 masked = 0;
        event->GetActionMasked(&masked);
        if (masked != IMotionEvent::ACTION_MOVE) {
            Float x = 0, y = 0;
            event->GetX(&x);
            event->GetY(&y);
            Logger::D("CPhoneStatusBar", "InterceptTouchEvent masked=[%d], x=[%d], y=[%d], mDisabled=[%d]",
                masked, (Int32)x, (Int32)y, mDisabled);
        }
    }

    if (SPEW) {
        Float fv = 0;
        event->GetRawY(&fv);
        Logger::D(TAG, "Touch: rawY=%f event=%s mDisabled=%d mTracking=%d"
            , fv, TO_CSTR(event), mDisabled, mTracking);
    }
    else if (CHATTY) {
        Int32 action;
        event->GetAction(&action);
        if (action != IMotionEvent::ACTION_MOVE) {
            AutoPtr<IMotionEventHelper> helper;
            CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
            String str;
            helper->ActionToString(action, &str);
            Float x = 0, y = 0;
            event->GetRawX(&x);
            event->GetRawY(&y);
            Logger::D(TAG, "panel: %s at (%f, %f) mDisabled=%d",
                str.string(), x, y, mDisabled);
        }
    }

    if (DEBUG_GESTURES) {
        mGestureRec->Add(event);
    }

    if (mStatusBarWindowState == IStatusBarManager::WINDOW_STATE_SHOWING) {
        Int32 action;
        event->GetAction(&action);
        Boolean upOrCancel =
                action == IMotionEvent::ACTION_UP ||
                action == IMotionEvent::ACTION_CANCEL;
        if (upOrCancel && !mExpandedVisible) {
            SetInteracting(IStatusBarManager::WINDOW_STATUS_BAR, FALSE);
        }
        else {
            SetInteracting(IStatusBarManager::WINDOW_STATUS_BAR, TRUE);
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CPhoneStatusBar::GetGestureRecorder(
    /* [out] */ IGestureRecorder** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (mGestureRec != NULL) {
        *result = (IGestureRecorder*)mGestureRec->Probe(EIID_IGestureRecorder);
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

void CPhoneStatusBar::SetNavigationIconHints(
    /* [in] */ Int32 hints)
{
    if (hints == mNavigationIconHints) return;

    mNavigationIconHints = hints;

    if (mNavigationBarView != NULL) {
        mNavigationBarView->SetNavigationIconHints(hints);
    }
    CheckBarModes();
}

// @Override // CommandQueue
ECode CPhoneStatusBar::SetWindowState(
    /* [in] */ Int32 window,
    /* [in] */ Int32 state)
{
    Boolean showing = state == IStatusBarManager::WINDOW_STATE_SHOWING;
    if (mStatusBarWindow != NULL
            && window == IStatusBarManager::WINDOW_STATUS_BAR
            && mStatusBarWindowState != state) {
        mStatusBarWindowState = state;
        if (DEBUG_WINDOW_STATE) {
            AutoPtr<IStatusBarManagerHelper> helper;
            CStatusBarManagerHelper::AcquireSingleton((IStatusBarManagerHelper**)&helper);
            String value;
            helper->WindowStateToString(state, &value);
            Logger::D(TAG, "Status bar %s", value.string());
        }
        if (!showing && mState == IStatusBarState::SHADE) {
            IPanelBar::Probe(mStatusBarView)->CollapseAllPanels(FALSE);
        }
    }
    if (mNavigationBarView != NULL
            && window == IStatusBarManager::WINDOW_NAVIGATION_BAR
            && mNavigationBarWindowState != state) {
        mNavigationBarWindowState = state;
        if (DEBUG_WINDOW_STATE) {
            AutoPtr<IStatusBarManagerHelper> helper;
            CStatusBarManagerHelper::AcquireSingleton((IStatusBarManagerHelper**)&helper);
            String value;
            helper->WindowStateToString(state, &value);
            Logger::D(TAG, "Navigation bar %s", value.string());
        }
    }
    return NOERROR;
}

// @Override // CommandQueue
ECode CPhoneStatusBar::BuzzBeepBlinked()
{
    if (mDozeServiceHost != NULL) {
        mDozeServiceHost->FireBuzzBeepBlinked();
    }
    return NOERROR;
}

ECode CPhoneStatusBar::NotificationLightOff()
{
    if (mDozeServiceHost != NULL) {
        mDozeServiceHost->FireNotificationLight(FALSE);
    }
    return NOERROR;
}

ECode CPhoneStatusBar::NotificationLightPulse(
    /* [in] */ Int32 argb,
    /* [in] */ Int32 onMillis,
    /* [in] */ Int32 offMillis)
{
    if (mDozeServiceHost != NULL) {
        mDozeServiceHost->FireNotificationLight(TRUE);
    }
    return NOERROR;
}

// @Override // CommandQueue
ECode CPhoneStatusBar::SetSystemUiVisibility(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 mask)
{
    const Int32 oldVal = mSystemUiVisibility;
    const Int32 newVal = (oldVal&~mask) | (vis&mask);
    const Int32 diff = newVal ^ oldVal;
    if (DEBUG) Logger::D(TAG, "setSystemUiVisibility vis=%s mask=%s oldVal=%s newVal=%s diff=%s",
            StringUtils::ToHexString(vis).string(), StringUtils::ToHexString(mask).string(),
            StringUtils::ToHexString(oldVal).string(), StringUtils::ToHexString(newVal).string(),
            StringUtils::ToHexString(diff).string());
    if (diff != 0) {
        // we never set the recents bit via this method, so save the prior state to prevent
        // clobbering the bit below
        const Boolean wasRecentsVisible = (mSystemUiVisibility & IView::RECENT_APPS_VISIBLE) > 0;

        mSystemUiVisibility = newVal;

        // update low profile
        if ((diff & IView::SYSTEM_UI_FLAG_LOW_PROFILE) != 0) {
            const Boolean lightsOut = (vis & IView::SYSTEM_UI_FLAG_LOW_PROFILE) != 0;
            if (lightsOut) {
                AnimateCollapsePanels();
                if (mTicking) {
                    HaltTicker();
                }
            }

            SetAreThereNotifications();
        }

        // update status bar mode
        AutoPtr<IBarTransitions> bt;
        mStatusBarView->GetBarTransitions((IBarTransitions**)&bt);
        const Int32 sbMode = ComputeBarMode(oldVal, newVal, bt,
                IView::STATUS_BAR_TRANSIENT, IView::STATUS_BAR_TRANSLUCENT);

        // update navigation bar mode
        bt = NULL;
        const Int32 nbMode = mNavigationBarView == NULL ? -1 : ComputeBarMode(
                oldVal, newVal, (mNavigationBarView->GetBarTransitions((IBarTransitions**)&bt), bt),
                IView::NAVIGATION_BAR_TRANSIENT, IView::NAVIGATION_BAR_TRANSLUCENT);
        const Boolean sbModeChanged = sbMode != -1;
        const Boolean nbModeChanged = nbMode != -1;
        Boolean checkBarModes = FALSE;
        if (sbModeChanged && sbMode != mStatusBarMode) {
            mStatusBarMode = sbMode;
            checkBarModes = TRUE;
        }
        if (nbModeChanged && nbMode != mNavigationBarMode) {
            mNavigationBarMode = nbMode;
            checkBarModes = TRUE;
        }
        if (checkBarModes) {
            CheckBarModes();
        }
        if (sbModeChanged || nbModeChanged) {
            // update transient bar autohide
            if (mStatusBarMode == BarTransitions::MODE_SEMI_TRANSPARENT
                || mNavigationBarMode == BarTransitions::MODE_SEMI_TRANSPARENT) {
                ScheduleAutohide();
            }
            else {
                CancelAutohide();
            }
        }

        // ready to unhide
        if ((vis & IView::STATUS_BAR_UNHIDE) != 0) {
            mSystemUiVisibility &= ~IView::STATUS_BAR_UNHIDE;
        }
        if ((vis & IView::NAVIGATION_BAR_UNHIDE) != 0) {
            mSystemUiVisibility &= ~IView::NAVIGATION_BAR_UNHIDE;
        }

        // restore the recents bit
        if (wasRecentsVisible) {
            mSystemUiVisibility |= IView::RECENT_APPS_VISIBLE;
        }

        // send updated sysui visibility to window manager
        NotifyUiVisibilityChanged(mSystemUiVisibility);
    }
    return NOERROR;
}

Int32 CPhoneStatusBar::ComputeBarMode(
    /* [in] */ Int32 oldVis,
    /* [in] */ Int32 newVis,
    /* [in] */ IBarTransitions* transitions,
    /* [in] */ Int32 transientFlag,
    /* [in] */ Int32 translucentFlag)
{
    const Int32 oldMode = BarMode(oldVis, transientFlag, translucentFlag);
    const Int32 newMode = BarMode(newVis, transientFlag, translucentFlag);
    if (oldMode == newMode) {
        return -1; // no mode change
    }
    return newMode;
}

Int32 CPhoneStatusBar::BarMode(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 transientFlag,
    /* [in] */ Int32 translucentFlag)
{
    return (vis & transientFlag) != 0 ? BarTransitions::MODE_SEMI_TRANSPARENT
            : (vis & translucentFlag) != 0 ? BarTransitions::MODE_TRANSLUCENT
            : (vis & IView::SYSTEM_UI_TRANSPARENT) != 0 ? BarTransitions::MODE_TRANSPARENT
            : (vis & IView::SYSTEM_UI_FLAG_LOW_PROFILE) != 0 ? BarTransitions::MODE_LIGHTS_OUT
            : BarTransitions::MODE_OPAQUE;
}

void CPhoneStatusBar::CheckBarModes()
{
    if (mDemoMode) return;
    AutoPtr<IBarTransitions> bt;
    mStatusBarView->GetBarTransitions((IBarTransitions**)&bt);
    CheckBarMode(mStatusBarMode, mStatusBarWindowState, bt);
    if (mNavigationBarView != NULL) {
        bt = NULL;
        mNavigationBarView->GetBarTransitions((IBarTransitions**)&bt);
        CheckBarMode(mNavigationBarMode, mNavigationBarWindowState, bt);
    }
}

void CPhoneStatusBar::CheckBarMode(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 windowState,
    /* [in] */ IBarTransitions* transitions)
{
    Boolean powerSave = FALSE;
    mBatteryController->IsPowerSave(&powerSave);
    const Boolean anim = mScreenOn && windowState != IStatusBarManager::WINDOW_STATE_HIDDEN && !powerSave;

    Int32 v = 0;
    if (powerSave && (GetBarState(&v), v) == IStatusBarState::SHADE) {
        mode = BarTransitions::MODE_WARNING;
    }
    transitions->TransitionTo(mode, anim);
}

void CPhoneStatusBar::FinishBarAnimations()
{
    AutoPtr<IBarTransitions> bt;
    mStatusBarView->GetBarTransitions((IBarTransitions**)&bt);
    bt->FinishAnimations();
    if (mNavigationBarView != NULL) {
        bt = NULL;
        mNavigationBarView->GetBarTransitions((IBarTransitions**)&bt);
        bt->FinishAnimations();
    }
}

ECode CPhoneStatusBar::SetInteracting(
    /* [in] */ Int32 barWindow,
    /* [in] */ Boolean interacting)
{
    mInteractingWindows = interacting
            ? (mInteractingWindows | barWindow)
            : (mInteractingWindows & ~barWindow);
    if (mInteractingWindows != 0) {
        SuspendAutohide();
    }
    else {
        ResumeSuspendedAutohide();
    }
    CheckBarModes();
    return NOERROR;
}

void CPhoneStatusBar::ResumeSuspendedAutohide()
{
    if (mAutohideSuspended) {
        ScheduleAutohide();
        Boolean tmp = FALSE;
        mHandler->PostDelayed(mCheckBarModes, 500, &tmp); // longer than home -> launcher
    }
}

void CPhoneStatusBar::SuspendAutohide()
{
    mHandler->RemoveCallbacks(mAutohide);
    mHandler->RemoveCallbacks(mCheckBarModes);
    mAutohideSuspended = (mSystemUiVisibility & STATUS_OR_NAV_TRANSIENT) != 0;
}

void CPhoneStatusBar::CancelAutohide()
{
    mAutohideSuspended = FALSE;
    mHandler->RemoveCallbacks(mAutohide);
}

void CPhoneStatusBar::ScheduleAutohide()
{
    CancelAutohide();
    Boolean tmp = FALSE;
    mHandler->PostDelayed(mAutohide, AUTOHIDE_TIMEOUT_MS, &tmp);
}

void CPhoneStatusBar::CheckUserAutohide(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event)
{
    Int32 value = 0;
    Float x = 0, y = 0;
    if ((mSystemUiVisibility & STATUS_OR_NAV_TRANSIENT) != 0  // a transient bar is revealed
            && (event->GetAction(&value), value) == IMotionEvent::ACTION_OUTSIDE // touch outside the source bar
            && (event->GetX(&x), x) == 0 && (event->GetY(&y), y) == 0  // a touch outside both bars
            ) {
        UserAutohide();
    }
}

void CPhoneStatusBar::UserAutohide()
{
    CancelAutohide();
    Boolean tmp = FALSE;
    mHandler->PostDelayed(mAutohide, 350, &tmp); // longer than app gesture -> flag clear
}

Boolean CPhoneStatusBar::AreLightsOn()
{
    return 0 == (mSystemUiVisibility & IView::SYSTEM_UI_FLAG_LOW_PROFILE);
}

ECode CPhoneStatusBar::SetLightsOn(
    /* [in] */ Boolean on)
{
    Logger::V(TAG, "setLightsOn(%d)", on);
    if (on) {
        SetSystemUiVisibility(0, IView::SYSTEM_UI_FLAG_LOW_PROFILE);
    }
    else {
        SetSystemUiVisibility(IView::SYSTEM_UI_FLAG_LOW_PROFILE, IView::SYSTEM_UI_FLAG_LOW_PROFILE);
    }
    return NOERROR;
}

void CPhoneStatusBar::NotifyUiVisibilityChanged(
    /* [in] */ Int32 vis)
{
    // try {
    mWindowManagerService->StatusBarVisibilityChanged(vis);
    // } catch (RemoteException ex) {
    // }
}

ECode CPhoneStatusBar::TopAppWindowChanged(
    /* [in] */ Boolean showMenu)
{
    if (DEBUG) {
        Logger::D(TAG, "%s the MENU button", (showMenu ? "showing" : "hiding"));
    }
    if (mNavigationBarView != NULL) {
        mNavigationBarView->SetMenuVisibility(showMenu);
    }

    // See above re: lights-out policy for legacy apps.
    if (showMenu) SetLightsOn(TRUE);
    return NOERROR;
}

ECode CPhoneStatusBar::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition,
    /* [in] */ Boolean showImeSwitcher)
{
    Boolean imeShown = (vis & IInputMethodService::IME_VISIBLE) != 0;
    Int32 flags = mNavigationIconHints;
    if ((backDisposition == IInputMethodService::BACK_DISPOSITION_WILL_DISMISS) || imeShown) {
        flags |= IStatusBarManager::NAVIGATION_HINT_BACK_ALT;
    }
    else {
        flags &= ~IStatusBarManager::NAVIGATION_HINT_BACK_ALT;
    }
    if (showImeSwitcher) {
        flags |= IStatusBarManager::NAVIGATION_HINT_IME_SHOWN;
    }
    else {
        flags &= ~IStatusBarManager::NAVIGATION_HINT_IME_SHOWN;
    }

    SetNavigationIconHints(flags);
    return NOERROR;
}

void CPhoneStatusBar::Tick(
    /* [in] */ IStatusBarNotification* n,
    /* [in] */ Boolean firstTime)
{
    if (!mTickerEnabled) return;

    // no ticking in lights-out mode
    if (!AreLightsOn()) return;

    // no ticking in Setup
    Boolean tmp = FALSE;
    if (IsDeviceProvisioned(&tmp), !tmp) return;

    // not for you
    if (IsNotificationForCurrentProfiles(n, &tmp), !tmp) return;

    // Show the ticker if one is requested. Also don't do this
    // until status bar window is attached to the window manager,
    // because...  well, what's the point otherwise?  And trying to
    // run a ticker without being attached will crash!
    AutoPtr<INotification> notification;
    n->GetNotification((INotification**)&notification);
    AutoPtr<ICharSequence> tickerText;
    notification->GetTickerText((ICharSequence**)&tickerText);
    AutoPtr<IBinder> d;
    if (tickerText != NULL && mStatusBarWindow != NULL
            && (IView::Probe(mStatusBarWindow)->GetWindowToken((IBinder**)&d), d) != NULL) {
        if (0 == (mDisabled & (IStatusBarManager::DISABLE_NOTIFICATION_ICONS
                | IStatusBarManager::DISABLE_NOTIFICATION_TICKER))) {
            mTicker->AddEntry(n);
        }
    }
}

AutoPtr<IAnimation> CPhoneStatusBar::LoadAnim(
    /* [in] */ Int32 id,
    /* [in] */ IAnimationAnimationListener* listener)
{
    AutoPtr<IAnimation> anim;
    AnimationUtils::LoadAnimation(mContext, id, (IAnimation**)&anim);
    if (listener != NULL) {
        anim->SetAnimationListener(listener);
    }
    return anim;
}

String CPhoneStatusBar::ViewInfo(
    /* [in] */ IView* view)
{
    Int32 v = 0;
    StringBuilder sb;
    sb.Append("[(");
    view->GetLeft(&v);
    sb.Append(v);
    sb.Append(",");
    view->GetTop(&v);
    sb.Append(v);
    sb.Append(")(");
    view->GetRight(&v);
    sb.Append(v);
    sb.Append(",");
    view->GetBottom(&v);
    sb.Append(v);
    sb.Append(")");
    view->GetWidth(&v);
    sb.Append(v);
    sb.Append("x");
    view->GetHeight(&v);
    sb.Append(v);
    sb.Append("]");
    return sb.ToString();
}

ECode CPhoneStatusBar::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    {    AutoLock syncLock(mQueueLock);
        pw->Println(String("Current Status Bar state:"));
        pw->Println(String("  mExpandedVisible=") + StringUtils::ToString(mExpandedVisible)
                + ", mTrackingPosition=" + StringUtils::ToString(mTrackingPosition));
        pw->Println(String("  mTickerEnabled=") + StringUtils::ToString(mTickerEnabled));
        if (mTickerEnabled) {
            pw->Println(String("  mTicking=") + StringUtils::ToString(mTicking));
            pw->Println(String("  mTickerView: ") + ViewInfo(mTickerView));
        }
        pw->Println(String("  mTracking=") + StringUtils::ToString(mTracking));
        pw->Println(String("  mDisplayMetrics=") + TO_STR(mDisplayMetrics));
        pw->Println(String("  mStackScroller: ") + ViewInfo(IView::Probe(mStackScroller)));


        Int32 x = 0, y = 0;
        IView::Probe(mStackScroller)->GetScrollX(&x);
        IView::Probe(mStackScroller)->GetScrollY(&y);
        pw->Println(String("  mStackScroller: ") + ViewInfo(IView::Probe(mStackScroller))
                + " scroll " + StringUtils::ToString(x)
                + "," + StringUtils::ToString(y));
    }

    pw->Print(String("  mInteractingWindows="));
    pw->Println(mInteractingWindows);
    pw->Print(String("  mStatusBarWindowState="));

    AutoPtr<IStatusBarManagerHelper> helper;
    CStatusBarManagerHelper::AcquireSingleton((IStatusBarManagerHelper**)&helper);
    String value;
    helper->WindowStateToString(mStatusBarWindowState, &value);
    pw->Println(value);
    pw->Print(String("  mStatusBarMode="));
    pw->Println(BarTransitions::ModeToString(mStatusBarMode));
    pw->Print(String("  mDozing="));
    pw->Println(mDozing);
    pw->Print(String("  mZenMode="));
    Elastos::Droid::Provider::Settings::Global::ZenModeToString(mZenMode, &value);
    pw->Println(value);
    pw->Print(String("  mUseHeadsUp="));
    pw->Println(mUseHeadsUp);
    pw->Print(String("  interrupting package: "));

    AutoPtr<INotificationDataEntry> entry;
    mHeadsUpNotificationView->GetEntry((INotificationDataEntry**)&entry);
    pw->Println(HunStateToString(entry));

    AutoPtr<IBarTransitions> bt;
    mStatusBarView->GetBarTransitions((IBarTransitions**)&bt);
    DumpBarTransitions(pw, String("mStatusBarView"), bt);
    if (mNavigationBarView != NULL) {
        pw->Print(String("  mNavigationBarWindowState="));
        helper->WindowStateToString(mStatusBarWindowState, &value);
        pw->Println(value);
        pw->Print(String("  mNavigationBarMode="));
        pw->Println(BarTransitions::ModeToString(mNavigationBarMode));

        bt = NULL;
        mNavigationBarView->GetBarTransitions((IBarTransitions**)&bt);
        DumpBarTransitions(pw, String("mNavigationBarView"), bt);
    }

    pw->Print(String("  mNavigationBarView="));
    if (mNavigationBarView == NULL) {
        pw->Println(String("NULL"));
    }
    else {
        mNavigationBarView->Dump(fd, pw, args);
    }

    pw->Print(String("  mMediaSessionManager="));
    pw->Println(mMediaSessionManager);
    pw->Print(String("  mMediaNotificationKey="));
    pw->Println(mMediaNotificationKey);
    pw->Print(String("  mMediaController="));
    pw->Print(mMediaController);
    if (mMediaController != NULL) {
        AutoPtr<IPlaybackState> state;
        mMediaController->GetPlaybackState((IPlaybackState**)&state);
        pw->Print(String(" state=") + TO_STR(state));
    }
    pw->Println();
    pw->Print(String("  mMediaMetadata="));
    pw->Print(mMediaMetadata);
    if (mMediaMetadata != NULL) {
        AutoPtr<ICharSequence> c;
        mMediaMetadata->GetText(IMediaMetadata::METADATA_KEY_TITLE, (ICharSequence**)&c);
        pw->Print(String(" title=") + TO_STR(c));
    }
    pw->Println();

    pw->Println(String("  Panels: "));
    if (mNotificationPanel != NULL) {
        AutoPtr<IViewGroupLayoutParams> vp;
        IView::Probe(mNotificationPanel)->GetLayoutParams((IViewGroupLayoutParams**)&vp);
        pw->Println(String("    mNotificationPanel=") +
            TO_STR(mNotificationPanel) + " params="
            + TO_STR(vp) /*mNotificationPanel->GetLayoutParams().Debug("")*/);
        pw->Print(String("      "));
        ((CNotificationPanelView*)mNotificationPanel.Get())->Dump(fd, pw, args);
    }

    DozeLog::Dump(pw);

    if (DUMPTRUCK) {
        {    AutoLock syncLock(mNotificationData);
            mNotificationData->Dump(pw, String("  "));
        }

        Int32 N = 0;
        IViewGroup::Probe(mStatusIcons)->GetChildCount(&N);
        pw->Println(String("  system icons: ") + StringUtils::ToString(N));
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IView> child;
            IViewGroup::Probe(mStatusIcons)->GetChildAt(i, (IView**)&child);
            AutoPtr<IStatusBarIconView> ic = IStatusBarIconView::Probe(child);
            pw->Println(String("    [") + StringUtils::ToString(i) + "] icon=" + TO_STR(ic));
        }

        if (FALSE) {
            pw->Println(String("see the logcat for a dump of the views we have created."));
            // must happen on ui thread
            AutoPtr<DebugLoacationRunnable> run = new DebugLoacationRunnable(this);
            Boolean tmp = FALSE;
            mHandler->Post(run, &tmp);
        }
    }

    if (DEBUG_GESTURES) {
        pw->Print(String("  status bar gestures: "));
        mGestureRec->Dump(fd, pw, args);
    }

    if (mNetworkController != NULL) {
        mNetworkController->Dump(fd, pw, args);
    }
    if (mBluetoothController != NULL) {
        mBluetoothController->Dump(fd, pw, args);
    }
    if (mCastController != NULL) {
        mCastController->Dump(fd, pw, args);
    }
    if (mUserSwitcherController != NULL) {
        mUserSwitcherController->Dump(fd, pw, args);
    }
    if (mBatteryController != NULL) {
        mBatteryController->Dump(fd, pw, args);
    }
    if (mNextAlarmController != NULL) {
        mNextAlarmController->Dump(fd, pw, args);
    }
    if (mSecurityController != NULL) {
        mSecurityController->Dump(fd, pw, args);
    }
    return NOERROR;
}

String CPhoneStatusBar::HunStateToString(
    /* [in] */ INotificationDataEntry* entry)
{
    if (entry == NULL) return String("NULL");

    AutoPtr<IStatusBarNotification> notification;
    entry->GetNotification((IStatusBarNotification**)&notification);
    if (notification == NULL) return String("corrupt");

    String name;
    notification->GetPackageName(&name);
    return name;
}

void CPhoneStatusBar::DumpBarTransitions(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& var,
    /* [in] */ IBarTransitions* transitions)
{
    pw->Print(String("  "));
    pw->Print(var);
    pw->Print(String(".BarTransitions.mMode="));
    Int32 mode = 0;
    transitions->GetMode(&mode);
    pw->Println(BarTransitions::ModeToString(mode));
}

void CPhoneStatusBar::CreateAndAddWindows()
{
    AddStatusBarWindow();
}

void CPhoneStatusBar::AddStatusBarWindow()
{
    MakeStatusBarView();
    mStatusBarWindowManager = new StatusBarWindowManager(mContext);
    Int32 h = 0;
    GetStatusBarHeight(&h);
    mStatusBarWindowManager->Add(IView::Probe(mStatusBarWindow), h);
}

Float CPhoneStatusBar::Saturate(
    /* [in] */ Float a)
{
    return a < 0.f ? 0.f : (a > 1.f ? 1.f : a);
}

void CPhoneStatusBar::UpdateExpandedViewPos(
    /* [in] */ Int32 thingy)
{
    if (SPEW) Logger::V(TAG, "UpdateExpandedViewPos");

    // on larger devices, the notification panel is propped open a bit
    Int32 y = 0;
    mCurrentDisplaySize->GetY(&y);
    IView::Probe(mNotificationPanel)->SetMinimumHeight((Int32)(mNotificationPanelMinHeightFrac * y));

    AutoPtr<IViewGroupLayoutParams> vp;
    IView::Probe(mNotificationPanel)->GetLayoutParams((IViewGroupLayoutParams**)&vp);
    AutoPtr<IFrameLayoutLayoutParams> lp = IFrameLayoutLayoutParams::Probe(vp);
    lp->SetGravity(mNotificationPanelGravity);
    IView::Probe(mNotificationPanel)->SetLayoutParams(vp);

    UpdateCarrierLabelVisibility(FALSE);
}

// called by makeStatusbar and also by PhoneStatusBarView
ECode CPhoneStatusBar::UpdateDisplaySize()
{
    mDisplay->GetMetrics(mDisplayMetrics);
    mDisplay->GetSize(mCurrentDisplaySize);
    if (DEBUG_GESTURES) {
        String format;
        Int32 widthPixels = 0, heightPixels = 0;
        mDisplayMetrics->GetWidthPixels(&widthPixels);
        mDisplayMetrics->GetHeightPixels(&heightPixels);
        format.AppendFormat("%dx%d", widthPixels, heightPixels);
        mGestureRec->Tag(String("display"), format);
    }
    return NOERROR;
}

ECode CPhoneStatusBar::StartActivityDismissingKeyguard(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean onlyProvisioned,
    /* [in] */ Boolean dismissShade)
{
    Boolean tmp = FALSE;
    if (onlyProvisioned && (IsDeviceProvisioned(&tmp), !tmp)) return NOERROR;

    Boolean afterKeyguardGone = PreviewInflater::WouldLaunchResolverActivity(
        mContext, intent, mCurrentUserId);
    Boolean keyguardShowing = FALSE;
    mStatusBarKeyguardViewManager->IsShowing(&keyguardShowing);
    AutoPtr<IKeyguardHostViewOnDismissAction> action = new KeyguardHostViewOnDismissAction(
        this, intent, afterKeyguardGone, keyguardShowing, dismissShade);
    DismissKeyguardThenExecute(action, afterKeyguardGone);
    return NOERROR;
}

void CPhoneStatusBar::ResetUserExpandedStates()
{
    AutoPtr<IArrayList> activeNotifications;  /*<INotificationDataEntry*/
    mNotificationData->GetActiveNotifications((IArrayList**)&activeNotifications);
    Int32 notificationCount = 0;
    activeNotifications->GetSize(&notificationCount);
    for (Int32 i = 0; i < notificationCount; i++) {
        AutoPtr<IInterface> obj;
        activeNotifications->Get(i, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> entry = INotificationDataEntry::Probe(obj);

        AutoPtr<IExpandableNotificationRow> row;
        entry->GetRow((IExpandableNotificationRow**)&row);
        if (row != NULL) {
            row->ResetUserExpansion();
        }
    }
}

void CPhoneStatusBar::DismissKeyguardThenExecute(
    /* [in] */ IKeyguardHostViewOnDismissAction* action,
    /* [in] */ Boolean afterKeyguardGone)
{
    Boolean tmp = FALSE;
    if (mStatusBarKeyguardViewManager->IsShowing(&tmp), tmp) {
        if (UnlockMethodCache::GetInstance(mContext)->IsMethodInsecure()
            && (mNotificationPanel->IsLaunchTransitionRunning(&tmp), tmp) && !afterKeyguardGone) {
            action->OnDismiss(&tmp);
            AutoPtr<DismissKeyguardViewManagerRunnable> run = new DismissKeyguardViewManagerRunnable(this);
            mNotificationPanel->SetLaunchTransitionEndRunnable(run);
        }
        else {
            mStatusBarKeyguardViewManager->DismissWithAction(action, afterKeyguardGone);
        }
    }
    else {
        action->OnDismiss(&tmp);
    }
}

// SystemUIService notifies SystemBars of configuration changes, which then calls down here
// @Override
ECode CPhoneStatusBar::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    BaseStatusBar::OnConfigurationChanged(newConfig); // calls refreshLayout

    if (DEBUG) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        Logger::V(TAG, "configuration changed: %s", TO_CSTR(config));
    }
    UpdateDisplaySize(); // populates mDisplayMetrics

    UpdateResources();
    UpdateClockSize();
    RepositionNavigationBar();
    UpdateExpandedViewPos(EXPANDED_LEAVE_ALONE);
    UpdateShowSearchHoldoff();
    UpdateRowStates();
    return NOERROR;
}

ECode CPhoneStatusBar::UserSwitched(
    /* [in] */ Int32 newUserId)
{
    if (MULTIUSER_DEBUG) {
        AutoPtr<ICharSequence> t;
        CString::New(String("USER ") + StringUtils::ToString(newUserId), (ICharSequence**)&t);
        mNotificationPanelDebugText->SetText(t);
    }

    AnimateCollapsePanels();
    UpdateNotifications();
    ResetUserSetupObserver();
    SetControllerUsers();
    return NOERROR;
}

void CPhoneStatusBar::SetControllerUsers()
{
    if (mZenModeController != NULL) {
        mZenModeController->SetUserId(mCurrentUserId);
    }
}

void CPhoneStatusBar::ResetUserSetupObserver()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(mUserSetupObserver);
    mUserSetupObserver->OnChange(FALSE);

    AutoPtr<IUri> uri;
    Elastos::Droid::Provider::Settings::Global::GetUriFor(
        ISettingsSecure::USER_SETUP_COMPLETE, (IUri**)&uri);
    cr->RegisterContentObserver(uri, TRUE, mUserSetupObserver, mCurrentUserId);
}

void CPhoneStatusBar::SetHeadsUpVisibility(
    /* [in] */ Boolean vis)
{
    if (!ENABLE_HEADS_UP) return;
    if (DEBUG) Logger::V(TAG, "%s heads up window", (vis ? "showing" : "hiding"));

    String key;
    mHeadsUpNotificationView->GetKey(&key);
    Logger::D("CPhoneStatusBar", "SetHeadsUpVisibility key=[%s], vis=[%d]"
            , key.string(), (vis ? 1 : 0));
    // EventLog.writeEvent(EventLogTags.SYSUI_HEADS_UP_STATUS,
    //         vis ? mHeadsUpNotificationView.getKey() : "",
    //         vis ? 1 : 0);
    IView::Probe(mHeadsUpNotificationView)->SetVisibility(vis ? IView::VISIBLE : IView::GONE);
}

ECode CPhoneStatusBar::OnHeadsUpDismissed()
{
    mHeadsUpNotificationView->Dismiss();
    return NOERROR;
}

/**
 * Reload some of our resources when the configuration changes.
 *
 * We don't reload everything when the configuration changes -- we probably
 * should, but getting that smooth is tough.  Someday we'll fix that.  In the
 * meantime, just update the things that we know change.
 */
ECode CPhoneStatusBar::UpdateResources()
{
    // Update the quick setting tiles
    if (mQSPanel != NULL) {
        mQSPanel->UpdateResources();
    }

    LoadDimens();
    AnimationUtils::LoadInterpolator(mContext
        , Elastos::Droid::R::interpolator::linear_out_slow_in, (IInterpolator**)&mLinearOutSlowIn);

    if (mNotificationPanel != NULL) {
        mNotificationPanel->UpdateResources();
    }
    if (mHeadsUpNotificationView != NULL) {
        mHeadsUpNotificationView->UpdateResources();
    }
    if (mBrightnessMirrorController != NULL) {
        mBrightnessMirrorController->UpdateResources();
    }
    return NOERROR;
}

void CPhoneStatusBar::UpdateClockSize()
{
    if (mStatusBarView == NULL) return;
    AutoPtr<IView> view;
    IView::Probe(mStatusBarView)->FindViewById(R::id::clock, (IView**)&view);
    AutoPtr<ITextView> clock = ITextView::Probe(view);
    if (clock != NULL) {
        FontSizeUtils::UpdateFontSize(clock, R::dimen::status_bar_clock_size);
    }
}

void CPhoneStatusBar::LoadDimens()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::status_bar_height, &mNaturalBarHeight);

    Int32 newIconSize = 0;
    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::status_bar_icon_size, &newIconSize);
    Int32 newIconHPadding = 0;
    res->GetDimensionPixelSize(R::dimen::status_bar_icon_padding, &newIconHPadding);

    if (newIconHPadding != mIconHPadding || newIconSize != mIconSize) {
//            Logger::D(TAG, "size=" + newIconSize + " padding=" + newIconHPadding);
        mIconHPadding = newIconHPadding;
        mIconSize = newIconSize;
        //reloadAllNotificationIcons(); // reload the tray
    }

    res->GetDimensionPixelSize(R::dimen::status_bar_edge_ignore, &mEdgeBorder);

    res->GetInteger(R::integer::notification_panel_layout_gravity, &mNotificationPanelGravity);
    if (mNotificationPanelGravity <= 0) {
        mNotificationPanelGravity = IGravity::START | IGravity::TOP;
    }

    res->GetDimensionPixelSize(R::dimen::carrier_label_height, &mCarrierLabelHeight);
    res->GetDimensionPixelSize(R::dimen::status_bar_header_height, &mStatusBarHeaderHeight);

    res->GetFraction(R::dimen::notification_panel_min_height_frac, 1, 1, &mNotificationPanelMinHeightFrac);
    if (mNotificationPanelMinHeightFrac < 0.f || mNotificationPanelMinHeightFrac > 1.f) {
        mNotificationPanelMinHeightFrac = 0.f;
    }

    res->GetInteger(R::integer::heads_up_notification_decay, &mHeadsUpNotificationDecay);
    res->GetDimensionPixelSize(R::dimen::notification_min_height, &mRowMinHeight);
    res->GetDimensionPixelSize(R::dimen::notification_max_height, &mRowMaxHeight);

    res->GetInteger(R::integer::keyguard_max_notification_count, &mKeyguardMaxNotificationCount);

    if (DEBUG) Logger::V(TAG, "UpdateResources");
}

// Visibility reporting
void CPhoneStatusBar::VisibilityChanged(
    /* [in] */ Boolean visible)
{
    mVisible = visible;
    if (visible) {
        StartNotificationLoggingIfScreenOnAndVisible();
    } else {
        StopNotificationLogging();
    }
    BaseStatusBar::VisibilityChanged(visible);
}

void CPhoneStatusBar::StopNotificationLogging()
{
    // Report all notifications as invisible and turn down the
    // reporter.
    Boolean tmp = FALSE;
    if (mCurrentlyVisibleNotifications->IsEmpty(&tmp), !tmp) {
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        AutoPtr<IList> list;
        helper->GetEmptyList((IList**)&list);
        LogNotificationVisibilityChanges(ICollection::Probe(list)
                , ICollection::Probe(mCurrentlyVisibleNotifications));
        mCurrentlyVisibleNotifications->Clear();
    }
    mHandler->RemoveCallbacks(mVisibilityReporter);
    mStackScroller->SetChildLocationsChangedListener(NULL);
}

void CPhoneStatusBar::StartNotificationLoggingIfScreenOnAndVisible()
{
    if (mVisible && mScreenOn) {
        mStackScroller->SetChildLocationsChangedListener(mNotificationLocationsChangedListener);
        // Some transitions like mScreenOn=FALSE -> mScreenOn=TRUE don't
        // cause the scroller to emit child location events. Hence generate
        // one ourselves to guarantee that we're reporting visible
        // notifications.
        // (Note that in cases where the scroller does emit events, this
        // additional event doesn't break anything.)
        mNotificationLocationsChangedListener->OnChildLocationsChanged(mStackScroller);
    }
}

void CPhoneStatusBar::LogNotificationVisibilityChanges(
    /* [in] */ ICollection/*<String>*/* newlyVisible,
    /* [in] */ ICollection/*<String>*/* noLongerVisible)
{
    Boolean tmp = FALSE;
    if ((newlyVisible->IsEmpty(&tmp), tmp) && (noLongerVisible->IsEmpty(&tmp), tmp)) {
        return;
    }
    Int32 size1 = 0, size2 = 0;
    newlyVisible->GetSize(&size1);
    noLongerVisible->GetSize(&size2);

    AutoPtr<ArrayOf<IInterface*> > obs1 = ArrayOf<IInterface*>::Alloc(size1);
    AutoPtr<ArrayOf<IInterface*> > obs2 = ArrayOf<IInterface*>::Alloc(size2);

    AutoPtr<ArrayOf<IInterface*> > newlyVisibleAr;
    newlyVisible->ToArray(obs1, (ArrayOf<IInterface*>**)&newlyVisibleAr);

    AutoPtr<ArrayOf<IInterface*> > noLongerVisibleAr;
    noLongerVisible->ToArray(obs2, (ArrayOf<IInterface*>**)&noLongerVisibleAr);
    // try {
    AutoPtr<ArrayOf<String> > newly = ArrayOf<String>::Alloc(newlyVisibleAr->GetLength());
    String v;
    for (Int32 i = 0; i < newlyVisibleAr->GetLength(); i++) {
        AutoPtr<ICharSequence> obj = ICharSequence::Probe((*newlyVisibleAr)[i]);
        obj->ToString(&v);
        (*newly)[i] = v;
    }

    AutoPtr<ArrayOf<String> > noLonger = ArrayOf<String>::Alloc(noLongerVisibleAr->GetLength());
    for (Int32 i = 0; i < noLongerVisibleAr->GetLength(); i++) {
        AutoPtr<ICharSequence> obj = ICharSequence::Probe((*noLongerVisibleAr)[i]);
        obj->ToString(&v);
        (*noLonger)[i] = v;
    }
    mBarService->OnNotificationVisibilityChanged(newly, noLonger);
    // } catch (RemoteException e) {
    //     // Ignore.
    // }
}

//
// tracing
//
ECode CPhoneStatusBar::PostStartTracing()
{
    Boolean tmp = FALSE;
    mHandler->PostDelayed(mStartTracing, 3000, &tmp);
    return NOERROR;
}

ECode CPhoneStatusBar::Vibrate()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
    AutoPtr<IVibrator> vib = IVibrator::Probe(obj);
    vib->Vibrate(250, VIBRATION_ATTRIBUTES);
    return NOERROR;
}

void CPhoneStatusBar::HaltTicker()
{
    if (mTickerEnabled) {
        mTicker->Halt();
    }
}

Boolean CPhoneStatusBar::ShouldDisableNavbarGestures()
{
    Boolean tmp = FALSE;
    return (IsDeviceProvisioned(&tmp), !tmp)
            || mExpandedVisible
            || (mDisabled & IStatusBarManager::DISABLE_SEARCH) != 0;
}

ECode CPhoneStatusBar::PostStartSettingsActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 delay)
{
    AutoPtr<StartSettingsActivityRunnable> run = new StartSettingsActivityRunnable(this, intent);
    Boolean tmp = FALSE;
    mHandler->PostDelayed(run, delay, &tmp);
    return NOERROR;
}

void CPhoneStatusBar::HandleStartSettingsActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean onlyProvisioned)
{
    StartActivityDismissingKeyguard(intent, onlyProvisioned, TRUE /* dismissShade */);
}

ECode CPhoneStatusBar::Destroy()
{
    BaseStatusBar::Destroy();
    if (mStatusBarWindow != NULL) {
        mWindowManager->RemoveViewImmediate(IView::Probe(mStatusBarWindow));
        mStatusBarWindow = NULL;
    }
    if (mNavigationBarView != NULL) {
        mWindowManager->RemoveViewImmediate(IView::Probe(mNavigationBarView));
        mNavigationBarView = NULL;
    }
    mContext->UnregisterReceiver(mBroadcastReceiver);
    return NOERROR;
}

ECode CPhoneStatusBar::DispatchDemoCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* args)
{
    if (!mDemoModeAllowed) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        Int32 v = 0;
        Elastos::Droid::Provider::Settings::Global::GetInt32(cr, String("sysui_demo_allowed"), 0, &v);
        mDemoModeAllowed = v != 0;
    }
    if (!mDemoModeAllowed) return NOERROR;
    if (command.Equals(COMMAND_ENTER)) {
        mDemoMode = TRUE;
    }
    else if (command.Equals(COMMAND_EXIT)) {
        mDemoMode = FALSE;
        CheckBarModes();
    }
    else if (!mDemoMode) {
        // automatically enter demo mode on first demo command
        AutoPtr<IBundle> b;
        CBundle::New((IBundle**)&b);
        DispatchDemoCommand(COMMAND_ENTER, b);
    }
    Boolean modeChange = command.Equals(COMMAND_ENTER) || command.Equals(COMMAND_EXIT);
    if (modeChange || command.Equals(COMMAND_CLOCK)) {
        DispatchDemoCommandToView(command, args, R::id::clock);
    }
    if (modeChange || command.Equals(COMMAND_BATTERY)) {
        DispatchDemoCommandToView(command, args, R::id::battery);
    }
    if (modeChange || command.Equals(COMMAND_STATUS)) {
        if (mDemoStatusIcons == NULL) {
            mDemoStatusIcons = new DemoStatusIcons(mStatusIcons, mIconSize);
        }
        mDemoStatusIcons->DispatchDemoCommand(command, args);
    }
    if (mNetworkController != NULL && (modeChange || command.Equals(COMMAND_NETWORK))) {
        mNetworkController->DispatchDemoCommand(command, args);
    }
    if (modeChange || command.Equals(COMMAND_NOTIFICATIONS)) {
        AutoPtr<IView> notifications;
        if (mStatusBarView != NULL) {
            IView::Probe(mStatusBarView)->FindViewById(R::id::notification_icon_area, (IView**)&notifications);
        }
        if (notifications != NULL) {
            String visible;
            args->GetString(String("visible"), &visible);
            Int32 vis = mDemoMode && visible.Equals("FALSE") ? IView::INVISIBLE : IView::VISIBLE;
            notifications->SetVisibility(vis);
        }
    }
    if (command.Equals(COMMAND_BARS)) {
        String mode;
        args->GetString(String("mode"), &mode);
        Int32 barMode = mode.Equals("opaque") ? BarTransitions::MODE_OPAQUE :
                mode.Equals("translucent") ? BarTransitions::MODE_TRANSLUCENT :
                mode.Equals("semi-transparent") ? BarTransitions::MODE_SEMI_TRANSPARENT :
                mode.Equals("transparent") ? BarTransitions::MODE_TRANSPARENT :
                mode.Equals("warning") ? BarTransitions::MODE_WARNING :
                -1;
        if (barMode != -1) {
            Boolean animate = TRUE;
            if (mStatusBarView != NULL) {
                AutoPtr<IBarTransitions> bt;
                mStatusBarView->GetBarTransitions((IBarTransitions**)&bt);
                bt->TransitionTo(barMode, animate);
            }
            if (mNavigationBarView != NULL) {
                AutoPtr<IBarTransitions> bt;
                mNavigationBarView->GetBarTransitions((IBarTransitions**)&bt);
                bt->TransitionTo(barMode, animate);
            }
        }
    }
    return NOERROR;
}

void CPhoneStatusBar::DispatchDemoCommandToView(
    /* [in] */ const String& command,
    /* [in] */ IBundle* args,
    /* [in] */ Int32 id)
{
    if (mStatusBarView == NULL) return;
    AutoPtr<IView> v;
    IView::Probe(mStatusBarView)->FindViewById(id, (IView**)&v);
    if (IDemoMode::Probe(v)) {
        IDemoMode::Probe(v)->DispatchDemoCommand(command, args);
    }
}

/**
 * @return The {@link StatusBarState} the status bar is in.
 */
ECode CPhoneStatusBar::GetBarState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CPhoneStatusBar::ShowKeyguard()
{
    if (mLaunchTransitionFadingAway) {
        AutoPtr<IViewPropertyAnimator> vpa;
        IView::Probe(mNotificationPanel)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        IView::Probe(mNotificationPanel)->SetAlpha(1.f);
        if (mLaunchTransitionEndRunnable != NULL) {
            mLaunchTransitionEndRunnable->Run();
        }
        mLaunchTransitionEndRunnable = NULL;
        mLaunchTransitionFadingAway = FALSE;
    }
    SetBarState(IStatusBarState::KEYGUARD);
    UpdateKeyguardState(FALSE /* goingToFullShade */, FALSE /* fromShadeLocked */);
    if (!mScreenOnFromKeyguard) {

        // If the screen is off already, we need to disable touch events because these might
        // collapse the panel after we expanded it, and thus we would end up with a blank
        // Keyguard.
        IPanelView::Probe(mNotificationPanel)->SetTouchDisabled(TRUE);
    }
    InstantExpandNotificationsPanel();
    mLeaveOpenOnKeyguardHide = FALSE;
    if (mDraggedDownRow != NULL) {
        mDraggedDownRow->SetUserLocked(FALSE);
        IExpandableView::Probe(mDraggedDownRow)->NotifyHeightChanged();
        mDraggedDownRow = NULL;
    }
    return NOERROR;
}

ECode CPhoneStatusBar::IsCollapsing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return IPanelView::Probe(mNotificationPanel)->IsCollapsing(result);
}

ECode CPhoneStatusBar::AddPostCollapseAction(
    /* [in] */ IRunnable* r)
{
    mPostCollapseRunnables->Add(r);
    return NOERROR;
}

ECode CPhoneStatusBar::IsInLaunchTransition(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean s1 = FALSE, s2 = FALSE;
    *result = (mNotificationPanel->IsLaunchTransitionRunning(&s1), s1)
            || (mNotificationPanel->IsLaunchTransitionFinished(&s2), s2);
    return NOERROR;
}

/**
 * Fades the content of the keyguard away after the launch transition is done.
 *
 * @param beforeFading the runnable to be run when the circle is fully expanded and the fading
 *                     starts
 * @param endRunnable the runnable to be run when the transition is done
 */
ECode CPhoneStatusBar::FadeKeyguardAfterLaunchTransition(
    /* [in] */ IRunnable* beforeFading,
    /* [in] */ IRunnable* endRunnable)
{
    mLaunchTransitionEndRunnable = endRunnable;
    AutoPtr<FadeKeyguardRunnable> hideRunnable = new FadeKeyguardRunnable(this, beforeFading);
    Boolean tmp = FALSE;
    if (mNotificationPanel->IsLaunchTransitionRunning(&tmp), tmp) {
        mNotificationPanel->SetLaunchTransitionEndRunnable(hideRunnable);
    }
    else {
        hideRunnable->Run();
    }
    return NOERROR;
}

/**
 * @return TRUE if we would like to stay in the shade, FALSE if it should go away entirely
 */
ECode CPhoneStatusBar::HideKeyguard(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean staying = mLeaveOpenOnKeyguardHide;
    SetBarState(IStatusBarState::SHADE);
    if (mLeaveOpenOnKeyguardHide) {
        mLeaveOpenOnKeyguardHide = FALSE;
        Int64 v = 0;
        CalculateGoingToFullShadeDelay(&v);
        mNotificationPanel->AnimateToFullShade(v);
        if (mDraggedDownRow != NULL) {
            mDraggedDownRow->SetUserLocked(FALSE);
            mDraggedDownRow = NULL;
        }
    }
    else {
        InstantCollapseNotificationPanel();
    }
    UpdateKeyguardState(staying, FALSE /* fromShadeLocked */);
    *result = staying;
    return NOERROR;
}

ECode CPhoneStatusBar::CalculateGoingToFullShadeDelay(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeyguardFadingAwayDelay + mKeyguardFadingAwayDuration;
    return NOERROR;
}

/**
 * Notifies the status bar the Keyguard is fading away with the specified timings.
 *
 * @param delay the animation delay in miliseconds
 * @param fadeoutDuration the duration of the exit animation, in milliseconds
 */
ECode CPhoneStatusBar::SetKeyguardFadingAway(
    /* [in] */ Int64 delay,
    /* [in] */ Int64 fadeoutDuration)
{
    mKeyguardFadingAway = TRUE;
    mKeyguardFadingAwayDelay = delay;
    mKeyguardFadingAwayDuration = fadeoutDuration;
    mWaitingForKeyguardExit = FALSE;
    Disable(mDisabledUnmodified, TRUE /* animate */);
    return NOERROR;
}

ECode CPhoneStatusBar::IsKeyguardFadingAway(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeyguardFadingAway;
    return NOERROR;
}

/**
 * Notifies that the Keyguard fading away animation is done.
 */
ECode CPhoneStatusBar::FinishKeyguardFadingAway()
{
    mKeyguardFadingAway = FALSE;
    return NOERROR;
}

void CPhoneStatusBar::UpdatePublicMode()
{
    Boolean s1 = FALSE, s2 = FALSE, s3 = FALSE;
    SetLockscreenPublicMode(
            ((mStatusBarKeyguardViewManager->IsShowing(&s1), s1) ||
                    (mStatusBarKeyguardViewManager->IsOccluded(&s2), s2))
            && (mStatusBarKeyguardViewManager->IsSecure(&s3), s3));
}

void CPhoneStatusBar::UpdateKeyguardState(
    /* [in] */ Boolean goingToFullShade,
    /* [in] */ Boolean fromShadeLocked)
{
    if (mState == IStatusBarState::KEYGUARD) {
        mKeyguardIndicationController->SetVisible(TRUE);
        IPanelView::Probe(mNotificationPanel)->ResetViews();
        if (mKeyguardUserSwitcher != NULL) {
            mKeyguardUserSwitcher->SetKeyguard(TRUE, fromShadeLocked);
        }
    }
    else {
        mKeyguardIndicationController->SetVisible(FALSE);
        if (mKeyguardUserSwitcher != NULL) {
            mKeyguardUserSwitcher->SetKeyguard(FALSE,
                    goingToFullShade || mState == IStatusBarState::SHADE_LOCKED || fromShadeLocked);
        }
    }
    if (mState == IStatusBarState::KEYGUARD || mState == IStatusBarState::SHADE_LOCKED) {
        mScrimController->SetKeyguardShowing(TRUE);
    }
    else {
        mScrimController->SetKeyguardShowing(FALSE);
    }
    mNotificationPanel->SetBarState(mState, mKeyguardFadingAway, goingToFullShade);
    UpdateDozingState();
    UpdatePublicMode();
    UpdateStackScrollerState(goingToFullShade);
    UpdateNotifications();
    CheckBarModes();
    UpdateCarrierLabelVisibility(FALSE);
    UpdateMediaMetaData(FALSE);
    Boolean s1 = FALSE, s2 = FALSE;
    mStatusBarKeyguardViewManager->IsShowing(&s1);
    mStatusBarKeyguardViewManager->IsSecure(&s2);
    mKeyguardMonitor->NotifyKeyguardState(s1, s2);
}

void CPhoneStatusBar::UpdateDozingState()
{
    if (mState != IStatusBarState::KEYGUARD
        && ((CNotificationPanelView*)mNotificationPanel.Get())->IsDozing()) {
        return;
    }
    mNotificationPanel->SetDozing(mDozing);
    if (mDozing) {
        IView::Probe(mKeyguardBottomArea)->SetVisibility(IView::INVISIBLE);
        mStackScroller->SetDark(TRUE, FALSE /*animate*/);
    }
    else {
        IView::Probe(mKeyguardBottomArea)->SetVisibility(IView::VISIBLE);
        mStackScroller->SetDark(FALSE, FALSE /*animate*/);
    }
    mScrimController->SetDozing(mDozing);
}

ECode CPhoneStatusBar::UpdateStackScrollerState(
    /* [in] */ Boolean goingToFullShade)
{
    if (mStackScroller == NULL) return NOERROR;
    Boolean onKeyguard = mState == IStatusBarState::KEYGUARD;
    Boolean tmp = FALSE;
    IsLockscreenPublicMode(&tmp);
    mStackScroller->SetHideSensitive(tmp, goingToFullShade);
    mStackScroller->SetDimmed(onKeyguard, FALSE /* animate */);
    mStackScroller->SetExpandingEnabled(!onKeyguard);
    AutoPtr<IActivatableNotificationView> activatedChild;
    mStackScroller->GetActivatedChild((IActivatableNotificationView**)&activatedChild);
    mStackScroller->SetActivatedChild(NULL);
    if (activatedChild != NULL) {
        activatedChild->MakeInactive(FALSE /* animate */);
    }
    return NOERROR;
}

ECode CPhoneStatusBar::UserActivity()
{
    if (mState == IStatusBarState::KEYGUARD) {
        if (mKeyguardViewMediatorCallback != NULL) {
            mKeyguardViewMediatorCallback->UserActivity();
        }
    }
    return NOERROR;
}

ECode CPhoneStatusBar::InterceptMediaKey(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    *result = mState == IStatusBarState::KEYGUARD
            && (mStatusBarKeyguardViewManager->InterceptMediaKey(event, &tmp), tmp);
    return NOERROR;
}

ECode CPhoneStatusBar::OnMenuPressed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    *result = mState == IStatusBarState::KEYGUARD && (mStatusBarKeyguardViewManager->OnMenuPressed(&tmp), tmp);
    return NOERROR;
}

ECode CPhoneStatusBar::OnBackPressed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if (mStatusBarKeyguardViewManager->OnBackPressed(&tmp), tmp) {
        *result = TRUE;
        return NOERROR;
    }
    if (mNotificationPanel->IsQsExpanded(&tmp), tmp) {
        if (mNotificationPanel->IsQsDetailShowing(&tmp), tmp) {
            mNotificationPanel->CloseQsDetail();
        }
        else {
            mNotificationPanel->AnimateCloseQs();
        }
        *result = TRUE;
        return NOERROR;
    }
    if (mState != IStatusBarState::KEYGUARD && mState != IStatusBarState::SHADE_LOCKED) {
        AnimateCollapsePanels();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CPhoneStatusBar::OnSpacePressed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mScreenOn && (mState == IStatusBarState::KEYGUARD || mState == IStatusBarState::SHADE_LOCKED)) {
        AnimateCollapsePanels(0 /* flags */, TRUE /* force */);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

void CPhoneStatusBar::ShowBouncer()
{
    if (mState == IStatusBarState::KEYGUARD || mState == IStatusBarState::SHADE_LOCKED) {
        mStatusBarKeyguardViewManager->IsShowing(&mWaitingForKeyguardExit);
        mStatusBarKeyguardViewManager->Dismiss();
    }
}

void CPhoneStatusBar::InstantExpandNotificationsPanel()
{
    // Make our window larger and the panel expanded.
    MakeExpandedVisible(TRUE);
    IPanelView::Probe(mNotificationPanel)->InstantExpand();
}

void CPhoneStatusBar::InstantCollapseNotificationPanel()
{
    IPanelView::Probe(mNotificationPanel)->InstantCollapse();
}

ECode CPhoneStatusBar::OnActivated(
    /* [in] */ IActivatableNotificationView* view)
{
    mKeyguardIndicationController->ShowTransientIndication(R::string::notification_tap_again);
    AutoPtr<IActivatableNotificationView> previousView;
    mStackScroller->GetActivatedChild((IActivatableNotificationView**)&previousView);
    if (previousView != NULL) {
        previousView->MakeInactive(TRUE /* animate */);
    }
    mStackScroller->SetActivatedChild(view);
    return NOERROR;
}

/**
 * @param state The {@link StatusBarState} to set.
 */
ECode CPhoneStatusBar::SetBarState(
    /* [in] */ Int32 state)
{
    mState = state;
    mStatusBarWindowManager->SetStatusBarState(state);
    return NOERROR;
}

// @Override
ECode CPhoneStatusBar::OnActivationReset(
    /* [in] */ IActivatableNotificationView* view)
{
    AutoPtr<IActivatableNotificationView> v;
    mStackScroller->GetActivatedChild((IActivatableNotificationView**)&v);
    if (view == v.Get()) {
        mKeyguardIndicationController->HideTransientIndication();
        mStackScroller->SetActivatedChild(NULL);
    }
    return NOERROR;
}

ECode CPhoneStatusBar::OnTrackingStarted()
{
    RunPostCollapseRunnables();
    return NOERROR;
}

ECode CPhoneStatusBar::OnUnlockHintStarted()
{
    mKeyguardIndicationController->ShowTransientIndication(R::string::keyguard_unlock);
    return NOERROR;
}

ECode CPhoneStatusBar::OnHintFinished()
{
    // Delay the reset a bit so the user can read the text.
    mKeyguardIndicationController->HideTransientIndicationDelayed(HINT_RESET_DELAY_MS);
    return NOERROR;
}

ECode CPhoneStatusBar::OnCameraHintStarted()
{
    mKeyguardIndicationController->ShowTransientIndication(R::string::camera_hint);
    return NOERROR;
}

ECode CPhoneStatusBar::OnPhoneHintStarted()
{
    mKeyguardIndicationController->ShowTransientIndication(R::string::phone_hint);
    return NOERROR;
}

ECode CPhoneStatusBar::OnTrackingStopped(
    /* [in] */ Boolean expand)
{
    if (mState == IStatusBarState::KEYGUARD || mState == IStatusBarState::SHADE_LOCKED) {
        if (!expand && !mUnlockMethodCache->IsMethodInsecure()) {
            ShowBouncer();
        }
    }
    return NOERROR;
}

Int32 CPhoneStatusBar::GetMaxKeyguardNotifications()
{
    return mKeyguardMaxNotificationCount;
}

ECode CPhoneStatusBar::GetNavigationBarView(
    /* [out] */ INavigationBarView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mNavigationBarView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

// ---------------------- DragDownHelper.OnDragDownListener ------------------------------------
ECode CPhoneStatusBar::OnDraggedDown(
    /* [in] */ IView* startingChild,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if (HasActiveNotifications(&tmp), tmp) {
        // We have notifications, go to locked shade.
        GoToLockedShade(startingChild);
        *result = TRUE;
        return NOERROR;
    }

    // No notifications - abort gesture.
    *result = FALSE;
    return NOERROR;
}

ECode CPhoneStatusBar::OnDragDownReset()
{
    mStackScroller->SetDimmed(TRUE /* dimmed */, TRUE /* animated */);
    return NOERROR;
}

ECode CPhoneStatusBar::OnThresholdReached()
{
    mStackScroller->SetDimmed(FALSE /* dimmed */, TRUE /* animate */);
    return NOERROR;
}

ECode CPhoneStatusBar::OnTouchSlopExceeded()
{
    mStackScroller->RemoveLongPressCallback();
    return NOERROR;
}

ECode CPhoneStatusBar::SetEmptyDragAmount(
    /* [in] */ Float amount)
{
    mNotificationPanel->SetEmptyDragAmount(amount);
    return NOERROR;
}

/**
 * If secure with redaction: Show bouncer, go to unlocked shade.
 *
 * <p>If secure without redaction or no security: Go to {@link StatusBarState#SHADE_LOCKED}.</p>
 *
 * @param expandView The view to expand after going to the shade.
 */
ECode CPhoneStatusBar::GoToLockedShade(
    /* [in] */ IView* expandView)
{
    AutoPtr<IExpandableNotificationRow> row;
    if (IExpandableNotificationRow::Probe(expandView)) {
        row = IExpandableNotificationRow::Probe(expandView);
        row->SetUserExpanded(TRUE);
    }
    Boolean tmp = FALSE;
    UserAllowsPrivateNotificationsInPublic(mCurrentUserId, &tmp);
    Boolean fullShadeNeedsBouncer = !tmp || !mShowLockscreenNotifications;
    if ((IsLockscreenPublicMode(&tmp), tmp) && fullShadeNeedsBouncer) {
        mLeaveOpenOnKeyguardHide = TRUE;
        ShowBouncer();
        mDraggedDownRow = row;
    }
    else {
        mNotificationPanel->AnimateToFullShade(0 /* delay */);
        SetBarState(IStatusBarState::SHADE_LOCKED);
        UpdateKeyguardState(FALSE /* goingToFullShade */, FALSE /* fromShadeLocked */);
        if (row != NULL) {
            row->SetUserLocked(FALSE);
        }
    }
    return NOERROR;
}

/**
 * Goes back to the keyguard after hanging around in {@link StatusBarState#SHADE_LOCKED}.
 */
ECode CPhoneStatusBar::GoToKeyguard()
{
    if (mState == IStatusBarState::SHADE_LOCKED) {
        mStackScroller->OnGoToKeyguard();
        SetBarState(IStatusBarState::KEYGUARD);
        UpdateKeyguardState(FALSE /* goingToFullShade */, TRUE /* fromShadeLocked*/);
    }
    return NOERROR;
}

/**
 * @return a ViewGroup that spans the entire panel which contains the quick settings
 */
ECode CPhoneStatusBar::GetQuickSettingsOverlayParent(
    /* [out] */ IViewGroup** parent)
{
    VALIDATE_NOT_NULL(parent);
    *parent = IViewGroup::Probe(mNotificationPanel);
    REFCOUNT_ADD(*parent);
    return NOERROR;
}

ECode CPhoneStatusBar::GetKeyguardFadingAwayDelay(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeyguardFadingAwayDelay;
    return NOERROR;
}

ECode CPhoneStatusBar::GetKeyguardFadingAwayDuration(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeyguardFadingAwayDuration;
    return NOERROR;
}

ECode CPhoneStatusBar::GetSystemIcons(
    /* [out] */ ILinearLayout** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSystemIcons;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPhoneStatusBar::GetSystemIconArea(
    /* [out] */ ILinearLayout** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSystemIconArea;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// @Override
ECode CPhoneStatusBar::SetBouncerShowing(
    /* [in] */ Boolean bouncerShowing)
{
    BaseStatusBar::SetBouncerShowing(bouncerShowing);
    Disable(mDisabledUnmodified, TRUE /* animate */);
    return NOERROR;
}

ECode CPhoneStatusBar::OnScreenTurnedOff()
{
    mScreenOnFromKeyguard = FALSE;
    mScreenOnComingFromTouch = FALSE;
    mStackScroller->SetAnimationsEnabled(FALSE);
    return NOERROR;
}

ECode CPhoneStatusBar::OnScreenTurnedOn()
{
    mScreenOnFromKeyguard = TRUE;
    mStackScroller->SetAnimationsEnabled(TRUE);
    mNotificationPanel->OnScreenTurnedOn();
    IPanelView::Probe(mNotificationPanel)->SetTouchDisabled(FALSE);
    return NOERROR;
}

/**
 * This handles long-press of both back and recents.  They are
 * handled together to capture them both being long-pressed
 * at the same time to exit screen pinning (lock task).
 *
 * When accessibility mode is on, only a long-press from recents
 * is required to exit.
 *
 * In all other circumstances we try to pass through long-press events
 * for Back, so that apps can still use it.  Which can be from two things.
 * 1) Not currently in screen pinning (lock task).
 * 2) Back is long-pressed without recents.
 */
void CPhoneStatusBar::HandleLongPressBackRecents(
    /* [in] */ IView* v)
{
    ECode ec = NOERROR;
    do {
        Boolean sendBackLongPress = FALSE;
        AutoPtr<IIActivityManager> activityManager = ActivityManagerNative::GetDefault();
        Boolean isAccessiblityEnabled = FALSE;
        if (mAccessibilityManager) {
            mAccessibilityManager->IsEnabled(&isAccessiblityEnabled);
        }
        Boolean tmp = FALSE;
        Int32 id = 0;
        v->GetId(&id);
        ec = activityManager->IsInLockTaskMode(&tmp);
        if (FAILED(ec)) {
            break;
        }
        if (tmp && !isAccessiblityEnabled) {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 time = 0;
            system->GetCurrentTimeMillis(&time);
            // If we recently Int64-pressed the other button then they were
            // Int64-pressed 'together'
            if ((time - mLastLockToAppLongPress) < LOCK_TO_APP_GESTURE_TOLERENCE) {
                ec = activityManager->StopLockTaskModeOnCurrent();
                if (FAILED(ec)) {
                    break;
                }
            }
            else if (id == R::id::back) {
                AutoPtr<IView> view;
                mNavigationBarView->GetRecentsButton((IView**)&view);
                view->IsPressed(&tmp);
                if (!tmp) {
                    // If we aren't pressing recents right now then they presses
                    // won't be together, so send the standard Int64-press action.
                    sendBackLongPress = TRUE;
                }
            }
            mLastLockToAppLongPress = time;
        }
        else {
            // If this is back still need to handle sending the Int64-press event.
            if (id == R::id::back) {
                sendBackLongPress = TRUE;
            }
            else if (isAccessiblityEnabled && (activityManager->IsInLockTaskMode(&tmp), tmp)) {
                // When in accessibility mode a Int64 press that is recents (not back)
                // should stop lock task.
                ec = activityManager->StopLockTaskModeOnCurrent();
                if (FAILED(ec)) {
                    break;
                }
            }
        }
        if (sendBackLongPress) {
            AutoPtr<IKeyButtonView> keyButtonView = IKeyButtonView::Probe(v);
            keyButtonView->SendEvent(IKeyEvent::ACTION_DOWN, IKeyEvent::FLAG_LONG_PRESS);
            IView::Probe(keyButtonView)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_LONG_CLICKED);
        }
    } while (0);

    if (FAILED(ec)) {
        Logger::D(TAG, "Unable to reach activity manager");
    }
}

// Recents
void CPhoneStatusBar::ShowRecents(
    /* [in] */ Boolean triggeredFromAltTab)
{
    // Set the recents visibility flag
    mSystemUiVisibility |= IView::RECENT_APPS_VISIBLE;
    NotifyUiVisibilityChanged(mSystemUiVisibility);
    BaseStatusBar::ShowRecents(triggeredFromAltTab);
}

void CPhoneStatusBar::HideRecents(
    /* [in] */ Boolean triggeredFromAltTab,
    /* [in] */ Boolean triggeredFromHomeKey)
{
    // Unset the recents visibility flag
    mSystemUiVisibility &= ~IView::RECENT_APPS_VISIBLE;
    NotifyUiVisibilityChanged(mSystemUiVisibility);
    BaseStatusBar::HideRecents(triggeredFromAltTab, triggeredFromHomeKey);
}

void CPhoneStatusBar::ToggleRecents()
{
    // Toggle the recents visibility flag
    mSystemUiVisibility ^= IView::RECENT_APPS_VISIBLE;
    NotifyUiVisibilityChanged(mSystemUiVisibility);
    BaseStatusBar::ToggleRecents();
}

String CPhoneStatusBar::GetClass()
{
    return String("Elastos.Droid.SystemUI.StatusBar.Phone.CPhoneStatusBar");
}

ECode CPhoneStatusBar::OnVisibilityChanged(
    /* [in] */ Boolean visible)
{
    // Update the recents visibility flag
    if (visible) {
        mSystemUiVisibility |= IView::RECENT_APPS_VISIBLE;
    }
    else {
        mSystemUiVisibility &= ~IView::RECENT_APPS_VISIBLE;
    }
    NotifyUiVisibilityChanged(mSystemUiVisibility);
    return NOERROR;
}

ECode CPhoneStatusBar::HasActiveNotifications(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayList> list;  /*<INotificationDataEntry*/
    mNotificationData->GetActiveNotifications((IArrayList**)&list);
    Boolean tmp = FALSE;
    list->IsEmpty(&tmp);
    *result = !tmp;
    return NOERROR;
}

ECode CPhoneStatusBar::WakeUpIfDozing(
    /* [in] */ Int64 time,
    /* [in] */ Boolean fromTouch)
{
    Boolean tmp = FALSE;
    if (mDozing && (mScrimController->IsPulsing(&tmp), tmp)) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
        AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
        pm->WakeUp(time);
        if (fromTouch) {
            mScreenOnComingFromTouch = TRUE;
        }
    }
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
