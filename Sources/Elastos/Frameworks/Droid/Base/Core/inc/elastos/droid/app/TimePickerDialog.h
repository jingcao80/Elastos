
#ifndef __ELASTOS_DROID_APP_TIMEPICKER_DIALOG_H__
#define __ELASTOS_DROID_APP_TIMEPICKER_DIALOG_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/AlertDialog.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Droid::Widget::ITimePickerOnTimeChangedListener;
using Elastos::Droid::Widget::ITimePickerValidationCallback;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * A dialog that prompts the user for the time of day using a {@link TimePicker}.
 *
 * <p>See the <a href="{@docRoot}guide/topics/ui/controls/pickers.html">Pickers</a>
 * guide.</p>
 */
class TimePickerDialog
    : public AlertDialog
    , public ITimePickerDialog
    , public IDialogInterfaceOnClickListener
    , public ITimePickerOnTimeChangedListener
{
private:
    class MyValidationCallback
        : public Object
        , public ITimePickerValidationCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyValidationCallback(
            /* [in] */ TimePickerDialog* host);

        //@Override
        CARAPI OnValidationChanged(
            /* [in] */ Boolean valid);
    private:
        TimePickerDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TimePickerDialog();

    virtual ~TimePickerDialog();

    /**
     * @param context Parent.
     * @param callBack How parent is notified.
     * @param hourOfDay The initial hour.
     * @param minute The initial minute.
     * @param is24HourView Whether this is a 24 hour view, or AM/PM.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ITimePickerDialogOnTimeSetListener* callBack,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Boolean is24HourView);

    /**
     * @param context Parent.
     * @param callBack How parent is notified.
     * @param hourOfDay The initial hour.
     * @param minute The initial minute.
     * @param is24HourView Whether this is a 24 hour view, or AM/PM.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ ITimePickerDialogOnTimeSetListener* callBack,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Boolean is24HourView);

    static Int32 ResolveDialogTheme(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resid);

    //@Override
    CARAPI OnTimeChanged(
        /* [in] */ ITimePicker* view,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    /**
     * Sets the current time.
     *
     * @param hourOfDay The current hour within the day.
     * @param minuteOfHour The current minute within the hour.
     */
    CARAPI UpdateTime(
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minuteOfHour);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IBundle** result);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

private:
    AutoPtr<ITimePickerValidationCallback> mValidationCallback;

    static const String HOUR;
    static const String MINUTE;
    static const String IS_24_HOUR;

    AutoPtr<ITimePicker> mTimePicker;
    AutoPtr<ITimePickerDialogOnTimeSetListener> mTimeSetCallback;

    Int32 mInitialHourOfDay;
    Int32 mInitialMinute;
    Boolean mIs24HourView;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_TIMEPICKER_DIALOG_H__
