#include "elastos/droid/systemui/statusbar/tablet/TabletStatusBar.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/systemui/statusbar/policy/Prefs.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/statusbar/CStatusBarNotification.h"
#include "elastos/droid/systemui/statusbar/CStatusBarIconView.h"
#include "elastos/droid/systemui/statusbar/tablet/TabletTicker.h"
#include "elastos/droid/systemui/statusbar/policy/NetworkController.h"
#include "elastos/droid/animation/CLayoutTransition.h"
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/R.h"

using Elastos::Core::Math;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::R;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::InputMethodService::IInputMethodService;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::Drawable::ILayerDrawable;
using Elastos::Droid::View::View;
using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::ImageViewScaleType_CENTER_INSIDE;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IScrollView;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Animation::EIID_ITransitionListener;
using Elastos::Droid::StatusBar::CStatusBarNotification;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::StatusBar::CStatusBarIconView;
using Elastos::Droid::SystemUI::StatusBar::Policy::Prefs;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkControllerSignalCluster;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

// {5accca39-c6fd-4930-ba8a-5530fff57edb}
extern "C" const InterfaceID EIID_TabletStatusBar =
    { 0x5accca39, 0xc6fd, 0x4930, { 0xba, 0x8a, 0x55, 0x30, 0xff, 0xf5, 0x7e, 0xdb } };

const String TabletStatusBar::TAG("TabletStatusBar");
const Boolean TabletStatusBar::DEBUG = FALSE;
const Boolean TabletStatusBar::DEBUG_COMPAT_HELP = FALSE;

// Fitts' Law assistance for LatinIME; see policy.EventHole
const Boolean TabletStatusBar::FAKE_SPACE_BAR = TRUE;

const Int32 TabletStatusBar::NOTIFICATION_PRIORITY_MULTIPLIER= 10; // see NotificationManagerService
const Int32 TabletStatusBar::HIDE_ICONS_BELOW_SCORE= INotification::PRIORITY_LOW * 10;

const Int32 TabletStatusBar::MSG_OPEN_NOTIFICATION_PANEL= 1000;
const Int32 TabletStatusBar::MSG_CLOSE_NOTIFICATION_PANEL= 1001;
const Int32 TabletStatusBar::MSG_OPEN_NOTIFICATION_PEEK= 1002;
const Int32 TabletStatusBar::MSG_CLOSE_NOTIFICATION_PEEK= 1003;
// 1020-1029 reserved for BaseStatusBar
const Int32 TabletStatusBar::MSG_SHOW_CHROME= 1030;
const Int32 TabletStatusBar::MSG_HIDE_CHROME= 1031;
const Int32 TabletStatusBar::MSG_OPEN_INPUT_METHODS_PANEL= 1040;
const Int32 TabletStatusBar::MSG_CLOSE_INPUT_METHODS_PANEL= 1041;
const Int32 TabletStatusBar::MSG_OPEN_COMPAT_MODE_PANEL= 1050;
const Int32 TabletStatusBar::MSG_CLOSE_COMPAT_MODE_PANEL= 1051;
const Int32 TabletStatusBar::MSG_STOP_TICKER= 2000;

// Notification "peeking" (flyover preview of individual notifications)
const Int32 TabletStatusBar::NOTIFICATION_PEEK_HOLD_THRESH= 200; // ms
const Int32 TabletStatusBar::NOTIFICATION_PEEK_FADE_DELAY= 3000; // ms

//================================================================================
//              TabletStatusBar::ShowSearchPanelRunnable
//================================================================================

