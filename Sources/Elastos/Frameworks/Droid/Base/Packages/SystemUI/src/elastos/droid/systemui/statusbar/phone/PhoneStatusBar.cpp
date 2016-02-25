#include "elastos/droid/systemui/statusbar/phone/PhoneStatusBar.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/systemui/statusbar/policy/Prefs.h"


using Elastos::Droid::Provide::Settings;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::SystemUI::StatusBar::Policy::Prefs;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

//================================================================================
//              PhoneStatusBar::MakeIconsInvisibleAnimatorListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::MakeIconsInvisibleAnimatorListener, IAnimatorListener);

PhoneStatusBar::MakeIconsInvisibleAnimatorListener::MakeIconsInvisibleAnimatorListener(
    /* [in] */ PhoneStatusBar* host)
    : mHost(host)
{
}

ECode PhoneStatusBar::MakeIconsInvisibleAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    Int32 alpha;
    mHost->mStatusBarContents->GetAlpha(&alpha);
    // double-check to avoid races
    if (alpha == 0) {
        // if (DEBUG) Slog.d(TAG, "makeIconsInvisible");
        mHost->mStatusBarContents->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::UserSetupObserver
//================================================================================
PhoneStatusBar::UserSetupObserver::UserSetupObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ PhoneStatusBar* host)
    : mHost(host)
{
}

ECode PhoneStatusBar::UserSetupObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 setUpComplete;
    Settings::Secure::GetInt32ForUser(
            resolver,
            ISettingsSecure::USER_SETUP_COMPLETE,
            0 /*default */,
            mHost->mCurrentUserId, &setUpComplete);
    Boolean userSetup = 0 != complete;
    // if (MULTIUSER_DEBUG) Slog.d(TAG, String.format("User setup changed: " +
    //         "selfChange=%s userSetup=%s mUserSetup=%s",
    //         selfChange, userSetup, mUserSetup));
    if (mHost->mSettingsButton != NULL && !mHost->mHasSettingsPanel) {
        mHost->mSettingsButton->SetVisibility(userSetup ? IView::VISIBLE : IView::INVISIBLE);
    }
    if (mHost->mSettingsPanel != NULL) {
        mHost->mSettingsPanel->SetEnabled(userSetup);
    }
    if (userSetup != mHost->mUserSetup) {
        mHost->mUserSetup = userSetup;
        if (!mHost->mUserSetup && mHost->mStatusBarView != NULL)
            mHost->AnimateCollapseQuickSettings();
    }
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::StatusBarWindowTouchListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::StatusBarWindowTouchListener, IViewOnTouchListener)

PhoneStatusBar::StatusBarWindowTouchListener::StatusBarWindowTouchListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::StatusBarWindowTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        if (mHost->mExpandedVisible && !mHost->mAnimating) {
            mHost->AnimateCollapsePanels();
        }
    }
    return mHost->mStatusBarWindow->OnTouchEvent(event);
}

//================================================================================
//              PhoneStatusBar::HeaderTouchListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::HeaderTouchListener, IViewOnTouchListener)

PhoneStatusBar::HeaderTouchListener::HeaderTouchListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::HeaderTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE; // e eats everything
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::EmergencyCallLabelClickListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::EmergencyCallLabelClickListener, IViewOnClickListener)

PhoneStatusBar::EmergencyCallLabelClickListener::EmergencyCallLabelClickListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::EmergencyCallLabelClickListener::OnClick(
    /* [in] */ IView* v)
{
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::EmergencyCallLabelLayoutChangeListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::EmergencyCallLabelLayoutChangeListener, IViewOnLayoutChangeListener)

PhoneStatusBar::EmergencyCallLabelLayoutChangeListener::EmergencyCallLabelLayoutChangeListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::EmergencyCallLabelLayoutChangeListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    mHost->UpdateCarrierLabelVisibility(FALSE);
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::PileSizeChangedListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::PileSizeChangedListener, IOnSizeChangedListener)

PhoneStatusBar::PileSizeChangedListener::PileSizeChangedListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::PileSizeChangedListener::OnSizeChanged(
    /* [in] */ IView* v,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    mHost->UpdateCarrierLabelVisibility(FALSE);
}

//================================================================================
//              PhoneStatusBar::RecentsClickListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::RecentsClickListener, IViewOnClickListener)

PhoneStatusBar::RecentsClickListener::RecentsClickListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::RecentsClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->ToggleRecentApps();
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::RotSettingClickListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::RotSettingClickListener, IViewOnClickListener)

PhoneStatusBar::RotSettingClickListener::RotSettingClickListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::RotSettingClickListenerOnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    AutoPtr<IImageView> rotBtn;
    mHost->mNavigationBarView->GetRotationSettingButton((IImageView**)&rotBtn);
    if (irot >= 0){
        irot = (irot + 1)%4;
        rotBtn->SetBackgroundDrawable((*mHost->mRotsrc)[irot]);
        mHost->SendRotBroadcast();
    }
}

//================================================================================
//              PhoneStatusBar::HomeSearchActionListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::HomeSearchActionListener, IViewOnTouchListener)

PhoneStatusBar::HomeSearchActionListener::HomeSearchActionListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::HomeSearchActionListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO
    // Int23 action;
    // event->GetAction(&action);
    // switch(action) {
    //     case IMotionEvent::ACTION_DOWN:
    //         if (!mHost->ShouldDisableNavbarGestures()) {
    //             mHost->mHandler->removeCallbacks(mHost->mShowSearchPanel);
    //             mHost->mHandler->postDelayed(mHost->mShowSearchPanel, mHost->mShowSearchHoldoff);
    //         }
    //     break;

    //     case IMotionEvent::ACTION_UP:
    //     case IMotionEvent::ACTION_CANCEL:
    //         mHost->mHandler->RemoveCallbacks(mHost->mShowSearchPanel);
    //         mHost->AwakenDreams();
    //     break;
    // }
    *result = FALSE;
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::ViewOnFocusChangeListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::ViewOnFocusChangeListener, IViewOnFocusChangeListener)

