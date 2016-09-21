
#include "elastos/droid/phone/common/util/ViewUtil.h"
#include "elastos/droid/phone/common/widget/ResizingTextEditText.h"

#ifdef TELESERVICE_USE_PHONECOMMON
#include "../../../../../../../../Service/Telephony/R.h"
#endif

#ifdef DIALER_USE_PHONECOMMON
#include "../../../../../../../Dialer/R.h"
#endif

using Elastos::Droid::Phone::Common::Util::ViewUtil;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Widget {

ResizingTextEditText::ResizingTextEditText()
    : mOriginalTextSize(0)
    , mMinTextSize(0)
{}

ECode ResizingTextEditText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(EditText::constructor(context, attrs));
    Float size;
    GetTextSize(&size);
    mOriginalTextSize = size;
    AutoPtr<ITypedArray> a;

    #ifdef TELESERVICE_USE_PHONECOMMON
        AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(Elastos::Droid::TeleService::R::styleable::ResizingText);
        context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
        a->GetDimension(Elastos::Droid::TeleService::R::styleable::ResizingText_resizing_text_min_size,
                mOriginalTextSize, &size);
    #endif

    #ifdef DIALER_USE_PHONECOMMON
        AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(Elastos::Droid::Dialer::R::styleable::ResizingText);
        context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
        a->GetDimension(Elastos::Droid::Dialer::R::styleable::ResizingText_resizing_text_min_size,
                mOriginalTextSize, &size);
    #endif

    mMinTextSize = size;
    a->Recycle();
    return NOERROR;
}

ECode ResizingTextEditText::OnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 lengthBefore,
    /* [in] */ Int32 lengthAfter)
{
    EditText::OnTextChanged(text, start, lengthBefore, lengthAfter);
    ViewUtil::ResizeText(this, mOriginalTextSize, mMinTextSize);
    return NOERROR;
}

ECode ResizingTextEditText::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    EditText::OnSizeChanged(w, h, oldw, oldh);
    ViewUtil::ResizeText(this, mOriginalTextSize, mMinTextSize);
    return NOERROR;
}

} // namespace Widget
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
