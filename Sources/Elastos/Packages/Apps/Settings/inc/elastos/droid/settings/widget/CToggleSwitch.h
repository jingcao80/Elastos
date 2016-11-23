#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__

#include "_Elastos_Droid_Settings_Widget_CToggleSwitch.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/Switch.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::Switch;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

CarClass(CToggleSwitch)
    , public Switch
    , public IToggleSwitch
{
public:
    TO_STRING_IMPL("CToggleSwitch")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CToggleSwitch();

    ~CToggleSwitch();

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

    CARAPI SetOnBeforeCheckedChangeListener(
        /* [in] */ IToggleSwitchOnBeforeCheckedChangeListener* listener);

    //@Override
    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    virtual CARAPI SetCheckedInternal(
        /* [in] */ Boolean checked);

private:
    AutoPtr<IToggleSwitchOnBeforeCheckedChangeListener> mOnBeforeListener;
};

} // namespace Widget
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__
