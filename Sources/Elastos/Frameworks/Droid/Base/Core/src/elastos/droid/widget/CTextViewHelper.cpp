#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CTextViewHelper.h"
#include "elastos/droid/widget/TextView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

ECode CTextViewHelper::GetTextColors(
    /* [in] */ IContext* context,
    /* [in] */ ITypedArray* attrs,
    /* [out] */ IColorStateList** color)
{
    VALIDATE_NOT_NULL(color);
    AutoPtr<IColorStateList> list = TextView::GetTextColors(context, attrs);
    *color = list;
    REFCOUNT_ADD(*color);
    return NOERROR;
}

/**
 * Returns the default color from the TextView_textColor attribute
 * from the AttributeSet, if set, or the default color from the
 * TextAppearance_textColor from the TextView_textAppearance attribute,
 * if TextView_textColor was not set directly.
 */
ECode CTextViewHelper::GetTextColor(
    /* [in] */ IContext* context,
    /* [in] */ ITypedArray* attrs,
    /* [in] */ Int32 def,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = TextView::GetTextColor(context, attrs, def);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
