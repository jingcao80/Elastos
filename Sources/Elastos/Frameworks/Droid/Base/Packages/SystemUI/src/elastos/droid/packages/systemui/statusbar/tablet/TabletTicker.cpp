#include "elastos/droid/systemui/statusbar/tablet/TabletTicker.h"
#include "elastos/droid/animation/CLayoutTransition.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/widget/CFrameLayout.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/statusbar/CStatusBarIcon.h"
#include "elastos/droid/systemui/statusbar/StatusBarIconView.h"
#include "elastos/droid/systemui/statusbar/tablet/TabletStatusBar.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CString;
using Elastos::Droid::R;
using Elastos::Droid::Animation::EIID_ITransitionListener;
using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::CFrameLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::StatusBar::CStatusBarIcon;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::StatusBar::StatusBarIconView;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

const String TabletTicker::TAG("StatusBar.TabletTicker");
const Boolean TabletTicker::DBG = FALSE;
const Boolean TabletTicker::CLICKABLE_TICKER = TRUE;

// 3 is enough to let us see most cases, but not get so far behind that it's too annoying.
const Int32 TabletTicker::QUEUE_LENGTH = 3;
const Int32 TabletTicker::MSG_ADVANCE = 1;
const Int32 TabletTicker::ADVANCE_DELAY = 5000; // 5 seconds

CAR_INTERFACE_IMPL_2(TabletTicker, ITransitionListener, IHandler)
IHANDLER_METHODS_IMPL(TabletTicker, Handler)

TabletTicker::TabletTicker(
    /* [in] */ TabletStatusBar* bar)
    : mQueuePos(0)
    , mLargeIconHeight(0)
    , mWindowShouldClose(FALSE)
{
    Handler::Init();
    mKeys = ArrayOf<IBinder*>::Alloc(QUEUE_LENGTH);
    mNotificaitonQueue = ArrayOf<IStatusBarNotification*>::Alloc(QUEUE_LENGTH);

    mBar = bar;
    mContext = bar->GetContext();
    assert(mContext != NULL);
    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&tmpObj);
    mWindowManager = IWindowManager::Probe(tmpObj.Get());
    assert(mWindowManager != NULL);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::notification_large_icon_height, &mLargeIconHeight);
}

void TabletTicker::Add(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    if (DBG) {
        String info;
        notification->ToString(&info);
        Slogger::D(TAG, "add notification at pos %d, notification=%s", mQueuePos, info.string());
    }

    // If it's already in here, remove whatever's in there and put the new one at the end.
    Remove(key, FALSE);

    mKeys->Set(mQueuePos, key);
    mNotificaitonQueue->Set(mQueuePos, notification);

    // If nothing is running now, start the next one.
    if (mQueuePos == 0 && mCurrentNotification == NULL) {
        Boolean result;
        SendEmptyMessage(MSG_ADVANCE, &result);
    }

    if (mQueuePos < QUEUE_LENGTH - 1) {
        mQueuePos++;
    }
}

void TabletTicker::Remove(
    /* [in] */ IBinder* key)
{
    Remove(key, TRUE);
}

void TabletTicker::Remove(
    /* [in] */ IBinder* key,
    /* [in] */ Boolean advance)
{
    if (mCurrentKey.Get() == key) {
        // Showing now
        if (advance) {
            RemoveMessages(MSG_ADVANCE);
            Boolean result;
            SendEmptyMessage(MSG_ADVANCE, &result);
        }
    }
    else {
        // In the queue
        for (Int32 i = 0; i < QUEUE_LENGTH; i++) {
            if ((*mKeys)[i] == key) {
                for (; i < QUEUE_LENGTH - 1; i++) {
                    mKeys->Set(i, (*mKeys)[i + 1]);
                    mNotificaitonQueue->Set(i, (*mNotificaitonQueue)[i + 1]);
                }

                mKeys->Set(QUEUE_LENGTH - 1, NULL);
                mNotificaitonQueue->Set(QUEUE_LENGTH - 1, NULL);
                if (mQueuePos > 0) {
                    mQueuePos--;
                }
                break;
            }
        }
    }
}

void TabletTicker::Halt()
{
    RemoveMessages(MSG_ADVANCE);

    if (mCurrentView != NULL || mQueuePos != 0) {
        for (Int32 i = 0; i < QUEUE_LENGTH; i++) {
            mKeys->Set(i, NULL);
            mNotificaitonQueue->Set(i, NULL);
        }
        mQueuePos = 0;

        Boolean result;
        SendEmptyMessage(MSG_ADVANCE, &result);
    }
}

ECode TabletTicker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case MSG_ADVANCE:
            Advance();
            break;
    }
    return NOERROR;
}