TabletStatusBar::ShowSearchPanelRunnable::ShowSearchPanelRunnable(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::ShowSearchPanelRunnable::Run()
{
    mHost->ShowSearchPanel();
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::HomeSearchActionListener
//================================================================================
CAR_INTERFACE_IMPL(TabletStatusBar::HomeSearchActionListener, IViewOnTouchListener);

TabletStatusBar::HomeSearchActionListener::HomeSearchActionListener(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::HomeSearchActionListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action;
    event->GetAction(&action);
    switch(action) {
        case IMotionEvent::ACTION_DOWN:
            if (!mHost->ShouldDisableNavbarGestures() && !mHost->InKeyguardRestrictedInputMode()) {
                Boolean result;
                mHost->mHandler->RemoveCallbacks(mHost->mShowSearchPanel);
                mHost->mHandler->PostDelayed(mHost->mShowSearchPanel,
                    mHost->mShowSearchHoldoff, &result);
            }
        break;

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL: {
            mHost->mHandler->RemoveCallbacks(mHost->mShowSearchPanel);
        }
        break;
    }

    *result = FALSE;
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::ScreenshotTimeoutRunnable
//================================================================================
TabletStatusBar::ScreenshotTimeoutRunnable::ScreenshotTimeoutRunnable(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::ScreenshotTimeoutRunnable::Run()
{
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::NotificationTriggerTouchListener::HiliteOnRRunnable
//================================================================================
TabletStatusBar::NotificationTriggerTouchListener::HiliteOnRRunnable::HiliteOnRRunnable(
    /* [in] */ TabletStatusBar* host)
    : mHost(host)
{}

ECode TabletStatusBar::NotificationTriggerTouchListener::HiliteOnRRunnable::Run()
{
    return mHost->mNotificationArea->SetBackgroundResource(
        R::drawable::list_selector_pressed_holo_dark);
}

//================================================================================
//              TabletStatusBar::NotificationTriggerTouchListener
//================================================================================
CAR_INTERFACE_IMPL(TabletStatusBar::NotificationTriggerTouchListener, IViewOnTouchListener);

TabletStatusBar::NotificationTriggerTouchListener::NotificationTriggerTouchListener(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
    , mInitialTouchX(0)
    , mInitialTouchY(0)
    , mTouchSlop(0)
{
    mHiliteOnR = new HiliteOnRRunnable(mHost);
}

void TabletStatusBar::NotificationTriggerTouchListener::Hilite(
    /* [in] */ Boolean on)
{
    Elastos::Droid::View::View* view = reinterpret_cast<Elastos::Droid::View::View*>(
        mHost->mNotificationArea->Probe(EIID_View));
    if (on) {
        view->PostDelayed(mHiliteOnR, 100);
    }
    else {
        view->RemoveCallbacks(mHiliteOnR);
        mHost->mNotificationArea->SetBackground(NULL);
    }
}

ECode TabletStatusBar::NotificationTriggerTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
//            Slog.d(TAG, String.format("touch: (%.1f, %.1f) initial: (%.1f, %.1f)",
//                        event.getX(),
//                        event.getY(),
//                        mInitialTouchX,
//                        mInitialTouchY));

    VALIDATE_NOT_NULL(result);

    if ((mHost->mDisabled & IStatusBarManager::DISABLE_EXPAND) != 0) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 action;
    event->GetAction(&action);
    switch (action) {
    case IMotionEvent::ACTION_DOWN:
        mVT = VelocityTracker::Obtain();
        event->GetX(&mInitialTouchX);
        event->GetY(&mInitialTouchY);
        Hilite(true);
        // fall through

    case IMotionEvent::ACTION_OUTSIDE:
    case IMotionEvent::ACTION_MOVE:
        // check for fling
        if (mVT != NULL) {
            mVT->AddMovement(event);
            mVT->ComputeCurrentVelocity(1000); // pixels per second
            // require a little more oomph once we're already in peekaboo mode
            Float y;
            mVT->GetYVelocity(&y);
            if (y < -mHost->mNotificationFlingVelocity) {
                mHost->AnimateExpandNotificationsPanel();
                mHost->VisibilityChanged(true);
                Hilite(FALSE);
                mVT->Recycle();
                mVT = NULL;
            }
        }
        *result = TRUE;
        return NOERROR;

    case IMotionEvent::ACTION_UP:
    case IMotionEvent::ACTION_CANCEL:
        Hilite(FALSE);
        if (mVT != NULL) {
            Float x, y;
            event->GetX(&x);
            event->GetY(&y);
            Int32 bottom;
            v->GetBottom(&bottom);
            if (action == IMotionEvent::ACTION_UP
             // was this a sloppy tap?
             && Elastos::Core::Math::Abs(x - mInitialTouchX) < mTouchSlop
             && Elastos::Core::Math::Abs(y - mInitialTouchY) < (mTouchSlop / 3)
             // dragging off the bottom doesn't count
             && (Int32)y < bottom) {
                mHost->AnimateExpandNotificationsPanel();
                mHost->VisibilityChanged(TRUE);
                Elastos::Droid::View::View* view = reinterpret_cast<Elastos::Droid::View::View*>(
                    v->Probe(EIID_View));
                view->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_CLICKED);
                v->PlaySoundEffect(SoundEffectConstants::CLICK);
            }

            mVT->Recycle();
            mVT = NULL;
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::TableStatusBarBroadcastReceiver
//================================================================================

TabletStatusBar::TableStatusBarBroadcastReceiver::TableStatusBarBroadcastReceiver(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::TableStatusBarBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::TouchOutsideListener
//================================================================================
CAR_INTERFACE_IMPL(TabletStatusBar::TouchOutsideListener, IViewOnTouchListener);

TabletStatusBar::TouchOutsideListener::TouchOutsideListener(
    /* [in] */ Int32 msg,
    /* [in] */ IStatusBarPanel* panel,
    /* [in] */ TabletStatusBar* bar)
    : mMsg(msg)
    , mPanel(panel)
    , mHost(bar)
{}

ECode TabletStatusBar::TouchOutsideListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    event->GetAction(&action);
    Float fx, fy;
    event->GetX(&fx);
    event->GetY(&fy);
    Boolean isIn;
    if (action == IMotionEvent::ACTION_OUTSIDE || (action == IMotionEvent::ACTION_DOWN
            && (mPanel->IsInContentArea((Int32)fx, (Int32)fy, &isIn), !isIn))) {
        mHost->RemoveAndSendMessage(mMsg);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::ShadowTouchListener
//================================================================================
CAR_INTERFACE_IMPL(TabletStatusBar::ShadowTouchListener, IViewOnTouchListener)

TabletStatusBar::ShadowTouchListener::ShadowTouchListener(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::ShadowTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        // even though setting the systemUI visibility below will turn these views
        // on, we need them to come up faster so that they can catch this motion
        // event
        mHost->mShadow->SetVisibility(IView::GONE);
        mHost->mBarContents->SetVisibility(IView::VISIBLE);

        // try {
        if (mHost->mBarService != NULL)
            mHost->mBarService->SetSystemUiVisibility(0, IView::SYSTEM_UI_FLAG_LOW_PROFILE);
        // } catch (RemoteException ex) {
        //     // system process dead
        // }
    }

    *result = FALSE;
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::CompatibilityHelpDialogClickListener
//================================================================================
CAR_INTERFACE_IMPL(TabletStatusBar::CompatibilityHelpDialogClickListener, IViewOnClickListener);

TabletStatusBar::CompatibilityHelpDialogClickListener::CompatibilityHelpDialogClickListener(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::CompatibilityHelpDialogClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->HideCompatibilityHelp();
    AutoPtr<ISharedPreferencesEditor> editor = Prefs::Edit(mHost->mContext);
    if (editor) {
        editor->PutBoolean(Prefs::SHOWN_COMPAT_MODE_HELP, TRUE);
        editor->Apply();
    }
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::CompatibilityHelpDialogClickListener
//================================================================================
CAR_INTERFACE_IMPL(TabletStatusBar::NotificationDNDClickListener, IViewOnClickListener);

TabletStatusBar::NotificationDNDClickListener::NotificationDNDClickListener(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::NotificationDNDClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<ISharedPreferencesEditor> editor = Prefs::Edit(mHost->mContext);
    if (editor) {
        editor->PutBoolean(Prefs::DO_NOT_DISTURB_PREF, TRUE);
        editor->Apply();
    }

    mHost->AnimateCollapsePanels();
    mHost->VisibilityChanged(FALSE);
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::TabletStatusBarClickListener
//================================================================================
CAR_INTERFACE_IMPL(TabletStatusBar::TabletStatusBarClickListener, IViewOnClickListener);

TabletStatusBar::TabletStatusBarClickListener::TabletStatusBarClickListener(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::TabletStatusBarClickListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);

    Int32 id;
    v->GetId(&id);
    if (id == SystemUIR::id::recent_apps) {
        mHost->OnClickRecentButton();
    }
    else if (id == SystemUIR::id::imeSwitchButton) {
        mHost->OnClickInputMethodSwitchButton();
    }
    else if (id == SystemUIR::id::compatModeButton) {
        mHost->OnClickCompatModeButton();
    }
    else if(id == SystemUIR::id::screenshot) {
        // mHost->TakeScreenshot();
    }
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::TakeScreenshotServiceConnection
//================================================================================
CAR_INTERFACE_IMPL(TabletStatusBar::TakeScreenshotServiceConnection, IViewOnClickListener);

TabletStatusBar::TakeScreenshotServiceConnection::TakeScreenshotServiceConnection(
    /* [in] */ TabletStatusBar* bar)
    : mHost(bar)
{}

ECode TabletStatusBar::TakeScreenshotServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    // AutoLock lock(mHost->mScreenshotLock);

    // if (mHost->mScreenshotConnection != this) {
    //     return NOERROR;
    // }

    // Messenger messenger = new Messenger(service);
    // Message msg = Message.obtain(NULL, 1);
    // ServiceConnection myConn = this;
    // Handler h = new Handler(mHandler->GetLooper()) {
    //     @Override
    //     public void handleMessage(Message msg) {
    //         synchronized(mScreenshotLock) {
    //             if (mScreenshotConnection == myConn) {
    //                 mContext.unbindService(mScreenshotConnection);
    //                 mScreenshotConnection = NULL;
    //                 mHandler.removeCallbacks(mScreenshotTimeout);
    //             }
    //         }
    //     }
    // };
    // msg.replyTo = new Messenger(h);
    // msg.arg1 = msg.arg2 = 0;
    // msg.arg1 = 1;
    // /*if (mStatusBar != NULL && mStatusBar.isVisibleLw())
    //     msg.arg1 = 1;
    // if (mNavigationBar != NULL && mNavigationBar.isVisibleLw())
    //     msg.arg2 = 1;*/
    // try {
    //     messenger.send(msg);
    // } catch (RemoteException e) {
    // }

    return NOERROR;
}

ECode TabletStatusBar::TakeScreenshotServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::MyTransitionListener
//================================================================================

CAR_INTERFACE_IMPL(TabletStatusBar::MyTransitionListener, ITransitionListener)

TabletStatusBar::MyTransitionListener::MyTransitionListener(
    /* [in] */ IViewGroup* v)
{
    mBar = v;
}

ECode TabletStatusBar::MyTransitionListener::StartTransition(
    /* [in] */ ILayoutTransition* transition,
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ Int32 transitionType)
{
    mBar->Invalidate();
    return NOERROR;
}

ECode TabletStatusBar::MyTransitionListener::EndTransition(
    /* [in] */ ILayoutTransition* transition,
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ Int32 transitionType)
{
    return NOERROR;
}

//================================================================================
//              TabletStatusBar::LocalHandler
//================================================================================

ECode TabletStatusBar::LocalHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
   BaseStatusBar::MyHandler::HandleMessage(msg);
   Int32 what;
   msg->GetWhat(&what);
    switch (what) {
        case TabletStatusBar::MSG_OPEN_NOTIFICATION_PEEK: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (DEBUG) Slogger::D(TAG, "opening notification peek window; arg=%d", arg1);

            if (arg1 >= 0) {
                const Int32 N = mHost->mNotificationData->Size();

                if (!mHost->mNotificationDNDMode) {
                    if (mHost->mNotificationPeekIndex >= 0 && mHost->mNotificationPeekIndex < N) {
                        AutoPtr<NotificationDataEntry> entry = mHost->mNotificationData->Get(N- 1 - mHost->mNotificationPeekIndex);
                        entry->mIcon->SetBackgroundColor(0);
                        mHost->mNotificationPeekIndex = -1;
                        mHost->mNotificationPeekKey = NULL;
                    }
                }

                const Int32 peekIndex = arg1;
                if (peekIndex < N) {
                    //Slog.d(TAG, "loading peek: " + peekIndex);
                    AutoPtr<NotificationDataEntry> entry =
                        mHost->mNotificationDNDMode
                            ? mHost->mNotificationDNDDummyEntry
                            : mHost->mNotificationData->Get(N-1-peekIndex);
                    AutoPtr<NotificationDataEntry> copy = new NotificationDataEntry(
                            entry->mKey,
                            entry->mNotification,
                            entry->mIcon);
                    mHost->InflateViews(copy, mHost->mNotificationPeekRow);

                    if (mHost->mNotificationDNDMode) {
                        AutoPtr<IViewOnClickListener> listener = new NotificationDNDClickListener(mHost);
                        copy->mContent->SetOnClickListener(listener);
                    }

                    entry->mIcon->SetBackgroundColor(0x20FFFFFF);

//                          mNotificationPeekRow.setLayoutTransition(
//                              peekIndex < mNotificationPeekIndex
//                                  ? mNotificationPeekScrubLeft
//                                  : mNotificationPeekScrubRight);

                    mHost->mNotificationPeekRow->RemoveAllViews();
                    mHost->mNotificationPeekRow->AddView(copy->mRow);

                    mHost->mNotificationPeekWindow->SetVisibility(IView::VISIBLE);
                    mHost->mNotificationPanel->Show(FALSE, TRUE);

                    mHost->mNotificationPeekIndex = peekIndex;
                    mHost->mNotificationPeekKey = entry->mKey;
                }
            }
            break;
        }
        case TabletStatusBar::MSG_CLOSE_NOTIFICATION_PEEK: {
            if (DEBUG) Slogger::D(TAG, "closing notification peek window");

            mHost->mNotificationPeekWindow->SetVisibility(IView::GONE);
            mHost->mNotificationPeekRow->RemoveAllViews();

            Int32 N = mHost->mNotificationData->Size();
            if (mHost->mNotificationPeekIndex >= 0 && mHost->mNotificationPeekIndex < N) {
                AutoPtr<NotificationDataEntry> entry =
                    mHost->mNotificationDNDMode
                        ? mHost->mNotificationDNDDummyEntry
                        : mHost->mNotificationData->Get(N-1-mHost->mNotificationPeekIndex);
                entry->mIcon->SetBackgroundColor(0);
            }

            mHost->mNotificationPeekIndex = -1;
            mHost->mNotificationPeekKey = NULL;
            break;
        }
        case TabletStatusBar::MSG_OPEN_NOTIFICATION_PANEL: {
            if (DEBUG) Slogger::D(TAG, "opening notifications panel");

            Boolean isShowing;
            mHost->mNotificationPanel->IsShowing(&isShowing);
            if (!isShowing) {
                mHost->mNotificationPanel->Show(TRUE, TRUE);
                mHost->mNotificationArea->SetVisibility(IView::INVISIBLE);
                mHost->mTicker->Halt();
            }
            break;
        }
        case TabletStatusBar::MSG_CLOSE_NOTIFICATION_PANEL: {
            if (DEBUG) Slogger::D(TAG, "closing notifications panel");

            Boolean isShowing;
            mHost->mNotificationPanel->IsShowing(&isShowing);
            if (isShowing) {
                mHost->mNotificationPanel->Show(FALSE, TRUE);
                mHost->mNotificationArea->SetVisibility(IView::VISIBLE);
            }
            break;
        }
        case TabletStatusBar::MSG_OPEN_INPUT_METHODS_PANEL:
            if (DEBUG) Slogger::D(TAG, "opening input methods panel");
            if (mHost->mInputMethodsPanel != NULL) mHost->mInputMethodsPanel->OpenPanel();
            break;
        case TabletStatusBar::MSG_CLOSE_INPUT_METHODS_PANEL:
            if (DEBUG) Slogger::D(TAG, "closing input methods panel");
            if (mHost->mInputMethodsPanel != NULL) mHost->mInputMethodsPanel->ClosePanel(FALSE);
            break;
        case TabletStatusBar::MSG_OPEN_COMPAT_MODE_PANEL:
            if (DEBUG) Slogger::D(TAG, "opening compat panel");
            if (mHost->mCompatModePanel != NULL) mHost->mCompatModePanel->OpenPanel();
            break;
        case TabletStatusBar::MSG_CLOSE_COMPAT_MODE_PANEL:
            if (DEBUG) Slogger::D(TAG, "closing compat panel");
            if (mHost->mCompatModePanel != NULL) mHost->mCompatModePanel->ClosePanel();
            break;
        case TabletStatusBar::MSG_SHOW_CHROME:
            if (DEBUG) Slogger::D(TAG, "hiding shadows (lights on)");

            mHost->mBarContents->SetVisibility(IView::VISIBLE);
            mHost->mShadow->SetVisibility(IView::GONE);
            mHost->mSystemUiVisibility &= ~IView::SYSTEM_UI_FLAG_LOW_PROFILE;
            mHost->NotifyUiVisibilityChanged();
            break;
        case TabletStatusBar::MSG_HIDE_CHROME:
            if (DEBUG) Slogger::D(TAG, "showing shadows (lights out)");

            mHost->AnimateCollapsePanels();
            mHost->VisibilityChanged(FALSE);
            mHost->mBarContents->SetVisibility(IView::GONE);
            mHost->mShadow->SetVisibility(IView::VISIBLE);
            mHost->mSystemUiVisibility |= IView::SYSTEM_UI_FLAG_LOW_PROFILE;
            mHost->NotifyUiVisibilityChanged();
            break;
        case TabletStatusBar::MSG_STOP_TICKER:
            mHost->mTicker->Halt();
            break;
    }
    return NOERROR;
}
//================================================================================
//              TabletStatusBar
//================================================================================

TabletStatusBar::TabletStatusBar()
    : mNaturalBarHeight(-1)
    , mIconSize(-1)
    , mIconHPadding(-1)
    , mNavIconWidth(-1)
    , mMenuNavIconWidth(-1)
    , mMaxNotificationIcons(5)
    , mNotificationDNDMode(0)
    , mNotificationPeekIndex(0)
    , mNotificationPeekTapDuration(0)
    , mNotificationFlingVelocity(0)
    , mDisabled(0)
    , mTakingScreenShot(FALSE)
    , mAltBackButtonEnabledForIme(FALSE)
    , mSystemUiVisibility(0)
    , mNavigationIconHints(0)
    , mShowSearchHoldoff(0)
    , mShow(TRUE)
    , mHasVolume(FALSE)
{
    // String value = SystemProperties::Get(String("hw.nobattery"), String("false"));
    // mHWNoBattery = value.CompareIgnoreCase("true") == 0 ? TRUE : FALSE;
    // value = SystemProperties::Get(String("ro.statusbar.volume"), String("false"));
    // mSoftVolumeKey = value.CompareIgnoreCase("true") == 0 ? TRUE : FALSE;

    mShowSearchPanel = new ShowSearchPanelRunnable(this);
    mHomeSearchActionListener = new HomeSearchActionListener(this);
    mScreenshotTimeout = new ScreenshotTimeoutRunnable(this);
    mOnClickListener = new TabletStatusBarClickListener(this);
    mBroadcastReceiver = new TableStatusBarBroadcastReceiver(this);
    mHandler = new LocalHandler(this);
}

AutoPtr<IContext> TabletStatusBar::GetContext()
{
    return mContext;
}

// @Override
ECode TabletStatusBar::CreateAndAddWindows()
{
    AddStatusBarWindow();
    AddPanelWindows();
    return NOERROR;
}

void TabletStatusBar::AddStatusBarWindow()
{
    AutoPtr<IView> sb = MakeStatusBarView();

    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
        IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::MATCH_PARENT,
        IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR,
        IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
            | IWindowManagerLayoutParams::FLAG_TOUCHABLE_WHEN_WAKING
            | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH,
        IPixelFormat::OPAQUE, (IWindowManagerLayoutParams**)&lp);

    // We explicitly leave FLAG_HARDWARE_ACCELERATED out of the flags.  The status bar occupies
    // very little screen real-estate and is updated fairly frequently.  By using CPU rendering
    // for the status bar, we prevent the GPU from having to wake up just to do these small
    // updates, which should help keep power consumption down.

    String title("SystemBar");
    String pkgName;
    mContext->GetPackageName(&pkgName);
    lp->SetGravity(GetStatusBarGravity());

    AutoPtr<ICharSequence> titleSeq;
    CString::New(title, (ICharSequence**)&titleSeq);
    lp->SetTitle(titleSeq);
    lp->SetPackageName(pkgName);

    mWindowManager->AddView(sb, lp);
}

void TabletStatusBar::AddPanelWindows()
{
    AutoPtr<IContext> context = mContext;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<IView> tmpView;

    // Notification Panel
    Elastos::Droid::View::View::Inflate(context, SystemUIR::layout::system_bar_notification_panel,
        NULL, (IView**)&tmpView);
    assert(tmpView != NULL);

    mNotificationPanel = INotificationPanel::Probe(tmpView.Get());
    assert(mNotificationPanel != NULL);
    mNotificationPanel->SetBar(THIS_PROBE(ITabletStatusBar));
    mNotificationPanel->Show(FALSE, FALSE);
    AutoPtr<IViewOnTouchListener> touchListener = new TouchOutsideListener(
        MSG_CLOSE_NOTIFICATION_PANEL, IStatusBarPanel::Probe(mNotificationPanel.Get()), this);
    mNotificationPanel->SetOnTouchListener(touchListener);

 //    // the battery icon
 //     tmpView = NULL;
 //     mNotificationPanel->FindViewById(SystemUIR::id::battery, (IView**)&tmpView);
    //     mBatteryController->AddIconView(IImageView::Probe(tmpView.Get()));
 //     tmpView = NULL;
 //     mNotificationPanel->FindViewById(SystemUIR::id::battery_text, (IView**)&tmpView);
    //     mBatteryController-AddLabelView(ITextView::Probe(tmpView.Get()));

 //    // Bt
    // tmpView = NULL;
    // mNotificationPanel->FindViewById(SystemUIR::id::bluetooth, (IView**)&tmpView);
 //    mBluetoothController->AddIconView(IImageView::Probe(tmpView.Get()));

    // network icons: either a combo icon that switches between mobile and data, or distinct
    // mobile and data icons
    tmpView = NULL;
    mNotificationPanel->FindViewById(SystemUIR::id::mobile_signal, (IView**)&tmpView);
    AutoPtr<IImageView> mobileRSSI = IImageView::Probe(tmpView.Get());
    if (mobileRSSI != NULL) {
        if (mNetworkController) mNetworkController->AddPhoneSignalIconView(mobileRSSI);
    }

    tmpView = NULL;
    mNotificationPanel->FindViewById(SystemUIR::id::wifi_signal, (IView**)&tmpView);
    AutoPtr<IImageView> wifiRSSI = IImageView::Probe(tmpView.Get());
    if (wifiRSSI != NULL) {
        if (mNetworkController) mNetworkController->AddWifiIconView(wifiRSSI);
    }
    tmpView = NULL;
    mNotificationPanel->FindViewById(SystemUIR::id::wifi_text, (IView**)&tmpView);
    assert(ITextView::Probe(tmpView.Get()) != NULL);
    if (mNetworkController) mNetworkController->AddWifiLabelView(ITextView::Probe(tmpView.Get()));

    tmpView = NULL;
    mNotificationPanel->FindViewById(SystemUIR::id::mobile_type, (IView**)&tmpView);
    assert(IImageView::Probe(tmpView.Get()) != NULL);
    if (mNetworkController) mNetworkController->AddDataTypeIconView(IImageView::Probe(tmpView.Get()));

    tmpView = NULL;
    mNotificationPanel->FindViewById(SystemUIR::id::mobile_text, (IView**)&tmpView);
    assert(ITextView::Probe(tmpView.Get()) != NULL);
    if (mNetworkController) mNetworkController->AddMobileLabelView(ITextView::Probe(tmpView.Get()));

    tmpView = NULL;
    mBarContents->FindViewById(SystemUIR::id::network_text, (IView**)&tmpView);
    assert(ITextView::Probe(tmpView.Get()) != NULL);
    if (mNetworkController) mNetworkController->AddCombinedLabelView(ITextView::Probe(tmpView.Get()));

    mStatusBarView->SetIgnoreChildren(0, mNotificationTrigger, mNotificationPanel);

    Int32 notificationPanelWidth;
    res->GetDimensionPixelSize(SystemUIR::dimen::notification_panel_width, &notificationPanelWidth);
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
        notificationPanelWidth,
        GetNotificationPanelHeight(),
        IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL,
        IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS
            | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM
            | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH
            | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED,
        IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);
    lp->SetGravity(IGravity::BOTTOM | IGravity::RIGHT);
    AutoPtr<ICharSequence> titleSeq;
    CString::New(String("NotificationPanel"), (ICharSequence**)&titleSeq);
    lp->SetTitle(titleSeq);
    lp->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNCHANGED
        | IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_NOTHING);
    lp->SetWindowAnimations(R::style::Animation); // == no animation
    // lp.windowAnimations = R::style::Animation_ZoomButtons; // simple fade

    mNotificationPanelParams = lp;
    mWindowManager->AddView(mNotificationPanel, lp);

    // Search Panel
    mStatusBarView->SetBar(THIS_PROBE(IBaseStatusBar));
    mHomeButton->SetOnTouchListener(mHomeSearchActionListener);

    UpdateSearchPanel();

    // Input methods Panel
    tmpView = NULL;
    Elastos::Droid::View::View::Inflate(context, SystemUIR::layout::system_bar_input_methods_panel,
        NULL, (IView**)&tmpView);
    mInputMethodsPanel = IInputMethodsPanel::Probe(tmpView.Get());
    mInputMethodsPanel->SetHardKeyboardEnabledChangeListener(THIS_PROBE(IOnHardKeyboardEnabledChangeListener));
    touchListener = new TouchOutsideListener(
        MSG_CLOSE_INPUT_METHODS_PANEL, IStatusBarPanel::Probe(mInputMethodsPanel.Get()), this);
    mInputMethodsPanel->SetOnTouchListener(touchListener);
    mInputMethodsPanel->SetImeSwitchButton(mInputMethodSwitchButton);
    mStatusBarView->SetIgnoreChildren(2, mInputMethodSwitchButton, mInputMethodsPanel);

    lp = NULL;
    CWindowManagerLayoutParams::New(
        IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::WRAP_CONTENT,
        IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL,
        IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM
            | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH
            | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED,
        IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);
    lp->SetGravity(IGravity::BOTTOM | IGravity::RIGHT);
    titleSeq = NULL;
    CString::New(String("InputMethodsPanel"), (ICharSequence**)&titleSeq);
    lp->SetTitle(titleSeq);
    lp->SetWindowAnimations(SystemUIR::style::Animation_RecentPanel);

    mWindowManager->AddView(mInputMethodsPanel, lp);

    // Compatibility mode selector panel
    tmpView = NULL;
    Elastos::Droid::View::View::Inflate(context, SystemUIR::layout::system_bar_compat_mode_panel,
        NULL, (IView**)&tmpView);

    mCompatModePanel = ICompatModePanel::Probe(tmpView.Get());
    touchListener = new TouchOutsideListener(
            MSG_CLOSE_COMPAT_MODE_PANEL, IStatusBarPanel::Probe(mCompatModePanel.Get()), this);
    mCompatModePanel->SetOnTouchListener(touchListener);
    mCompatModePanel->SetTrigger(mCompatModeButton);
    mCompatModePanel->SetVisibility(IView::GONE);
    mStatusBarView->SetIgnoreChildren(3, mCompatModeButton, mCompatModePanel);

    lp = NULL;
    CWindowManagerLayoutParams::New(
        250,
        IViewGroupLayoutParams::WRAP_CONTENT,
        IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL,
        IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM
            | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH
            | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED,
        IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);
    lp->SetGravity(IGravity::BOTTOM | IGravity::RIGHT);
    titleSeq = NULL;
    CString::New(String("CompatModePanel"), (ICharSequence**)&titleSeq);
    lp->SetTitle(titleSeq);
    lp->SetWindowAnimations(R::style::Animation_Dialog);
    mWindowManager->AddView(mCompatModePanel, lp);

    mRecentButton->SetOnTouchListener(mRecentsPreloadOnTouchListener);

    tmpView = NULL;
    mNotificationPanel->FindViewById(SystemUIR::id::content, (IView**)&tmpView);
    mPile = INotificationRowLayout::Probe(tmpView);
    assert(mPile != NULL);
    mPile->RemoveAllViews();
    AutoPtr<IViewOnLongClickListener> lpl = GetNotificationLongClicker();
    mPile->SetLongPressListener(lpl);

    AutoPtr<IViewParent> parent;
    mPile->GetParent((IViewParent**)&parent);
    AutoPtr<IScrollView> scroller = IScrollView::Probe(parent.Get());
    assert(scroller != NULL);
    scroller->SetFillViewport(TRUE);
}

Int32 TabletStatusBar::GetExpandedViewMaxHeight()
{
    return GetNotificationPanelHeight();
}

Int32 TabletStatusBar::GetNotificationPanelHeight()
{
    using Elastos::Core::Math;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IDisplay> d;
    mWindowManager->GetDefaultDisplay((IDisplay**)&d);
    AutoPtr<IPoint> size;
    CPoint::New((IPoint**)&size);
    d->GetRealSize(size);
    Int32 height;
    res->GetDimensionPixelSize(SystemUIR::dimen::notification_panel_min_height, &height);
    Int32 y;
    size->GetY(&y);
    return Math::Max(height, y);
}

ECode TabletStatusBar::Start()
{
    BaseStatusBar::Start(); // will add the main bar view
    // AutoPtr<IInterface> tmpObj;
    // mContext->GetSystemService(IContext::STORAGE_SERVICE, (IInterface**)&tmpObj);
    // mStorageManager = IStorageManager::Probe(tmpObj.Get());
    // TODO
    // mStorageManager->RegisterListener(
    //         new com.android.systemui.usb.StorageNotification(mContext));
    return NOERROR;
}

ECode TabletStatusBar::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    mStatusBarView->SetShowVolume(mShow, mContext);
    LoadDimens();
    mNotificationPanelParams->SetHeight(GetNotificationPanelHeight());
    mWindowManager->UpdateViewLayout(mNotificationPanel, mNotificationPanelParams);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(SystemUIR::integer::config_show_search_delay, &mShowSearchHoldoff);
    UpdateSearchPanel();
    return NOERROR;
}

void TabletStatusBar::LoadDimens()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    res->GetDimensionPixelSize(R::dimen::navigation_bar_height, &mNaturalBarHeight);

    Int32 newIconSize, newIconHPadding, newNavIconWidth, newMenuNavIconWidth;
    res->GetDimensionPixelSize(R::dimen::system_bar_icon_size, &newIconSize);
    res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_icon_padding, &newIconHPadding);
    res->GetDimensionPixelSize(SystemUIR::dimen::navigation_key_width, &newNavIconWidth);
    res->GetDimensionPixelSize(SystemUIR::dimen::navigation_menu_key_width, &newMenuNavIconWidth);

    if (mNavigationArea != NULL && newNavIconWidth != mNavIconWidth) {
        mNavIconWidth = newNavIconWidth;

        AutoPtr<ILinearLayoutLayoutParams> lp;
        CLinearLayoutLayoutParams::New(mNavIconWidth, IViewGroupLayoutParams::MATCH_PARENT,
            (ILinearLayoutLayoutParams**)&lp);
        mBackButton->SetLayoutParams(IViewGroupLayoutParams::Probe(lp.Get()));
        mHomeButton->SetLayoutParams(IViewGroupLayoutParams::Probe(lp.Get()));
        mRecentButton->SetLayoutParams(IViewGroupLayoutParams::Probe(lp.Get()));
    }

    if (mNavigationArea != NULL && newMenuNavIconWidth != mMenuNavIconWidth) {
        mMenuNavIconWidth = newMenuNavIconWidth;

        AutoPtr<ILinearLayoutLayoutParams> lp;
        CLinearLayoutLayoutParams::New(mMenuNavIconWidth, IViewGroupLayoutParams::MATCH_PARENT,
            (ILinearLayoutLayoutParams**)&lp);
        mMenuButton->SetLayoutParams(IViewGroupLayoutParams::Probe(lp.Get()));
    }

    if (newIconHPadding != mIconHPadding || newIconSize != mIconSize) {
//            Slogger::D(TAG, "size=" + newIconSize + " padding=" + newIconHPadding);
        mIconHPadding = newIconHPadding;
        mIconSize = newIconSize;
        ReloadAllNotificationIcons(); // reload the tray
    }

    Int32 numIcons;
    res->GetInteger(SystemUIR::integer::config_maxNotificationIcons, &numIcons);
    if (numIcons != mMaxNotificationIcons) {
        mMaxNotificationIcons = numIcons;
        // if (DEBUG) Slogger::D(TAG, "max notification icons: " + mMaxNotificationIcons);
        ReloadAllNotificationIcons();
    }
}

AutoPtr<IView> TabletStatusBar::GetStatusBarView()
{
    return mStatusBarView;
}

AutoPtr<IView> TabletStatusBar::MakeStatusBarView()
{
    AutoPtr<IContext> context = mContext;

    LoadDimens();

    AutoPtr<ITabletStatusBarView> sb;
    Elastos::Droid::View::View::Inflate(context, SystemUIR::layout::system_bar,
        NULL, (IView**)&sb);
    mStatusBarView = sb;
    assert(sb != NULL);
    sb->SetHandler(mHandler);

    Boolean bval;
    // try {
        // Sanity-check that someone hasn't set up the config wrong and asked for a navigation
        // bar on a tablet that has only the system bar
        mWindowManagerService->HasNavigationBar(&bval);
        if (bval) {
            Slogger::E(TAG, "Tablet device cannot show navigation bar and system bar");
        }
    // } catch (RemoteException ex) {
    // }

    mBarContents = NULL;
    sb->FindViewById(SystemUIR::id::bar_contents, (IView**)&mBarContents);
    assert(mBarContents != NULL);

    // the whole right-hand side of the bar
    mNotificationArea = NULL;
    sb->FindViewById(SystemUIR::id::notificationArea, (IView**)&mNotificationArea);
    assert(mNotificationArea != NULL);
    AutoPtr<IViewOnTouchListener> touchListener = new NotificationTriggerTouchListener(this);
    mNotificationArea->SetOnTouchListener(touchListener);

    // the button to open the notification area
    mNotificationTrigger = NULL;
    sb->FindViewById(SystemUIR::id::notificationTrigger, (IView**)&mNotificationTrigger);
    assert(mNotificationTrigger != NULL);

    // the more notifications icon
    mNotificationIconArea = NULL;
    sb->FindViewById(SystemUIR::id::notificationIcons, (IView**)&mNotificationIconArea);
    assert(mNotificationIconArea != NULL);

    // where the icons go
    mIconLayout = NULL;
    sb->FindViewById(SystemUIR::id::icons, (IView**)&mIconLayout);
    assert(mIconLayout != NULL);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    helper->GetTapTimeout(&mNotificationPeekTapDuration);
    mNotificationFlingVelocity = 300; // px/s

    mTicker = new TabletTicker(this);

    // The icons
    // mLocationController = new LocAddPanelWindows()ationController(mContext); // will post a notification

    // watch the PREF_DO_NOT_DISTURB and convert to appropriate disable() calls
    mDoNotDisturb = new DoNotDisturb(mContext);

    //     mBatteryController = new BatteryController(mContext);
    //     mBatteryController.addIconView((ImageView)sb->FindViewById(SystemUIR::id::battery));
    //     mBatteryController.addLabelView((TextView)sb.findViewById(R.id.battery_text));
    // mBluetoothController = new BluetoothController(mContext);
    // mBluetoothController.addIconView((ImageView)sb->FindViewById(SystemUIR::id::bluetooth));

    mNetworkController = new NetworkController(mContext);
    AutoPtr<ISignalClusterView> signalCluster;
    sb->FindViewById(SystemUIR::id::signal_cluster, (IView**)&signalCluster);
    assert(signalCluster != NULL);
    AutoPtr<INetworkControllerSignalCluster> cluster = INetworkControllerSignalCluster::Probe(signalCluster);
    mNetworkController->AddSignalCluster(cluster);

    // The navigation buttons
    mBackButton = NULL;
    sb->FindViewById(SystemUIR::id::back, (IView**)&mBackButton);
    assert(mBackButton != NULL);

    mNavigationArea = NULL;
    sb->FindViewById(SystemUIR::id::navigationArea, (IView**)&mNavigationArea);
    assert(mNavigationArea != NULL);

    mHomeButton = NULL;
    mNavigationArea->FindViewById(SystemUIR::id::home, (IView**)&mHomeButton);
    assert(mHomeButton != NULL);

    mMenuButton = NULL;
    mNavigationArea->FindViewById(SystemUIR::id::menu, (IView**)&mMenuButton);
    assert(mMenuButton != NULL);

    mRecentButton = NULL;
    mNavigationArea->FindViewById(SystemUIR::id::recent_apps, (IView**)&mRecentButton);
    mRecentButton->SetOnClickListener(mOnClickListener);
    assert(mRecentButton != NULL);

    mScreenShotButton = NULL;
    mNavigationArea->FindViewById(SystemUIR::id::screenshot, (IView**)&mScreenShotButton);
    mScreenShotButton->SetOnClickListener(mOnClickListener);
    assert(mScreenShotButton != NULL);

    mVolumeDownButton = NULL;
    mNavigationArea->FindViewById(SystemUIR::id::volume_down, (IView**)&mVolumeDownButton);
    assert(mVolumeDownButton != NULL);

    mVolumeUpButton = NULL;
    mNavigationArea->FindViewById(SystemUIR::id::volume_up, (IView**)&mVolumeUpButton);
    assert(mVolumeUpButton != NULL);

    AutoPtr<ILayoutTransition> lt;
    CLayoutTransition::New((ILayoutTransition**)&lt);
    lt->SetDuration(250);
    // don't wait for these transitions; we just want icons to fade in/out, not move around
    lt->SetDuration(ILayoutTransition::CHANGE_APPEARING, 0);
    lt->SetDuration(ILayoutTransition::CHANGE_DISAPPEARING, 0);
    AutoPtr<ITransitionListener> l = new MyTransitionListener(mBarContents);
    lt->AddTransitionListener(l);

    mNavigationArea->SetLayoutTransition(lt);
    // no multi-touch on the nav buttons
    mNavigationArea->SetMotionEventSplittingEnabled(FALSE);

    // The bar contents buttons
    mFeedbackIconArea = NULL;
    sb->FindViewById(SystemUIR::id::feedbackIconArea, (IView**)&mFeedbackIconArea);
    assert(mFeedbackIconArea != NULL);

    mInputMethodSwitchButton = NULL;
    sb->FindViewById(SystemUIR::id::imeSwitchButton, (IView**)&mInputMethodSwitchButton);
    assert(mInputMethodSwitchButton != NULL);
    // Overwrite the listener
    mInputMethodSwitchButton->SetOnClickListener(mOnClickListener);

    mCompatModeButton = NULL;
    sb->FindViewById(SystemUIR::id::compatModeButton, (IView**)&mCompatModeButton);
    assert(mCompatModeButton != NULL);
    mCompatModeButton->SetOnClickListener(mOnClickListener);
    mCompatModeButton->SetVisibility(IView::GONE);

    // for redirecting errant bar taps to the IME
    mFakeSpaceBar = NULL;
    sb->FindViewById(SystemUIR::id::fake_space_bar, (IView**)&mFakeSpaceBar);
    assert(mFakeSpaceBar != NULL);

    // "shadows" of the status bar features, for lights-out mode
    mShadow = NULL;
    sb->FindViewById(SystemUIR::id::bar_shadow, (IView**)&mShadow);
    assert(mShadow != NULL);
    touchListener = new ShadowTouchListener(this);
    mShadow->SetOnTouchListener(touchListener);

    // tuning parameters
    Int32 LIGHTS_GOING_OUT_SYSBAR_DURATION = 750;
    Int32 LIGHTS_GOING_OUT_SHADOW_DURATION = 750;
    Int32 LIGHTS_GOING_OUT_SHADOW_DELAY    = 0;

    Int32 LIGHTS_COMING_UP_SYSBAR_DURATION = 200;
//        Int32 LIGHTS_COMING_UP_SYSBAR_DELAY    = 50;
    Int32 LIGHTS_COMING_UP_SHADOW_DURATION = 0;

    AutoPtr<ILayoutTransition> xition;
    CLayoutTransition::New((ILayoutTransition**)&xition);
    AutoPtr<ArrayOf<Float> > param1 = ArrayOf<Float>::Alloc(2);
    (*param1)[0] = 0.5f; (*param1)[1] = 1.f;
    AutoPtr<IObjectAnimator> anim1 = CObjectAnimator::OfFloat(NULL, String("alpha"), param1);
    xition->SetAnimator(ILayoutTransition::APPEARING, anim1);
    xition->SetDuration(ILayoutTransition::APPEARING, LIGHTS_COMING_UP_SYSBAR_DURATION);
    xition->SetStartDelay(ILayoutTransition::APPEARING, 0);
    AutoPtr<ArrayOf<Float> > param2 = ArrayOf<Float>::Alloc(2);
    (*param2)[0] = 1.f; (*param2)[1] = 0.f;
    AutoPtr<IObjectAnimator> anim2 = CObjectAnimator::OfFloat(NULL, String("alpha"), param2);
    xition->SetAnimator(ILayoutTransition::DISAPPEARING, anim2);
    xition->SetDuration(ILayoutTransition::DISAPPEARING, LIGHTS_GOING_OUT_SYSBAR_DURATION);
    xition->SetStartDelay(ILayoutTransition::DISAPPEARING, 0);
    AutoPtr<IViewGroup> group;
    sb->FindViewById(SystemUIR::id::bar_contents_holder, (IView**)&group);
    group->SetLayoutTransition(xition);

    xition = NULL;
    CLayoutTransition::New((ILayoutTransition**)&xition);
        AutoPtr<ArrayOf<Float> > param3 = ArrayOf<Float>::Alloc(2);
    (*param3)[0] = 0.f; (*param3)[1] = 1.f;
    AutoPtr<IObjectAnimator> anim3 = CObjectAnimator::OfFloat(NULL, String("alpha"), param3);
    xition->SetAnimator(ILayoutTransition::APPEARING, anim3);
    xition->SetDuration(ILayoutTransition::APPEARING, LIGHTS_GOING_OUT_SHADOW_DURATION);
    xition->SetStartDelay(ILayoutTransition::APPEARING, LIGHTS_GOING_OUT_SHADOW_DELAY);
    AutoPtr<ArrayOf<Float> > param4 = ArrayOf<Float>::Alloc(2);
    (*param4)[0] = 1.f; (*param4)[1] = 0.f;
    AutoPtr<IObjectAnimator> anim4 = CObjectAnimator::OfFloat(NULL, String("alpha"), param4);
    xition->SetAnimator(ILayoutTransition::DISAPPEARING, anim4);
    xition->SetDuration(ILayoutTransition::DISAPPEARING, LIGHTS_COMING_UP_SHADOW_DURATION);
    xition->SetStartDelay(ILayoutTransition::DISAPPEARING, 0);
    AutoPtr<IViewGroup> group1;
    sb->FindViewById(SystemUIR::id::bar_shadow_holder, (IView**)&group1);
    group1->SetLayoutTransition(xition);

    // set the initial view visibility
    SetAreThereNotifications();

    // receive broadcasts
    AutoPtr<IIntentFilter> filter;
    AutoPtr<IIntent> stickyIntent;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    context->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&stickyIntent);

    AutoPtr<IView> returnView = IView::Probe(sb.Get());
    return sb;
}

AutoPtr<IWindowManagerLayoutParams> TabletStatusBar::GetRecentsLayoutParams(
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Float width;
    res->GetDimension(SystemUIR::dimen::status_bar_recents_width, &width);
    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
            (Int32)width,
            IViewGroupLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL,
            IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
            | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM
            | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH
            | IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED,
            IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);
    lp->SetGravity(IGravity::BOTTOM | IGravity::LEFT);
    AutoPtr<ICharSequence> titleSeq;
    CString::New(String("RecentsPanel"), (ICharSequence**)&titleSeq);
    lp->SetTitle(titleSeq);
    lp->SetWindowAnimations(R::style::Animation_RecentApplications);
    lp->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNCHANGED
        | IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_NOTHING);

    return lp;
}

