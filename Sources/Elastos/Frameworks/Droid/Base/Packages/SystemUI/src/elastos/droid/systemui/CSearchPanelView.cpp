
#include "elastos/droid/systemui/CSearchPanelView.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/droid/provider/Settings.h>
#include "R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::SystemUI::StatusBar::ICommandQueue;
using Elastos::Droid::SystemUI::StatusBar::ICommandQueueCallbacks;
using Elastos::Droid::SystemUI::StatusBar::EIID_IStatusBarPanel;
using Elastos::Droid::SystemUI::StatusBar::Phone::CPhoneStatusBar;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CSearchPanelView::Runnable1::Runnable1(
    /* [in] */ CSearchPanelView* host,
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityOptions* opts)
    : mHost(host)
    , mIntent(intent)
    , mOpts(opts)
{}

ECode CSearchPanelView::Runnable1::Run()
{
    AutoPtr<IBundle> b;
    mOpts->ToBundle((IBundle**)&b);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&user);
    mHost->mContext->StartActivityAsUser(mIntent, b, user);
    return NOERROR;
}

CSearchPanelView::Runnable2::Runnable2(
    /* [in] */ CSearchPanelView* host)
    : mHost(host)
{}

ECode CSearchPanelView::Runnable2::Run()
{
    mHost->mCircle->SetAnimatingOut(FALSE);
    mHost->SetVisibility(IView::INVISIBLE);
    return NOERROR;
}

CSearchPanelView::Runnable3::Runnable3(
    /* [in] */ CSearchPanelView* host)
    : mHost(host)
{}

ECode CSearchPanelView::Runnable3::Run()
{
    mHost->StartExitAnimation();
    return NOERROR;
}

CSearchPanelView::Runnable4::Runnable4(
    /* [in] */ CSearchPanelView* host)
    : mHost(host)
{}

ECode CSearchPanelView::Runnable4::Run()
{
    mHost->mLaunching = FALSE;
    mHost->mCircle->SetAnimatingOut(FALSE);
    mHost->SetVisibility(IView::INVISIBLE);
    return NOERROR;
}

const String CSearchPanelView::TAG("SearchPanelView");
const String CSearchPanelView::ASSIST_ICON_METADATA_NAME("com.android.systemui.action_assist_icon");
Boolean CSearchPanelView::sInit = InitStatic();
AutoPtr<IAudioAttributes> CSearchPanelView::VIBRATION_ATTRIBUTES;
CAR_OBJECT_IMPL(CSearchPanelView);
CAR_INTERFACE_IMPL_2(CSearchPanelView, FrameLayout, ISearchPanelView, IStatusBarPanel);
CSearchPanelView::CSearchPanelView()
    : mThreshold(0)
    , mHorizontal(FALSE)
    , mLaunching(FALSE)
    , mDragging(FALSE)
    , mDraggedFarEnough(FALSE)
    , mStartTouch(0)
    , mStartDrag(0)
    , mLaunchPending(FALSE)
{}

ECode CSearchPanelView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CSearchPanelView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);
    mContext = context;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::search_panel_threshold, &mThreshold);
    return NOERROR;
}

Boolean CSearchPanelView::InitStatic()
{
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&builder);
    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    builder->Build((IAudioAttributes**)&VIBRATION_ATTRIBUTES);
    return TRUE;
}

void CSearchPanelView::StartAssistActivity()
{
    Boolean tmp = FALSE;
    if (mBar->IsDeviceProvisioned(&tmp), !tmp) return;

    // Close Recent Apps if needed
    ICommandQueueCallbacks::Probe(mBar)->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_SEARCH_PANEL);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<ISearchManager> sm = ISearchManager::Probe(obj);
    AutoPtr<IIntent> intent;
    sm->GetAssistIntent(mContext, TRUE, IUserHandle::USER_CURRENT, (IIntent**)&intent);
    if (intent == NULL) return;

    AutoPtr<IActivityOptionsHelper> helper;
    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
    AutoPtr<IActivityOptions> opts;
    helper->MakeCustomAnimation(mContext,
            R::anim::search_launch_enter, R::anim::search_launch_exit, (IActivityOptions**)&opts);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<Runnable1> run = new Runnable1(this, intent, opts);
    if (FAILED(AsyncTask::Execute(run))) {
        String action;
        intent->GetAction(&action);
        Logger::W(TAG, "Activity not found for %s", action.string());
    }
}

ECode CSearchPanelView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<IView> view;
    FindViewById(R::id::search_panel_circle, (IView**)&view);
    mCircle = ISearchPanelCircleView::Probe(view);
    view = NULL;
    FindViewById(R::id::search_logo, (IView**)&view);
    mLogo = IImageView::Probe(view);

    view = NULL;
    FindViewById(R::id::search_panel_scrim, (IView**)&mScrim);
    return NOERROR;
}