PhoneStatusBar::ViewOnFocusChangeListener::ViewOnFocusChangeListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::ViewOnFocusChangeListenerOnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    // Because 'v' is a ViewGroup, all its children will be (un)selected
    // too, which allows marqueeing to work.
    v->SetSelected(hasFocus);
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::ExpandedInvisibleRunnable
//================================================================================
PhoneStatusBar::ExpandedInvisibleRunnable::ExpandedInvisibleRunnable(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::ExpandedInvisibleRunnable::Run()
{
    mHost->MakeExpandedInvisible();
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::ClingRunnable
//================================================================================
PhoneStatusBar::ClingRunnable::ClingRunnable(
    /* [in] */ IViewStub* viewStub,
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
    , mViewStub(viewStub)
{}

ECode PhoneStatusBar::ClingRunnable::Run()
{
    AutoPtr<IView> tmpView;
    mViewStub->Inflate((IView**)&tmpView);
    mHost->mCling = IViewGroup::Probe(tmpView.Get());

    AutoPtr<IViewOnTouchListener> touchListener = new ClingTouchListener(this);
    AutoPtr<IViewOnClickListener> clickListener = new ClingClickListener(this);
    mHost->mCling->SetOnTouchListener(touchListener);
    AutoPtr<IView> okView;
    mHost->mCling->FindViewById(R::id::ok, (IView**)&okView);
    okView->SetOnClickListener(clickListener);

    mHost->mCling->SetAlpha(0.0f);
    mHost->mCling->SetVisibility(IView::VISIBLE);
    AutoPtr<IViewPropertyAnimator> animator;
    mHost->mCling->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(1.0f);

    mHost->mClingShown = TRUE;
    AutoPtr<ISharedPreferencesEditor> editor = Prefs::Edit(mContext);
    editor->PutBoolean(Prefs::SHOWN_QUICK_SETTINGS_HELP, TRUE);
    editor->Apply();

    mHost->MakeExpandedVisible(TRUE); // enforce visibility in case the shade is still animating closed
    mHost->AnimateExpandNotificationsPanel();

    mHost->mSuppressStatusBarDrags = FALSE;
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::ClingClickListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::ClingClickListener, IViewOnClickListener)

PhoneStatusBar::ClingClickListener::ClingClickListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::ClingClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->HideCling();
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::ClingTouchListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::ClingTouchListener, IViewOnTouchListener)

PhoneStatusBar::ClingTouchListener::ClingTouchListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::ClingTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return TRUE; // e eats everything
}

//================================================================================
//              PhoneStatusBar::ClearButtonClickRunnable
//================================================================================
PhoneStatusBar::ClearButtonClickRunnable::ClearButtonClickRunnable(
    /* [in] */ List<AutoPtr<IView> >& snapshot,
    /* [in] */ PhoneStatusBar* bar)
    : mSnapshot(snapshot)
    , mHost(bar)
{
}

ECode PhoneStatusBar::ClearButtonClickRunnable::Run()
{
    // Decrease the delay for every row we animate to give the sense of
    // accelerating the swipes
    Int32 ROW_DELAY_DECREMENT = 10;
    Int32 currentDelay = 140;
    Int32 totalDelay = 0;

    // Set the shade-animating state to avoid doing other work during
    // all of these animations. In particular, avoid layout and
    // redrawing when collapsing the shade.
    mHost->mPile->SetViewRemoval(FALSE);

    mHost->mPostCollapseCleanup = new CollapseCleanupRunnable(mHost);

    AutoPtr<IView> sampleView = mSnapshot[0];
    Int32 width;
    sampleView->GetWidth(&width);
    Int32 velocity = width * 8; // 1000/8 = 125 ms duration

    Boolean result;
    List<AutoPtr<IView> >::Iterator it;
    for (it = mSnapshot.Begin(); it != mSnapshot.End(); ++it) {
        AutoPtr<IView> view = *it;
        AutoPtr<IRunnable> runnable = new DismissRowAnimatedRunnable(view, velocity, mHost);
        mHost->mHandler->PostDelayed(runnable, totalDelay, &result);
        currentDelay = Math::Max(50, currentDelay - ROW_DELAY_DECREMENT);
        totalDelay += currentDelay;
    }

    // Delay the collapse animation until after all swipe animations have
    // finished. Provide some buffer because there may be some extra delay
    // before actually starting each swipe animation. Ideally, we'd
    // synchronize the end of those animations with the start of the collaps
    // exactly.
    AutoPtr<IRunnable> runnable = new DelayCollapseAnimationRunnable(mHost);
    mHost->mHandler->PostDelayed(runnable, totalDelay + 225, &result);
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::CollapseCleanupRunnable
//================================================================================
PhoneStatusBar::CollapseCleanupRunnable::CollapseCleanupRunnable(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{
}

ECode PhoneStatusBar::CollapseCleanupRunnable::Run()
{
    // if (DEBUG) {
    //     Slog.v(TAG, "running post-collapse cleanup");
    // }
    // try {
        mHost->mPile->SetViewRemoval(TRUE);
        mHost->mBarService->OnClearAllNotifications();
    // } catch (Exception ex) { }
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::DismissRowAnimatedRunnable
//================================================================================
PhoneStatusBar::DismissRowAnimatedRunnable::DismissRowAnimatedRunnable(
    /* [in] */ IView* view,
    /* [in] */ Int32 velocity,
    /* [in] */ PhoneStatusBar* bar)
    : mView(view)
    , mVelocity(velocity)
    , mHost(bar)
{
}

ECode PhoneStatusBar::DismissRowAnimatedRunnable::Run()
{
    mHost->mPile->DismissRowAnimated(view, velocity);
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::DelayCollapseAnimationRunnable
//================================================================================
PhoneStatusBar::DelayCollapseAnimationRunnable::DelayCollapseAnimationRunnable(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{
}

ECode PhoneStatusBar::DelayCollapseAnimationRunnable::Run()
{
    mHost->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::ClearButtonClickListener
//================================================================================

CAR_INTERFACE_IMPL(PhoneStatusBar::ClearButtonClickListener, IViewOnClickListener)

PhoneStatusBar::ClearButtonClickListener::ClearButtonClickListener(
    /* [in] */ PhoneStatusBar* bar)
    :  mHost(bar)
{}

ECode PhoneStatusBar::ClearButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    // animate-swipe all dismissable notifications, then animate the shade closed
    Int32 numChildren;
    mHost->mPile->GetChildCount(&numChildren);

    Int32 scrollTop;
    mHost->mScrollView->GetScrollY(&scrollTop);
    Int32 scrollBottom;
    mHost->mScrollView->GetHeight(&scrollBottom)
    scrollBottom += scrollTop;

    Int32 bottom, top;
    Boolean canBeDismissed;
    List<AutoPtr<IView> > snapshot(numChildren);
    for (Int32 i = 0; i< numChildren; i++) {
        AutoPtr<IView> child;
        mHost->mPile->GetChildAt(i, (IView**)&child);
        mHost->mPile->CanChildBeDismissed(child, &canBeDismissed);
        if (canBeDismissed) {
            child->GetBottom(&bottom);
            child->GetTop(&top);

            if (bottom > scrollTop && top < scrollBottom) {
                snapshot->Add(child);
            }
        }
    }

    if (snapshot->IsEmpty()) {
        mHost->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
        return NOERROR;
    }

    AutoPtr<IRunnable> runnable = new ClearButtonClickRunnable(snapshot, mHost);
    AutoPtr<IThread> thread;
    CThread::New(runnable, (IThread**)&thread);
    thread->Start();
}

//================================================================================
//              PhoneStatusBar::SettingsButtonClickListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::SettingsButtonClickListener, IViewOnClickListener)

PhoneStatusBar::SettingsButtonClickListener::SettingsButtonClickListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::SettingsButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (mHasSettingsPanel) {
        mHost->AnimateExpandSettingsPanel();
    }
    else {
        AutoPtr<IIntent> intent;
        CIntent::New(ISettings::ACTION_SETTINGS, TRUE, (IIntent**)&intent);
        mHost->StartActivityDismissingKeyguard(intent);
    }
}

//================================================================================
//              PhoneStatusBar::ClockClickListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::ClockClickListener, IViewOnClickListener)

PhoneStatusBar::ClockClickListener::ClockClickListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::ClockClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_QUICK_CLOCK, TRUE, (IIntent**)&intent);
    mHost->StartActivityDismissingKeyguard(intent); // have fun, everyone
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::NotificationButtonListener
//================================================================================
CAR_INTERFACE_IMPL(PhoneStatusBar::NotificationButtonListener, IViewOnClickListener)

PhoneStatusBar::NotificationButtonListener::NotificationButtonListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::NotificationButtonListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->AnimateExpandNotificationsPanel();
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::PhoneStatusBarBroadcastReceiver
//================================================================================
PhoneStatusBar::PhoneStatusBarBroadcastReceiver::PhoneStatusBarBroadcastReceiver(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{}

ECode PhoneStatusBar::PhoneStatusBarBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    assert(intent != NULL);

    // if (DEBUG) Slog.v(TAG, "onReceive: " + intent);
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_CLOSE_SYSTEM_DIALOGS.Equals(action)) {
        Int32 flags = ICommandQueue::FLAG_EXCLUDE_NONE;
        if (IIntent::ACTION_CLOSE_SYSTEM_DIALOGS.equals(action)) {
            String reason;
            intent->GetStringExtra("reason", &reason);
            if (!reason.IsNull() && reason.Equals(SYSTEM_DIALOG_REASON_RECENT_APPS)) {
                flags |= ICommandQueue::FLAG_EXCLUDE_RECENTS_PANEL;
            }
        }
        mHost->AnimateCollapsePanels(flags);
    }
    else if (IIntent::ACTION_SCREEN_OFF.equals(action)) {
        // no waiting!
        mHost->MakeExpandedInvisible();
        mHost->NotifyNavigationBarScreenOn(FALSE);
    }
    else if (IIntent::ACTION_CONFIGURATION_CHANGED.Equals(action)) {
        // if (DEBUG) {
        //     Slog.v(TAG, "configuration changed: " + mContext.getResources().getConfiguration());
        // }
        mHost->mDisplay->GetSize(mCurrentDisplaySize);

        mHost->UpdateResources();
        mHost->RepositionNavigationBar();
        mHost->UpdateExpandedViewPos(EXPANDED_LEAVE_ALONE);
        mHost->UpdateShowSearchHoldoff();
    }
    else if (IIntent::ACTION_SCREEN_ON.equals(action)) {
        // work around problem where mDisplay.getRotation() is not stable while screen is off (bug 7086018)
        mHost->RepositionNavigationBar();
        mHost->NotifyNavigationBarScreenOn(TRUE);
    }
    else if (PhoneStatusBar::setRotShow.Equals(action)){
        AutoPtr<IImageView> rotBtn;
        mNavigationBarView->GetRotationSettingButton((IImageView**)&rotBtn);
        Int32 rot;
        intent->GetIntExtra(setRotStr, -1, &rot);
        if (rotBtn != NULL && rot >= -1){
            Boolean isShown;
            rotBtn->IsShown(&isShown);
            if (rot == 0 && (isShown == FALSE)){
                rotBtn->SetBackgroundDrawable((*mHost->mRotsrc)[0]);
                rotBtn->SetVisibility(IView::VISIBLE);
                irot = 0;
            }
            else if (rot == -1 && isShown){
                rotBtn->SetVisibility(IView::GONE);
                irot = -1;
                mHost->SendRotBroadcast();
            }
        }
    }
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::MyTicker
//================================================================================
PhoneStatusBar::MyTicker::MyTicker(
    /* [in] */ IContext* context,
    /* [in] */ IView* sb,
    /* [in] */ PhoneStatusBar* bar)
    : Ticker(context, sb)
    , mHost(bar)
{
}

//@Override
ECode PhoneStatusBar::MyTicker::TickerStarting()
{
    mHost->mTicking = TRUE;
    mHost->mStatusBarContents->SetVisibility(IView::GONE);
    mHost->mTickerView->SetVisibility(IView::VISIBLE);
    AutoPtr<IAnimation> animation = mHost->LoadAnim(R::anim::push_up_in, NULL);
    mHost->mTickerView->StartAnimation(animation);
    animation = mHost->LoadAnim(R::anim::push_up_out, NULL);
    mHost->mStatusBarContents->StartAnimation(animation);
}

//@Override
ECode PhoneStatusBar::MyTicker::TickerDone()
{
    mHost->mStatusBarContents->SetVisibility(IView::VISIBLE);
    mHost->mTickerView->SetVisibility(IView::GONE);
    AutoPtr<IAnimation> animation = mHost->LoadAnim(R::anim::push_down_in, NULL);
    mHost->mStatusBarContents->StartAnimation(animation);
    animation = mHost->LoadAnim(R::anim::push_down_out, NULL);
    mHost->mTickerView->StartAnimation(animation);
}

//@Override
ECode PhoneStatusBar::MyTicker::TickerHalting()
{
    StatusBarContents->SetVisibility(IView::VISIBLE);
    mHost->mTickerView->SetVisibility(IView::GONE);
    AutoPtr<IAnimation> animation = mHost->LoadAnim(R::anim::fade_in, NULL);
    mHost->mStatusBarContents->StartAnimation(animation);
    // we do not animate the ticker away at this point, just get rid of it (b/6992707)
}

//================================================================================
//              PhoneStatusBar::TickingDoneListener
//================================================================================
PhoneStatusBar::MyTicker::TickingDoneListener(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{
}

ECode PhoneStatusBar::MyTicker::OnAnimationStart(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

ECode PhoneStatusBar::MyTicker::OnAnimationRepeat(
    /* [in] */ IAnimation* animation)
{
    return NOERROR;
}

ECode PhoneStatusBar::MyTicker::OnAnimationEnd(
    /* [in] */ IAnimation* animation)
{
    mHost->mTicking = FALSE;
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar::ShowSearchPanelRunnable
//================================================================================
PhoneStatusBar::ShowSearchPanelRunnable::ShowSearchPanelRunnable(
    /* [in] */ PhoneStatusBar* bar)
    : mHost(bar)
{
}

ECode PhoneStatusBar::ShowSearchPanelRunnable::Run()
{
    mHost->ShowSearchPanel();
    mHost->AwakenDreams();
    return NOERROR;
}

//================================================================================
//              PhoneStatusBar
//================================================================================
//@Override
ECode PhoneStatusBar::Start()
{
    AutoPtr<IInterface> tmpObj;
    ASSERT_SUCCEEDED(context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&tmpObj));
    AutoPtr<IWindowManager> wm = IWindowManager::Probe(tmpObj.Get());

    mDisplay = NULL;
    wm->GetDefaultDisplay((IDisplay**)&mDisplay);

    tmpObj = NULL;
    ServiceManager::CheckService(DreamService.DREAM_SERVICE, (IInterface**)&tmpObj);
    mDreamManager = IIDreamManager::Probe(tmpObj.Get());

    BaseStatusBar::Start(); // calls createAndAddWindows()

    AddNavigationBar();

    if (ENABLE_INTRUDERS) AddIntruderView();

    // Lastly, call to the icon policy to install/update all the icons.
    mIconPolicy = NULL;
    CPhoneStatusBarPolicy::New(mContext, (IPhoneStatusBarPolicy**)&mIconPolicy);
}

AutoPtr<IPhoneStatusBarView> MakeStatusBarView()
{
    AutoPtr<IContext> context = mContext;

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    UpdateDisplaySize(); // populates mDisplayMetrics
    LoadDimens();

    res->GetDimensionPixelSize(R::dimen::status_bar_icon_size, &mIconSize);

    AutoPtr<IView> tmpView;
    View::Inflate(context, R::layout::super_status_bar, NULL, (IView**)&tmpView);
    mStatusBarWindow = IStatusBarWindowView::Probe(tmpView.Get());
    assert(mStatusBarWindow != NULL);
    ((CStatusBarWindowView*)mStatusBarWindow.Get())->mService = this;
    AutoPtr<IViewOnTouchListener> touchListener = new StatusBarWindowTouchListener(this);
    mStatusBarWindow->SetOnTouchListener(touchListener);

    tmpView = NULL;
    mStatusBarWindow->FindViewById(R.id.status_bar, (IView**)&tmpView);
    assert(mStatusBarView != NULL);
    mStatusBarView = IPhoneStatusBarView::Probe(tmpView.Get());
    mStatusBarView->SetBar(this);

    tmpView = NULL;
    mStatusBarWindow->FindViewById(R.id.panel_holder, (IView**)&tmpView);
    AutoPtr<IPanelHolder> holder = IPanelHolder::Probe(tmpView.Get());
    mStatusBarView->SetPanelHolder(holder);

    tmpView = NULL;
    mStatusBarWindow->FindViewById(R.id.notification_panel, (IView**)&tmpView);
    mNotificationPanel = INotificationPanelView::Probe(tmpView.Get());
    assert(mNotificationPanel != NULL);
    mNotificationPanel->SetStatusBar(this);
    AutoPtr<IViewGroupLayoutParams> vglp;
    mNotificationPanel.getLayoutParams((IViewGroupLayoutParams**)&vglp);
    Int32 width;
    vglp->GetWidth(&width);
    mNotificationPanelIsFullScreenWidth = width == IViewGroupLayoutParams::MATCH_PARENT);

    // make the header non-responsive to clicks
    AutoPtr<IViewOnClickListener> headerTouchListener = new HeaderTouchListener(this);
    tmpView = NULL;
    mNotificationPanel->FindViewById(R.id.header, &tmpView);
    tmpView->SetOnTouchListener(headerTouchListener);

    if (!CActivityManager::IsHighEndGfx()) {
        mStatusBarWindow->SetBackground(NULL);
        Int32 color;
        res->GetColor(SystemUIR::color::notification_panel_solid_background, &color);
        AutoPtr<IFastColorDrawable> fastColor;
        CFastColorDrawable::New(color, (IFastColorDrawable**)&fastColor);
        mNotificationPanel->SetBackground(IDrawable::Probe(fastColor.Get()));
    }
    if (ENABLE_INTRUDERS) {
        tmpView = NULL;
        View::Inflate(context, R.layout.intruder_alert, NULL, (IView**)&tmpView);
        mIntruderAlertView = IIntruderAlertView::Probe(tmpView.Get());
        assert(mIntruderAlertView != NULL);
        mIntruderAlertView->SetVisibility(IView::GONE);
        mIntruderAlertView->SetBar(this);
    }
    if (MULTIUSER_DEBUG) {
        tmpView = NULL;
        mNotificationPanel->FindViewById(R.id.header_debug_info, (IView**)&tmpView);
        mNotificationPanelDebugText = ITextView::Probe(tmpView.Get());
        mNotificationPanelDebugText->SetVisibility(IView::VISIBLE);
    }

    UpdateShowSearchHoldoff();

    // try {
        Boolean showNav;
        mWindowManagerService->HasNavigationBar(&showNav);
        // if (DEBUG) Slog.v(TAG, "hasNavigationBar=" + showNav);
        if (showNav) {
            tmpView = NULL;
            View::Inflate(context, SystemUIR::layout::navigation_bar, NULL, (IView**)&tmpView);
            mNavigationBarView = INavigationBarView::Probe(tmpView.Get());

            mNavigationBarView->SetDisabledFlags(mDisabled);
            mNavigationBarView->SetBar(THIS_PROBE(IStatusBar));
        }
    // } catch (RemoteException ex) {
    //     // no window manager? good luck with that
    // }

    // figure out which pixel-format to use for the status bar.
    mPixelFormat = IPixelFormat::OPAQUE;

    tmpView = NULL;
    mStatusBarView->FindViewById(R.id.system_icon_area, (IView**)&tmpView));
    mSystemIconArea = ILinearLayout::Probe(tmpView.Get());

    tmpView = NULL;
    mStatusBarView->FindViewById(R.id.statusIcons, (IView**)&tmpView));
    mStatusIcons = ILinearLayout::Probe(tmpView.Get());

    tmpView = NULL;
    mStatusBarView->FindViewById(R.id.notificationIcons, (IView**)&tmpView));
    mNotificationIcons = ILinearLayout::Probe(tmpView.Get());
    mNotificationIcons->SetOverflowIndicator(mMoreIcon);

    tmpView = NULL;
    mStatusBarView->FindViewById(R.id.status_bar_contents, (IView**)&tmpView));
    mStatusBarContents = ILinearLayout::Probe(tmpView.Get());

    mTickerView = NULL;
    mStatusBarView->FindViewById(R.id.ticker, (IView**)&mTickerView);

    tmpView = NULL;
    mStatusBarWindow->FindViewById(R.id.latestItems, (IView**)&tmpView));
    mPile = INotificationRowLayout::Probe(tmpView.Get());
    mPile->SetLayoutTransitionsEnabled(FALSE);
    AutoPtr<IViewOnLongClickListener> longClickListener = GetNotificationLongClicker();
    mPile->SetLongPressListener(longClickListener);
    mExpandedContents = mPile; // was: expanded->FindViewById(R.id.notificationLinearLayout);

    mNotificationPanelHeader = NULL;
    mStatusBarWindow->FindViewById(R.id.header, (IView**)&mNotificationPanelHeader);

    mClearButton = NULL;
    mStatusBarWindow->FindViewById(R.id.clear_all_button, (IView**)&mClearButton);
    mClearButton->SetOnClickListener(mClearButtonListener);
    mClearButton->SetAlpha(0.0f);
    mClearButton->SetVisibility(IView::INVISIBLE);
    mClearButton->SetEnabled(FALSE);

    tmpView = NULL;
    mStatusBarWindow->FindViewById(R::id::date, (IView**)&tmpView);
    mDateView = IDateView::Probe(tmpView.Get());

    res->GetBoolean(SystemUIR::bool_::config_hasSettingsPanel, &mHasSettingsPanel);
    res->GetBoolean(SystemUIR::bool_::config_hasFlipSettingsPanel, &mHasFlipSettings);

    mDateTimeView = mNotificationPanelHeader->FindViewById(R::id::datetime);
    if (mHasFlipSettings) {
        mDateTimeView->SetOnClickListener(mClockClickListener);
        mDateTimeView->SetEnabled(TRUE);
    }

    mSettingsButton = NULL;
    StatusBarWindow->FindViewById(R.id.settings_button, (IView**)&mSettingsButton);
    if (mSettingsButton != NULL) {
        mSettingsButton->SetOnClickListener(mSettingsButtonListener);
        if (mHasSettingsPanel) {
            Boolean hasFWN;
            if (mStatusBarView->HasFullWidthNotifications(&hasFWN), hasFWN) {
                // the settings panel is hiding behind this button
                mSettingsButton->SetImageResource(SystemUIR::drawable::ic_notify_quicksettings);
                mSettingsButton->SetVisibility(IView::VISIBLE);
            }
            else {
                // there is a settings panel, but it's on the other side of the (large) screen
                AutoPtr<IView> buttonHolder;
               mStatusBarWindow->FindViewById(R.id.settings_button_holder, (IView**)&buttonHolder);
                if (buttonHolder != NULL) {
                    buttonHolder->SetVisibility(IView::GONE);
                }
            }
        }
        else {
            // no settings panel, go straight to settings
            mSettingsButton->SetVisibility(IView::VISIBLE);
            mSettingsButton->SetImageResource(SystemUIR::drawable::ic_notify_settings);
        }
    }
    if (mHasFlipSettings) {
        tmpView = NULL;
        mStatusBarWindow->FindViewById(R.id.notification_button, (IView**)&tmpView);
        mNotificationButton = IImageView::Probe(tmpView.Get());
        if (mNotificationButton != NULL) {
            mNotificationButton->SetOnClickListener(mNotificationButtonListener);
        }
    }

    tmpView = NULL;
    mStatusBarWindow->FindViewById(R.id.scroll, (IView**)&tmpView);
    mScrollView = IScrollView::Probe(tmpView.Get());
    mScrollView->SetVerticalScrollBarEnabled(FALSE); // less drawing during pulldowns
    if (!mNotificationPanelIsFullScreenWidth) {
        mScrollView->SetSystemUiVisibility(
            IView::STATUS_BAR_DISABLE_NOTIFICATION_TICKER |
            IView::STATUS_BAR_DISABLE_NOTIFICATION_ICONS |
            IView::STATUS_BAR_DISABLE_CLOCK);
    }

    mTicker = new MyTicker(context, mStatusBarView);

    tmpView = NULL;
    mStatusBarView->FindViewById(SystemUIR::id::tickerText, (IView**)&tmpView);
    AutoPtr<ITickerView> tickerView = ITickerView::Probe(tmpView.Get());
    tickerView->SetTicker(mTicker);

    res->GetDimensionPixelSize(SystemUIR::dimen::status_bar_edge_ignore, &mEdgeBorder);

    // set the inital view visibility
    SetAreThereNotifications();

    // Other icons
    // mLocationController = new LocationController(mContext); // will post a notification
    // mBatteryController = new BatteryController(mContext);
    // mBatteryController.addIconView((ImageView)mStatusBarView->FindViewById(R.id.battery));
    // mNetworkController = new NetworkController(mContext);
    // mBluetoothController = new BluetoothController(mContext);
    tmpView = NULL;
    mStatusBarView->FindViewById(SystemUIR::id::signal_cluster, (IView**)&tmpView);
    AutoPtr<ISignalClusterView> signalCluster = ISignalClusterView::Probe(tmpView.Get());

    mNetworkController->AddSignalCluster(signalCluster);
    signalCluster->SetNetworkController(mNetworkController);

    tmpView = NULL;
    mStatusBarWindow->FindViewById(R::id::emergency_calls_only, (IView**)&tmpView);
    mEmergencyCallLabel = ITextView::Probe(tmpView.Get());
    if (mEmergencyCallLabel != NULL) {
        mNetworkController->AddEmergencyLabelView(mEmergencyCallLabel);
        AutoPtr<IViewOnClickListener> eclClickListener = new EmergencyCallLabelClickListener(this);
        mEmergencyCallLabel->SetOnClickListener(eclClickListener);
    }

    tmpView = NULL;
    mStatusBarWindow->FindViewById(R::id::carrier_label, (IView**)&tmpView);
    mCarrierLabel = ITextView::Probe(tmpView.Get());
    mShowCarrierInPanel = (mCarrierLabel != NULL);
    // if (DEBUG) Slog.v(TAG, "carrierlabel=" + mCarrierLabel + " show=" + mShowCarrierInPanel);
    if (mShowCarrierInPanel) {
        mCarrierLabel->SetVisibility(mCarrierLabelVisible ? IView::VISIBLE : IView::INVISIBLE);

        // for mobile devices, we always show mobile connection info here (SPN/PLMN)
        // for other devices, we show whatever network is connected
        Boolean hasMobileDataFeature;
        mNetworkController->HasMobileDataFeature(&hasMobileDataFeature);
        if (hasMobileDataFeature) {
            mNetworkController->AddMobileLabelView(mCarrierLabel);
        }
        else {
            mNetworkController->AddCombinedLabelView(mCarrierLabel);
        }

        // set up the dynamic hide/show of the label
        mPile->SetOnSizeChangedListener(new OnSizeChangedListener() {
            @Override
            public void onSizeChanged(View view, Int32 w, Int32 h, Int32 oldw, Int32 oldh) {
                updateCarrierLabelVisibility(FALSE);
            }
        });
    }

    // Quick Settings (where available, some restrictions apply)
    if (mHasSettingsPanel) {
        // first, figure out where quick settings should be inflated
        View settings_stub;
        if (mHasFlipSettings) {
            // a version of quick settings that flips around behind the notifications
            settings_stub = mStatusBarWindow->FindViewById(R.id.flip_settings_stub);
            if (settings_stub != NULL) {
                mFlipSettingsView = ((ViewStub)settings_stub).inflate();
                mFlipSettingsView->SetVisibility(IView::GONE);
                mFlipSettingsView->SetVerticalScrollBarEnabled(FALSE);
            }
        } else {
            // full quick settings panel
            settings_stub = mStatusBarWindow->FindViewById(R.id.quick_settings_stub);
            if (settings_stub != NULL) {
                mSettingsPanel = (SettingsPanelView) ((ViewStub)settings_stub).inflate();
            } else {
                mSettingsPanel = (SettingsPanelView) mStatusBarWindow->FindViewById(R.id->Settings_panel);
            }

            if (mSettingsPanel != NULL) {
                if (!ActivityManager.isHighEndGfx()) {
                    mSettingsPanel->SetBackground(new FastColorDrawable(context.getResources().getColor(
                            R.color.notification_panel_solid_background)));
                }
            }
        }

        // wherever you find it, Quick Settings needs a container to survive
        mSettingsContainer = (QuickSettingsContainerView)
                mStatusBarWindow->FindViewById(R.id.quick_settings_container);
        if (mSettingsContainer != NULL) {
            mQS = new QuickSettings(mContext, mSettingsContainer);
            if (!mNotificationPanelIsFullScreenWidth) {
                mSettingsContainer->SetSystemUiVisibility(
                        IView::STATUS_BAR_DISABLE_NOTIFICATION_TICKER
                        | IView::STATUS_BAR_DISABLE_SYSTEM_INFO);
            }
            if (mSettingsPanel != NULL) {
                mSettingsPanel->SetQuickSettings(mQS);
            }
            mQS->SetService(this);
            mQS->SetBar(mStatusBarView);
            mQS->Setup(mNetworkController, mBluetoothController, mBatteryController,
                    mLocationController);
        } else {
            mQS = NULL; // fly away, be free
        }
    }

    mClingShown = ! (DEBUG_CLINGS
        || !Prefs.read(mContext)->GetBoolean(Prefs.SHOWN_QUICK_SETTINGS_HELP, FALSE));

    if (!ENABLE_NOTIFICATION_PANEL_CLING || ActivityManager.isRunningInTestHarness()) {
        mClingShown = TRUE;
    }

//        ImageView wimaxRSSI =
//                (ImageView)sb->FindViewById(R.id.wimax_signal);
//        if (wimaxRSSI != NULL) {
//            mNetworkController.addWimaxIconView(wimaxRSSI);
//        }

    // receive broadcasts
    IntentFilter filter = new IntentFilter();
    filter.addAction(IIntent::ACTION_CONFIGURATION_CHANGED);
    filter.addAction(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS);
    filter.addAction(IIntent::ACTION_SCREEN_OFF);
    filter.addAction(IIntent::ACTION_SCREEN_ON);
    filter.addAction(PhoneStatusBar::setRotShow);
    context.registerReceiver(mBroadcastReceiver, filter);

    // listen for USER_SETUP_COMPLETE setting (per-user)
    resetUserSetupObserver();

    rotsrc[0]=res.getDrawable(R.drawable.ic_sysbar_rotate0);
    rotsrc[1]=res.getDrawable(R.drawable.ic_sysbar_rotate90);
    rotsrc[2]=res.getDrawable(R.drawable.ic_sysbar_rotate180);
    rotsrc[3]=res.getDrawable(R.drawable.ic_sysbar_rotate270);
    return mStatusBarView;
}

