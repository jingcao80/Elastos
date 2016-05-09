
#ifndef __Elastos_DevSamples_DatePickerDemo_CActivityOne_H__
#define __Elastos_DevSamples_DatePickerDemo_CActivityOne_H__

#include <Elastos.Droid.Widget.h>
#include <elastos/droid/app/Activity.h>
#include "_Elastos_DevSamples_DatePickerDemo_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::IDatePickerOnDateChangedListener;

namespace Elastos {
namespace DevSamples {
namespace DatePickerDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    class DatePickerOnDateChangedListener
        : public Object
        , public IDatePickerOnDateChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        DatePickerOnDateChangedListener(
            /* [in] */ CActivityOne* host);

        ~DatePickerOnDateChangedListener();

        CARAPI OnDateChanged(
            /* [in] */ IDatePicker* view,
            /* [in] */ Int32 year,
            /* [in] */ Int32 monthOfYear,
            /* [in] */ Int32 dayOfMonth);
    private:
        CActivityOne* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    AutoPtr<IDatePicker> mDatePicker;
    AutoPtr<IEditText> mEditText;
};

} // namespace DatePickerDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DatePickerDemo_CActivityOne_H__
