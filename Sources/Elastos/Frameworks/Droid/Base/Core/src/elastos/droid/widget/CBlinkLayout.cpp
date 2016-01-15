
#include "elastos/droid/widget/CBlinkLayout.h"
#include "elastos/droid/os/CHandler.h"

using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_ViewGroup;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::EIID_IViewManager;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IOnScrollChangedListener;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityEventSource;
using Elastos::Droid::Widget::EIID_IFrameLayout;

namespace Elastos {
namespace Droid {
namespace Widget {

//=======================================================================================
// BlinkLayout::BlickHandlerCallback
//=======================================================================================
const Int32 BlinkLayout::MESSAGE_BLINK = 0x42;
const Int32 BlinkLayout::BLINK_DELAY = 500;

CAR_INTERFACE_IMPL(BlinkLayout::BlickHandlerCallback, IHandlerCallback)

ECode BlinkLayout::BlickHandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 what;
    msg->GetWhat(&what);
    if (what == BlinkLayout::MESSAGE_BLINK) {
        AutoPtr<IInterface> obj;
        mWeakHost->Resolve(EIID_IInterface, (IInterface**)&obj);
        if (obj) {
            IFrameLayout* frameLayout = IFrameLayout::Probe(obj);
            CBlinkLayout* blinkLayout = (CBlinkLayout*)frameLayout;
            if (blinkLayout->mBlink) {
                blinkLayout->mBlinkState = !blinkLayout->mBlinkState;
                blinkLayout->MakeBlink();
            }
            blinkLayout->Invalidate();
        }

        *result = TRUE;
    }

    return NOERROR;
}

//=======================================================================================
// BlinkLayout
//=======================================================================================

BlinkLayout::BlinkLayout()
    : mBlink(FALSE)
    , mBlinkState(FALSE)
{
}

BlinkLayout::BlinkLayout(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
    , mBlink(FALSE)
    , mBlinkState(FALSE)
{
}

ECode BlinkLayout::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(FrameLayout::Init(context, attrs));
    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)this->Probe(EIID_IWeakReferenceSource);
    AutoPtr<IWeakReference> wr;
    wrs->GetWeakReference((IWeakReference**)&wr);
    AutoPtr<IHandlerCallback> callback = new BlickHandlerCallback(wr);
    return CHandler::New(callback, TRUE, (IHandler**)&mBlinkHandler);
}

ECode BlinkLayout::OnAttachedToWindow()
{
    FAIL_RETURN(FrameLayout::OnAttachedToWindow());

    mBlink = TRUE;
    mBlinkState = TRUE;

    MakeBlink();
    return NOERROR;
}

ECode BlinkLayout::OnDetachedFromWindow()
{
    FAIL_RETURN(FrameLayout::OnDetachedFromWindow());

    mBlink = FALSE;
    mBlinkState = FALSE;

    return mBlinkHandler->RemoveMessages(MESSAGE_BLINK);
}

void BlinkLayout::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mBlinkState) {
        FrameLayout::DispatchDraw(canvas);
    }
}

void BlinkLayout::MakeBlink()
{
    Boolean result;
    mBlinkHandler->SendEmptyMessageDelayed(MESSAGE_BLINK, BLINK_DELAY, &result);
}

//=======================================================================================
// CBlinkLayout
//=======================================================================================
IVIEW_METHODS_IMPL(CBlinkLayout, BlinkLayout)
IVIEWGROUP_METHODS_IMPL(CBlinkLayout, BlinkLayout)
IVIEWPARENT_METHODS_IMPL(CBlinkLayout, BlinkLayout)
IVIEWMANAGER_METHODS_IMPL(CBlinkLayout, BlinkLayout)
IDRAWABLECALLBACK_METHODS_IMPL(CBlinkLayout, BlinkLayout)
IKEYEVENTCALLBACK_METHODS_IMPL(CBlinkLayout, BlinkLayout)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CBlinkLayout, BlinkLayout)
IFRAMELAYOUT_METHODS_IMPL(CBlinkLayout, BlinkLayout)

PInterface CBlinkLayout::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CBlinkLayout::Probe(riid);
}

ECode CBlinkLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return BlinkLayout::Init(context, attrs);
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
