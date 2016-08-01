
#ifndef __ELASTOS_DROID_INCALLUI_CHOVERIGNORINGLINEARLAYOUT_H__
#define __ELASTOS_DROID_INCALLUI_CHOVERIGNORINGLINEARLAYOUT_H__

#include "_Elastos_Droid_InCallUI_CHoverIgnoringLinearLayout.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * LinearLayout that always returns true for onHoverEvent callbacks, to fix
 * problems with accessibility due to the dialpad overlaying other fragments.
 */
CarClass(CHoverIgnoringLinearLayout)
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

    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CHOVERIGNORINGLINEARLAYOUT_H__
