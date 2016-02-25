#include "elastos/droid/systemui/statusbar/tablet/TabletStatusBarView.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/systemui/statusbar/tablet/TabletStatusBar.h"

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::StatusBar::Tablet::TabletStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyButtonView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


const Boolean TabletStatusBarView::DEBUG = FALSE;

const Int32 TabletStatusBarView::MAX_PANELS = 5;
const Int32 TabletStatusBarView::PORT_WIDTH_DP = 650;

TabletStatusBarView::TabletStatusBarView()
    : mDisplayLock(0)
{
    mIgnoreChildren = ArrayOf<IView*>::Alloc(MAX_PANELS);
    mPanels = ArrayOf<IView*>::Alloc(MAX_PANELS);
    mPos[0] = mPos[1] = 0;
}

TabletStatusBarView::TabletStatusBarView(
    /* [in] */ IContext* context)
    : FrameLayout(context, NULL)
    , mDisplayLock(0)
{
    mIgnoreChildren = ArrayOf<IView*>::Alloc(MAX_PANELS);
    mPanels = ArrayOf<IView*>::Alloc(MAX_PANELS);
    mPos[0] = mPos[1] = 0;
    InitImpl(context, NULL);
}

TabletStatusBarView::TabletStatusBarView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
    , mDisplayLock(0)
{
    mIgnoreChildren = ArrayOf<IView*>::Alloc(MAX_PANELS);
    mPanels = ArrayOf<IView*>::Alloc(MAX_PANELS);
    mPos[0] = mPos[1] = 0;
    InitImpl(context, attrs);
}

ECode TabletStatusBarView::Init(
    /* [in] */ IContext* context)
{
    FrameLayout::Init(context, NULL);
    return InitImpl(context, NULL);
}

ECode TabletStatusBarView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::Init(context, attrs);
    return InitImpl(context, attrs);
}

ECode TabletStatusBarView::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    mDelegateHelper = new DelegateViewHelper(THIS_PROBE(IView));
    return NOERROR;
}

ECode TabletStatusBarView::SetDelegateView(
    /* [in] */ IView* view)
{
    mDelegateHelper->SetDelegateView(view);
    return NOERROR;
}

ECode TabletStatusBarView::SetBar(
    /* [in] */ IBaseStatusBar* bar)
{
    mDelegateHelper->SetBar(bar);
    return NOERROR;
}

//@Override
Boolean TabletStatusBarView::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    if (mDelegateHelper != NULL) {
        mDelegateHelper->OnInterceptTouchEvent(event);
    }
    return true;
}

//@Override
void TabletStatusBarView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    FrameLayout::OnLayout(changed, left, top, right, bottom);
    // Find the view we wish to grab events from in order to detect search gesture.
    // Depending on the device, this will be one of the id's listed below.
    // If we don't find one, we'll use the view provided in the constructor above (this view).
    AutoPtr<IView> view = FindViewById(SystemUIR::id::navigationArea);
    if (view == NULL) {
        view = FindViewById(SystemUIR::id::nav_buttons);
    }
    mDelegateHelper->SetSourceView(view);
    AutoPtr<ArrayOf<IView*> > views = ArrayOf<IView*>::Alloc(1);
    views->Set(0, view);
    mDelegateHelper->SetInitialTouchRegion(views);
}

//@Override
Boolean TabletStatusBarView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent*  ev)
{
    Int32 action;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        if (TabletStatusBar::DEBUG) {
            Slogger::D(TabletStatusBar::TAG, "TabletStatusBarView intercepting touch event.");
        }

        // do not close the recents panel here- the intended behavior is that recents is dismissed
        // on touch up when clicking on status bar buttons
        // TODO: should we be closing the notification panel and input methods panel?

        ITabletStatusBar* tsb = ITabletStatusBar::Probe(mDelegateHelper->mBar.Get());
        if (tsb) {
        // mHandler.removeMessages(TabletStatusBar.MSG_CLOSE_NOTIFICATION_PANEL);
        // mHandler.sendEmptyMessage(TabletStatusBar.MSG_CLOSE_NOTIFICATION_PANEL);
        // mHandler.removeMessages(TabletStatusBar.MSG_CLOSE_INPUT_METHODS_PANEL);
        // mHandler.sendEmptyMessage(TabletStatusBar.MSG_CLOSE_INPUT_METHODS_PANEL);
        // mHandler.removeMessages(TabletStatusBar.MSG_STOP_TICKER);
        // mHandler.sendEmptyMessage(TabletStatusBar.MSG_STOP_TICKER);

            TabletStatusBar* tabletStatusBar = reinterpret_cast<TabletStatusBar*>(
                tsb->Probe(EIID_TabletStatusBar));
            assert(tabletStatusBar);
            tabletStatusBar->OnStatusBarViewInterceptTouchEvent();
        }

        Int32 visibility;
        for (Int32 i = 0; i < mPanels->GetLength(); i++) {
            if ((*mPanels)[i] == NULL) continue;
            (*mPanels)[i]->GetVisibility(&visibility);
            if (visibility == IView::VISIBLE) {
                if (EventInside((*mIgnoreChildren)[i], ev)) {
                    if (TabletStatusBar::DEBUG) {
                        Int32 viewId;
                        (*mIgnoreChildren)[i]->GetId(&viewId);
                        Slogger::D(TabletStatusBar::TAG,
                                "TabletStatusBarView eating event for view: %08x", viewId);
                    }
                    return TRUE;
                }
            }
        }
    }
    if (TabletStatusBar::DEBUG) {
        Slogger::D(TabletStatusBar::TAG, "TabletStatusBarView not intercepting event");
    }
    if (mDelegateHelper != NULL && mDelegateHelper->OnInterceptTouchEvent(ev)) {
        return TRUE;
    }
    return FrameLayout::OnInterceptTouchEvent(ev);
}