AutoPtr<IWindowManagerLayoutParams> TabletStatusBar::GetSearchLayoutParams(
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
    Int32 flags;
    lp->GetFlags(&flags);
    if (CActivityManager::IsHighEndGfx()) {
        flags |= IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED;
    } else {
        flags |= IWindowManagerLayoutParams::FLAG_DIM_BEHIND;
        lp->SetDimAmount(0.7f);
    }
    lp->SetFlags(flags);
    lp->SetGravity(IGravity::BOTTOM | IGravity::LEFT);
    AutoPtr<ICharSequence> titleSeq;
    CString::New(String("SearchPanel"), (ICharSequence**)&titleSeq);
    lp->SetTitle(titleSeq);
    lp->SetWindowAnimations(R::style::Animation_RecentApplications);
    lp->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNCHANGED
            | IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_NOTHING);
    return lp;
}

ECode TabletStatusBar::UpdateSearchPanel()
{
    Slogger::W(TAG, "TODO: TabletStatusBar::UpdateSearchPanel is not Implement!");
    //TODO
    // BaseStatusBar::UpdateSearchPanel();
    // mSearchPanelView->SetStatusBarView(IView::Probe(mStatusBarView.Get()));
    // mStatusBarView->SetDelegateView(mSearchPanelView);
    return NOERROR;
}

