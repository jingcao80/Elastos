#include "elastos/droid/systemui/statusbar/tablet/NotificationPanel.h"
#include "elastos/droid/view/CMotionEventHelper.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/systemui/CExpandHelper.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/view/animation/CAccelerateInterpolator.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/droid/graphics/CRect.h"

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::SystemUI::CExpandHelper;
using Elastos::Droid::SystemUI::StatusBar::Tablet::EIID_INotificationPanel;
using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

//============================================================================
//              NotificationPanel::ClearButtonClickListener
//============================================================================
CAR_INTERFACE_IMPL(NotificationPanel::ClearButtonClickListener, IViewOnClickListener)

NotificationPanel::ClearButtonClickListener::ClearButtonClickListener(
    /* [in] */ NotificationPanel* host)
    : mHost(host)
{}

ECode NotificationPanel::ClearButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->mBar->ClearAll();
    return NOERROR;
}

//============================================================================
//              NotificationPanel::PreDrawListener
//============================================================================

CAR_INTERFACE_IMPL(NotificationPanel::PreDrawListener, IOnPreDrawListener)

NotificationPanel::PreDrawListener::PreDrawListener(
    /* [in] */ NotificationPanel* host)
    : mHost(host)
{}

ECode NotificationPanel::PreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    AutoPtr<IViewTreeObserver> viewTreeObserver = mHost->GetViewTreeObserver();
    viewTreeObserver->RemoveOnPreDrawListener(THIS_PROBE(IOnPreDrawListener));
    mHost->mChoreo->StartAnimation(TRUE);
    return NOERROR;
}

//============================================================================
//              NotificationPanel::Choreographer
//============================================================================

const Int32 NotificationPanel::Choreographer::OPEN_DURATION = 250;
const Int32 NotificationPanel::Choreographer::CLOSE_DURATION = 250;

// the panel will start to appear this many px from the end
const Int32 NotificationPanel::Choreographer::HYPERSPACE_OFFRAMP = 200;

CAR_INTERFACE_IMPL(NotificationPanel::Choreographer, IAnimatorListener)

NotificationPanel::Choreographer::Choreographer(
    /* [in] */ NotificationPanel* host)
    : mHost(host)
    , mVisible(FALSE)
    , mPanelHeight(0)
{}

void NotificationPanel::Choreographer::CreateAnimation(
    /* [in] */ Boolean appearing)
{
    // mVisible: previous state; appearing: new state

    Float start, end;

    // 0: on-screen
    // height: off-screen
    Float y;
    mHost->mContentParent->GetTranslationY(&y);
    if (appearing) {
        // we want to go from near-the-top to the top, unless we're half-open in the right
        // general vicinity
        end = 0;
        if (mHost->mNotificationCount == 0) {
            end += mHost->mContentFrameMissingTranslation;
        }
        start = HYPERSPACE_OFFRAMP + end;
    }
    else {
        start = y;
        end = y + HYPERSPACE_OFFRAMP;
    }

    AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
    (*params)[0] = start; (*params)[1] = end;
    AutoPtr<IAnimator> posAnim = CObjectAnimator::OfFloat(mHost->mContentParent, String("translationY"),
            params);
    posAnim->SetInterpolator(appearing ? sDecelerateInterpolator : sAccelerateInterpolator);

    Boolean isRunning;
    if (mContentAnim != NULL && (mContentAnim->IsRunning(&isRunning), isRunning)) {
        mContentAnim->Cancel();
    }

    AutoPtr<ArrayOf<Float> > fadeParams = ArrayOf<Float>::Alloc(1);
    if(appearing) {
        (*fadeParams)[0] = 1.f;
    } else {
        (*fadeParams)[0] = 0.f;
    }
    AutoPtr<IAnimator> fadeAnim = CObjectAnimator::OfFloat(mHost->mContentParent, String("alpha"),
            fadeParams);
    fadeAnim->SetInterpolator(appearing ? sAccelerateInterpolator : sDecelerateInterpolator);

    CAnimatorSet::New((IAnimatorSet**)&mContentAnim);
    AutoPtr<IAnimatorSetBuilder> build;
    mContentAnim->Play(fadeAnim, (IAnimatorSetBuilder**)&build);
    build->With(posAnim);
    mContentAnim->SetDuration((appearing ? OPEN_DURATION : CLOSE_DURATION));
    mContentAnim->AddListener(THIS_PROBE(IAnimatorListener));
}