void TabletStatusBarView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    /*KeyButtonView volume_up=(KeyButtonView)findViewById(R.id.volume_up);
    KeyButtonView volume_down=(KeyButtonView)findViewById(R.id.volume_down);
    KeyButtonView menu =(KeyButtonView)findViewById(R.id.menu);
    KeyButtonView screenshot =(KeyButtonView)findViewById(R.id.screenshot);
    View navigationArea=(View)findViewById(R.id.navigationArea);
    View mNotificationArea = (View)findViewById(R.id.notificationArea);
    if(displayLock){
        if(w<navigationArea.getWidth()+mNotificationArea.getWidth()){
            volume_up.setVisibility(View.GONE);
            volume_down.setVisibility(View.GONE);
            screenshot.setVisibility(View.GONE);   //GONE
            // menu.setVisibility(View.GONE);
        }else{
            volume_up.setVisibility(View.VISIBLE);
            volume_down.setVisibility(View.VISIBLE);
            screenshot.setVisibility(View.VISIBLE);
            // menu.setVisibility(View.VISIBLE);
        }
        menu.setVisibility(View.VISIBLE);
    } else {
        volume_up.setVisibility(View.GONE);
        volume_down.setVisibility(View.GONE);
        screenshot.setVisibility(View.GONE);
        menu.setVisibility(View.GONE);
    }*/
    FrameLayout::OnSizeChanged(w, h, oldw, oldh);
}

Boolean TabletStatusBarView::EventInside(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* ev)
{
    assert(v != NULL && ev != NULL);
    // assume that x and y are window coords because we are.
    Float fx, fy;
    ev->GetX(&fx);
    ev->GetY(&fy);
    Int32 x = (Int32)fx;
    Int32 y = (Int32)fy;

    Int32 w,h;
    v->GetWidth(&w);
    v->GetHeight(&h);
    v->GetLocationInWindow(&mPos[0], &mPos[1]);

    Int32 l = mPos[0];
    Int32 t = mPos[1];
    Int32 r = mPos[0] + w;
    Int32 b = mPos[1] + h;

    return x >= l && x < r && y >= t && y < b;
}

ECode TabletStatusBarView::SetShowVolume(
    /* [in] */ Boolean show,
    /* [in] */ IContext* context)
{
    mDisplayLock = show;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);

    AutoPtr<IView> tempView = FindViewById(SystemUIR::id::volume_up);
    AutoPtr<IKeyButtonView> volume_up = IKeyButtonView::Probe(tempView.Get());

    tempView = FindViewById(SystemUIR::id::volume_down);
    AutoPtr<IKeyButtonView> volume_down = IKeyButtonView::Probe(tempView.Get());

    tempView = FindViewById(SystemUIR::id::menu);
    AutoPtr<IKeyButtonView> menu = IKeyButtonView::Probe(tempView.Get());

    tempView = FindViewById(SystemUIR::id::screenshot);
    AutoPtr<IKeyButtonView> screenshot = IKeyButtonView::Probe(tempView.Get());

    if (DEBUG) {
        Slogger::I("setShowVolume", "displayLock: %d", show);
    }

    //modified by Derek for display of virtual volume keys when screen lock and unlock, 2012.10.24
    if (show) {
        Int32 screenWidthDp = 0;
        config->GetScreenWidthDp(&screenWidthDp);
        if (screenWidthDp > PORT_WIDTH_DP)  {
            volume_up->SetVisibility(IView::VISIBLE);
            volume_down->SetVisibility(IView::VISIBLE);
            //screenshot.setVisibility(IView::VISIBLE);
            // menu.setVisibility(IView::VISIBLE);
        }
        else {
            volume_up->SetVisibility(IView::GONE);
            volume_down->SetVisibility(IView::GONE);
            //screenshot->SetVisibility(View.GONE);
            // menu->SetVisibility(View.GONE);
        }

        menu->SetVisibility(IView::VISIBLE);
    }
    else {
        volume_up->SetVisibility(IView::GONE);
        volume_down->SetVisibility(IView::GONE);
        //screenshot->SetVisibility(View.GONE);
        menu->SetVisibility(IView::GONE);
    }//end modify

    /*if(config.screenWidthDp>480&&show){
      volume_up.setVisibility(View.VISIBLE);
      volume_down.setVisibility(View.VISIBLE);
    //menu.setVisibility(View.VISIBLE);
    screenshot.setVisibility(View.VISIBLE);
    }else{
    volume_up.setVisibility(View.INVISIBLE);
    volume_down.setVisibility(View.INVISIBLE);
    menu.setVisibility(View.INVISIBLE);
    screenshot.setVisibility(View.INVISIBLE);
    }*/
    return NOERROR;
}

ECode TabletStatusBarView::SetHandler(
    /* [in] */ IHandler* h)
{
    mHandler = h;
    return NOERROR;
}

/**
 * Let the status bar know that if you tap on ignore while panel is showing, don't do anything.
 *
 * Debounces taps on, say, a popup's trigger when the popup is already showing.
 */
ECode TabletStatusBarView::SetIgnoreChildren(
    /* [in] */ Int32 index,
    /* [in] */ IView* ignore,
    /* [in] */ IView* panel)
{
    mIgnoreChildren->Set(index, ignore);
    mPanels->Set(index, panel);
    return NOERROR;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
