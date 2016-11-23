
#include "elastos/droid/settings/widget/CToggleSwitch.h"
#include "elastos/droid/widget/Switch.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::Switch;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

CAR_INTERFACE_IMPL(CToggleSwitch, Switch, IToggleSwitch)

CAR_OBJECT_IMPL(CToggleSwitch)

CToggleSwitch::CToggleSwitch()
{}

CToggleSwitch::~CToggleSwitch()
{}

ECode CToggleSwitch::constructor(
    /* [in] */ IContext* context)
{
    return Switch::constructor(context);
}

ECode CToggleSwitch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Switch::constructor(context, attrs);
}

ECode CToggleSwitch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return Switch::constructor(context, attrs, defStyleAttr);
}

ECode CToggleSwitch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return Switch::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CToggleSwitch::SetOnBeforeCheckedChangeListener(
    /* [in] */ IToggleSwitchOnBeforeCheckedChangeListener* listener)
{
    mOnBeforeListener = listener;
    return NOERROR;
}

ECode CToggleSwitch::SetChecked(
    /* [in] */ Boolean checked)
{
    Boolean res;
    if (mOnBeforeListener != NULL
            && (mOnBeforeListener->OnBeforeCheckedChanged(this, checked, &res), res)) {
        return NOERROR;
    }
    return Switch::SetChecked(checked);
}

ECode CToggleSwitch::SetCheckedInternal(
    /* [in] */ Boolean checked)
{
    return Switch::SetChecked(checked);
}

} // namespace Widget
} // namespace Settings
} // namespace Droid
} // namespace Elastos
