
#include "elastos/droid/widget/ViewFlipper.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::R;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

String ViewFlipper::TAG("ViewFlipper");
const Boolean ViewFlipper::LOGD = FALSE;
const Int32 ViewFlipper::DEFAULT_INTERVAL = 3000;
const Int32 ViewFlipper::FLIP_MSG = 1;

ViewFlipper::MyHandler::MyHandler(
    /* [in] */ ViewFlipper* host)
    : mHost(host)
{}

ECode ViewFlipper::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);

    if (what == ViewFlipper::FLIP_MSG) {
        if (mHost->mRunning) {
            mHost->ShowNext();
            Boolean result = FALSE;
            SendEmptyMessageDelayed(ViewFlipper::FLIP_MSG, mHost->mFlipInterval, &result);
        }
    }
    return NOERROR;
}

ViewFlipper::FlipperReceiver::FlipperReceiver(
    /* [in] */ ViewFlipper* host)
    : mHost(host)
{}

ECode ViewFlipper::FlipperReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    assert(intent != NULL);
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mHost->mUserPresent = FALSE;
        mHost->UpdateRunning();
    }
    else if (IIntent::ACTION_USER_PRESENT.Equals(action)) {
        mHost->mUserPresent = TRUE;
        mHost->UpdateRunning(FALSE);
    }
    return NOERROR;
}

ECode ViewFlipper::FlipperReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("ViewFlipper::FlipperReceiver: ");
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

CAR_INTERFACE_IMPL(ViewFlipper, ViewAnimator, IViewFlipper);
ViewFlipper::ViewFlipper()
    : mFlipInterval(DEFAULT_INTERVAL)
    , mAutoStart(FALSE)
    , mRunning(FALSE)
    , mStarted(FALSE)
    , mVisible(FALSE)
    , mUserPresent(TRUE)
{
    mReceiver = new FlipperReceiver(this);
    mHandler = new MyHandler(this);
}

ECode ViewFlipper::constructor(
    /* [in] */ IContext* ctx)
{
    return ViewAnimator::constructor(ctx);
}

ECode ViewFlipper::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(ViewAnimator::constructor(ctx, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ViewFlipper);
    AutoPtr<ITypedArray> a;
    ctx->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetInt32(R::styleable::ViewFlipper_flipInterval, DEFAULT_INTERVAL, &mFlipInterval);
    a->GetBoolean(R::styleable::ViewFlipper_autoStart, FALSE, &mAutoStart);
    a->Recycle();
    return NOERROR;
}

ECode ViewFlipper::OnAttachedToWindow()
{
    ViewAnimator::OnAttachedToWindow();

    // Listen for broadcasts related to user-presence
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IIntent::ACTION_USER_PRESENT);

    // OK, this is gross but needed. This class is supported by the
    // remote views machanism and as a part of that the remote views
    // can be inflated by a context for another user without the app
    // having interact users permission - just for loading resources.
    // For exmaple, when adding widgets from a user profile to the
    // home screen. Therefore, we register the receiver as the current
    // user not the one the context is for.
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IUserHandle> uh;
    Process::MyUserHandle((IUserHandle**)&uh);
    AutoPtr<IIntent> intent;
    context->RegisterReceiverAsUser(mReceiver, uh, filter, String(NULL), mHandler, (IIntent**)&intent);

    if (mAutoStart) {
        // Automatically start when requested
        StartFlipping();
    }
    return NOERROR;
}

ECode ViewFlipper::OnDetachedFromWindow()
{
    ViewAnimator::OnDetachedFromWindow();
    mVisible = FALSE;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->UnregisterReceiver(mReceiver);
    UpdateRunning();
    return NOERROR;
}

void ViewFlipper::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    ViewAnimator::OnWindowVisibilityChanged(visibility);
    mVisible = visibility == IView::VISIBLE;
    UpdateRunning(FALSE);
}

ECode ViewFlipper::SetFlipInterval(
    /* [in] */ Int32 milliseconds)
{
    mFlipInterval = milliseconds;
    return NOERROR;
}

ECode ViewFlipper::StartFlipping()
{
    mStarted = TRUE;
    UpdateRunning();
    return NOERROR;
}

ECode ViewFlipper::StopFlipping()
{
    mStarted = FALSE;
    UpdateRunning();
    return NOERROR;
}

ECode ViewFlipper::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewAnimator::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> name;
    CString::New(String("CViewFlipper"), (ICharSequence**)&name);
    IAccessibilityRecord::Probe(event)->SetClassName(name);
    return NOERROR;
}

ECode ViewFlipper::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewAnimator::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> name;
    CString::New(String("CViewFlipper"), (ICharSequence**)&name);
    info->SetClassName(name);
    return NOERROR;
}

void ViewFlipper::UpdateRunning()
{
    UpdateRunning(TRUE);
}

void ViewFlipper::UpdateRunning(
    /* [in] */ Boolean flipNow)
{
    Boolean running = mVisible && mStarted && mUserPresent;
    if (running != mRunning) {
        if (running) {
            ShowOnly(mWhichChild, flipNow);
            Boolean result FALSE;
            mHandler->SendEmptyMessageDelayed(FLIP_MSG, mFlipInterval, &result);
        }
        else {
            mHandler->RemoveMessages(FLIP_MSG);
        }
        mRunning = running;
    }
    // if (LOGD) {
    //     Log.d(TAG, "updateRunning() mVisible=" + mVisible + ", mStarted=" + mStarted
    //             + ", mUserPresent=" + mUserPresent + ", mRunning=" + mRunning);
    // }
}

ECode ViewFlipper::IsFlipping(
    /* [out] */ Boolean* flipping)
{
    VALIDATE_NOT_NULL(flipping);
    *flipping = mStarted;
    return NOERROR;
}

ECode ViewFlipper::SetAutoStart(
    /* [in] */ Boolean autoStart)
{
    mAutoStart = autoStart;
    return NOERROR;
}

ECode ViewFlipper::IsAutoStart(
    /* [out] */ Boolean* start)
{
    VALIDATE_NOT_NULL(start);
    *start = mAutoStart;
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
