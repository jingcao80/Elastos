
#ifndef __ELASTOS_DROID_WIDGET_RADIOBUTTON_H__
#define __ELASTOS_DROID_WIDGET_RADIOBUTTON_H__

#include "elastos/droid/widget/CompoundButton.h"

namespace Elastos {
namespace Droid {
namespace Widget {

class RadioButton
    : public CompoundButton
    , public IRadioButton
{
public:
    CAR_INTERFACE_DECL();

    RadioButton();

    ~RadioButton();

    CARAPI constructor(
        /* [in] */ IContext * context);

    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IAttributeSet * attrs);

    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IAttributeSet * attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * {@inheritDoc}
     * <p>
     * If the radio button is already checked, this method will not toggle the radio button.
     */
    // @Override
    virtual CARAPI Toggle();

    //@Override
    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);
};


} // namespace Widget
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_RADIOBUTTON_H__
