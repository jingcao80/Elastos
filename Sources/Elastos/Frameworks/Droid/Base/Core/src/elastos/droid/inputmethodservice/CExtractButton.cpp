
#include "elastos/droid/inputmethodservice/CExtractButton.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CAR_OBJECT_IMPL(CExtractButton);
ECode CExtractButton::constructor(
    /* [in] */ IContext* context)
{
    return Button::constructor(context, NULL);
}

ECode CExtractButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Button::constructor(context, attrs, R::attr::buttonStyle);
}

ECode CExtractButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CExtractButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return Button::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CExtractButton::HasWindowFocus(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 v = 0;
    *res = (IsEnabled(res), *res) && (GetVisibility(&v), v) == IView::VISIBLE;
    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