//@Override
AutoPtr<IView> GetStatusBarView()
{
    return mStatusBarView;
}

//@Override
AutoPtr<IWindowManagerLayoutParams> GetRecentsLayoutParams(
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    Boolean opaque = FALSE;
    WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
            layoutParams.width,
            layoutParams.height,
            WindowManager.LayoutParams.TYPE_STATUS_BAR_PANEL,
            WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN
            | WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM
            | WindowManager.LayoutParams.FLAG_SPLIT_TOUCH,
            (opaque ? PixelFormat.OPAQUE : PixelFormat.TRANSLUCENT));
    if (ActivityManager.isHighEndGfx()) {
        lp.flags |= WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED;
    } else {
        lp.flags |= WindowManager.LayoutParams.FLAG_DIM_BEHIND;
        lp.dimAmount = 0.75f;
    }
    lp.gravity = IGravity::BOTTOM | IGravity::LEFT;
    lp->SetTitle("RecentsPanel");
    lp.windowAnimations = com.android.internal.R.style.Animation_RecentApplications;
    lp.softInputMode = WindowManager.LayoutParams.SOFT_INPUT_STATE_UNCHANGED
    | WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING;
    return lp;
}

//@Override
AutoPtr<IWindowManagerLayoutParams> GetSearchLayoutParams(
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    Boolean opaque = FALSE;
    WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
            LayoutParams.MATCH_PARENT,
            LayoutParams.MATCH_PARENT,
            WindowManager.LayoutParams.TYPE_NAVIGATION_BAR_PANEL,
            WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN
            | WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM
            | WindowManager.LayoutParams.FLAG_SPLIT_TOUCH,
            (opaque ? PixelFormat.OPAQUE : PixelFormat.TRANSLUCENT));
    if (ActivityManager.isHighEndGfx()) {
        lp.flags |= WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED;
    }
    lp.gravity = IGravity::BOTTOM | IGravity::LEFT;
    lp->SetTitle("SearchPanel");
    // TODO: Define custom animation for Search panel
    lp.windowAnimations = com.android.internal.R.style.Animation_RecentApplications;
    lp.softInputMode = WindowManager.LayoutParams.SOFT_INPUT_STATE_UNCHANGED
    | WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING;
    return lp;
}