ECode TabletStatusBar::ShowSearchPanel()
{
    Slogger::W(TAG, "TODO: TabletStatusBar::ShowSearchPanel is not Implement!");
    // BaseStatusBar::ShowSearchPanel();
    // AutoPtr<IViewGroupLayoutParams> vlp;
    // mStatusBarView->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
    // AutoPtr<IWindowManagerLayoutParams> lp = IWindowManagerLayoutParams::Probe(vlp.Get());
    // Int32 flags;
    // lp->GetFlags(&flags);
    // flags &= ~IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL;
    // lp->SetFlags(flags);
    // mWindowManager->UpdateViewLayout(mStatusBarView, lp);
    return NOERROR;
}

ECode TabletStatusBar::HideSearchPanel()
{
    Slogger::W(TAG, "TODO: TabletStatusBar::HideSearchPanel is not Implement!");
    // BaseStatusBar::HideSearchPanel();
    // AutoPtr<IViewGroupLayoutParams> vlp;
    // mStatusBarView->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
    // AutoPtr<IWindowManagerLayoutParams> lp = IWindowManagerLayoutParams::Probe(vlp.Get());
    // Int32 flags;
    // lp->GetFlags(&flags);
    // flags |= IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL;
    // lp->SetFlags(flags);
    // mWindowManager->UpdateViewLayout(mStatusBarView, lp);
    return NOERROR;
}

