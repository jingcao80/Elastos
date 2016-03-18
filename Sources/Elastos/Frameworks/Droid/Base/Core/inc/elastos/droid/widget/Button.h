
#ifndef __ELASTOS_DROID_WIDGET_BUTTON_H__
#define __ELASTOS_DROID_WIDGET_BUTTON_H__

#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC Button
    : public TextView
    , public IButton
{
public:
    CAR_INTERFACE_DECL()

    Button();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyleAttr = R::attr::buttonStyle,
        /* [in] */ Int32 defStyleRes = 0);

    //@Override
    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_BUTTON_H__
