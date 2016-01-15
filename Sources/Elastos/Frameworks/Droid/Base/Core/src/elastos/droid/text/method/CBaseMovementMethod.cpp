#include "elastos/droid/text/method/CBaseMovementMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_OBJECT_IMPL(CBaseMovementMethod)

ECode CBaseMovementMethod::Initialize(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text)
{
    BaseMovementMethod::Initialize(widget,text);
    return NOERROR;
}

ECode CBaseMovementMethod::OnKeyDown(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result =  BaseMovementMethod::OnKeyDown(widget, text, keyCode, event);
    return NOERROR;
}

ECode CBaseMovementMethod::OnKeyUp(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result =  BaseMovementMethod::OnKeyUp(widget, text, keyCode, event);
    return NOERROR;
}

ECode CBaseMovementMethod::OnKeyOther(
    /* [in] */ ITextView* view,
    /* [in] */ ISpannable* text,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result =  BaseMovementMethod::OnKeyOther(view, text, event);
    return NOERROR;
}

ECode CBaseMovementMethod::OnTakeFocus(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 direction)
{
    BaseMovementMethod::OnTakeFocus(widget, text, direction);
    return NOERROR;
}

ECode CBaseMovementMethod::OnTrackballEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result =  BaseMovementMethod::OnTrackballEvent(widget, text, event);
    return NOERROR;
}

ECode CBaseMovementMethod::OnTouchEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result =  BaseMovementMethod::OnTouchEvent(widget, text, event);
    return NOERROR;
}

ECode CBaseMovementMethod::OnGenericMotionEvent(
    /* [in] */ ITextView* widget,
    /* [in] */ ISpannable* text,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result =  BaseMovementMethod::OnGenericMotionEvent(widget, text, event);
    return NOERROR;
}

ECode CBaseMovementMethod::CanSelectArbitrarily(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result =  BaseMovementMethod::CanSelectArbitrarily();
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