Int32 TabletStatusBar::GetStatusBarHeight()
{
    Int32 height = 0;
    if (mStatusBarView != NULL) {
        mStatusBarView->GetHeight(&height);
    }
    else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetDimensionPixelSize(R::dimen::navigation_bar_height, &height);
    }
    return height;
}

Int32 TabletStatusBar::GetStatusBarGravity()
{
    return IGravity::BOTTOM | IGravity::FILL_HORIZONTAL;
}

ECode TabletStatusBar::OnBarHeightChanged(
    /* [in] */ Int32 height)
{
    AutoPtr<IViewGroupLayoutParams> vlp;
    mStatusBarView->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
    AutoPtr<IWindowManagerLayoutParams> lp = IWindowManagerLayoutParams::Probe(vlp.Get());
    if (lp == NULL) {
        // haven't been added yet
        return NOERROR;
    }

    Int32 lpHeight;
    lp->GetHeight(&lpHeight);
    if (lpHeight != height) {
        lp->SetHeight(height);
        mWindowManager->UpdateViewLayout(mStatusBarView, lp);
    }
    return NOERROR;
}

void TabletStatusBar::OnStatusBarViewInterceptTouchEvent()
{
    // From TabletStatusBarView::OnInterceptTouchEvent
    //
    RemoveAndSendMessage(MSG_CLOSE_NOTIFICATION_PANEL);
    RemoveAndSendMessage(MSG_CLOSE_INPUT_METHODS_PANEL);
    RemoveAndSendMessage(MSG_STOP_TICKER);
}

