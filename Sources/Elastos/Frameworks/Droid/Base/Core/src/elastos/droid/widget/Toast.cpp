
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/widget/CToast.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/app/NotificationManager.h"
#include "elastos/droid/view/CGravity.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/widget/CToastTransientNotification.h"
#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IITransientNotification;
using Elastos::Droid::App::NotificationManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::CAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

const String Toast::TAG("Toast");
const Boolean Toast::localLOGV = FALSE;

AutoPtr<IINotificationManager> Toast::sService;

//==============================================================================
//          Toast::TN
//==============================================================================

CAR_INTERFACE_IMPL_2(Toast::TN, Object, IITransientNotification, IBinder);

Toast::TN::TN()
    : mGravity(0)
    , mX(0)
    , mY(0)
    , mHorizontalMargin(0.0f)
    , mVerticalMargin(0.0f)
{
    CWindowManagerLayoutParams::New((IWindowManagerLayoutParams**)&mParams);
    CHandler::New((IHandler**)&mHandler);
}

Toast::TN::~TN()
{}

ECode Toast::TN::constructor()
{
    // XXX This should be changed to use a Dialog, with a Theme.Toast
    // defined that sets up the layout params appropriately.

    mShow = new ShowAction(this);
    mHide = new HideAction(this);

    AutoPtr<IWindowManagerLayoutParams> params = mParams;

    IViewGroupLayoutParams::Probe(params)->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
    IViewGroupLayoutParams::Probe(params)->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
    params->SetFormat(IPixelFormat::TRANSLUCENT);
    params->SetWindowAnimations(R::style::Animation_Toast);
    params->SetType(IWindowManagerLayoutParams::TYPE_TOAST);
    params->SetTitle(CoreUtils::Convert("Toast"));
    params->SetFlags(IWindowManagerLayoutParams::FLAG_KEEP_SCREEN_ON
        | IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE
        | IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE);

    return NOERROR;
}

ECode Toast::TN::Show()
{
    if (localLOGV) {
        Logger::V(TAG, "SHOW: %p", this);
    }
    Boolean result;
    return mHandler->Post(mShow, &result);
}

ECode Toast::TN::Hide()
{
    if (localLOGV) {
        Logger::V(TAG, "HIDE: %p", this);
    }
    AddRef();
    Boolean result;
    return mHandler->Post(mHide, &result);
}

ECode Toast::TN::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("ToastTransientNotification");
    return NOERROR;
}

ECode Toast::TN::HandleShow()
{
    if (localLOGV) {
        Logger::V(TAG, "HANDLE SHOW: %p, mView= %p, mNextView= %p", this, mView.Get(), mNextView.Get());
    }
    if (mView != mNextView) {
        HandleHide();
        mView = mNextView;

        AutoPtr<IContext> context;
        mView->GetContext((IContext**)&context);
        String packageName;
        context->GetOpPackageName(&packageName);
        if (mWM == NULL) {
            AutoPtr<IContext> appContext;
            context->GetApplicationContext((IContext**)&appContext);
            if (appContext == NULL)
                appContext = context;

            AutoPtr<IInterface> service;
            appContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
            mWM = IWindowManager::Probe(service);
        }

        // We can resolve the Gravity here by using the Locale for getting
        // the layout direction
        AutoPtr<IConfiguration> config;
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetConfiguration((IConfiguration**)&config);
        Int32 layoutDirection;
        config->GetLayoutDirection(&layoutDirection);

        Int32 gravity = Gravity::GetAbsoluteGravity(mGravity, layoutDirection);
        mParams->SetGravity(gravity);

        if ((gravity & IGravity::HORIZONTAL_GRAVITY_MASK) == IGravity::FILL_HORIZONTAL) {
            ((CWindowManagerLayoutParams*)mParams.Get())->mHorizontalWeight = 1.0f;
        }
        if ((gravity & IGravity::VERTICAL_GRAVITY_MASK) == IGravity::FILL_VERTICAL) {
            ((CWindowManagerLayoutParams*)mParams.Get())->mVerticalWeight = 1.0f;
        }
        mParams->SetX(mX);
        mParams->SetY(mY);
        mParams->SetVerticalMargin(mVerticalMargin);
        mParams->SetHorizontalMargin(mHorizontalMargin);
        mParams->SetPackageName(packageName);

        AutoPtr<IViewParent> parent;
        mView->GetParent((IViewParent**)&parent);
        if (parent != NULL) {
            if (localLOGV) {
                Logger::V(TAG, "REMOVE! mView: %p in this: %p", mView.Get(), this);
            }
            IViewManager::Probe(mWM)->RemoveView(mView);
        }
        if (localLOGV) {
            Logger::V(TAG, "ADD! mView: %p in this: %p", mView.Get(), this);
        }
        IViewManager::Probe(mWM)->AddView(mView, IViewGroupLayoutParams::Probe(mParams));

        TrySendAccessibilityEvent();
    }
    return NOERROR;
}

