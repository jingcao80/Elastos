
#ifndef __ELASTOS_DROID_WIDGET_CHECKBOX_H__
#define __ELASTOS_DROID_WIDGET_CHECKBOX_H__

#include "elastos/droid/widget/CompoundButton.h"

namespace Elastos {
namespace Droid {
namespace Widget {

class CheckBox
    : public CompoundButton
    , public ICheckBox
{
public:
    CAR_INTERFACE_DECL()

    CheckBox();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);


    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos
#endif //__CHECKBOXBUTTON_H__
