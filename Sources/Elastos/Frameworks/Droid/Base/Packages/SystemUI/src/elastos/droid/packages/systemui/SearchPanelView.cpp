#include "elastos/droid/systemui/SearchPanelView.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/animation/CLayoutTransition.h"

using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::EIID_IIWindowManager;
using Elastos::Droid::SystemUI::SystemUIR;


namespace Elastos {
namespace Droid {
namespace SystemUI {

SearchPanelView::SearchPanelView()
    : mShowing(FALSE)
{
}

SearchPanelView::SearchPanelView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
{
    InitImpl(context, attrs, 0);
}

SearchPanelView::SearchPanelView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : FrameLayout(context, attrs, defStyle)
{
    InitImpl(context, attrs, defStyle);
}

ECode SearchPanelView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, attrs, 0);
}

ECode SearchPanelView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::Init(context, attrs, defStyle);
    return InitImpl(context, attrs, defStyle);
}

ECode SearchPanelView::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    mContext = context;
    AutoPtr<IInterface> service = ServiceManager::GetService(String("window"));
    mWm = (IIWindowManager*)service->Probe(EIID_IIWindowManager);
    return NOERROR;
}

void SearchPanelView::StartAssistActivity()
{
    // if (!mBar.isDeviceProvisioned()) return;

    // // Close Recent Apps if needed
    // mBar.animateCollapsePanels(CommandQueue.FLAG_EXCLUDE_SEARCH_PANEL);
    // boolean isKeyguardShowing = false;
    // try {
    //     isKeyguardShowing = mWm.isKeyguardLocked();
    // } catch (RemoteException e) {

    // }

    // if (isKeyguardShowing) {
    //     // Have keyguard show the bouncer and launch the activity if the user succeeds.
    //     try {
    //         mWm.showAssistant();
    //     } catch (RemoteException e) {
    //         // too bad, so sad...
    //     }
    //     onAnimationStarted();
    // } else {
    //     // Otherwise, keyguard isn't showing so launch it from here.
    //     Intent intent = ((SearchManager) mContext.getSystemService(Context.SEARCH_SERVICE))
    //             .getAssistIntent(mContext, UserHandle.USER_CURRENT);
    //     if (intent == null) return;

    //     try {
    //         ActivityManagerNative.getDefault().dismissKeyguardOnNextActivity();
    //     } catch (RemoteException e) {
    //         // too bad, so sad...
    //     }

    //     try {
    //         ActivityOptions opts = ActivityOptions.makeCustomAnimation(mContext,
    //                 R.anim.search_launch_enter, R.anim.search_launch_exit,
    //                 getHandler(), this);
    //         intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    //         mContext.startActivityAsUser(intent, opts.toBundle(),
    //                 new UserHandle(UserHandle.USER_CURRENT));
    //     } catch (ActivityNotFoundException e) {
    //         Slog.w(TAG, "Activity not found for " + intent.getAction());
    //         onAnimationStarted();
    //     }
    // }
}

ECode SearchPanelView::OnAnimationStarted()
{
    //     postDelayed(new Runnable() {
    //     public void run() {
    //         mGlowPadViewListener.mWaitingForLaunch = false;
    //         mBar.hideSearchPanel();
    //     }
    // }, SEARCH_PANEL_HOLD_DURATION);
    return NOERROR;
}

ECode SearchPanelView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&tmpObj);
    mSearchTargetsContainer = NULL;
    mSearchTargetsContainer = FindViewById(SystemUIR::id::search_panel_container);
    assert(0);
    // mStatusBarTouchProxy = (StatusBarTouchProxy) findViewById(R.id.status_bar_touch_proxy);
    // // TODO: fetch views
    // mGlowPadView = (GlowPadView) findViewById(R.id.glow_pad_view);
    // mGlowPadView.setOnTriggerListener(mGlowPadViewListener);
    return NOERROR;
}

void SearchPanelView::MaybeSwapSearchIcon()
{
    // Intent intent = ((SearchManager) mContext.getSystemService(Context.SEARCH_SERVICE))
    //         .getAssistIntent(mContext, UserHandle.USER_CURRENT);
    // if (intent != null) {
    //     ComponentName component = intent.getComponent();
    //     if (component == null || !mGlowPadView.replaceTargetDrawablesIfPresent(component,
    //             ASSIST_ICON_METADATA_NAME,
    //             com.android.internal.R.drawable.ic_action_assist_generic)) {
    //         if (DEBUG) Slog.v(TAG, "Couldn't grab icon for component " + component);
    //     }
    // }
}

Boolean SearchPanelView::PointInside(Int32 x, Int32 y, IView* v)
{
    // final Int32 l = v.getLeft();
    // final Int32 r = v.getRight();
    // final Int32 t = v.getTop();
    // final Int32 b = v.getBottom();
    // return x >= l && x < r && y >= t && y < b;
    return NOERROR;
}