void NotificationPanel::Choreographer::StartAnimation(
    /* [in] */ Boolean appearing)
{
    // if (DEBUG) Slog.d(TAG, "startAnimation(appearing=" + appearing + ")");

    CreateAnimation(appearing);
    mContentAnim->Start();

    mVisible = appearing;

    // we want to start disappearing promptly
    if (!mVisible) mHost->UpdateClearButton();
}

ECode NotificationPanel::Choreographer::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    // if (DEBUG) Slog.d(TAG, "onAnimationCancel");
    return NOERROR;
}

ECode NotificationPanel::Choreographer::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    // if (DEBUG) Slog.d(TAG, "onAnimationEnd");
    if (! mVisible) {
        mHost->SetVisibility(IView::GONE);
    }
    mHost->mContentParent->SetLayerType(IView::LAYER_TYPE_NONE, NULL);

    // we want to show the X lazily
    if (mVisible) mHost->UpdateClearButton();
    return NOERROR;
}

ECode NotificationPanel::Choreographer::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode NotificationPanel::Choreographer::OnAnimationStart(
    /* [out] */ IAnimator* animation)
{
    return NOERROR;
}

//============================================================================
//              MyListener
//============================================================================

NotificationPanel::MyListener::MyListener(
    /* [in] */ IView* show,
    /* [in] */ IView* hide,
    /* [in] */ NotificationPanel* host)
{
    mShow = show;
    mHide = hide;
    mHost = host;
}

ECode NotificationPanel::MyListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHide->SetVisibility(IView::VISIBLE);
    if(mShow) {
        mShow->SetVisibility(IView::VISIBLE);
        if (mShow == mHost->mSettingsView || mHost->mNotificationCount > 0) {
            AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
            (*params)[0] = 0.f;
            (*params)[1] = 1.f;

            AutoPtr<IAnimator> anim = CObjectAnimator::OfFloat(mShow, String("alpah"), params);
            anim->SetDuration(PANEL_FADE_DURATION);
            anim->Start();
        }

        if (mHide = mHost->mSettingsView) {
            mHost->RemoveSettingsView();
        }
    }
    mHost->UpdateClearButton();
    mHost->UpdatePanelModeButtons();
    return NOERROR;
}


//============================================================================
//              NotificationPanel
//============================================================================
static AutoPtr<IInterpolator> InitStaticAcc()
{
    AutoPtr<CAccelerateInterpolator> temp;
    CAccelerateInterpolator::NewByFriend((CAccelerateInterpolator**)&temp);
    return temp;
}

static AutoPtr<IInterpolator> InitStaticDec()
{
    AutoPtr<CDecelerateInterpolator> temp;
    CDecelerateInterpolator::NewByFriend((CDecelerateInterpolator**)&temp);
    return temp;
}

AutoPtr<IInterpolator> NotificationPanel::sAccelerateInterpolator = InitStaticAcc();
AutoPtr<IInterpolator> NotificationPanel::sDecelerateInterpolator = InitStaticDec();
const Int32 NotificationPanel::PANEL_FADE_DURATION = 150;
const String NotificationPanel::TAG("NotificationPanel");
const Boolean NotificationPanel::DEBUG = FALSE;

NotificationPanel::NotificationPanel()
    : mShowing(FALSE)
    , mHasClearableNotifications(FALSE)
    , mNotificationCount(0)
    , mContentFrameMissingTranslation(0)
{
    mChoreo = new Choreographer(this);
    mClearButtonListener = new ClearButtonClickListener(this);
    mPreDrawListener = new PreDrawListener(this);
    CRect::New((IRect**)&mContentArea);
}