//@Override
ECode PhoneStatusBar::ShowSearchPanel()
{
    super.showSearchPanel();
    mHandler.removeCallbacks(mShowSearchPanel);

    // we want to freeze the sysui state wherever it is
    mSearchPanelView->SetSystemUiVisibility(mSystemUiVisibility);

    WindowManager.LayoutParams lp =
        (android.view.WindowManager.LayoutParams) mNavigationBarView.getLayoutParams();
    lp.flags &= ~WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
    mWindowManager.updateViewLayout(mNavigationBarView, lp);
}

//@Override
ECode PhoneStatusBar::HideSearchPanel()
{
    super.hideSearchPanel();
    WindowManager.LayoutParams lp =
        (android.view.WindowManager.LayoutParams) mNavigationBarView.getLayoutParams();
    lp.flags |= WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
    mWindowManager.updateViewLayout(mNavigationBarView, lp);
}

//@Override
ECode PhoneStatusBar::UpdateSearchPanel()
{
    super.updateSearchPanel();
    mSearchPanelView->SetStatusBarView(mNavigationBarView);
    mNavigationBarView->SetDelegateView(mSearchPanelView);
}

Int32 PhoneStatusBar::GetStatusBarGravity()
{
    return IGravity::TOP | IGravity::FILL_HORIZONTAL;
}

Int32 GetStatusBarHeight()
{
    if (mNaturalBarHeight < 0) {
        Resources res = mContext.getResources();
        mNaturalBarHeight =
                res.getDimensionPixelSize(R::dimen::status_bar_height);
    }
    return mNaturalBarHeight;
}

void PhoneStatusBar::SendRotBroadcast()
{
    Intent intent=new Intent(setRotVal);
    intent.putExtra(setRotStr, irot);
    mContext.sendBroadcast(intent);
    Log.d("rot","sendbroadcast  PhoneStatusBar");
}

void PhoneStatusBar::AwakenDreams()
{
    if (mDreamManager != NULL) {
        try {
            mDreamManager.awaken();
        } catch (RemoteException e) {
            // fine, stay asleep then
        }
    }
}

void PhoneStatusBar::PrepareNavigationBarView()
{
    mNavigationBarView.reorient();

    mNavigationBarView.getRecentsButton()->SetOnClickListener(mRecentsClickListener);
    mNavigationBarView.getRecentsButton()->SetOnTouchListener(mRecentsPreloadOnTouchListener);
    mNavigationBarView.getRotationSettingButton()->SetOnClickListener(mRotSettingClickListener);
    mNavigationBarView.getHomeButton()->SetOnTouchListener(mHomeSearchActionListener);
    mNavigationBarView.getSearchLight()->SetOnTouchListener(mHomeSearchActionListener);
    updateSearchPanel();
}

// For small-screen devices (read: phones) that lack hardware navigation buttons
void PhoneStatusBar::AddNavigationBar()
{
    if (DEBUG) Slog.v(TAG, "addNavigationBar: about to add " + mNavigationBarView);
    if (mNavigationBarView == NULL) return;

    prepareNavigationBarView();

    mWindowManager.addView(mNavigationBarView, getNavigationBarLayoutParams());
}

void PhoneStatusBar::RepositionNavigationBar()
{
    if (mNavigationBarView == NULL) return;

    prepareNavigationBarView();

    mWindowManager.updateViewLayout(mNavigationBarView, getNavigationBarLayoutParams());
}

void PhoneStatusBar::NotifyNavigationBarScreenOn(
    /* [in] */ Boolean screenOn)
{
    if (mNavigationBarView == NULL) return;
    mNavigationBarView.notifyScreenOn(screenOn);
}

AutoPtr<IWindowManagerLayoutParams> GetNavigationBarLayoutParams()
{
    WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
            LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT,
            WindowManager.LayoutParams.TYPE_NAVIGATION_BAR,
                0
                | WindowManager.LayoutParams.FLAG_TOUCHABLE_WHEN_WAKING
                | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                | WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                | WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH
                | WindowManager.LayoutParams.FLAG_SPLIT_TOUCH,
            PixelFormat.OPAQUE);
    // this will allow the navbar to run in an overlay on devices that support this
    if (ActivityManager.isHighEndGfx()) {
        lp.flags |= WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED;
    }

    lp->SetTitle("NavigationBar");
    lp.windowAnimations = 0;
    return lp;
}

void PhoneStatusBar::AddIntruderView()
{
    WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.WRAP_CONTENT,
            WindowManager.LayoutParams.TYPE_STATUS_BAR_PANEL, // above the status bar!
            WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN
                | WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS
                | WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                | WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM
                | WindowManager.LayoutParams.FLAG_SPLIT_TOUCH,
            PixelFormat.TRANSLUCENT);
    lp.gravity = IGravity::TOP | IGravity::FILL_HORIZONTAL;
    //lp.y += height * 1.5; // FIXME
    lp->SetTitle("IntruderAlert");
    lp.packageName = mContext.getPackageName();
    lp.windowAnimations = R.style.Animation_StatusBar_IntruderAlert;

    mWindowManager.addView(mIntruderAlertView, lp);
}

// CommandQueueCallbacks interfaces
//
ECode PhoneStatusBar::AddIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* icon)
{
    if (SPEW) Slog.d(TAG, "addIcon slot=" + slot + " index=" + index + " viewIndex=" + viewIndex
            + " icon=" + icon);
    StatusBarIconView view = new StatusBarIconView(mContext, slot, NULL);
    view->Set(icon);
    mStatusIcons.addView(view, viewIndex, new LinearLayout.LayoutParams(mIconSize, mIconSize));
}

ECode PhoneStatusBar::UpdateIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex,
    /* [in] */ IStatusBarIcon* old,
    /* [in] */ IStatusBarIcon* icon)
{
    if (SPEW) Slog.d(TAG, "updateIcon slot=" + slot + " index=" + index + " viewIndex=" + viewIndex
            + " old=" + old + " icon=" + icon);
    StatusBarIconView view = (StatusBarIconView)mStatusIcons.getChildAt(viewIndex);
    view->Set(icon);
}

ECode PhoneStatusBar::RemoveIcon(
    /* [in] */ const String& slot,
    /* [in] */ Int32 index,
    /* [in] */ Int32 viewIndex)
{
    if (SPEW) Slog.d(TAG, "removeIcon slot=" + slot + " index=" + index + " viewIndex=" + viewIndex);
    mStatusIcons.removeViewAt(viewIndex);
}

ECode PhoneStatusBar::AddNotification(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    if (DEBUG) Slog.d(TAG, "addNotification score=" + notification.score);
    StatusBarIconView iconView = addNotificationViews(key, notification);
    if (iconView == NULL) return;

    Boolean immersive = FALSE;
    try {
        immersive = ActivityManagerNative.getDefault().isTopActivityImmersive();
        if (DEBUG) {
            Slog.d(TAG, "Top activity is " + (immersive?"immersive":"not immersive"));
        }
    } catch (RemoteException ex) {
    }

    /*
     * DISABLED due to missing API
    if (ENABLE_INTRUDERS && (
               // TODO(dsandler): Only if the screen is on
            notification.notification.intruderView != NULL)) {
        Slog.d(TAG, "Presenting high-priority notification");
        // special new transient ticker mode
        // 1. Populate mIntruderAlertView

        if (notification.notification.intruderView == NULL) {
            Slog.e(TAG, notification.notification.toString() + " wanted to intrude but intruderView was NULL");
            return;
        }

        // bind the click event to the content area
        PendingIntent contentIntent = notification.notification.contentIntent;
        View.OnClickListener listener = (contentIntent != NULL)
                ? new NotificationClicker(contentIntent,
                        notification.pkg, notification.tag, notification.id)
                : NULL;

        mIntruderAlertView.applyIntruderContent(notification.notification.intruderView, listener);

        mCurrentlyIntrudingNotification = notification;

        // 2. Animate mIntruderAlertView in
        mHandler.sendEmptyMessage(MSG_SHOW_INTRUDER);

        // 3. Set alarm to age the notification off (TODO)
        mHandler.removeMessages(MSG_HIDE_INTRUDER);
        if (INTRUDER_ALERT_DECAY_MS > 0) {
            mHandler.sendEmptyMessageDelayed(MSG_HIDE_INTRUDER, INTRUDER_ALERT_DECAY_MS);
        }
    } else
     */

    if (notification.notification.fullScreenIntent != NULL) {
        // Stop screensaver if the notification has a full-screen intent.
        // (like an incoming phone call)
        awakenDreams();

        // not immersive & a full-screen alert should be shown
        if (DEBUG) Slog.d(TAG, "Notification has fullScreenIntent; sending fullScreenIntent");
        try {
            notification.notification.fullScreenIntent.send();
        } catch (PendingIntent.CanceledException e) {
        }
    } else {
        // usual case: status bar visible & not immersive

        // show the ticker if there isn't an intruder too
        if (mCurrentlyIntrudingNotification == NULL) {
            tick(NULL, notification, TRUE);
        }
    }

    // Recalculate the position of the sliding windows and the titles.
    setAreThereNotifications();
    updateExpandedViewPos(EXPANDED_LEAVE_ALONE);
}

