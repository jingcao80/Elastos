#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_TOGGLESWITCH_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_TOGGLESWITCH_H__

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

class ToggleSwitch
    : public Switch
    , public IToggleSwitch
{
public:
    CAR_INTERFACE_DECL();

    ToggleSwitch();

    ~ToggleSwitch();

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

    CARAPI SetCheckedInternal(
        /* [in] */ Boolean checked);

private:
    AutoPtr<IToggleSwitchOnBeforeCheckedChangeListener> mOnBeforeListener;
};

} // namespace Widget
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_TOGGLESWITCH_H__
