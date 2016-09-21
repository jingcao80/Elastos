
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/phone/common/dialpad/DigitsEditText.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Text::IInputType;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

ECode DigitsEditText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(ResizingTextEditText::constructor(context, attrs));
    Int32 itype;
    GetInputType(&itype);
    SetInputType(itype | IInputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS);
    SetShowSoftInputOnFocus(FALSE);
    return NOERROR;
}

void DigitsEditText::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    ResizingTextEditText::OnFocusChanged(focused, direction, previouslyFocusedRect);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    AutoPtr<IInputMethodManager> imm = IInputMethodManager::Probe(obj);
    Boolean isActive;
    if (imm != NULL && (imm->IsActive(this, &isActive), isActive)) {
        AutoPtr<IBinder> token;
        GetApplicationWindowToken((IBinder**)&token);
        Boolean result;
        imm->HideSoftInputFromWindow(token, 0, &result);
    }
}

ECode DigitsEditText::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    ResizingTextEditText::OnTouchEvent(event, res);
    // Must be done after super.onTouchEvent()
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    AutoPtr<IInputMethodManager> imm = IInputMethodManager::Probe(obj);
    Boolean isActive;
    if (imm != NULL && (imm->IsActive(this, &isActive), isActive)) {
        AutoPtr<IBinder> token;
        GetApplicationWindowToken((IBinder**)&token);
        Boolean result;
        imm->HideSoftInputFromWindow(token, 0, &result);
    }
    return NOERROR;
}

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