ECode PhoneStatusBar::RemoveNotification(
    /* [in] */ IBinder* key)
{
    StatusBarNotification old = removeNotificationViews(key);
    if (SPEW) Slog.d(TAG, "removeNotification key=" + key + " old=" + old);

    if (old != NULL) {
        // Cancel the ticker if it's still running
        mTicker.removeEntry(old);

        // Recalculate the position of the sliding windows and the titles.
        updateExpandedViewPos(EXPANDED_LEAVE_ALONE);

        if (ENABLE_INTRUDERS && old == mCurrentlyIntrudingNotification) {
            mHandler.sendEmptyMessage(MSG_HIDE_INTRUDER);
        }

        if (CLOSE_PANEL_WHEN_EMPTIED && mNotificationData.size() == 0 && !mAnimating) {
            animateCollapsePanels();
        }
    }

    setAreThereNotifications();
}


void PhoneStatusBar::UpdateShowSearchHoldoff()
{
    mShowSearchHoldoff = mContext.getResources().getInteger(
        R.integer.config_show_search_delay);
}

void PhoneStatusBar::LoadNotificationShade()
{
    if (mPile == NULL) return;

    Int32 N = mNotificationData.size();

    ArrayList<View> toShow = new ArrayList<View>();

    Boolean provisioned = isDeviceProvisioned();
    // If the device hasn't been through Setup, we only show system notifications
    for (Int32 i=0; i<N; i++) {
        Entry ent = mNotificationData.get(N-i-1);
        if (!(provisioned || showNotificationEvenIfUnprovisioned(ent.notification))) continue;
        if (!notificationIsForCurrentUser(ent.notification)) continue;
        toShow.add(ent.row);
    }

    ArrayList<View> toRemove = new ArrayList<View>();
    for (Int32 i=0; i<mPile.getChildCount(); i++) {
        View child = mPile.getChildAt(i);
        if (!toShow.contains(child)) {
            toRemove.add(child);
        }
    }

    for (View remove : toRemove) {
        mPile.removeView(remove);
    }

    for (Int32 i=0; i<toShow.size(); i++) {
        View v = toShow.get(i);
        if (v.getParent() == NULL) {
            mPile.addView(v, i);
        }
    }

    if (mSettingsButton != NULL) {
        mSettingsButton->SetEnabled(isDeviceProvisioned());
    }
}

//@Override
ECode PhoneStatusBar::UpdateNotificationIcons()
{
    if (mNotificationIcons == NULL) return;

    loadNotificationShade();

    LinearLayout.LayoutParams params
        = new LinearLayout.LayoutParams(mIconSize + 2*mIconHPadding, mNaturalBarHeight);

    Int32 N = mNotificationData.size();

    if (DEBUG) {
        Slog.d(TAG, "refreshing icons: " + N + " notifications, mNotificationIcons=" + mNotificationIcons);
    }

    ArrayList<View> toShow = new ArrayList<View>();

    Boolean provisioned = isDeviceProvisioned();
    // If the device hasn't been through Setup, we only show system notifications
    for (Int32 i=0; i<N; i++) {
        Entry ent = mNotificationData.get(N-i-1);
        if (!((provisioned && ent.notification.score >= HIDE_ICONS_BELOW_SCORE)
                || showNotificationEvenIfUnprovisioned(ent.notification))) continue;
        if (!notificationIsForCurrentUser(ent.notification)) continue;
        toShow.add(ent.icon);
    }

    ArrayList<View> toRemove = new ArrayList<View>();
    for (Int32 i=0; i<mNotificationIcons.getChildCount(); i++) {
        View child = mNotificationIcons.getChildAt(i);
        if (!toShow.contains(child)) {
            toRemove.add(child);
        }
    }

    for (View remove : toRemove) {
        mNotificationIcons.removeView(remove);
    }

    for (Int32 i=0; i<toShow.size(); i++) {
        View v = toShow.get(i);
        if (v.getParent() == NULL) {
            mNotificationIcons.addView(v, i, params);
        }
    }
}

void PhoneStatusBar::UpdateCarrierLabelVisibility(Boolean force)
{
    if (!mShowCarrierInPanel) return;
    // The idea here is to only show the carrier label when there is enough room to see it,
    // i.e. when there aren't enough notifications to fill the panel.
    if (DEBUG) {
        Slog.d(TAG, String.format("pileh=%d scrollh=%d carrierh=%d",
                mPile.getHeight(), mScrollView.getHeight(), mCarrierLabelHeight));
    }

    Boolean emergencyCallsShownElsewhere = mEmergencyCallLabel != NULL;
    Boolean makeVisible =
        !(emergencyCallsShownElsewhere && mNetworkController.isEmergencyOnly())
        && mPile.getHeight() < (mNotificationPanel.getHeight() - mCarrierLabelHeight - mNotificationHeaderHeight)
        && mScrollView.getVisibility() == IView::VISIBLE;

    if (force || mCarrierLabelVisible != makeVisible) {
        mCarrierLabelVisible = makeVisible;
        if (DEBUG) {
            Slog.d(TAG, "making carrier label " + (makeVisible?"visible":"invisible"));
        }
        mCarrierLabel.animate().cancel();
        if (makeVisible) {
            mCarrierLabel->SetVisibility(IView::VISIBLE);
        }
        mCarrierLabel.animate()
            .alpha(makeVisible ? 1.0f : 0.0f)
            //->SetStartDelay(makeVisible ? 500 : 0)
            //->SetDuration(makeVisible ? 750 : 100)
            ->SetDuration(150)
            ->SetListener(makeVisible ? NULL : new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    if (!mCarrierLabelVisible) { // race
                        mCarrierLabel->SetVisibility(IView::INVISIBLE);
                        mCarrierLabel->SetAlpha(0.0f);
                    }
                }
            })
            ->Start();
    }
}

//@Override
ECode PhoneStatusBar::SetAreThereNotifications()
{
    Boolean any = mNotificationData.size() > 0;

    Boolean clearable = any && mNotificationData.hasClearableItems();

    if (DEBUG) {
        Slog.d(TAG, "setAreThereNotifications: N=" + mNotificationData.size()
                + " any=" + any + " clearable=" + clearable);
    }

    if (mHasFlipSettings
            && mFlipSettingsView != NULL
            && mFlipSettingsView.getVisibility() == IView::VISIBLE
            && mScrollView.getVisibility() != IView::VISIBLE) {
        // the flip settings panel is unequivocally showing; we should not be shown
        mClearButton->SetVisibility(IView::INVISIBLE);
    } else if (mClearButton.isShown()) {
        if (clearable != (mClearButton.getAlpha() == 1.0.0f)) {
            ObjectAnimator clearAnimation = ObjectAnimator.ofFloat(
                    mClearButton, "alpha", clearable ? 1.0.0f : 0.0.0f)->SetDuration(250);
            clearAnimation.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    if (mClearButton.getAlpha() <= 0.0.0f) {
                        mClearButton->SetVisibility(IView::INVISIBLE);
                    }
                }

                @Override
                public void onAnimationStart(Animator animation) {
                    if (mClearButton.getAlpha() <= 0.0.0f) {
                        mClearButton->SetVisibility(IView::VISIBLE);
                    }
                }
            });
            clearAnimation->Start();
        }
    } else {
        mClearButton->SetAlpha(clearable ? 1.0.0f : 0.0.0f);
        mClearButton->SetVisibility(clearable ? IView::VISIBLE : IView::INVISIBLE);
    }
    mClearButton->SetEnabled(clearable);

    View nlo = mStatusBarView->FindViewById(R.id.notification_lights_out);
    Boolean showDot = (any&&!areLightsOn());
    if (showDot != (nlo.getAlpha() == 1.0.0f)) {
        if (showDot) {
            nlo->SetAlpha(0.0f);
            nlo->SetVisibility(IView::VISIBLE);
        }
        nlo.animate()
            .alpha(showDot?1:0)
            ->SetDuration(showDot?750:250)
            ->SetInterpolator(new AccelerateInterpolator(2.0.0f))
            ->SetListener(showDot ? NULL : new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator _a) {
                    nlo->SetVisibility(IView::GONE);
                }
            })
            ->Start();
    }

    updateCarrierLabelVisibility(FALSE);
}

ECode PhoneStatusBar::ShowClock(
    /* [in] */ Boolean show)
{
    if (mStatusBarView == NULL) return;
    View clock = mStatusBarView->FindViewById(R.id.clock);
    if (clock != NULL) {
        clock->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    }
}

/**
 * State is one or more of the DISABLE constants from StatusBarManager.
 */