ECode TabletStatusBar::RemoveAndSendMessage(
    /* [in] */ Int32 msg)
{
    mHandler->RemoveMessages(msg);
    Boolean result;
    return mHandler->SendEmptyMessage(msg, &result);
}

ECode TabletStatusBar::AddIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* icon)
{
    // if (DEBUG) Slogger::D(TAG, "addIcon(" + slot + ") -> " + icon);
    return NOERROR;
}

ECode TabletStatusBar::UpdateIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* old,
    /* [in] */ IStatusBarIcon* icon)
{
    // if (DEBUG) Slogger::D(TAG, "updateIcon(" + slot + ") -> " + icon);
    return NOERROR;
}

ECode TabletStatusBar::RemoveIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex)
{
    // if (DEBUG) Slogger::D(TAG, "removeIcon(" + slot + ")");
    return NOERROR;
}

ECode TabletStatusBar::AddNotification(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    assert(notification != NULL);
    if (DEBUG) {
        String sbnStr;
        notification->ToString(&sbnStr);
        Slogger::D(TAG, "AddNotification: key: %p -> notification: [%s]", key, sbnStr.string());
    }

    AddNotificationViews(key, notification);

    Boolean immersive = IsImmersive();
    if (FALSE && immersive) {
        // TODO: immersive mode popups for tablet
    }
    else {
        AutoPtr<INotification> n;
        notification->GetNotification((INotification**)&n);
        AutoPtr<IPendingIntent> pi;
        n->GetFullScreenIntent((IPendingIntent**)&pi);
        if (pi != NULL) {
            // not immersive & a full-screen alert should be shown
            Slogger::W(TAG, "Notification has fullScreenIntent and activity is not immersive; sending fullScreenIntent");
            // try {
                pi->Send();
            // } catch (PendingIntent.CanceledException e) {
            // }
        }
        else {
            Tick(key, notification, TRUE);
        }
    }

    SetAreThereNotifications();
    return NOERROR;
}

ECode TabletStatusBar::RemoveNotification(
    /* [in] */ IBinder* key)
{
    VALIDATE_NOT_NULL(key);

    if (DEBUG) {
        String info;
        key->ToString(&info);
        Slogger::D(TAG, "TabletStatusBar::RemoveNotification, %s", info.string());
    }

    RemoveNotificationViews(key);
    mTicker->Remove(key);
    SetAreThereNotifications();
    return NOERROR;
}

ECode TabletStatusBar::ShowClock(
    /* [in] */ Boolean show)
{
    AutoPtr<IView> clock, network_text;
    mBarContents->FindViewById(SystemUIR::id::clock, (IView**)&clock);
    mBarContents->FindViewById(SystemUIR::id::network_text, (IView**)&network_text);
    if (clock != NULL) {
        clock->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    }
    if (network_text != NULL) {
        network_text->SetVisibility((!show) ? IView::VISIBLE : IView::GONE);
    }
    return NOERROR;
}

ECode TabletStatusBar::Disable(
    /* [in] */ Int32 state)
{
    Int32 old = mDisabled;
    Int32 diff = state ^ old;
    mDisabled = state;

    // act accordingly
    if ((diff & IStatusBarManager::DISABLE_CLOCK) != 0) {
        Boolean show = (state & IStatusBarManager::DISABLE_CLOCK) == 0;
        mShow = show & mHasVolume;
        mStatusBarView->SetShowVolume(mShow, mContext);
        Slogger::I(TAG, "DISABLE_CLOCK: %s", (show ? "no" : "yes"));
        ShowClock(show);
    }
    if ((diff & IStatusBarManager::DISABLE_SYSTEM_INFO) != 0) {
        Boolean show = (state & IStatusBarManager::DISABLE_SYSTEM_INFO) == 0;
        Slogger::I(TAG, "DISABLE_SYSTEM_INFO: %s", (show ? "no" : "yes"));
        mNotificationTrigger->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    }
    if ((diff & IStatusBarManager::DISABLE_EXPAND) != 0) {
        if ((state & IStatusBarManager::DISABLE_EXPAND) != 0) {
            Slogger::I(TAG, "DISABLE_EXPAND: yes");
            AnimateCollapsePanels();
            VisibilityChanged(FALSE);
        }
    }
    if ((diff & IStatusBarManager::DISABLE_NOTIFICATION_ICONS) != 0) {
        AutoPtr<ISharedPreferences> prefs = Prefs::Read(mContext);
        prefs->GetBoolean(Prefs::DO_NOT_DISTURB_PREF, Prefs::DO_NOT_DISTURB_DEFAULT,
            &mNotificationDNDMode);

        if ((state & IStatusBarManager::DISABLE_NOTIFICATION_ICONS) != 0) {
            Slogger::I(TAG, "DISABLE_NOTIFICATION_ICONS: yes%s", (mNotificationDNDMode?" (DND)":""));
            mTicker->Halt();
        } else {
            Slogger::I(TAG, "DISABLE_NOTIFICATION_ICONS: no%s", (mNotificationDNDMode?" (DND)":""));
        }

        // refresh icons to show either notifications or the DND message
        ReloadAllNotificationIcons();
    }
    else if ((diff & IStatusBarManager::DISABLE_NOTIFICATION_TICKER) != 0) {
        if ((state & IStatusBarManager::DISABLE_NOTIFICATION_TICKER) != 0) {
            mTicker->Halt();
        }
    }
    if ((diff & (IStatusBarManager::DISABLE_RECENT
                    | IStatusBarManager::DISABLE_BACK
                    | IStatusBarManager::DISABLE_HOME)) != 0) {
        SetNavigationVisibility(state);

        if ((state & IStatusBarManager::DISABLE_RECENT) != 0) {
            // close recents if it's visible
            RemoveAndSendMessage(MSG_CLOSE_RECENTS_PANEL);
        }
    }
    return NOERROR;
}

void TabletStatusBar::SetNavigationVisibility(
    /* [in] */ Int32 visibility)
{
    Boolean disableHome = ((visibility & IStatusBarManager::DISABLE_HOME) != 0);
    Boolean disableRecent = ((visibility & IStatusBarManager::DISABLE_RECENT) != 0);
    Boolean disableBack = ((visibility & IStatusBarManager::DISABLE_BACK) != 0);

    mBackButton->SetVisibility(disableBack ? IView::INVISIBLE : IView::VISIBLE);
    mHomeButton->SetVisibility(disableHome ? IView::INVISIBLE : IView::VISIBLE);
    mRecentButton->SetVisibility(disableRecent ? IView::INVISIBLE : IView::VISIBLE);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    res->GetBoolean(SystemUIR::bool_::hasVolumeButton, &mHasVolume);

    mInputMethodSwitchButton->SetScreenLocked(
            (visibility & IStatusBarManager::DISABLE_SYSTEM_INFO) != 0);
}

Boolean TabletStatusBar::HasTicker(
    /* [in] */ INotification* n)
{
    AutoPtr<IRemoteViews> tickerView;
    n->GetTickerView((IRemoteViews**)&tickerView);
    AutoPtr<ICharSequence> tickerText;
    n->GetTickerText((ICharSequence**)&tickerText);
    return tickerView != NULL || !TextUtils::IsEmpty(tickerText);
}

ECode TabletStatusBar::Tick(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* n,
    /* [in] */ Boolean firstTime)
{
    Boolean isShowing;
    mNotificationPanel->IsShowing(&isShowing);
    // Don't show the ticker when the windowshade is open.
    if (isShowing) {
        return NOERROR;
    }

    AutoPtr<INotification> notification;
    n->GetNotification((INotification**)&notification);
    Int32 flags;
    notification->GetFlags(&flags);
    // If they asked for FLAG_ONLY_ALERT_ONCE, then only show this notification
    // if it's a new notification.
    if (!firstTime && (flags & INotification::FLAG_ONLY_ALERT_ONCE) != 0) {
        return NOERROR;
    }

    // Show the ticker if one is requested. Also don't do this
    // until status bar window is attached to the window manager,
    // because...  well, what's the point otherwise?  And trying to
    // run a ticker without being attached will crash!
    AutoPtr<IBinder> token;
    mStatusBarView->GetWindowToken((IBinder**)&token);
    if (HasTicker(notification) && token != NULL) {
        if (0 == (mDisabled & (IStatusBarManager::DISABLE_NOTIFICATION_ICONS
                | IStatusBarManager::DISABLE_NOTIFICATION_TICKER))) {
            mTicker->Add(key, n);
            mFeedbackIconArea->SetVisibility(IView::GONE);
        }
    }
    return NOERROR;
}

// called by TabletTicker when it's done with all queued ticks
ECode TabletStatusBar::DoneTicking()
{
    if (mFeedbackIconArea) {
        mFeedbackIconArea->SetVisibility(IView::VISIBLE);
    }
    return NOERROR;
}

ECode TabletStatusBar::AnimateExpandNotificationsPanel()
{
    RemoveAndSendMessage(MSG_OPEN_NOTIFICATION_PANEL);
    return NOERROR;
}

ECode TabletStatusBar::AnimateCollapsePanels()
{
    AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
    return NOERROR;
}

