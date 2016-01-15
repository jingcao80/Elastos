
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_DATETIMEPICKERDIALOG_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_DATETIMEPICKERDIALOG_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::IDatePickerOnDateChangedListener;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Droid::Widget::ITimePickerOnTimeChangedListener;

// import org.chromium.content.R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

class DateTimePickerDialog
    : public Object
    , public IAlertDialog
    , public IDialogInterfaceOnClickListener
    , public IDatePickerOnDateChangedListener
    , public ITimePickerOnTimeChangedListener
{
public:
    /**
     * The callback used to indicate the user is done filling in the date.
     */
    class OnDateTimeSetListener : public Object
    {
    public:
        /**
         * @param dateView The DatePicker view associated with this listener.
         * @param timeView The TimePicker view associated with this listener.
         * @param year The year that was set.
         * @param monthOfYear The month that was set (0-11) for compatibility
         *            with {@link java.util.Calendar}.
         * @param dayOfMonth The day of the month that was set.
         * @param hourOfDay The hour that was set.
         * @param minute The minute that was set.
         */
        virtual CARAPI OnDateTimeSet(
            /* [in] */ IDatePicker* dateView,
            /* [in] */ ITimePicker* timeView,
            /* [in] */ Int32 year,
            /* [in] */ Int32 monthOfYear,
            /* [in] */ Int32 dayOfMonth,
            /* [in] */ Int32 hourOfDay,
            /* [in] */ Int32 minute) = 0;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * @param context The context the dialog is to run in.
     * @param callBack How the parent is notified that the date is set.
     * @param year The initial year of the dialog.
     * @param monthOfYear The initial month of the dialog.
     * @param dayOfMonth The initial day of the dialog.
     */
    DateTimePickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ OnDateTimeSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Boolean is24HourView,
        /* [in] */ Double min,
        /* [in] */ Double max);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    //@Override
    CARAPI OnDateChanged(
        /* [in] */ IDatePicker* view,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    //@Override
    CARAPI OnTimeChanged(
        /* [in] */ ITimePicker* view,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute);

    /**
     * Sets the current date.
     *
     * @param year The date year.
     * @param monthOfYear The date month.
     * @param dayOfMonth The date day of month.
     */
    CARAPI_(void) UpdateDateTime(
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minutOfHour);

private:
    CARAPI TryNotifyDateTimeSet();

private:
    /*const*/ AutoPtr<IDatePicker> mDatePicker;
    /*const*/ AutoPtr<ITimePicker> mTimePicker;
    /*const*/ AutoPtr<OnDateTimeSetListener> mCallBack;

    const Int64 mMinTimeMillis;
    const Int64 mMaxTimeMillis;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_DATETIMEPICKERDIALOG_H__