ECode Toast::TN::TrySendAccessibilityEvent()
{
    AutoPtr<IContext> context;
    mView->GetContext((IContext**)&context);
    AutoPtr<IAccessibilityManager> accessibilityManager;
    CAccessibilityManager::GetInstance(context, (IAccessibilityManager**)&accessibilityManager);
    Boolean enabled;
    accessibilityManager->IsEnabled(&enabled);
    if (!enabled) {
        return NOERROR;
    }
    // treat toasts as notifications since they are used to
    // announce a transient piece of information to the user
    AutoPtr<IAccessibilityEvent> event;
    CAccessibilityEvent::Obtain(
            IAccessibilityEvent::TYPE_NOTIFICATION_STATE_CHANGED, (IAccessibilityEvent**)&event);

    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert("Toast"));

    String strPkgName;
    context->GetPackageName(&strPkgName);
    event->SetPackageName(CoreUtils::Convert(strPkgName));

    Boolean result;
    mView->DispatchPopulateAccessibilityEvent(event, &result);
    accessibilityManager->SendAccessibilityEvent(event);
    return NOERROR;
}

ECode Toast::TN::HandleHide()
{
    if (localLOGV) {
        Logger::V(TAG, "HANDLE HIDE: this: %p, mView= %p", this, mView.Get());
    }
    if (mView != NULL) {
        // note: checking parent() just to make sure the view has
        // been added...  i have seen cases where we get here when
        // the view isn't yet added, so let's try not to crash.
        AutoPtr<IViewParent> parent;
        mView->GetParent((IViewParent**)&parent);
        if (parent != NULL) {
            if (localLOGV) {
                Logger::V(TAG, "REMOVE! mView: %p, in this: %p", mView.Get(), this);
            }
            if (mWM) {
                IViewManager::Probe(mWM)->RemoveView(mView);
            }
        }

        mView = NULL;
    }

    return NOERROR;
}

ECode Toast::TN::SetNextView(
    /* [in] */ IView* nextView)
{
    mNextView = nextView;
    return NOERROR;
}

ECode Toast::TN::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    return NOERROR;
}

ECode Toast::TN::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = mGravity;
    return NOERROR;
}

ECode Toast::TN::SetX(
    /* [in] */ Int32 x)
{
    mX = x;
    return NOERROR;
}

ECode Toast::TN::GetX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mX;
    return NOERROR;
}

ECode Toast::TN::SetY(
    /* [in] */ Int32 y)
{
    mY = y;
    return NOERROR;
}

ECode Toast::TN::GetY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mY;
    return NOERROR;
}

ECode Toast::TN::SetHorizontalMargin(
    /* [in] */ Float horizontalMargin)
{
    mHorizontalMargin = horizontalMargin;
    return NOERROR;
}

ECode Toast::TN::GetHorizontalMargin(
    /* [out] */ Float* horizontalMargin)
{
    VALIDATE_NOT_NULL(horizontalMargin);
    *horizontalMargin = mHorizontalMargin;
    return NOERROR;
}

ECode Toast::TN::SetVerticalMargin(
    /* [in] */ Float verticalMargin)
{
    mVerticalMargin = verticalMargin;
    return NOERROR;
}

ECode Toast::TN::GetVerticalMargin(
    /* [out] */ Float* verticalMargin)
{
    VALIDATE_NOT_NULL(verticalMargin);
    *verticalMargin = mVerticalMargin;
    return NOERROR;
}

//==============================================================================
//          Toast::TN::ShowAction::ShowAction
//==============================================================================
Toast::TN::ShowAction::ShowAction(
    /* [in] */ TN* host)
    : mHost(host)
{
}

ECode Toast::TN::ShowAction::Run()
{
    mHost->HandleShow();
    return NOERROR;
}

//==============================================================================
//          Toast::HideAction
//==============================================================================
Toast::TN::HideAction::HideAction(
    /* [in] */ TN* host)
    : mHost(host)
{}

ECode Toast::TN::HideAction::Run()
{
    mHost->HandleHide();
    // Don't do this in handleHide() because it is also invoked by handleShow()
    mHost->mNextView = NULL;
    return NOERROR;
}

//==============================================================================
//          Toast
//==============================================================================

CAR_INTERFACE_IMPL(Toast, Object, IToast);

Toast::Toast()
    : mDuration(0)
{}

Toast::~Toast()
{}