NotificationPanel::NotificationPanel(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : RelativeLayout(context, attrs)
    , mShowing(FALSE)
    , mHasClearableNotifications(FALSE)
    , mNotificationCount(0)
    , mContentFrameMissingTranslation(0)
{
    mChoreo = new Choreographer(this);
    mClearButtonListener = new ClearButtonClickListener(this);
    mPreDrawListener = new PreDrawListener(this);
}

NotificationPanel::NotificationPanel(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : RelativeLayout(context, attrs, defStyle)
{
}

ECode NotificationPanel::SetBar(
    /* [in] */ ITabletStatusBar* b)
{
    mBar = b;
    return NOERROR;
}

ECode NotificationPanel::OnFinishInflate()
{
    if (DEBUG) Slogger::D(TAG, "OnFinishInflate");
    RelativeLayout::OnFinishInflate();

    SetWillNotDraw(FALSE);

    AutoPtr<IView> tmpView;

    tmpView = FindViewById(SystemUIR::id::content_parent);
    mContentParent = IViewGroup::Probe(tmpView.Get());
    mContentParent->BringToFront();

    tmpView = FindViewById(SystemUIR::id::title_area);
    mTitleArea = INotificationPanelTitle::Probe(tmpView.Get());
    mTitleArea->SetPanel(THIS_PROBE(INotificationPanel));

    mSettingsButton = FindViewById(SystemUIR::id::settings_button);
    mNotificationButton = FindViewById(SystemUIR::id::notification_button);
    mNotificationScroller = FindViewById(SystemUIR::id::notification_scroller);

    tmpView = FindViewById(SystemUIR::id::content_frame);
    mContentFrame = IViewGroup::Probe(tmpView.Get());
    mContentFrameMissingTranslation = 0; // not needed with current assets

    // the "X" that appears in place of the clock when the panel is showing notifications
    mClearButton = FindViewById(SystemUIR::id::clear_all_button);
    mClearButton->SetOnClickListener(mClearButtonListener);

    mShowing = FALSE;
    return NOERROR;
}

ECode NotificationPanel::OnAttachedToWindow()
{
    RelativeLayout::OnAttachedToWindow();

    AutoPtr<IView> tmpView;

    tmpView = FindViewById(SystemUIR::id::content);
    mLatestItems = INotificationRowLayout::Probe(tmpView.Get());

    AutoPtr<IResources> res = GetResources();
    Int32 minHeight, maxHeight;
    res->GetDimensionPixelSize(SystemUIR::dimen::notification_row_min_height, &minHeight);
    res->GetDimensionPixelSize(SystemUIR::dimen::notification_row_max_height, &maxHeight);

    mExpandHelper = NULL;
    CExpandHelper::New(mContext, IExpandHelperCallback::Probe(mLatestItems.Get()),
        minHeight, maxHeight, (IExpandHelper**)&mExpandHelper);
    mExpandHelper->SetEventSource(THIS_PROBE(IView));
    mExpandHelper->SetGravity(IGravity::BOTTOM);
    return NOERROR;
}

AutoPtr<IView> NotificationPanel::GetClearButton()
{
    return mClearButton;
}

ECode NotificationPanel::Show(
    /* [in] */ Boolean show,
    /* [in] */ Boolean animate)
{
    if (animate) {
        if (mShowing != show) {
            mShowing = show;
            if (show) {
                SetVisibility(IView::VISIBLE);
                // Don't start the animation until we've created the layer, which is done
                // right before we are drawn
                mContentParent->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
                AutoPtr<IViewTreeObserver> viewTreeObserver = GetViewTreeObserver();
                viewTreeObserver->AddOnPreDrawListener(mPreDrawListener);
            }
            else {
                mChoreo->StartAnimation(show);
            }
        }
    }
    else {
        mShowing = show;
        SetVisibility(show ? IView::VISIBLE : IView::GONE);
    }
    return NOERROR;
}

Boolean NotificationPanel::IsShowing()
{
    return mShowing;
}

void NotificationPanel::OnVisibilityChanged(
    /* [in] */ IView* v,
    /* [in] */ Int32 vis)
{
    RelativeLayout::OnVisibilityChanged(v, vis);
    // when we hide, put back the notifications
    if (vis != IView::VISIBLE) {
        if (mSettingsView != NULL) RemoveSettingsView();
        mNotificationScroller->SetVisibility(IView::VISIBLE);
        mNotificationScroller->SetAlpha(1.0f);
        mNotificationScroller->ScrollTo(0, 0);
        UpdatePanelModeButtons();
    }
}

Boolean NotificationPanel::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // Ignore hover events outside of this panel bounds since such events
    // generate spurious accessibility events with the panel content when
    // tapping outside of it, thus confusing the user.
    Float fx, fy;
    event->GetX(&fx);
    event->GetX(&fy);
    Int32 x = fx;
    Int32 y = fy;
    if (x >= 0 && x < GetWidth() && y >= 0 && y < GetHeight()) {
        return RelativeLayout::DispatchHoverEvent(event);
    }
    return TRUE;
}

Boolean NotificationPanel::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    Int32 keyCode, action;
    event->GetKeyCode(&keyCode);
    event->GetAction(&action);
    switch (keyCode) {
        // We exclusively handle the back key by hiding this panel.
        case IKeyEvent::KEYCODE_BACK: {
            if (action == IKeyEvent::ACTION_UP) {
                mBar->AnimateCollapsePanels();
            }
            return TRUE;
        }

        // We react to the home key but let the system handle it.
        case IKeyEvent::KEYCODE_HOME: {
            if (action == IKeyEvent::ACTION_UP) {
                mBar->AnimateCollapsePanels();
            }
        }
        break;
    }
    return RelativeLayout::DispatchKeyEvent(event);
}