void CSearchPanelView::MaybeSwapSearchIcon()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<IIntent> intent;
    ISearchManager::Probe(obj)->GetAssistIntent(mContext, FALSE,
            IUserHandle::USER_CURRENT, (IIntent**)&intent);
    if (intent != NULL) {
        AutoPtr<IComponentName> component;
        intent->GetComponent((IComponentName**)&component);
        ReplaceDrawable(mLogo, component, ASSIST_ICON_METADATA_NAME);
    }
    else {
        mLogo->SetImageDrawable(NULL);
    }
}

ECode CSearchPanelView::ReplaceDrawable(
    /* [in] */ IImageView* v,
    /* [in] */ IComponentName* component,
    /* [in] */ const String& name)
{
    if (component != NULL) {
        ECode ec = NOERROR;
        do {
            AutoPtr<IPackageManager> packageManager;
            mContext->GetPackageManager((IPackageManager**)&packageManager);
            // Look for the search icon specified in the activity meta-data
            AutoPtr<IActivityInfo> info;
            ec = packageManager->GetActivityInfo(
                    component, IPackageManager::GET_META_DATA, (IActivityInfo**)&info);
            if (FAILED(ec)) break;
            AutoPtr<IBundle> metaData;
            ec = IPackageItemInfo::Probe(info)->GetMetaData((IBundle**)&metaData);
            if (FAILED(ec)) break;
            if (metaData != NULL) {
                Int32 iconResId = 0;
                ec = metaData->GetInt32(name, &iconResId);
                if (FAILED(ec)) break;
                if (iconResId != 0) {
                    AutoPtr<IResources> res;
                    ec = packageManager->GetResourcesForActivity(component, (IResources**)&res);
                    if (FAILED(ec)) break;
                    AutoPtr<IDrawable> drawable;
                    ec = res->GetDrawable(iconResId, (IDrawable**)&drawable);
                    if (FAILED(ec)) break;
                    v->SetImageDrawable(drawable);
                    return NOERROR;
                }
            }
        } while (0);
        if (ec == (Int32)E_NAME_NOT_FOUND_EXCEPTION) {
            String str;
            component->FlattenToShortString(&str);
            Logger::W(TAG, "Failed to swap drawable; %s not found", str.string());
        }
        else if (ec == (Int32)E_NOT_FOUND_EXCEPTION) {
            String str;
            component->FlattenToShortString(&str);
            Logger::W(TAG, "Failed to swap drawable from %s", str.string());
        }
    }
    v->SetImageDrawable(NULL);
    return NOERROR;
}

ECode CSearchPanelView::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

void CSearchPanelView::Vibrate()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 state = 0;
    Elastos::Droid::Provider::Settings::System::GetInt32ForUser(resolver,
            ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 1, IUserHandle::USER_CURRENT, &state);
    if (state != 0) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
        AutoPtr<IVibrator> vibrator = IVibrator::Probe(obj);
        Int32 value = 0;
        res->GetInteger(R::integer::config_search_panel_view_vibration_duration, &value);
        vibrator->Vibrate(value, VIBRATION_ATTRIBUTES);
    }
}

ECode CSearchPanelView::Show(
    /* [in] */ Boolean show,
    /* [in] */ Boolean animate)
{
    if (show) {
        MaybeSwapSearchIcon();
        Int32 v = 0;
        if ((GetVisibility(&v), v) != IView::VISIBLE) {
            SetVisibility(IView::VISIBLE);
            Vibrate();
            if (animate) {
                StartEnterAnimation();
            }
            else {
                mScrim->SetAlpha(1.f);
            }
        }
        SetFocusable(TRUE);
        SetFocusableInTouchMode(TRUE);
        Boolean tmp = FALSE;
        RequestFocus(&tmp);
    }
    else {
        if (animate) {
            StartAbortAnimation();
        }
        else {
            SetVisibility(IView::INVISIBLE);
        }
    }
    return NOERROR;
}

void CSearchPanelView::StartEnterAnimation()
{
    mCircle->StartEnterAnimation();
    mScrim->SetAlpha(0.f);
    AutoPtr<IViewPropertyAnimator> animator;
    mScrim->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(1.f);
    animator->SetDuration(300);
    animator->SetStartDelay(50);
    animator->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_IN));
    animator->Start();

}

void CSearchPanelView::StartAbortAnimation()
{
    AutoPtr<Runnable2> run = new Runnable2(this);
    mCircle->StartAbortAnimation(run);
    mCircle->SetAnimatingOut(TRUE);
    AutoPtr<IViewPropertyAnimator> animator;
    mScrim->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(0.f);
    animator->SetDuration(300);
    animator->SetStartDelay(0);
    animator->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT));
}

