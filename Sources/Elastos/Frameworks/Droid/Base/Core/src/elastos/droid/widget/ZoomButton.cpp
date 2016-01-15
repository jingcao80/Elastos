
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/ZoomButton.h"
#include "elastos/droid/os/CHandler.h"

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

//=============================================================================
//                  ZoomButton::ZoomButtonRunnable
//=============================================================================
ZoomButton::ZoomButtonRunnable::ZoomButtonRunnable(
    /* [in] */ ZoomButton* host)
    : mHost(host)
{
}

ECode ZoomButton::ZoomButtonRunnable::Run()
{
    Boolean value = FALSE;
    mHost->HasOnClickListeners(&value);
    if (value && mHost->mIsInLongpress && (mHost->IsEnabled(&value), value)) {
        mHost->CallOnClick(&value);
        mHost->mHandler->PostDelayed(this, mHost->mZoomSpeed, &value);
    }
    return NOERROR;
}

//=============================================================================
//                  ZoomButton::LongClickListener
//=============================================================================
CAR_INTERFACE_IMPL(ZoomButton::LongClickListener, Object, IViewOnLongClickListener);
ZoomButton::LongClickListener::LongClickListener(
    /* [in] */ ZoomButton* host)
    : mHost(host)
{}

ECode ZoomButton::LongClickListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mHost->OnLongClick(v, result);
}


CAR_INTERFACE_IMPL_2(ZoomButton, ImageButton, IZoomButton, IViewOnLongClickListener);
ZoomButton::ZoomButton()
    : mZoomSpeed(1000)
    , mIsInLongpress(FALSE)
{
    mRunnable = new ZoomButtonRunnable(this);
}

ECode ZoomButton::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ZoomButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode ZoomButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ZoomButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ImageButton::constructor(context, attrs, defStyleAttr, defStyleRes);
    CHandler::New((IHandler**)&mHandler);
    AutoPtr<IViewOnLongClickListener> listener = new LongClickListener(this);
    SetOnLongClickListener(listener);
    return NOERROR;
}

ECode ZoomButton::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action = 0;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_CANCEL || action == IMotionEvent::ACTION_UP) {
        mIsInLongpress = FALSE;
    }
    return ImageButton::OnTouchEvent(event, result);
}

ECode ZoomButton::SetZoomSpeed(
    /* [in] */ Int64 speed)
{
    mZoomSpeed = speed;
    return NOERROR;
}

ECode ZoomButton::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mIsInLongpress = TRUE;
    Boolean res = FALSE;
    mHandler->Post(mRunnable, &res);
    *result = TRUE;
    return NOERROR;
}

ECode ZoomButton::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mIsInLongpress = FALSE;
    return ImageButton::OnKeyUp(keyCode, event, result);
}

ECode ZoomButton::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (!enabled) {
        SetPressed(FALSE);
    }
    ImageButton::SetEnabled(enabled);
    return NOERROR;
}

ECode ZoomButton::DispatchUnhandledMove(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ClearFocus();
    return ImageButton::DispatchUnhandledMove(focused, direction, result);
}

ECode ZoomButton::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ImageButton::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(String("CZoomButton"), (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode ZoomButton::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ImageButton::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(String("CZoomButton"), (ICharSequence**)&seq));
    info->SetClassName(seq);
    return NOERROR;
}

} //namespace Widget
} //namespace Droid
} //namespace Elastos