/*
@Override
protected void onLayout(boolean changed, int l, int t, int r, int b) {
    super.onLayout(changed, l, t, r, b);

    if (DEBUG) Slog.d(TAG, String.format("PANEL: onLayout: (%d, %d, %d, %d)", l, t, r, b));
}

@Override
public void onSizeChanged(int w, int h, int oldw, int oldh) {
    super.onSizeChanged(w, h, oldw, oldh);

    if (DEBUG) {
        Slog.d(TAG, String.format("PANEL: onSizeChanged: (%d -> %d, %d -> %d)",
                    oldw, w, oldh, h));
    }
}
*/

ECode NotificationPanel::OnClick(
    /* [in] */ IView* v)
{
    Boolean enabled;
    mSettingsButton->IsEnabled(&enabled);
    if (enabled && v == mTitleArea) {
        SwapPanels();
    }
    return NOERROR;
}

ECode NotificationPanel::SetNotificationCount(
    /* [in] */ Int32 n)
{
    mNotificationCount = n;
    return NOERROR;
}

ECode NotificationPanel::SetContentFrameVisible(
    /* [in] */ Boolean showing,
    /* [in] */ Boolean animate)
{
    return NOERROR;
}

ECode NotificationPanel::SwapPanels()
{
    AutoPtr<IView> toShow, toHide;
    if (mSettingsView == NULL) {
        AddSettingsView();
        toShow = mSettingsView;
        toHide = mNotificationScroller;
    }
    else {
        toShow = mNotificationScroller;
        toHide = mSettingsView;
    }

    AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
    (*params)[0] = 1.f;
    (*params)[1] = 0.f;
    AutoPtr<IAnimator> a = CObjectAnimator::OfFloat(toHide, String("alpha"), params);
    a->SetDuration(PANEL_FADE_DURATION);
    AutoPtr<AnimatorListenerAdapter> l = new MyListener(toShow, toHide, this);
    a->AddListener(l);
    a->Start();
    return NOERROR;
}