ECode CSearchPanelView::Hide(
    /* [in] */ Boolean animate)
{
    if (mBar != NULL) {
        // This will indirectly cause show(FALSE, ...) to get called
        ICommandQueueCallbacks::Probe(mBar)->AnimateCollapsePanels(ICommandQueue::FLAG_EXCLUDE_NONE);
    }
    else {
        if (animate) {
            StartAbortAnimation();
        }
        else {
            SetVisibility(IView::INVISIBLE);
        }
    }
    return NOERROR;
}

ECode CSearchPanelView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Ignore hover events outside of this panel bounds since such events
    // generate spurious accessibility events with the panel content when
    // tapping outside of it, thus confusing the user.
    Float value = 0;
    event->GetX(&value);
    const Int32 x = (Int32) value;
    event->GetY(&value);
    const Int32 y = (Int32) value;
    Int32 w = 0, h = 0;
    if (x >= 0 && x < (GetWidth(&w), w) && y >= 0 && y < (GetHeight(&h), h)) {
        *result = FrameLayout::DispatchHoverEvent(event);
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CSearchPanelView::IsShowing(
    /* [out] */ Boolean* showing)
{
    VALIDATE_NOT_NULL(showing);
    Int32 v = 0;
    GetVisibility(&v);
    Boolean tmp = FALSE;
    *showing = v == IView::VISIBLE && (mCircle->IsAnimatingOut(&tmp), !tmp);
    return NOERROR;
}

ECode CSearchPanelView::SetBar(
    /* [in] */ IBaseStatusBar* bar)
{
    mBar = bar;
    return NOERROR;
}

ECode CSearchPanelView::IsAssistantAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<IIntent> intent;
    ISearchManager::Probe(obj)->GetAssistIntent(mContext, FALSE,
        IUserHandle::USER_CURRENT, (IIntent**)&intent);
    *result = intent.Get() != NULL;
    return NOERROR;
}

ECode CSearchPanelView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mLaunching || mLaunchPending) {
        *result = FALSE;
        return NOERROR;
    }
    using Elastos::Core::Math;
    Int32 action = 0;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            if (mHorizontal) {
                event->GetX(&mStartTouch);
            }
            else {
                event->GetY(&mStartTouch);
            }
            mDragging = FALSE;
            mDraggedFarEnough = FALSE;
            mCircle->Reset();
            break;
        }
        case IMotionEvent::ACTION_MOVE: {
            Float currentTouch = 0;
            if (mHorizontal) {
                event->GetX(&currentTouch);
            }
            else {
                event->GetY(&currentTouch);
            }
            Int32 v = 0;
            Boolean tmp = FALSE;
            if ((GetVisibility(&v), v) == IView::VISIBLE && !mDragging &&
                    ((mCircle->IsAnimationRunning(TRUE /* enterAnimation */, &tmp), tmp)
                            || Math::Abs(mStartTouch - currentTouch) > mThreshold)) {
                mStartDrag = currentTouch;
                mDragging = TRUE;
            }
            if (mDragging) {
                Float offset = Math::Max(mStartDrag - currentTouch, 0.0f);
                mCircle->SetDragDistance(offset);
                mDraggedFarEnough = Math::Abs(mStartTouch - currentTouch) > mThreshold;
                mCircle->SetDraggedFarEnough(mDraggedFarEnough);
            }
            break;
        }
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            if (mDraggedFarEnough) {
                Boolean tmp = FALSE;
                if (mCircle->IsAnimationRunning(TRUE  /* enterAnimation */, &tmp), tmp) {
                    mLaunchPending = TRUE;
                    mCircle->SetAnimatingOut(TRUE);
                    AutoPtr<Runnable3> run = new Runnable3(this);
                    mCircle->PerformOnAnimationFinished(run);
                }
                else {
                    StartExitAnimation();
                }
            }
            else {
                StartAbortAnimation();
            }
            break;
    }
    *result = TRUE;
    return NOERROR;
}

void CSearchPanelView::StartExitAnimation()
{
    mLaunchPending = FALSE;
    Int32 v = 0;
    if (mLaunching || (GetVisibility(&v), v) != IView::VISIBLE) {
        return;
    }
    mLaunching = TRUE;
    StartAssistActivity();
    Vibrate();
    mCircle->SetAnimatingOut(TRUE);
    AutoPtr<Runnable4> run = new Runnable4(this);
    mCircle->StartExitAnimation(run);
    AutoPtr<IViewPropertyAnimator> animator;
    mScrim->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(0.f);
    animator->SetDuration(300);
    animator->SetStartDelay(0);
    animator->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT));
}

ECode CSearchPanelView::SetHorizontal(
    /* [in] */ Boolean horizontal)
{
    mHorizontal = horizontal;
    mCircle->SetHorizontal(horizontal);
    return NOERROR;
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