ECode PhoneStatusBar::Disable(
    /* [in] */ Int32 state)
{
    Int32 old = mDisabled;
    Int32 diff = state ^ old;
    mDisabled = state;

    if (DEBUG) {
        Slog.d(TAG, String.format("disable: 0x%08x -> 0x%08x (diff: 0x%08x)",
            old, state, diff));
    }

    StringBuilder flagdbg = new StringBuilder();
    flagdbg.append("disable: < ");
    flagdbg.append(((state & StatusBarManager.DISABLE_EXPAND) != 0) ? "EXPAND" : "expand");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_EXPAND) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_NOTIFICATION_ICONS) != 0) ? "ICONS" : "icons");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_NOTIFICATION_ICONS) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_NOTIFICATION_ALERTS) != 0) ? "ALERTS" : "alerts");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_NOTIFICATION_ALERTS) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_NOTIFICATION_TICKER) != 0) ? "TICKER" : "ticker");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_NOTIFICATION_TICKER) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_SYSTEM_INFO) != 0) ? "SYSTEM_INFO" : "system_info");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_SYSTEM_INFO) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_BACK) != 0) ? "BACK" : "back");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_BACK) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_HOME) != 0) ? "HOME" : "home");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_HOME) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_RECENT) != 0) ? "RECENT" : "recent");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_RECENT) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_CLOCK) != 0) ? "CLOCK" : "clock");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_CLOCK) != 0) ? "* " : " ");
    flagdbg.append(((state & StatusBarManager.DISABLE_SEARCH) != 0) ? "SEARCH" : "search");
    flagdbg.append(((diff  & StatusBarManager.DISABLE_SEARCH) != 0) ? "* " : " ");
    flagdbg.append(">");
    Slog.d(TAG, flagdbg.toString());

    if ((diff & StatusBarManager.DISABLE_SYSTEM_INFO) != 0) {
        mSystemIconArea.animate().cancel();
        if ((state & StatusBarManager.DISABLE_SYSTEM_INFO) != 0) {
            mSystemIconArea.animate()
                .alpha(0.0f)
                .translationY(mNaturalBarHeight*0.5f)
                ->SetDuration(175)
                ->SetInterpolator(new DecelerateInterpolator(1.5f))
                ->SetListener(mMakeIconsInvisible)
                ->Start();
        } else {
            mSystemIconArea->SetVisibility(IView::VISIBLE);
            mSystemIconArea.animate()
                .alpha(1.0f)
                .translationY(0)
                ->SetStartDelay(0)
                ->SetInterpolator(new DecelerateInterpolator(1.5f))
                ->SetDuration(175)
                ->Start();
        }
    }

    if ((diff & StatusBarManager.DISABLE_CLOCK) != 0) {
        Boolean show = (state & StatusBarManager.DISABLE_CLOCK) == 0;
        showClock(show);
    }
    if ((diff & StatusBarManager.DISABLE_EXPAND) != 0) {
        if ((state & StatusBarManager.DISABLE_EXPAND) != 0) {
            animateCollapsePanels();
        }
    }

    if ((diff & (StatusBarManager.DISABLE_HOME
                    | StatusBarManager.DISABLE_RECENT
                    | StatusBarManager.DISABLE_BACK
                    | StatusBarManager.DISABLE_SEARCH)) != 0) {
        // the nav bar will take care of these
        if (mNavigationBarView != NULL) mNavigationBarView->SetDisabledFlags(state);

        if ((state & StatusBarManager.DISABLE_RECENT) != 0) {
            // close recents if it's visible
            mHandler.removeMessages(MSG_CLOSE_RECENTS_PANEL);
            mHandler.sendEmptyMessage(MSG_CLOSE_RECENTS_PANEL);
        }
    }

    if ((diff & StatusBarManager.DISABLE_NOTIFICATION_ICONS) != 0) {
        if ((state & StatusBarManager.DISABLE_NOTIFICATION_ICONS) != 0) {
            if (mTicking) {
                haltTicker();
            }

            mNotificationIcons.animate()
                .alpha(0.0f)
                .translationY(mNaturalBarHeight*0.5f)
                ->SetDuration(175)
                ->SetInterpolator(new DecelerateInterpolator(1.5f))
                ->SetListener(mMakeIconsInvisible)
                ->Start();
        } else {
            mNotificationIcons->SetVisibility(IView::VISIBLE);
            mNotificationIcons.animate()
                .alpha(1.0f)
                .translationY(0)
                ->SetStartDelay(0)
                ->SetInterpolator(new DecelerateInterpolator(1.5f))
                ->SetDuration(175)
                ->Start();
        }
    } else if ((diff & StatusBarManager.DISABLE_NOTIFICATION_TICKER) != 0) {
        if (mTicking && (state & StatusBarManager.DISABLE_NOTIFICATION_TICKER) != 0) {
            haltTicker();
        }
    }
}

AutoPtr<IHandler> GetHandler()
{
    return mHandler;
}

void PhoneStatusBar::MakeExpandedVisible(
    /* [in] */ Boolean revealAfterDraw)
{
    if (SPEW) Slog.d(TAG, "Make expanded visible: expanded visible=" + mExpandedVisible);
    if (mExpandedVisible) {
        return;
    }

    mExpandedVisible = TRUE;
    mPile->SetLayoutTransitionsEnabled(TRUE);
    if (mNavigationBarView != NULL)
        mNavigationBarView->SetSlippery(TRUE);

    updateCarrierLabelVisibility(TRUE);

    updateExpandedViewPos(EXPANDED_LEAVE_ALONE);

    // Expand the window to encompass the full screen in anticipation of the drag.
    // This is only possible to do atomically because the status bar is at the top of the screen!
    WindowManager.LayoutParams lp = (WindowManager.LayoutParams) mStatusBarWindow.getLayoutParams();
    lp.flags &= ~WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
    lp.flags |= WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM;
    lp.height = ViewGroup.LayoutParams.MATCH_PARENT;
    mWindowManager.updateViewLayout(mStatusBarWindow, lp);

    // Updating the window layout will force an expensive traversal/redraw.
    // Kick off the reveal animation after this is complete to avoid animation latency.
    if (revealAfterDraw) {
//            mHandler.post(mStartRevealAnimation);
    }

    visibilityChanged(TRUE);
}

ECode PhoneStatusBar::AnimateExpandNotificationsPanel()
{

}

ECode PhoneStatusBar::AnimateCollapsePanels()
{
    animateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
}

ECode PhoneStatusBar::AnimateCollapsePanels(
    /* [in] */ Int32 flags)
{
    if (SPEW) {
        Slog.d(TAG, "animateCollapse():"
                + " mExpandedVisible=" + mExpandedVisible
                + " mAnimating=" + mAnimating
                + " mAnimatingReveal=" + mAnimatingReveal
                + " mAnimY=" + mAnimY
                + " mAnimVel=" + mAnimVel
                + " flags=" + flags);
    }

    if ((flags & ICommandQueue::FLAG_EXCLUDE_RECENTS_PANEL) == 0) {
        mHandler.removeMessages(MSG_CLOSE_RECENTS_PANEL);
        mHandler.sendEmptyMessage(MSG_CLOSE_RECENTS_PANEL);
    }

    if ((flags & ICommandQueue::FLAG_EXCLUDE_SEARCH_PANEL) == 0) {
        mHandler.removeMessages(MSG_CLOSE_SEARCH_PANEL);
        mHandler.sendEmptyMessage(MSG_CLOSE_SEARCH_PANEL);
    }

    mStatusBarWindow.cancelExpandHelper();
    mStatusBarView.collapseAllPanels(TRUE);
}


AutoPtr<IViewPropertyAnimator> SetVisibilityWhenDone()
{
    a->SetListener(new AnimatorListenerAdapter() {
        @Override
        public void onAnimationEnd(Animator animation) {
            v->SetVisibility(vis);
            a->SetListener(NULL); // oneshot
        }
    });
    return a;
}

AutoPtr<IAnimator> SetVisibilityWhenDone()
{
    a.addListener(new AnimatorListenerAdapter() {
        @Override
        public void onAnimationEnd(Animator animation) {
            v->SetVisibility(vis);
        }
    });
    return a;
}

AutoPtr<IAnimator> Interpolator(
    /* [in] */ ITimeInterpolator* ti,
    /* [in] */ IAnimator* a)
{
    // a->SetInterpolator(ti);
    // return a;
}

AutoPtr<IAnimator> StartDelay(
    /* [in] */ Int32 d,
    /* [in] */ IAnimator* a)
{
    // a->SetStartDelay(d);
    // return a;
}

AutoPtr<IAnimator> Start(
    /* [in] */ IAnimator* a)
{
    // a->Start();
    // return a;
}

ECode PhoneStatusBar::animateExpandNotificationsPanel()
{
    if (SPEW) Slog.d(TAG, "animateExpand: mExpandedVisible=" + mExpandedVisible);
    if ((mDisabled & StatusBarManager.DISABLE_EXPAND) != 0) {
        return ;
    }

    mNotificationPanel.expand();
    if (mHasFlipSettings && mScrollView.getVisibility() != IView::VISIBLE) {
        flipToNotifications();
    }

    if (FALSE) postStartTracing();
}

ECode PhoneStatusBar::FlipToNotifications()
{
    if (mFlipSettingsViewAnim != NULL) mFlipSettingsViewAnim.cancel();
    if (mScrollViewAnim != NULL) mScrollViewAnim.cancel();
    if (mSettingsButtonAnim != NULL) mSettingsButtonAnim.cancel();
    if (mNotificationButtonAnim != NULL) mNotificationButtonAnim.cancel();
    if (mClearButtonAnim != NULL) mClearButtonAnim.cancel();

    mScrollView->SetVisibility(IView::VISIBLE);
    mScrollViewAnim = start(
        startDelay(FLIP_DURATION_OUT,
            interpolator(mDecelerateInterpolator,
                ObjectAnimator.ofFloat(mScrollView, View.SCALE_X, 0.0f, 1.0f)
                    ->SetDuration(FLIP_DURATION_IN)
                )));
    mFlipSettingsViewAnim = start(
        setVisibilityWhenDone(
            interpolator(mAccelerateInterpolator,
                    ObjectAnimator.ofFloat(mFlipSettingsView, View.SCALE_X, 1.0f, 0.0f)
                    )
                ->SetDuration(FLIP_DURATION_OUT),
            mFlipSettingsView, IView::INVISIBLE));
    mNotificationButtonAnim = start(
        setVisibilityWhenDone(
            ObjectAnimator.ofFloat(mNotificationButton, View.ALPHA, 0.0f)
                ->SetDuration(FLIP_DURATION),
            mNotificationButton, IView::INVISIBLE));
    mSettingsButton->SetVisibility(IView::VISIBLE);
    mSettingsButtonAnim = start(
        ObjectAnimator.ofFloat(mSettingsButton, View.ALPHA, 1.0f)
            ->SetDuration(FLIP_DURATION));
    mClearButton->SetVisibility(IView::VISIBLE);
    mClearButton->SetAlpha(0.0f);
    setAreThereNotifications(); // this will show/hide the button as necessary
    mNotificationPanel.postDelayed(new Runnable() {
        public void run() {
            updateCarrierLabelVisibility(FALSE);
        }
    }, FLIP_DURATION - 150);
}

ECode PhoneStatusBar::AnimateExpandSettingsPanel()
{
    if (SPEW) Slog.d(TAG, "animateExpand: mExpandedVisible=" + mExpandedVisible);
    if ((mDisabled & StatusBarManager.DISABLE_EXPAND) != 0) {
        return;
    }

    if (mHasFlipSettings) {
        mNotificationPanel.expand();
        if (mFlipSettingsView.getVisibility() != IView::VISIBLE) {
            flipToSettings();
        }
    } else if (mSettingsPanel != NULL) {
        mSettingsPanel.expand();
    }

    if (FALSE) postStartTracing();
}


ECode PhoneStatusBar::SwitchToSettings()
{
    mFlipSettingsView->SetScaleX(1.0f);
    mFlipSettingsView->SetVisibility(IView::VISIBLE);
    mSettingsButton->SetVisibility(IView::GONE);
    mScrollView->SetVisibility(IView::GONE);
    mScrollView->SetScaleX(0.0f);
    mNotificationButton->SetVisibility(IView::VISIBLE);
    mNotificationButton->SetAlpha(1.0f);
    mClearButton->SetVisibility(IView::GONE);
}

ECode PhoneStatusBar::FlipToSettings()
{
    if (mFlipSettingsViewAnim != NULL) mFlipSettingsViewAnim.cancel();
    if (mScrollViewAnim != NULL) mScrollViewAnim.cancel();
    if (mSettingsButtonAnim != NULL) mSettingsButtonAnim.cancel();
    if (mNotificationButtonAnim != NULL) mNotificationButtonAnim.cancel();
    if (mClearButtonAnim != NULL) mClearButtonAnim.cancel();

    mFlipSettingsView->SetVisibility(IView::VISIBLE);
    mFlipSettingsView->SetScaleX(0.0f);
    mFlipSettingsViewAnim = start(
        startDelay(FLIP_DURATION_OUT,
            interpolator(mDecelerateInterpolator,
                ObjectAnimator.ofFloat(mFlipSettingsView, View.SCALE_X, 0.0f, 1.0f)
                    ->SetDuration(FLIP_DURATION_IN)
                )));
    mScrollViewAnim = start(
        setVisibilityWhenDone(
            interpolator(mAccelerateInterpolator,
                    ObjectAnimator.ofFloat(mScrollView, View.SCALE_X, 1.0f, 0.0f)
                    )
                ->SetDuration(FLIP_DURATION_OUT),
            mScrollView, IView::INVISIBLE));
    mSettingsButtonAnim = start(
        setVisibilityWhenDone(
            ObjectAnimator.ofFloat(mSettingsButton, View.ALPHA, 0.0f)
                ->SetDuration(FLIP_DURATION),
                mScrollView, IView::INVISIBLE));
    mNotificationButton->SetVisibility(IView::VISIBLE);
    mNotificationButtonAnim = start(
        ObjectAnimator.ofFloat(mNotificationButton, View.ALPHA, 1.0f)
            ->SetDuration(FLIP_DURATION));
    mClearButtonAnim = start(
        setVisibilityWhenDone(
            ObjectAnimator.ofFloat(mClearButton, View.ALPHA, 0.0f)
            ->SetDuration(FLIP_DURATION),
            mClearButton, IView::INVISIBLE));
    mNotificationPanel.postDelayed(new Runnable() {
        public void run() {
            updateCarrierLabelVisibility(FALSE);
        }
    }, FLIP_DURATION - 150);
}

