#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CTOGGLESLIDER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CTOGGLESLIDER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CToggleSlider.h"



#include "elastos/droid/systemui/statusbar/policy/ToggleSlider.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CToggleSlider), public ToggleSlider
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IRELATIVELAYOUT_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /*  [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetOnChangedListener(
        /* [in] */ IToggleSliderListener* listener);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI IsChecked(
        /* [out] */ Boolean* settingsEnabled);

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
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CTOGGLESLIDER_H__