void TabletTicker::Advance()
{
    // Out with the old...
    if (mCurrentView != NULL) {
        if (mWindow != NULL) {
            mWindow->RemoveViewInLayout(mCurrentView);
        }
        mCurrentView = NULL;
        mCurrentKey = NULL;
        mPreNotification = mCurrentNotification;
        mCurrentNotification = NULL;
    }

    Boolean result;

    // In with the new...
    Dequeue();
    while (mCurrentNotification != NULL) {
        mCurrentView = MakeTickerView(mCurrentNotification);
        if (mCurrentView != NULL) {
            if (mWindow == NULL) {
                mWindow = MakeWindow();
                AutoPtr<IViewGroupLayoutParams> lp;
                mWindow->GetLayoutParams((IViewGroupLayoutParams**)&lp);
                mWindowManager->AddView(mWindow, lp);
            }

            mWindow->AddView(mCurrentView);
            SendEmptyMessageDelayed(MSG_ADVANCE, ADVANCE_DELAY, &result);
            break;
        }
        Dequeue();
    }

    // if there's nothing left, close the window
    mWindowShouldClose = (mCurrentView == NULL && mWindow != NULL);

    // TODO: delete this if Transition is ready.
    EndTransition(NULL, NULL, NULL, 0);
}

void TabletTicker::Dequeue()
{
    mCurrentKey = (*mKeys)[0];
    mCurrentNotification = (*mNotificaitonQueue)[0];
    if (DBG) {
        Slogger::D(TAG, "dequeue mQueuePos=%d", mQueuePos);
    }
    Int32 N = mQueuePos;
    for (Int32 i = 0; i < N; i++) {
        mKeys->Set(i, (*mKeys)[i+1]);
        mNotificaitonQueue->Set(i, (*mNotificaitonQueue)[i+1]);
    }
    mKeys->Set(N, NULL);
    mNotificaitonQueue->Set(N, NULL);
    if (mQueuePos > 0) {
        mQueuePos--;
    }
}

AutoPtr<IViewGroup> TabletTicker::MakeWindow()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IFrameLayout> view;
    CFrameLayout::New(mContext, (IFrameLayout**)&view);
    Int32 width;
    res->GetDimensionPixelSize(SystemUIR::dimen::notification_ticker_width, &width);
    Int32 windowFlags = IWindowManagerLayoutParams::FLAG_LAYOUT_IN_SCREEN
                | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
                | IWindowManagerLayoutParams::FLAG_LAYOUT_NO_LIMITS;
    if (CLICKABLE_TICKER) {
        windowFlags |= IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL;
    }
    else {
        windowFlags |= IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE;
    }

    AutoPtr<IWindowManagerLayoutParams> lp;
    CWindowManagerLayoutParams::New(width, mLargeIconHeight,
            IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL, windowFlags,
            IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&lp);
    lp->SetGravity(IGravity::BOTTOM | IGravity::RIGHT);
    lp->SetWindowAnimations(R::style::Animation_Toast);

    mLayoutTransition = NULL;
    CLayoutTransition::New((ILayoutTransition**)&mLayoutTransition);
    mLayoutTransition->AddTransitionListener(THIS_PROBE(ITransitionListener));
    view->SetLayoutTransition(mLayoutTransition);

    AutoPtr<ICharSequence> titleSeq;
    CString::New(String("NotificationTicker"), (ICharSequence**)&titleSeq);
    lp->SetTitle(titleSeq);
    view->SetLayoutParams(lp);

    AutoPtr<IViewGroup> vg = IViewGroup::Probe(view.Get());
    return vg;
}

ECode TabletTicker::StartTransition(
    /* [in] */ ILayoutTransition* transition,
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ Int32 transitionType)
{
    return NOERROR;
}

ECode TabletTicker::EndTransition(
    /* [in] */ ILayoutTransition* transition,
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ Int32 transitionType)
{
    if (mWindowShouldClose) {
        mWindowManager->RemoveView(mWindow);
        mWindow = NULL;
        mWindowShouldClose = FALSE;
        mBar->DoneTicking();
    }
    return NOERROR;
}