ECode TabletStatusBar::AnimateCollapsePanels(
    /* [in] */ Int32 flags)
{
    if ((flags & ICommandQueue::FLAG_EXCLUDE_NOTIFICATION_PANEL) == 0) {
        RemoveAndSendMessage(MSG_CLOSE_NOTIFICATION_PANEL);
    }
    if ((flags & ICommandQueue::FLAG_EXCLUDE_RECENTS_PANEL) == 0) {
        RemoveAndSendMessage(MSG_CLOSE_RECENTS_PANEL);
    }
    if ((flags & ICommandQueue::FLAG_EXCLUDE_SEARCH_PANEL) == 0) {
        RemoveAndSendMessage(MSG_CLOSE_SEARCH_PANEL);
    }
    if ((flags & ICommandQueue::FLAG_EXCLUDE_INPUT_METHODS_PANEL) == 0) {
        RemoveAndSendMessage(MSG_CLOSE_INPUT_METHODS_PANEL);
    }
    if ((flags & ICommandQueue::FLAG_EXCLUDE_COMPAT_MODE_PANEL) == 0) {
        RemoveAndSendMessage(MSG_CLOSE_COMPAT_MODE_PANEL);
    }
    return NOERROR;
}

ECode TabletStatusBar::AnimateExpandSettingsPanel()
{
    // TODO: Implement when TabletStatusBar begins to be used.
    return NOERROR;
}

ECode TabletStatusBar::SetNavigationIconHints(
    /* [in] */ Int32 hints)
{
    if (hints == mNavigationIconHints) return NOERROR;

 //    if (DEBUG) {
 //        android.widget.Toast.makeText(mContext,
 //            "Navigation icon hints = " + hints,
 //            500).show();
 //    }

    mNavigationIconHints = hints;

    mBackButton->SetAlpha(
        (0 != (hints & IStatusBarManager::NAVIGATION_HINT_BACK_NOP)) ? 0.5f : 1.0f);
    mHomeButton->SetAlpha(
        (0 != (hints & IStatusBarManager::NAVIGATION_HINT_HOME_NOP)) ? 0.5f : 1.0f);
    mRecentButton->SetAlpha(
        (0 != (hints & IStatusBarManager::NAVIGATION_HINT_RECENT_NOP)) ? 0.5f : 1.0f);

    mBackButton->SetImageResource(
        (0 != (hints & IStatusBarManager::NAVIGATION_HINT_BACK_ALT))
            ? SystemUIR::drawable::ic_sysbar_back_ime
            : SystemUIR::drawable::ic_sysbar_back);
    return NOERROR;
}

void TabletStatusBar::NotifyUiVisibilityChanged()
{
    // try {
        mWindowManagerService->StatusBarVisibilityChanged(mSystemUiVisibility);
    // } catch (RemoteException ex) {
    // }
}

ECode TabletStatusBar::SetSystemUiVisibility(Int32 vis, Int32 mask)
{
    Int32 oldVal = mSystemUiVisibility;
    Int32 newVal = (oldVal&~mask) | (vis&mask);
    Int32 diff = newVal ^ oldVal;

    if (diff != 0) {
        mSystemUiVisibility = newVal;

        if (0 != (diff & IView::SYSTEM_UI_FLAG_LOW_PROFILE)) {
            Boolean result;
            mHandler->RemoveMessages(MSG_HIDE_CHROME);
            mHandler->RemoveMessages(MSG_SHOW_CHROME);
            mHandler->SendEmptyMessage(0 == (vis & IView::SYSTEM_UI_FLAG_LOW_PROFILE)
                ? MSG_SHOW_CHROME : MSG_HIDE_CHROME, &result);
        }

        NotifyUiVisibilityChanged();
    }
    return NOERROR;
}


ECode TabletStatusBar::SetLightsOn(
    /* [in] */ Boolean on)
{
    // Policy note: if the frontmost activity needs the menu key, we assume it is a legacy app
    // that can't handle lights-out mode.
    Int32 visibility;
    mMenuButton->GetVisibility(&visibility);
    if (visibility == IView::VISIBLE) {
        on = TRUE;
    }

    // Slog.v(TAG, "setLightsOn(" + on + ")");
    if (on) {
        SetSystemUiVisibility(0, IView::SYSTEM_UI_FLAG_LOW_PROFILE);
    }
    else {
        SetSystemUiVisibility(IView::SYSTEM_UI_FLAG_LOW_PROFILE,
            IView::SYSTEM_UI_FLAG_LOW_PROFILE);
    }
    return NOERROR;
}


ECode TabletStatusBar::TopAppWindowChanged(
    /* [in] */ Boolean showMenu)
{
    if (DEBUG) {
        Slogger::D(TAG, "%s the MENU button", (showMenu? "showing" : "hiding"));
    }
    // mMenuButton->SetVisibility(showMenu ? IView::VISIBLE : IView::GONE);

    // See above re: lights-out policy for legacy apps.
    if (showMenu) SetLightsOn(TRUE);

    mCompatModeButton->Refresh();
    Int32 visibility;
    mCompatModeButton->GetVisibility(&visibility);
    if (visibility == IView::VISIBLE) {
        Boolean bval;
        AutoPtr<ISharedPreferences> prefs = Prefs::Read(mContext);
        if (prefs)
            prefs->GetBoolean(Prefs::SHOWN_COMPAT_MODE_HELP, FALSE, &bval);

        if (DEBUG_COMPAT_HELP || !bval) {
            ShowCompatibilityHelp();
        }
    }
    else {
        HideCompatibilityHelp();
        mCompatModePanel->ClosePanel();
    }
    return NOERROR;
}

void TabletStatusBar::ShowCompatibilityHelp()
{
    if (mCompatibilityHelpDialog != NULL) {
        return;
    }

    mCompatibilityHelpDialog = NULL;
    Elastos::Droid::View::View::Inflate(mContext, SystemUIR::layout::compat_mode_help,
        NULL, (IView**)&mCompatibilityHelpDialog);
    AutoPtr<IView> button;
    mCompatibilityHelpDialog->FindViewById(SystemUIR::id::button, (IView**)&button);

    AutoPtr<IViewOnClickListener> listener = new CompatibilityHelpDialogClickListener(this);
    button->SetOnClickListener(listener);

    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG,
            IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
                | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS
                | IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM,
            IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);
    AutoPtr<ICharSequence> titleSeq;
    CString::New(String("CompatibilityModeDialog"), (ICharSequence**)&titleSeq);
    lp->SetTitle(titleSeq);
    lp->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_STATE_UNCHANGED
            | IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_NOTHING);
    lp->SetWindowAnimations(R::style::Animation_ZoomButtons); // simple fade

    mWindowManager->AddView(mCompatibilityHelpDialog, lp);
}

void TabletStatusBar::HideCompatibilityHelp()
{
    if (mCompatibilityHelpDialog != NULL) {
        mWindowManager->RemoveView(mCompatibilityHelpDialog);
        mCompatibilityHelpDialog = NULL;
    }
}

ECode TabletStatusBar::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition)
{
    if (!mInputMethodSwitchButton) return NOERROR;

    mInputMethodSwitchButton->SetImeWindowStatus(token,
            (vis & IInputMethodService::IME_ACTIVE) != 0);
    UpdateNotificationIcons();
    if (mInputMethodsPanel) mInputMethodsPanel->SetImeToken(token);

    Boolean altBack = (backDisposition == IInputMethodService::BACK_DISPOSITION_WILL_DISMISS)
        || ((vis & IInputMethodService::IME_VISIBLE) != 0);
    mAltBackButtonEnabledForIme = altBack;

    mCommandQueue->SetNavigationIconHints(
            altBack ? (mNavigationIconHints | IStatusBarManager::NAVIGATION_HINT_BACK_ALT)
                    : (mNavigationIconHints & ~IStatusBarManager::NAVIGATION_HINT_BACK_ALT));

    if (FAKE_SPACE_BAR) {
        mFakeSpaceBar->SetVisibility(((vis & IInputMethodService::IME_VISIBLE) != 0)
                ? IView::VISIBLE : IView::GONE);
    }
    return NOERROR;
}

ECode TabletStatusBar::SetHardKeyboardStatus(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    if (!mInputMethodSwitchButton) return NOERROR;

    // if (DEBUG) {
    //     Slogger::D(TAG, "Set hard keyboard status: available=" + available
    //             + ", enabled=" + enabled);
    // }
    mInputMethodSwitchButton->SetHardKeyboardStatus(available);
    UpdateNotificationIcons();
    if (mInputMethodsPanel) mInputMethodsPanel->SetHardKeyboardStatus(available, enabled);
    return NOERROR;
}


ECode TabletStatusBar::OnHardKeyboardEnabledChange(
    /* [in] */ Boolean enabled)
{
    // try {
    if (mBarService != NULL)
        mBarService->SetHardKeyboardEnabled(enabled);
    // } catch (RemoteException ex) {
    // }
    return NOERROR;
}

Boolean TabletStatusBar::IsImmersive()
{
    // try {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        Boolean result;
        ECode ec = am->IsTopActivityImmersive(&result);
        if (FAILED(ec))
            result = FALSE;
        return result;
        //Slogger::D(TAG, "Top activity is " + (immersive?"immersive":"not immersive"));
    // } catch (RemoteException ex) {
    //     // the end is nigh
    //     return FALSE;
    // }
}

ECode TabletStatusBar::SetAreThereNotifications()
{
    if (mNotificationPanel != NULL) {
        mNotificationPanel->SetClearable(
            IsDeviceProvisioned() && mNotificationData->HasClearableItems());
    }
    return NOERROR;
}

// Assume this is called from the Handler thread.
void TabletStatusBar::TakeScreenshot()
{
    // {
    //     AutoLock lock(mScreenshotLock);
    //     if (mScreenshotConnection != NULL) {
    //         return;
    //     }

    //     AutoPtr<IComponentName> cn;
    //     CComponentName::New(
    //         String("Elastos.Droid.Core.eco"),
    //         String("elastos.droid.systemui.screenshot.TakeScreenshotService"));
    //     Autolock<IIntent> intent;
    //     CIntent::New((IIntent**)&intent);
    //     intent->SetComponent(cn);
    //     Autoptr<IServiceConnection> conn = new TakeScreenshotServiceConnection(this);
    //     if (mContext.bindService(intent, conn, Context.BIND_AUTO_CREATE)) {
    //         mScreenshotConnection = conn;
    //         mHandler.postDelayed(mScreenshotTimeout, 10000);
    //     }
    // }
}

