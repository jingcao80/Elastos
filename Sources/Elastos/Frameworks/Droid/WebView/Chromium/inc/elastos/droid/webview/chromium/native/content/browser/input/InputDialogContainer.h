
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_INPUTDIALOGCONTAINER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_INPUTDIALOGCONTAINER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimePickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/TwoFieldDatePickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/MultiFieldTimePickerDialog.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDatePickerDialogOnDateSetListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITimePicker;

// import org.chromium.base.ApiCompatibilityUtils;
// import org.chromium.content.R;
// import org.chromium.content.browser.input.DateTimePickerDialog.OnDateTimeSetListener;

// import java.util.Arrays;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;
using Elastos::Utility::IGregorianCalendar;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::Concurrent::ITimeUnit;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

//class DateTimeSuggestion;
class DateTimeSuggestionListAdapter;

/**
 * Opens the approprate date/time picker dialog for the given dialog type.
 */
class InputDialogContainer : public Object
{
public:
    class InputActionDelegate : public Object
    {
    public:
        virtual CARAPI_(void) CancelDateTimeDialog() = 0;

        virtual CARAPI_(void) ReplaceDateTime(
            /* [in] */ Double value) = 0;
    };

private:
    class InnerAdapterViewOnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerAdapterViewOnItemClickListener(
            /* [in] */ InputDialogContainer* owner,
            /* [in] */ Int32 dialogType,
            /* [in] */ Double dialogValue,
            /* [in] */ Double min,
            /* [in] */ Double max,
            /* [in] */ Double step,
            /* [in] */ DateTimeSuggestionListAdapter* adapter);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        InputDialogContainer* mOwner;
        Int32 mDialogType;
        Double mDialogValue;
        Double mMin;
        Double mMax;
        Double mStep;
        DateTimeSuggestionListAdapter* mAdapter;
    };

    class InnerDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerDialogInterfaceOnClickListener(
            /* [in] */ InputDialogContainer* owner);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        InputDialogContainer* mOwner;
    };

    class InnerDialogInterfaceOnDismissListener
        : public Object
        , public IDialogInterfaceOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerDialogInterfaceOnDismissListener(
            /* [in] */ InputDialogContainer* owner);

        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        InputDialogContainer* mOwner;
    };

    class SetButtonDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        SetButtonDialogInterfaceOnClickListener(
            /* [in] */ InputDialogContainer* owner);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        InputDialogContainer* mOwner;
    };

    class SetOnDismissListenerDialogInterfaceOnDismissListener
        : public Object
        , public IDialogInterfaceOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        SetOnDismissListenerDialogInterfaceOnDismissListener(
            /* [in] */ InputDialogContainer* owner);

        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        InputDialogContainer* mOwner;
    };

    class DateListener
        : public Object
        , public IDatePickerDialogOnDateSetListener
    {
    public:
        CAR_INTERFACE_DECL()

        DateListener(
            /* [in] */ InputDialogContainer* owner,
            /* [in] */ Int32 dialogType);

        //@Override
        CARAPI OnDateSet(
            /* [in] */ IDatePicker* view,
            /* [in] */ Int32 year,
            /* [in] */ Int32 month,
            /* [in] */ Int32 monthDay);

    private:
        InputDialogContainer* mOwner;
        const Int32 mDialogType;
    };

    class FullTimeListener
        : public MultiFieldTimePickerDialog::OnMultiFieldTimeSetListener
    {
    public:
        FullTimeListener(
            /* [in] */ InputDialogContainer* owner,
            /* [in] */ Int32 dialogType);

        //@Override
        CARAPI OnTimeSet(
            /* [in] */ Int32 hourOfDay,
            /* [in] */ Int32 minute,
            /* [in] */ Int32 second,
            /* [in] */ Int32 milli);

    private:
        InputDialogContainer* mOwner;
        const Int32 mDialogType;
    };

    class DateTimeListener
        : public DateTimePickerDialog::OnDateTimeSetListener
    {
    public:
        DateTimeListener(
            /* [in] */ InputDialogContainer* owner,
            /* [in] */ Int32 dialogType);

        //@Override
        CARAPI OnDateTimeSet(
            /* [in] */ IDatePicker* dateView,
            /* [in] */ ITimePicker* timeView,
            /* [in] */ Int32 year,
            /* [in] */ Int32 month,
            /* [in] */ Int32 monthDay,
            /* [in] */ Int32 hourOfDay,
            /* [in] */ Int32 minute);

    private:
        InputDialogContainer* mOwner;
        const Boolean mLocal;
        const Int32 mDialogType;
    };

    class MonthOrWeekListener
        : public TwoFieldDatePickerDialog::OnValueSetListener
    {
    public:
        MonthOrWeekListener(
            /* [in] */ InputDialogContainer* owner,
            /* [in] */ Int32 dialogType);

        //@Override
        CARAPI_(void) OnValueSet(
            /* [in] */ Int32 year,
            /* [in] */ Int32 positionInYear);

    private:
        InputDialogContainer* mOwner;
        const Int32 mDialogType;
    };

public:
    InputDialogContainer(
        /* [in] */ IContext* context,
        /* [in] */ InputActionDelegate* inputActionDelegate);

    static CARAPI_(void) InitializeInputTypes(
        /* [in] */ Int32 textInputTypeDate,
        /* [in] */ Int32 textInputTypeDateTime,
        /* [in] */ Int32 textInputTypeDateTimeLocal,
        /* [in] */ Int32 textInputTypeMonth,
        /* [in] */ Int32 textInputTypeTime,
        /* [in] */ Int32 textInputTypeWeek);

    static CARAPI_(Boolean) IsDialogInputType(
        /* [in] */ Int32 type);

    CARAPI_(void) ShowPickerDialog(
        /* [in] */ Int32 dialogType,
        /* [in] */ Double dialogValue,
        /* [in] */ Double min,
        /* [in] */ Double max,
        /* [in] */ Double step);

    CARAPI_(void) ShowSuggestionDialog(
        /* [in] */ Int32 dialogType,
        /* [in] */ Double dialogValue,
        /* [in] */ Double min,
        /* [in] */ Double max,
        /* [in] */ Double step,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    CARAPI_(void) ShowDialog(
        /* [in] */ Int32 type,
        /* [in] */ Double value,
        /* [in] */ Double min,
        /* [in] */ Double max,
        /* [in] */ Double step,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    CARAPI_(void) ShowPickerDialog(
        /* [in] */ Int32 dialogType,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 monthDay,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [in] */ Int32 millis,
        /* [in] */ Int32 week,
        /* [in] */ Double min,
        /* [in] */ Double max,
        /* [in] */ Double step);

    CARAPI_(Boolean) IsDialogShowing();

    CARAPI_(void) DismissDialog();

protected:
    CARAPI_(void) SetFieldDateTimeValue(
        /* [in] */ Int32 dialogType,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 monthDay,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [in] */ Int32 millis,
        /* [in] */ Int32 week);

private:
    static Int32 sTextInputTypeDate;
    static Int32 sTextInputTypeDateTime;
    static Int32 sTextInputTypeDateTimeLocal;
    static Int32 sTextInputTypeMonth;
    static Int32 sTextInputTypeTime;
    static Int32 sTextInputTypeWeek;

    /*const*/ AutoPtr<IContext> mContext;

    // Prevents sending two notifications (from onClick and from onDismiss)
    Boolean mDialogAlreadyDismissed;

    AutoPtr<IAlertDialog> mDialog;
    /*const*/ AutoPtr<InputActionDelegate> mInputActionDelegate;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_INPUTDIALOGCONTAINER_H__