ECode NotificationPanel::UpdateClearButton()
{
    if (mBar != NULL) {
        Boolean showX = (IsShowing() && (mHasClearableNotifications != NULL));
        if (showX) {
            Int32 visibility;
            mNotificationScroller->GetVisibility(&visibility);
            showX = (visibility == IView::VISIBLE);
        }
        AutoPtr<IView> button = GetClearButton();
        button->SetVisibility(showX ? IView::VISIBLE : IView::INVISIBLE);
    }
    return NOERROR;
}

ECode NotificationPanel::SetClearable(
    /* [in] */ Boolean clearable)
{
    mHasClearableNotifications = clearable;
    return NOERROR;
}

ECode NotificationPanel::UpdatePanelModeButtons()
{
    Boolean settingsVisible = (mSettingsView != NULL);
    Boolean enabled;
    mSettingsButton->IsEnabled(&enabled);
    mSettingsButton->SetVisibility((!settingsVisible && enabled) ? IView::VISIBLE : IView::GONE);
    mNotificationButton->SetVisibility(settingsVisible ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

Boolean NotificationPanel::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Float py;
    mContentParent->GetTranslationY(&py);
    Int32 cl, ct, cr, cb, pl, pt, pr, pb;
    mContentFrame->GetLeft(&cl);
    mContentFrame->GetTop(&ct);
    mContentFrame->GetRight(&cr);
    mContentFrame->GetBottom(&cb);
    mContentFrame->GetPaddingLeft(&pl);
    mContentFrame->GetPaddingTop(&pt);
    mContentFrame->GetPaddingRight(&pr);
    mContentFrame->GetPaddingBottom(&pb);
    Int32 l = cl + pl;
    Int32 t = ct + pt + (Int32)py; // account for any adjustment
    Int32 r = cr - pr;
    Int32 b = cb - pb;

    mContentArea->Set(l, t, r, b);
    OffsetDescendantRectToMyCoords(mContentParent, mContentArea);
    Boolean result;
    mContentArea->Contains(x, y, &result);
    return result;
}

void NotificationPanel::RemoveSettingsView()
{
    if (mSettingsView != NULL) {
        mContentFrame->RemoveViewInLayout(mSettingsView);
        mSettingsView = NULL;
    }
}

// NB: it will be invisible until you show it
void NotificationPanel::AddSettingsView()
{
    AutoPtr<IContext> context = GetContext();
    AutoPtr<ILayoutInflater> infl;
    LayoutInflater::From(context, (ILayoutInflater**)&infl);

    mSettingsView = NULL;
    infl->Inflate(SystemUIR::layout::system_bar_settings_view, mContentFrame,
        FALSE, (IView**)&mSettingsView);
    mSettingsView->SetVisibility(IView::GONE);
    mContentFrame->AddView(mSettingsView);
}

Boolean NotificationPanel::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> cancellation;
    helper->Obtain(ev, (IMotionEvent**)&cancellation);
    cancellation->SetAction(IMotionEvent::ACTION_CANCEL);

    Boolean intercept;
    mExpandHelper->OnInterceptTouchEvent(ev, &intercept);
    if (!intercept)
        intercept = RelativeLayout::OnInterceptTouchEvent(ev);
    if (intercept) {
        mLatestItems->OnInterceptTouchEvent(cancellation, &intercept);
    }
    return intercept;
}

Boolean NotificationPanel::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Boolean handled;
    mExpandHelper->OnTouchEvent(ev, &handled);
    if (!handled)
        handled = RelativeLayout::OnTouchEvent(ev);
    return handled;
}

ECode NotificationPanel::SetSettingsEnabled(
    /* [in] */ Boolean settingsEnabled)
{
    if (mSettingsButton != NULL) {
        mSettingsButton->SetEnabled(settingsEnabled);
        Int32 visibility = settingsEnabled ? IView::VISIBLE : IView::GONE;
        mSettingsButton->SetVisibility(visibility);
    }
    return NOERROR;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
