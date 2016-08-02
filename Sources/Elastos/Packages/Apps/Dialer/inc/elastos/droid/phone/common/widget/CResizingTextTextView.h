
#ifndef __ELASTOS_DROID_PHONE_COMMON_WIDGET_CRESIZINGTEXTTEXTVIEW_H__
#define __ELASTOS_DROID_PHONE_COMMON_WIDGET_CRESIZINGTEXTTEXTVIEW_H__

#include "_Elastos_Droid_Phone_Common_Widget_CResizingTextTextView.h"
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Widget {

/**
 * TextView which resizes dynamically with respect to text length.
 */
CarClass(CResizingTextTextView)
    , public TextView
{
public:
    CResizingTextTextView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 lengthBefore,
        /* [in] */ Int32 lengthAfter);

    // @Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    Int32 mOriginalTextSize;
    Int32 mMinTextSize;
};

} // namespace Widget
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_COMMON_WIDGET_CRESIZINGTEXTTEXTVIEW_H__