Boolean SearchPanelView::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    //     if (pointInside(x, y, mSearchTargetsContainer)) {
    //     return true;
    // } else if (mStatusBarTouchProxy != null &&
    //         pointInside(x, y, mStatusBarTouchProxy)) {
    //     return true;
    // } else {
    //     return false;
    // }
    return NOERROR;
}

void SearchPanelView::Vibrate()
{
    //     Context context = getContext();
    // if (Settings.System.getIntForUser(context.getContentResolver(),
    //         Settings.System.HAPTIC_FEEDBACK_ENABLED, 1, UserHandle.USER_CURRENT) != 0) {
    //     Resources res = context.getResources();
    //     Vibrator vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
    //     vibrator.vibrate(res.getInteger(R.integer.config_search_panel_view_vibration_duration));
    // }
}


ECode SearchPanelView::Show(
    /* [in] */ Boolean show,
    /* [in] */ Boolean animate)
{
    // if (!show) {
    //     final LayoutTransition transitioner = animate ? createLayoutTransitioner() : null;
    //     ((ViewGroup) mSearchTargetsContainer).setLayoutTransition(transitioner);
    // }
    // mShowing = show;
    // if (show) {
    //     maybeSwapSearchIcon();
    //     if (getVisibility() != View.VISIBLE) {
    //         setVisibility(View.VISIBLE);
    //         // Don't start the animation until we've created the layer, which is done
    //         // right before we are drawn
    //         mGlowPadView.suspendAnimations();
    //         mGlowPadView.ping();
    //         getViewTreeObserver().addOnPreDrawListener(mPreDrawListener);
    //         vibrate();
    //     }
    //     setFocusable(true);
    //     setFocusableInTouchMode(true);
    //     requestFocus();
    // } else {
    //     setVisibility(View.INVISIBLE);
    // }
    return NOERROR;
}

ECode SearchPanelView::Hide(
    /* [in] */ Boolean animate)
{
    //     if (mBar != null) {
    //     // This will indirectly cause show(false, ...) to get called
    //     mBar.animateCollapsePanels(CommandQueue.FLAG_EXCLUDE_NONE);
    // } else {
    //     setVisibility(View.INVISIBLE);
    // }
    return NOERROR;
}

void SearchPanelView::OnLayout(Boolean changed, Int32 l, Int32 t, Int32 r, Int32 b)
{
    FrameLayout::OnLayout(changed, l, t, r, b);
    // Int32 height;
    // mSearchTargetsContainer->GetHeight(&height);
    // SetPanelHeight(height);
}

Boolean SearchPanelView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // Ignore hover events outside of this panel bounds since such events
    // generate spurious accessibility events with the panel content when
    // tapping outside of it, thus confusing the user.
    Float fx, fy;
    event->GetX(&fx);
    event->GetY(&fy);
    Int32 x = (Int32) fx;
    Int32 y = (Int32) fy;
    if (x >= 0 && x < GetWidth() && y >= 0 && y < GetHeight()) {
        return FrameLayout::DispatchHoverEvent(event);
    }
    return TRUE;
}

Boolean SearchPanelView::IsShowing()
{
    return mShowing;
}

ECode SearchPanelView::SetBar(
    /* [in] */ IBaseStatusBar* bar)
{
    mBar = bar;
    return NOERROR;
}

ECode SearchPanelView::SetStatusBarView(
    /* [in] */ IView* statusBarView)
{
//     if (mStatusBarTouchProxy != null) {
//         mStatusBarTouchProxy.setStatusBar(statusBarView);
// //            mGlowPadView.setOnTouchListener(new OnTouchListener() {
// //                public boolean onTouch(View v, MotionEvent event) {
// //                    return statusBarView.onTouchEvent(event);
// //                }
// //            });
//     }
    return NOERROR;
}

AutoPtr<ILayoutTransition> SearchPanelView::CreateLayoutTransitioner()
{
    AutoPtr<ILayoutTransition> transitioner;
    CLayoutTransition::New((ILayoutTransition**)&transitioner);
    transitioner->SetDuration(200);
    transitioner->SetStartDelay(ILayoutTransition::CHANGE_DISAPPEARING, 0);
    transitioner->SetAnimator(ILayoutTransition::DISAPPEARING, NULL);
    return transitioner;
}

Boolean SearchPanelView::IsAssistantAvailable()
{
    // return ((SearchManager) mContext.getSystemService(Context.SEARCH_SERVICE))
    //     .getAssistIntent(mContext, UserHandle.USER_CURRENT) != null;
    return FALSE;
}


}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