ECode TabletStatusBar::OnClickRecentButton()
{
    if (DEBUG) Slogger::D(TAG, "clicked recent apps; disabled=%d", mDisabled);
    if ((mDisabled & IStatusBarManager::DISABLE_EXPAND) == 0) {
        ToggleRecentApps();
    }
    return NOERROR;
}

ECode TabletStatusBar::OnClickInputMethodSwitchButton()
{
    if (DEBUG) Slogger::D(TAG, "clicked input methods panel; disabled=%d", mDisabled);
    Int32 visibility;
    mInputMethodsPanel->GetVisibility(&visibility);
    Int32 msg = (visibility == IView::GONE) ?
            MSG_OPEN_INPUT_METHODS_PANEL : MSG_CLOSE_INPUT_METHODS_PANEL;
    RemoveAndSendMessage(msg);
    return NOERROR;
}

ECode TabletStatusBar::OnClickCompatModeButton()
{
    Int32 visibility;
    mCompatModePanel->GetVisibility(&visibility);
    Int32 msg = (visibility == IView::GONE) ?
        MSG_OPEN_COMPAT_MODE_PANEL : MSG_CLOSE_COMPAT_MODE_PANEL;
    RemoveAndSendMessage(msg);
    return NOERROR;
}

ECode TabletStatusBar::ResetNotificationPeekFadeTimer()
{
    if (DEBUG) {
        Slogger::D(TAG, "setting peek fade timer for %d ms from now", NOTIFICATION_PEEK_FADE_DELAY);
    }

    Boolean result;
    mHandler->RemoveMessages(MSG_CLOSE_NOTIFICATION_PEEK);
    return mHandler->SendEmptyMessageDelayed(MSG_CLOSE_NOTIFICATION_PEEK,
        NOTIFICATION_PEEK_FADE_DELAY, &result);
}

void TabletStatusBar::ReloadAllNotificationIcons()
{
    if (mIconLayout == NULL) return;
    mIconLayout->RemoveAllViews();
    UpdateNotificationIcons();
}

ECode TabletStatusBar::UpdateNotificationIcons()
{
    // XXX: need to implement a new limited linear layout class
    // to avoid removing & readding everything

    if (mIconLayout == NULL) return NOERROR;

    // first, populate the main notification panel
    LoadNotificationPanel();

    AutoPtr<ILinearLayoutLayoutParams> params;
    CLinearLayoutLayoutParams::New(
        mIconSize + 2 * mIconHPadding, mNaturalBarHeight,
        (ILinearLayoutLayoutParams**)&params);

    // alternate behavior in DND mode
    if (mNotificationDNDMode) {
        Int32 childCount;
        mIconLayout->GetChildCount(&childCount);
        if (childCount == 0) {
            AutoPtr<INotification> dndNotification;
            AutoPtr<INotificationBuilder> builder;
            CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
            AutoPtr<ICharSequence> title, text;
            mContext->GetText(SystemUIR::string::notifications_off_title, (ICharSequence**)&title);
            mContext->GetText(SystemUIR::string::notifications_off_text, (ICharSequence**)&text);
            builder->SetContentTitle(title);
            builder->SetContentText(text);
            builder->SetSmallIcon(SystemUIR::drawable::ic_notification_dnd);
            builder->SetOngoing(TRUE);
            builder->GetNotification((INotification**)&dndNotification);

            AutoPtr<IStatusBarIconView> iconView;
            CStatusBarIconView::New(mContext, String("_dnd"), dndNotification,
                (IStatusBarIconView**)&iconView);
            iconView->SetImageResource(SystemUIR::drawable::ic_notification_dnd);
            iconView->SetScaleType(ImageViewScaleType_CENTER_INSIDE);
            iconView->SetPadding(mIconHPadding, 0, mIconHPadding, 0);

            AutoPtr<IUserHandle> user;
            Process::MyUserHandle((IUserHandle**)&user);
            AutoPtr<IStatusBarNotification> notification;
            CStatusBarNotification::New(
                String(""), 0, String(""), 0, 0,
                INotification::PRIORITY_MAX,
                dndNotification, user, (IStatusBarNotification**)&notification);

            mNotificationDNDDummyEntry = new NotificationDataEntry(NULL, notification, iconView);
            mIconLayout->AddView(iconView, params);
        }

        return NOERROR;
    }
    else if (0 != (mDisabled & IStatusBarManager::DISABLE_NOTIFICATION_ICONS)) {
        // if icons are disabled but we're not in DND mode, this is probably Setup and we should
        // just leave the area totally empty
        return NOERROR;
    }

    Int32 N = mNotificationData->Size();
    if (DEBUG) {
        Slogger::D(TAG, "refreshing icons: %d notifications", N);
    }

    List<AutoPtr<IView> > toShow;

    // Extra Special Icons
    // The IME switcher and compatibility mode icons take the place of notifications. You didn't
    // need to see all those new emails, did you?
    Int32 maxNotificationIconsCount = mMaxNotificationIcons;
    Int32 visibility;
    mInputMethodSwitchButton->GetVisibility(&visibility);
    if (visibility != IView::GONE) maxNotificationIconsCount --;
    mCompatModeButton->GetVisibility(&visibility);
    if (visibility != IView::GONE) maxNotificationIconsCount --;

    Boolean provisioned = IsDeviceProvisioned();
    Int32 score;
    // If the device hasn't been through Setup, we only show system notifications
    for (Int32 i = 0; toShow.GetSize() < maxNotificationIconsCount; i++) {
        if (i >= N) break;
        AutoPtr<NotificationDataEntry> ent = mNotificationData->Get(N-i-1);
        ent->mNotification->GetScore(&score);
        if ((provisioned && score >= HIDE_ICONS_BELOW_SCORE)
                || ShowNotificationEvenIfUnprovisioned(ent->mNotification)) {
            toShow.PushBack(ent->mIcon);
        }
    }

    List<AutoPtr<IView> > toRemove;
    List<AutoPtr<IView> >::Iterator it;
    Int32 childCount;
    mIconLayout->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mIconLayout->GetChildAt(i, (IView**)&child);
        it = Find(toShow.Begin(), toShow.End(), child);
        if (it == toShow.End()) {
            toRemove.PushBack(child);
        }
    }

    for (it = toRemove.Begin(); it != toRemove.End(); ++it) {
        mIconLayout->RemoveViewInLayout(*it);
    }

    Int32 i = 0;
    for (it = toShow.Begin(); it != toShow.End(); ++it, ++i) {
        AutoPtr<IView> v = *it;
        v->SetPadding(mIconHPadding, 0, mIconHPadding, 0);
        AutoPtr<IViewParent> parent;
        v->GetParent((IViewParent**)&parent);
        if (parent == NULL) {
            mIconLayout->AddView(v, i, params);
        }
    }
    return NOERROR;
}

void TabletStatusBar::LoadNotificationPanel()
{
    if (!mNotificationPanel) return;

    using Elastos::Core::Math;

    Int32 N = mNotificationData->Size();

    List<AutoPtr<IView> > toShow;

    Boolean provisioned = IsDeviceProvisioned();

    // If the device hasn't been through Setup, we only show system notifications
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<NotificationDataEntry> ent = mNotificationData->Get(N - i - 1);
        if (provisioned || ShowNotificationEvenIfUnprovisioned(ent->mNotification)) {
            toShow.PushBack(ent->mRow);
        }
    }

    List<AutoPtr<IView> >::Iterator it;
    List<AutoPtr<IView> > toRemove;
    Int32 childCount;
    mPile->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mPile->GetChildAt(i, (IView**)&child);
        it = Find(toShow.Begin(), toShow.End(), child);
        if (it == toShow.End()) {
            toRemove.PushBack(child);
        }
    }

    for (it = toRemove.Begin(); it != toRemove.End(); ++it) {
        AutoPtr<IView> remove = *it;
        mPile->RemoveViewInLayout(remove);
    }

    Int32 i = 0;
    for (it = toShow.Begin(); it != toShow.End(); ++it, ++i) {
        AutoPtr<IView> v = *it;
        AutoPtr<IViewParent> parent;
        v->GetParent((IViewParent**)&parent);
        if (parent == NULL) {
            // the notification panel has the most important things at the bottom
            mPile->GetChildCount(&childCount);
            mPile->AddView(v, Math::Min(toShow.GetSize() - 1 - i, childCount));
        }
    }

    mNotificationPanel->SetNotificationCount(toShow.GetSize());
    mNotificationPanel->SetSettingsEnabled(IsDeviceProvisioned());
}

ECode TabletStatusBar::WorkAroundBadLayerDrawableOpacity(
    /* [in] */ IView* v)
{
    AutoPtr<IDrawable> bgd;
    v->GetBackground((IDrawable**)&bgd);
    ILayerDrawable* d = ILayerDrawable::Probe(bgd.Get());
    if (!d) return NOERROR;

    v->SetBackground(NULL);
    d->SetOpacity(IPixelFormat::TRANSLUCENT);
    v->SetBackground(d);
    return NOERROR;
}

ECode TabletStatusBar::ClearAll()
{
    // try {
    if (mBarService != NULL)
        mBarService->OnClearAllNotifications();
    // } catch (RemoteException ex) {
    //     // system process is dead if we're here.
    // }
    AnimateCollapsePanels();
    VisibilityChanged(FALSE);
    return NOERROR;
}

Boolean TabletStatusBar::IsTopNotification(
    /* [in] */ IViewGroup* parent,
    /* [in] */ NotificationDataEntry* entry)
{
    if (parent == NULL || entry == NULL) return FALSE;

    Int32 index, childCount;
    parent->GetChildCount(&childCount);
    parent->IndexOfChild(entry->mRow, &index);
    return index == childCount - 1;
}

ECode TabletStatusBar::HaltTicker()
{
    mTicker->Halt();
    return NOERROR;
}

ECode TabletStatusBar::UpdateExpandedViewPos(
    /* [in] */ Int32 expandedPosition)
{
    return NOERROR;
}

Boolean TabletStatusBar::ShouldDisableNavbarGestures()
{
    Int32 visibility;
    mNotificationPanel->GetVisibility(&visibility);
    return visibility == IView::VISIBLE
        || (mDisabled & IStatusBarManager::DISABLE_HOME) != 0;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
