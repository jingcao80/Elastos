
#include "elastos/droid/phone/common/util/ViewUtil.h"
#include "elastos/droid/phone/common/widget/CResizingTextTextView.h"
#include "R.h"

using Elastos::Droid::Phone::Common::Util::ViewUtil;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Widget {

CResizingTextTextView::CResizingTextTextView()
    : mOriginalTextSize(0)
    , mMinTextSize(0)
{}

ECode CResizingTextTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(TextView::constructor(context, attrs));
    Float size;
    GetTextSize(&size);
    mOriginalTextSize = size;
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(Elastos::Droid::Dialer::R::styleable::ResizingText);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetDimension(Elastos::Droid::Dialer::R::styleable::ResizingText_resizing_text_min_size,
            mOriginalTextSize, &size);
    mMinTextSize = size;
    a->Recycle();
    return NOERROR;
}

ECode CResizingTextTextView::OnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 lengthBefore,
    /* [in] */ Int32 lengthAfter)
{
    TextView::OnTextChanged(text, start, lengthBefore, lengthAfter);
    ViewUtil::ResizeText(this, mOriginalTextSize, mMinTextSize);
    return NOERROR;
}

ECode CResizingTextTextView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    TextView::OnSizeChanged(w, h, oldw, oldh);
    ViewUtil::ResizeText(this, mOriginalTextSize, mMinTextSize);
    return NOERROR;
}

} // namespace Widget
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
