
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/widget/ImageButton.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Widget::ImageView;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Widget {

const String ImageButton::IMAGEBUTTON_NAME("ImageButton");
CAR_INTERFACE_IMPL(ImageButton, ImageView, IImageButton);
ECode ImageButton::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ImageButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::imageButtonStyle);
}

ECode ImageButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ImageButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(ImageView::constructor(context, attrs, defStyleAttr, defStyleRes));
    SetFocusable(TRUE);
    return NOERROR;
}

Boolean ImageButton::OnSetAlpha(
    /* [in] */ Int32 alpha)
{
    return FALSE;
}

ECode ImageButton::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ImageView::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(IMAGEBUTTON_NAME, (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode ImageButton::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ImageView::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(IMAGEBUTTON_NAME, (ICharSequence**)&seq));
    info->SetClassName(seq);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
