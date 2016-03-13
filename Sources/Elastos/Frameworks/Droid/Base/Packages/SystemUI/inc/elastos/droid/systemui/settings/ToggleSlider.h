
#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_TOGGLESLIDER_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_TOGGLESLIDER_H__

#include "_SystemUI.h"
#include <elastos/droid/widget/RelativeLayout.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBrightnessMirrorController;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::RelativeLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

class ToggleSlider
    : public RelativeLayout
    , public IToggleSlider
{
private:
    class MyCheckListener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyCheckListener(
            /* [in] */ ToggleSlider* host);

        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        ToggleSlider* mHost;
    };

    class MySeekListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MySeekListener(
            /* [in] */ ToggleSlider* host);

        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        CARAPI OnStopTrackingTouch(
             /* [in] */ ISeekBar* seekBar);

    private:
        ToggleSlider* mHost;
    };

public:
    CAR_INTERFACE_DECL()

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

    CARAPI SetMirror(
        /* [in] */ IToggleSlider* toggleSlider);

    CARAPI SetMirrorController(
        /* [in] */ IBrightnessMirrorController* c);

    CARAPI SetOnChangedListener(
        /* [in] */ IToggleSliderListener* l);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI IsChecked(
        /* [out] */ Boolean* isChecked);

    CARAPI SetMax(
        /* [in] */ Int32 max);

    CARAPI SetValue(
        /* [in] */ Int32 value);

    // @Override
    CARAPI OnStartTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    // @Override
    CARAPI OnStopTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

protected:
    // @Override
    CARAPI OnAttachedToWindow();

private:
    AutoPtr<IToggleSliderListener> mListener;
    Boolean mTracking;

    AutoPtr<ICompoundButton> mToggle;
    AutoPtr<ISeekBar> mSlider;
    AutoPtr<ITextView> mLabel;

    AutoPtr<ToggleSlider> mMirror;
    AutoPtr<IBrightnessMirrorController> mMirrorController;

    AutoPtr<MyCheckListener> mCheckListener;
    AutoPtr<MySeekListener> mSeekListener;
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_TOGGLESLIDER_H__