ECode Toast::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    mTN = new TN();

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 y;
    res->GetDimensionPixelSize(R::dimen::toast_y_offset, &y);
    mTN->SetY(y);

    Int32 gravity;
    res->GetInteger(R::integer::config_toastDefaultGravity, &gravity);
    mTN->SetGravity(gravity);

    return NOERROR;
}

ECode Toast::Show()
{
    if (mNextView == NULL) {
        Logger::E("Toast", "setView must have been called");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IINotificationManager> service = GetService();
    String pkgName;
    mContext->GetOpPackageName(&pkgName);

    AutoPtr<TN> tn = mTN;
    tn->SetNextView(mNextView);

    return service->EnqueueToast(pkgName, (IITransientNotification*)tn.Get(), mDuration);
}

ECode Toast::Cancel()
{
    mTN->Hide();

    String pkgName;
    mContext->GetPackageName(&pkgName);
    AutoPtr<IINotificationManager> service = GetService();
    return service->CancelToast(pkgName, (IITransientNotification*)mTN.Get());
}

ECode Toast::SetView(
    /* [in] */ IView* view)
{
    mNextView = view;
    return NOERROR;
}

ECode Toast::GetView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mNextView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode Toast::SetDuration(
    /* [in] */ Int32 duration)
{
    mDuration = duration;
    return NOERROR;
}

ECode Toast::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = mDuration;
    return NOERROR;
}

ECode Toast::SetMargin(
    /* [in] */ Float horizontalMargin,
    /* [in] */ Float verticalMargin)
{
    mTN->SetHorizontalMargin(horizontalMargin);
    mTN->SetVerticalMargin(verticalMargin);
    return NOERROR;
}

ECode Toast::GetHorizontalMargin(
    /* [out] */ Float* horizontalMargin)
{
    VALIDATE_NOT_NULL(horizontalMargin);
    *horizontalMargin = mTN->mHorizontalMargin;
    return NOERROR;
}

ECode Toast::GetVerticalMargin(
    /* [out] */ Float* verticalMargin)
{
    VALIDATE_NOT_NULL(verticalMargin);
    *verticalMargin = mTN->mVerticalMargin;
    return NOERROR;
}

ECode Toast::SetGravity(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 xOffset,
    /* [in] */ Int32 yOffset)
{
    mTN->SetGravity(gravity);
    mTN->SetX(xOffset);
    mTN->SetY(yOffset);
    return NOERROR;
}

ECode Toast::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = mTN->mGravity;
    return NOERROR;
}

ECode Toast::GetXOffset(
    /* [out] */ Int32* xOffset)
{
    VALIDATE_NOT_NULL(xOffset);
    *xOffset = mTN->mX;
    return NOERROR;
}

ECode Toast::GetYOffset(
    /* [out] */ Int32* yOffset)
{
    VALIDATE_NOT_NULL(yOffset);
    *yOffset = mTN->mY;

    return NOERROR;
}

ECode Toast::MakeText(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 duration,
    /* [out] */ IToast** toast)
{
    VALIDATE_NOT_NULL(toast);

    AutoPtr<IToast> result;
    CToast::New(context, (IToast**)&result);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);

    AutoPtr<IView> v;
    inflater->Inflate(R::layout::transient_notification, NULL, (IView**)&v);
    assert(v != NULL);

    AutoPtr<IView> tv;
    v->FindViewById(R::id::message, (IView**)&tv);
    AutoPtr<ITextView> textView = ITextView::Probe(tv);
    textView->SetText(text);

    result->SetView(v);
    result->SetDuration(duration);
    *toast = result;
    REFCOUNT_ADD(*toast);

    return NOERROR;
}

ECode Toast::MakeText(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 duration,
    /* [out] */ IToast** toast)
{
    VALIDATE_NOT_NULL(toast);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ICharSequence> text;
    res->GetText(resId, (ICharSequence**)&text);
    return MakeText(context, text, duration, toast);
}

ECode Toast::SetText(
    /* [in] */ Int32 resId)
{
    AutoPtr<ICharSequence> text;
    mContext->GetText(resId, (ICharSequence**)&text);
    return SetText(text);
}

ECode Toast::SetText(
    /* [in] */ ICharSequence* s)
{
    if (mNextView == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ITextView> tv;
    mNextView->FindViewById(R::id::message, (IView**)&tv);

    if (tv == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return tv->SetText(s);
}

AutoPtr<IINotificationManager> Toast::GetService()
{
    if (sService != NULL) {
        return sService;
    }

    String name("notification");
    AutoPtr<IInterface> obj = ServiceManager::GetService(name);
    sService = IINotificationManager::Probe(obj.Get());
    return sService;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
