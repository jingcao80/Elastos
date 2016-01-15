#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_TOGGLESLIDER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_TOGGLESLIDER_H__

#include "elastos/droid/widget/RelativeLayout.h"

using Elastos::Droid::Widget::RelativeLayout;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class ToggleSlider : public RelativeLayout
{
public:
    ToggleSlider();

    ToggleSlider(
        /* [in] */ IContext* context);

    ToggleSlider(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ToggleSlider(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetOnChangedListener(
        /* [in] */ IToggleSliderListener* listener);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    Boolean IsChecked();

    CARAPI SetMax(
        /* [in] */ Int32 maxValue);

    CARAPI SetValue(
        /* [in] */ Int32 value);

    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

    CARAPI OnProgressChanged(
        /* [in] */ ISeekBar* seekBar,
        /* [in] */ Int32 progress,
        /* [in] */ Boolean fromUser);

    CARAPI OnStartTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    CARAPI OnStopTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

protected:
    CARAPI Init(
        /* [in] */ IContext* context);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    virtual CARAPI OnAttachedToWindow();

private:
    AutoPtr<IToggleSliderListener> mListener;
    Boolean mTracking;

    AutoPtr<ICompoundButton> mToggle;
    AutoPtr<ISeekBar> mSlider;
    AutoPtr<ITextView> mLabel;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_TOGGLESLIDER_H__
