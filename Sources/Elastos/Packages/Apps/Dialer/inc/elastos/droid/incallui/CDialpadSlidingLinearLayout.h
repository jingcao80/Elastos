
#ifndef __ELASTOS_DROID_INCALLUI_CDIALPADSLIDINGLINEARLAYOUT_H__
#define __ELASTOS_DROID_INCALLUI_CDIALPADSLIDINGLINEARLAYOUT_H__

#include "_Elastos_Droid_InCallUI_CDialpadSlidingLinearLayout.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * LinearLayout with getter and setter methods for the translationY property using floats,
 * for animation purposes.
 */
CarClass(CDialpadSlidingLinearLayout)
    , public LinearLayout
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI_(Float) GetYFraction();

    CARAPI_(void) SetYFraction(
        /* [in] */ Float yFraction);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CDIALPADSLIDINGLINEARLAYOUT_H__