AutoPtr<IView> TabletTicker::MakeTickerView(
    /* [in] */ IStatusBarNotification* notification)
{
    AutoPtr<INotification> n;
    notification->GetNotification((INotification**)&n);

    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(tmpObj.Get());

    AutoPtr<IViewGroup> group;
    Int32 iconId;
    AutoPtr<IBitmap> largeIcon;
    n->GetLargeIcon((IBitmap**)&largeIcon);
    if (largeIcon != NULL) {
        iconId = SystemUIR::id::right_icon;
    }
    else {
        iconId = SystemUIR::id::left_icon;
    }

    AutoPtr<IView> tmpView;
    AutoPtr<IRemoteViews> tickerView;
    n->GetTickerView((IRemoteViews**)&tickerView);
    AutoPtr<ICharSequence> tickerText;
    n->GetTickerText((ICharSequence**)&tickerText);

    if (DBG) {
        String str;
        n->ToString(&str);
        Slogger::D(TAG, "> Notification:%s\n >MakeTickerView:%p, text:%p", str.string(), tickerView.Get(), tickerText.Get());
    }

    if (tickerView != NULL) {
        tmpView = NULL;
        inflater->Inflate(SystemUIR::layout::system_bar_ticker_panel,
            NULL, FALSE, (IView**)&tmpView);
        group = IViewGroup::Probe(tmpView.Get());

        tmpView = NULL;
        group->FindViewById(SystemUIR::id::ticker_expanded, (IView**)&tmpView);
        AutoPtr<IViewGroup> content = IViewGroup::Probe(tmpView.Get());
        AutoPtr<IView> expanded;
        ECode ec = tickerView->Apply(mContext, content, (IView**)&expanded);
        if (expanded == NULL || FAILED(ec)) {
            String str;
            n->ToString(&str);
            Slogger::E(TAG, "couldn't inflate view for notification %p, ec=%08x", str.string(), ec);
            return NULL;
        }

        AutoPtr<IFrameLayoutLayoutParams> lp;
        CFrameLayoutLayoutParams::New(
                IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::MATCH_PARENT,
                (IFrameLayoutLayoutParams**)&lp);
        content->AddView(expanded, lp);
    }
    else if (tickerText != NULL) {
        tmpView = NULL;
        inflater->Inflate(SystemUIR::layout::system_bar_ticker_compat,
            mWindow, FALSE, (IView**)&tmpView);
        group = IViewGroup::Probe(tmpView.Get());

        String pkg;
        AutoPtr<IUserHandle> user;
        Int32 icon, iconLevel;
        notification->GetPkg(&pkg);
        notification->GetUser((IUserHandle**)&user);
        n->GetIcon(&icon);
        n->GetIconLevel(&iconLevel);

        AutoPtr<IStatusBarIcon> sbIcon;
        CStatusBarIcon::New(pkg, user, icon, iconLevel,
            0, tickerText, (IStatusBarIcon**)&sbIcon);
        AutoPtr<IDrawable> iconDrawable = StatusBarIconView::GetIcon(mContext, sbIcon);

        tmpView = NULL;
        group->FindViewById(iconId, (IView**)&tmpView);
        AutoPtr<IImageView> iv = IImageView::Probe(tmpView.Get());
        iv->SetImageDrawable(iconDrawable);
        iv->SetVisibility(IView::VISIBLE);

        tmpView = NULL;
        group->FindViewById(SystemUIR::id::text, (IView**)&tmpView);
        AutoPtr<ITextView> tv =  ITextView::Probe(tmpView.Get());
        tv->SetText(tickerText);

        if (DBG) {
            String info;
            tickerText->ToString(&info);
            Slogger::D(TAG, "TabletTicker::MakeTickerView: %s", info.string());
        }
    }
    else {
        Slogger::E(TAG, "tickerView==NULL && tickerText==NULL");
        return NULL;
    }

    tmpView = NULL;
    group->FindViewById(SystemUIR::id::large_icon, (IView**)&tmpView);
    AutoPtr<IImageView> largeIconView = IImageView::Probe(tmpView.Get());

    if (largeIcon != NULL) {
        largeIconView->SetImageBitmap(largeIcon);
        largeIconView->SetVisibility(IView::VISIBLE);
        AutoPtr<IViewGroupLayoutParams> lp;
        largeIconView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        Int32 statusBarHeight = mBar->GetStatusBarHeight();

        Int32 iconHeight;
        largeIconView->GetHeight(&iconHeight);
        if (iconHeight <= statusBarHeight) {
            // for smallish largeIcons, it looks a little odd to have them floating halfway up
            // the ticker, so we vertically center them in the status bar area instead
            lp->SetHeight(statusBarHeight);
        }
        else {
            lp->SetHeight(mLargeIconHeight);
        }
        largeIconView->SetLayoutParams(lp);
    }

    if (CLICKABLE_TICKER) {
        AutoPtr<IPendingIntent> contentIntent;
        n->GetContentIntent((IPendingIntent**)&contentIntent);
        if (contentIntent != NULL) {
            // create the usual notification clicker, but chain it together with a halt() call
            // to abort the ticker too
            String pkg, tag;
            Int32 id;
            notification->GetPkg(&pkg);
            notification->GetTag(&tag);
            notification->GetId(&id);
            AutoPtr<IViewOnClickListener> clicker = mBar->MakeClicker(contentIntent, pkg, tag, id);
            AutoPtr<IViewOnClickListener> listener = new TickerClickListener(clicker, this);
            group->SetOnClickListener(listener);
        } else {
            group->SetOnClickListener(NULL);
        }
    }

    return group;
}

//===========================================================================================
//                  TabletTicker::TickerClickListener
//===========================================================================================
CAR_INTERFACE_IMPL(TabletTicker::TickerClickListener, IViewOnClickListener)

TabletTicker::TickerClickListener::TickerClickListener(
    /* [in] */ IViewOnClickListener* clicker,
    /* [in] */ TabletTicker* host)
    : mClicker(clicker)
    , mHost(host)
{
}

ECode TabletTicker::TickerClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->Halt();
    return mClicker->OnClick(v);
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
