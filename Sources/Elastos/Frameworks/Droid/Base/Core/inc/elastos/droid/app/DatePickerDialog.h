
#ifndef __ELASTOS_DROID_APP_DATEPICKER_DIALOG_H__
#define __ELASTOS_DROID_APP_DATEPICKER_DIALOG_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/AlertDialog.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::IDatePickerOnDateChangedListener;
using Elastos::Droid::Widget::IDatePickerValidationCallback;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * A simple dialog containing an {@link android.widget.DatePicker}.
 *
 * <p>See the <a href="{@docRoot}guide/topics/ui/controls/pickers.html">Pickers</a>
 * guide.</p>
 */
class ECO_PUBLIC DatePickerDialog
    : public AlertDialog
    , public IDatePickerDialog
    , public IDialogInterfaceOnClickListener
    , public IDatePickerOnDateChangedListener
{
private:
    class MyValidationCallback
        : public Object
        , public IDatePickerValidationCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyValidationCallback(
            /* [in] */ DatePickerDialog* host);

        //@Override
        CARAPI OnValidationChanged(
            /* [in] */ Boolean valid);
    private:
        DatePickerDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DatePickerDialog();

    virtual ~DatePickerDialog();

    /**
     * @param context The context the dialog is to run in.
     * @param callBack How the parent is notified that the date is set.
     * @param year The initial year of the dialog.
     * @param monthOfYear The initial month of the dialog.
     * @param dayOfMonth The initial day of the dialog.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDatePickerDialogOnDateSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth);

    /**
     * @param context The context the dialog is to run in.
     * @param theme the theme to apply to this dialog
     * @param listener How the parent is notified that the date is set.
     * @param year The initial year of the dialog.
     * @param monthOfYear The initial month of the dialog.
     * @param dayOfMonth The initial day of the dialog.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ IDatePickerDialogOnDateSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth);

    static Int32 ResolveDialogTheme(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resid);

    //@Override
    CARAPI OnDateChanged(
        /* [in] */ IDatePicker* view,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    /**
     * Gets the {@link DatePicker} contained in this dialog.
     *
     * @return The calendar view.
     */
    CARAPI GetDatePicker(
        /* [out] */ IDatePicker** picker);

    /**
     * Sets the current date.
     *
     * @param year The date year.
     * @param monthOfYear The date month.
     * @param dayOfMonth The date day of month.
     */
    CARAPI UpdateDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth);

    // @Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IBundle** bundle);

    // @Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

private:
    void UpdateTitle(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

private:
    AutoPtr<IDatePickerValidationCallback> mValidationCallback;

    static const String YEAR;// = "year";
    static const String MONTH;// = "month";
    static const String DAY;// = "day";

    AutoPtr<IDatePicker> mDatePicker;
    AutoPtr<IDatePickerDialogOnDateSetListener> mDateSetListener;
    AutoPtr<ICalendar> mCalendar;

    Boolean mTitleNeedsUpdate;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_DATEPICKER_DIALOG_H__