ECode PhoneStatusBar::FlipPanels()
{
    if (mHasFlipSettings) {
        if (mFlipSettingsView.getVisibility() != IView::VISIBLE) {
            flipToSettings();
        } else {
            flipToNotifications();
        }
    }
}

ECode PhoneStatusBar::AnimateCollapseQuickSettings()
{
    mStatusBarView.collapseAllPanels(TRUE);
}

void PhoneStatusBar::MakeExpandedInvisibleSoon()
{
    mHandler.postDelayed(new Runnable() { public void run() { makeExpandedInvisible(); }}, 50);
}

void PhoneStatusBar::MakeExpandedInvisible()
{
    if (SPEW) Slog.d(TAG, "makeExpandedInvisible: mExpandedVisible=" + mExpandedVisible
            + " mExpandedVisible=" + mExpandedVisible);

    if (!mExpandedVisible) {
        return;
    }

    // Ensure the panel is fully collapsed (just in case; bug 6765842, 7260868)
    mStatusBarView.collapseAllPanels(/*animate=*/ FALSE);

    if (mHasFlipSettings) {
        // reset things to their proper state
        if (mFlipSettingsViewAnim != NULL) mFlipSettingsViewAnim.cancel();
        if (mScrollViewAnim != NULL) mScrollViewAnim.cancel();
        if (mSettingsButtonAnim != NULL) mSettingsButtonAnim.cancel();
        if (mNotificationButtonAnim != NULL) mNotificationButtonAnim.cancel();
        if (mClearButtonAnim != NULL) mClearButtonAnim.cancel();

        mScrollView->SetScaleX(1.0f);
        mScrollView->SetVisibility(IView::VISIBLE);
        mSettingsButton->SetAlpha(1.0f);
        mSettingsButton->SetVisibility(IView::VISIBLE);
        mNotificationPanel->SetVisibility(IView::GONE);
        mFlipSettingsView->SetVisibility(IView::GONE);
        mNotificationButton->SetVisibility(IView::GONE);
        setAreThereNotifications(); // show the clear button
    }

    mExpandedVisible = FALSE;
    mPile->SetLayoutTransitionsEnabled(FALSE);
    if (mNavigationBarView != NULL)
        mNavigationBarView->SetSlippery(FALSE);
    visibilityChanged(FALSE);

    // Shrink the window to the size of the status bar only
    WindowManager.LayoutParams lp = (WindowManager.LayoutParams) mStatusBarWindow.getLayoutParams();
    lp.height = getStatusBarHeight();
    lp.flags |= WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
    lp.flags &= ~WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM;
    mWindowManager.updateViewLayout(mStatusBarWindow, lp);

    if ((mDisabled & StatusBarManager.DISABLE_NOTIFICATION_ICONS) == 0) {
        setNotificationIconVisibility(TRUE, R::anim::fade_in);
    }

    // Close any "App info" popups that might have snuck on-screen
    dismissPopups();

    if (mPostCollapseCleanup != NULL) {
        mPostCollapseCleanup->Run();
        mPostCollapseCleanup = NULL;
    }
}

void PhoneStatusBar::SetPileLayers(
    /* [in] */ Int32 layerType)
{
    Int32 count = mPile.getChildCount();

    switch (layerType) {
        case View.LAYER_TYPE_NONE:
            for (Int32 i = 0; i < count; i++) {
                mPile.getChildAt(i)->SetLayerType(layerType, NULL);
            }
            break;
        case View.LAYER_TYPE_HARDWARE:
            Int32[] location = new Int32[2];
            mNotificationPanel.getLocationInWindow(location);

            Int32 left = location[0];
            Int32 top = location[1];
            Int32 right = left + mNotificationPanel.getWidth();
            Int32 bottom = top + getExpandedViewMaxHeight();

            Rect childBounds = new Rect();

            for (Int32 i = 0; i < count; i++) {
                View view = mPile.getChildAt(i);
                view.getLocationInWindow(location);

                childBounds->Set(location[0], location[1],
                        location[0] + view.getWidth(), location[1] + view.getHeight());

                if (childBounds.intersects(left, top, right, bottom)) {
                    view->SetLayerType(layerType, NULL);
                }
            }

            break;
    }
}


Boolean IsClinging()
{
    return mCling != NULL && mCling.getVisibility() == IView::VISIBLE;
}

ECode PhoneStatusBar::HideCling()
{
    if (isClinging()) {
        mCling.animate().alpha(0.0f)->SetDuration(250)->Start();
        mCling->SetVisibility(IView::GONE);
        mSuppressStatusBarDrags = FALSE;
    }
}

ECode PhoneStatusBar::ShowCling()
{
    // lazily inflate this to accommodate orientation change
    ViewStub stub = (ViewStub) mStatusBarWindow->FindViewById(R.id.status_bar_cling_stub);
    if (stub == NULL) {
        mClingShown = TRUE;
        return; // no clings on this device
    }

    mSuppressStatusBarDrags = TRUE;

    mHandler.postDelayed(new Runnable() {
        @Override
        public void run() {
            mCling = (ViewGroup) stub.inflate();

            mCling->SetOnTouchListener(new View.OnTouchListener() {
                @Override
                public Boolean onTouch(View v, MotionEvent event) {
                    return TRUE; // e eats everything
                }});
            mCling->FindViewById(R.id.ok)->SetOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    hideCling();
                }});

            mCling->SetAlpha(0.0f);
            mCling->SetVisibility(IView::VISIBLE);
            mCling.animate().alpha(1.0f);

            mClingShown = TRUE;
            SharedPreferences.Editor editor = Prefs.edit(mContext);
            editor.putBoolean(Prefs.SHOWN_QUICK_SETTINGS_HELP, TRUE);
            editor.apply();

            makeExpandedVisible(TRUE); // enforce visibility in case the shade is still animating closed
            animateExpandNotificationsPanel();

            mSuppressStatusBarDrags = FALSE;
        }
    }, 500);

    animateExpandNotificationsPanel();
}

Boolean PhoneStatusBar::InterceptTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    if (SPEW) {
        Slog.d(TAG, "Touch: rawY=" + event.getRawY() + " event=" + event + " mDisabled="
            + mDisabled + " mTracking=" + mTracking);
    } else if (CHATTY) {
        if (event.getAction() != IMotionEvent::ACTION_MOVE) {
            Slog.d(TAG, String.format(
                        "panel: %s at (%f, %f) mDisabled=0x%08x",
                        MotionEvent.actionToString(event.getAction()),
                        event.getRawX(), event.getRawY(), mDisabled));
        }
    }

    if (DEBUG_GESTURES) {
        mGestureRec.add(event);
    }

    // Cling (first-run help) handling.
    // The cling is supposed to show the first time you drag, or even tap, the status bar.
    // It should show the notification panel, then fade in after half a second, giving you
    // an explanation of what just happened, as well as teach you how to access quick
    // settings (another drag). The user can dismiss the cling by clicking OK or by
    // dragging quick settings into view.
    Int32 act = event.getActionMasked();
    if (mSuppressStatusBarDrags) {
        return TRUE;
    } else if (act == IMotionEvent::ACTION_UP && !mClingShown) {
        showCling();
    } else {
        hideCling();
    }

    return FALSE;
}

// GestureRecorder getGestureRecorder()
// {
//  return mGestureRec;
// }

//@Override // CommandQueue
ECode PhoneStatusBar::SetNavigationIconHints(
    /* [in] */ Int32 hints)
{
    if (hints == mNavigationIconHints) return;

    mNavigationIconHints = hints;

    if (mNavigationBarView != NULL) {
        mNavigationBarView->SetNavigationIconHints(hints);
    }
}

//@Override // CommandQueue
ECode PhoneStatusBar::SetSystemUiVisibility(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 mask)
{
    Int32 oldVal = mSystemUiVisibility;
    Int32 newVal = (oldVal&~mask) | (vis&mask);
    Int32 diff = newVal ^ oldVal;

    if (diff != 0) {
        mSystemUiVisibility = newVal;

        if (0 != (diff & View.SYSTEM_UI_FLAG_LOW_PROFILE)) {
            Boolean lightsOut = (0 != (vis & View.SYSTEM_UI_FLAG_LOW_PROFILE));
            if (lightsOut) {
                animateCollapsePanels();
                if (mTicking) {
                    haltTicker();
                }
            }

            if (mNavigationBarView != NULL) {
                mNavigationBarView->SetLowProfile(lightsOut);
            }

            setStatusBarLowProfile(lightsOut);
        }

        notifyUiVisibilityChanged();
    }
}

void PhoneStatusBar::SetStatusBarLowProfile(
    /* [in] */ Boolean lightsOut)
{
    if (mLightsOutAnimation == NULL) {
        View notifications = mStatusBarView->FindViewById(R.id.notification_icon_area);
        View systemIcons = mStatusBarView->FindViewById(R.id.statusIcons);
        View signal = mStatusBarView->FindViewById(R.id.signal_cluster);
        View battery = mStatusBarView->FindViewById(R.id.battery);
        View clock = mStatusBarView->FindViewById(R.id.clock);

        AnimatorSet lightsOutAnim = new AnimatorSet();
        lightsOutAnim.playTogether(
                ObjectAnimator.ofFloat(notifications, View.ALPHA, 0),
                ObjectAnimator.ofFloat(systemIcons, View.ALPHA, 0),
                ObjectAnimator.ofFloat(signal, View.ALPHA, 0),
                ObjectAnimator.ofFloat(battery, View.ALPHA, 0.5f),
                ObjectAnimator.ofFloat(clock, View.ALPHA, 0.5f)
            );
        lightsOutAnim->SetDuration(750);

        AnimatorSet lightsOnAnim = new AnimatorSet();
        lightsOnAnim.playTogether(
                ObjectAnimator.ofFloat(notifications, View.ALPHA, 1),
                ObjectAnimator.ofFloat(systemIcons, View.ALPHA, 1),
                ObjectAnimator.ofFloat(signal, View.ALPHA, 1),
                ObjectAnimator.ofFloat(battery, View.ALPHA, 1),
                ObjectAnimator.ofFloat(clock, View.ALPHA, 1)
            );
        lightsOnAnim->SetDuration(250);

        mLightsOutAnimation = lightsOutAnim;
        mLightsOnAnimation = lightsOnAnim;
    }

    mLightsOutAnimation.cancel();
    mLightsOnAnimation.cancel();

    Animator a = lightsOut ? mLightsOutAnimation : mLightsOnAnimation;
    a->Start();

    setAreThereNotifications();
}

Boolean PhoneStatusBar::AreLightsOn()
{
    return 0 == (mSystemUiVisibility & View.SYSTEM_UI_FLAG_LOW_PROFILE);
}

ECode PhoneStatusBar::SetLightsOn(
    /* [in] */ Boolean on)
{
    Log.v(TAG, "setLightsOn(" + on + ")");
    if (on) {
        setSystemUiVisibility(0, View.SYSTEM_UI_FLAG_LOW_PROFILE);
    } else {
        setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE, View.SYSTEM_UI_FLAG_LOW_PROFILE);
    }
}

void PhoneStatusBar::NotifyUiVisibilityChanged()
{
    try {
        mWindowManagerService.statusBarVisibilityChanged(mSystemUiVisibility);
    } catch (RemoteException ex) {
    }
}

