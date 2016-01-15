
#ifndef __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATEHELPER_H__
#define __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATEHELPER_H__

#include "_Elastos_Droid_Widget_CTimePickerClockDelegateHelper.h"
#include "elastos/droid/widget/TimePickerClockDelegate.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Widget::ITimePickerClockDelegateHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTimePickerClockDelegateHelper)
    , public Singleton
    , public ITimePickerClockDelegateHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    virtual CARAPI GetAmPmStrings(
    	/* [in] */ IContext* context,
    	/* [out, callee] */ ArrayOf<String>** result);
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CTIMEPICKERCLOCKDELEGATEHELPER_H__