ECode PhoneStatusBar::TopAppWindowChanged(
    /* [in] */ Boolean showMenu)
{
    if (DEBUG) {
        Slog.d(TAG, (showMenu?"showing":"hiding") + " the MENU button");
    }
    if (mNavigationBarView != NULL) {
        mNavigationBarView->SetMenuVisibility(showMenu);
    }

    // See above re: lights-out policy for legacy apps.
    if (showMenu) setLightsOn(TRUE);
}

ECode PhoneStatusBar::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition)
{
    Boolean altBack = (backDisposition == InputMethodService.BACK_DISPOSITION_WILL_DISMISS)
        || ((vis & InputMethodService.IME_VISIBLE) != 0);

    mCommandQueue->SetNavigationIconHints(
            altBack ? (mNavigationIconHints | StatusBarManager.NAVIGATION_HINT_BACK_ALT)
                    : (mNavigationIconHints & ~StatusBarManager.NAVIGATION_HINT_BACK_ALT));
    if (mQS != NULL) mQS->SetImeWindowStatus(vis > 0);
}

ECode PhoneStatusBar::SetHardKeyboardStatus(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    return NOERROR;
}

//@Override
ECode PhoneStatusBar::Tick(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* n,
    /* [in] */ Boolean firstTime)
{
    // no ticking in lights-out mode
    if (!areLightsOn()) return;

    // no ticking in Setup
    if (!isDeviceProvisioned()) return;

    // not for you
    if (!notificationIsForCurrentUser(n)) return;

    // Show the ticker if one is requested. Also don't do this
    // until status bar window is attached to the window manager,
    // because...  well, what's the point otherwise?  And trying to
    // run a ticker without being attached will crash!
    if (n.notification.tickerText != NULL && mStatusBarWindow.getWindowToken() != NULL) {
        if (0 == (mDisabled & (StatusBarManager.DISABLE_NOTIFICATION_ICONS
                        | StatusBarManager.DISABLE_NOTIFICATION_TICKER))) {
            mTicker.addEntry(n);
        }
    }
}

AutoPtr<IAnimation> LoadAnim(
    /* [in] */ Int32 id,
    /* [in] */ IAnimationAnimationListener* listener)
{
    Animation anim = AnimationUtils.loadAnimation(mContext, id);
    if (listener != NULL) {
        anim->SetAnimationListener(listener);
    }
    return anim;
}

String ViewInfo(
    /* [in] */ IView* v)
{
    return "[(" + v.getLeft() + "," + v.getTop() + ")(" + v.getRight() + "," + v.getBottom()
            + ") " + v.getWidth() + "x" + v.getHeight() + "]";
}

//@Override
ECode PhoneStatusBar::CreateAndAddWindows()
{
    addStatusBarWindow();
}

void PhoneStatusBar::AddStatusBarWindow()
{
    // Put up the view
    Int32 height = getStatusBarHeight();

    // Now that the status bar window encompasses the sliding panel and its
    // translucent backdrop, the entire thing is made TRANSLUCENT and is
    // hardware-accelerated.
    WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            height,
            WindowManager.LayoutParams.TYPE_STATUS_BAR,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                | WindowManager.LayoutParams.FLAG_TOUCHABLE_WHEN_WAKING
                | WindowManager.LayoutParams.FLAG_SPLIT_TOUCH,
            PixelFormat.TRANSLUCENT);

    lp.flags |= WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED;

    lp.gravity = getStatusBarGravity();
    lp->SetTitle("StatusBar");
    lp.packageName = mContext.getPackageName();

    makeStatusBarView();
    mWindowManager.addView(mStatusBarWindow, lp);
}

void PhoneStatusBar::SetNotificationIconVisibility(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 anim)
{
    Int32 old = mNotificationIcons.getVisibility();
    Int32 v = visible ? IView::VISIBLE : IView::INVISIBLE;
    if (old != v) {
        mNotificationIcons->SetVisibility(v);
        mNotificationIcons->StartAnimation(loadAnim(anim, NULL));
    }
}

void PhoneStatusBar::UpdateExpandedInvisiblePosition()
{
    mTrackingPosition = -mDisplayMetrics.heightPixels;
}

Float PhoneStatusBar::Saturate(
    /* [in] */ Float a)
{
    return a < 0.0f ? 0.0f : (a > 1.0f ? 1.0f : a);
}

//@Override
Int32 PhoneStatusBar::GetExpandedViewMaxHeight()
{
    return mDisplayMetrics.heightPixels - mNotificationPanelMarginBottomPx;
}

//@Override
ECode PhoneStatusBar::UpdateExpandedViewPos(
    /* [in] */ Int32 thingy)
{
    if (DEBUG) Slog.v(TAG, "updateExpandedViewPos");

    // on larger devices, the notification panel is propped open a bit
    mNotificationPanel->SetMinimumHeight(
            (Int32)(mNotificationPanelMinHeightFrac * mCurrentDisplaySize.y));

    FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) mNotificationPanel.getLayoutParams();
    lp.gravity = mNotificationPanelGravity;
    lp.leftMargin = mNotificationPanelMarginPx;
    mNotificationPanel->SetLayoutParams(lp);

    if (mSettingsPanel != NULL) {
        lp = (FrameLayout.LayoutParams) mSettingsPanel.getLayoutParams();
        lp.gravity = mSettingsPanelGravity;
        lp.rightMargin = mNotificationPanelMarginPx;
        mSettingsPanel->SetLayoutParams(lp);
    }

    updateCarrierLabelVisibility(FALSE);
}

// called by makeStatusbar and also by PhoneStatusBarView
void PhoneStatusBar::UpdateDisplaySize()
{
    mDisplay.getMetrics(mDisplayMetrics);
    if (DEBUG_GESTURES) {
        mGestureRec.tag("display",
                String.format("%dx%d", mDisplayMetrics.widthPixels, mDisplayMetrics.heightPixels));
    }
}

ECode PhoneStatusBar::StartActivityDismissingKeyguard(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean onlyProvisioned)
{
    if (onlyProvisioned && !isDeviceProvisioned()) return;
    try {
        // Dismiss the lock screen when Settings starts.
        ActivityManagerNative.getDefault().dismissKeyguardOnNextActivity();
    } catch (RemoteException e) {
    }
    intent->SetFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
    mContext->StartActivityAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
    animateCollapsePanels();
}

//@Override
ECode PhoneStatusBar::UserSwitched(
    /* [in] */ Int32 newUserId)
{
    if (MULTIUSER_DEBUG) mNotificationPanelDebugText->SetText("USER " + newUserId);
    animateCollapsePanels();
    updateNotificationIcons();
    resetUserSetupObserver();
}

void PhoneStatusBar::ResetUserSetupObserver()
{
    mContext.getContentResolver().unregisterContentObserver(mUserSetupObserver);
    mUserSetupObserver.onChange(FALSE);
    mContext.getContentResolver().registerContentObserver(
            Settings.Secure.getUriFor(Settings.Secure.USER_SETUP_COMPLETE), TRUE,
            mUserSetupObserver,
            mCurrentUserId);
}

void PhoneStatusBar::SetIntruderAlertVisibility(
    /* [in] */ Boolean vis)
{
    if (!ENABLE_INTRUDERS) return;
    if (DEBUG) {
        Slog.v(TAG, (vis ? "showing" : "hiding") + " intruder alert window");
    }
    mIntruderAlertView->SetVisibility(vis ? IView::VISIBLE : IView::GONE);
}

ECode PhoneStatusBar::DismissIntruder()
{
    if (mCurrentlyIntrudingNotification == NULL) return;

    try {
        mBarService.onNotificationClear(
                mCurrentlyIntrudingNotification.pkg,
                mCurrentlyIntrudingNotification.tag,
                mCurrentlyIntrudingNotification.id);
    } catch (android.os.RemoteException ex) {
        // oh well
    }
}

void PhoneStatusBar::UpdateResources()
{
    Context context = mContext;
    Resources res = context.getResources();

    if (mClearButton instanceof TextView) {
        ((TextView)mClearButton)->SetText(context.getText(R.string.status_bar_clear_all_button));
    }

    // Update the QuickSettings container
    if (mQS != NULL) mQS.updateResources();

    loadDimens();
}

void PhoneStatusBar::LoadDimens()
{
    Resources res = mContext.getResources();

    mNaturalBarHeight = res.getDimensionPixelSize(
            R::dimen::status_bar_height);

    Int32 newIconSize = res.getDimensionPixelSize(
        R::dimen::status_bar_icon_size);
    Int32 newIconHPadding = res.getDimensionPixelSize(
        R.dimen.status_bar_icon_padding);

    if (newIconHPadding != mIconHPadding || newIconSize != mIconSize) {
//            Slog.d(TAG, "size=" + newIconSize + " padding=" + newIconHPadding);
        mIconHPadding = newIconHPadding;
        mIconSize = newIconSize;
        //reloadAllNotificationIcons(); // reload the tray
    }

    mEdgeBorder = res.getDimensionPixelSize(R.dimen.status_bar_edge_ignore);

    mSelfExpandVelocityPx = res.getDimension(R.dimen.self_expand_velocity);
    mSelfCollapseVelocityPx = res.getDimension(R.dimen.self_collapse_velocity);
    mFlingExpandMinVelocityPx = res.getDimension(R.dimen.fling_expand_min_velocity);
    mFlingCollapseMinVelocityPx = res.getDimension(R.dimen.fling_collapse_min_velocity);

    mCollapseMinDisplayFraction = res.getFraction(R.dimen.collapse_min_display_fraction, 1, 1);
    mExpandMinDisplayFraction = res.getFraction(R.dimen.expand_min_display_fraction, 1, 1);

    mExpandAccelPx = res.getDimension(R.dimen.expand_accel);
    mCollapseAccelPx = res.getDimension(R.dimen.collapse_accel);

    mFlingGestureMaxXVelocityPx = res.getDimension(R.dimen.fling_gesture_max_x_velocity);

    mFlingGestureMaxOutputVelocityPx = res.getDimension(R.dimen.fling_gesture_max_output_velocity);

    mNotificationPanelMarginBottomPx
        = (Int32) res.getDimension(R.dimen.notification_panel_margin_bottom);
    mNotificationPanelMarginPx
        = (Int32) res.getDimension(R.dimen.notification_panel_margin_left);
    mNotificationPanelGravity = res.getInteger(R.integer.notification_panel_layout_gravity);
    if (mNotificationPanelGravity <= 0) {
        mNotificationPanelGravity = IGravity::LEFT | IGravity::TOP;
    }
    mSettingsPanelGravity = res.getInteger(R.integer.settings_panel_layout_gravity);
    if (mSettingsPanelGravity <= 0) {
        mSettingsPanelGravity = IGravity::RIGHT | IGravity::TOP;
    }

    mCarrierLabelHeight = res.getDimensionPixelSize(R.dimen.carrier_label_height);
    mNotificationHeaderHeight = res.getDimensionPixelSize(R.dimen.notification_panel_header_height);

    mNotificationPanelMinHeightFrac = res.getFraction(R.dimen.notification_panel_min_height_frac, 1, 1);
    if (mNotificationPanelMinHeightFrac < 0.0f || mNotificationPanelMinHeightFrac > 1.0f) {
        mNotificationPanelMinHeightFrac = 0.0f;
    }

    if (FALSE) Slog.v(TAG, "updateResources");
}

//
// tracing
//
void PhoneStatusBar::PostStartTracing()
{
mHandler.postDelayed(mStartTracing, 3000);
}

void PhoneStatusBar::Vibrate()
{
    android.os.Vibrator vib = (android.os.Vibrator)mContext.getSystemService(
            Context.VIBRATOR_SERVICE);
    vib.vibrate(250);
}

//@Override
void PhoneStatusBar::HaltTicker()
{
    mTicker.halt();
}

//@Override
Boolean PhoneStatusBar::ShouldDisableNavbarGestures()
{
    return !isDeviceProvisioned()
            || mExpandedVisible
            || (mDisabled & StatusBarManager.DISABLE_SEARCH) != 0;
}


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
