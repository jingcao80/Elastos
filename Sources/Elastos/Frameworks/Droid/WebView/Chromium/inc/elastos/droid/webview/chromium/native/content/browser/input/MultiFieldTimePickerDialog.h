
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_MULTIFIELDTIMEPICKERDIALOG_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_MULTIFIELDTIMEPICKERDIALOG_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/app/AlertDialog.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::INumberPicker;
using Elastos::Droid::Widget::INumberPickerFormatter;

// import org.chromium.content.R;

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

/**
 * A time picker dialog with upto 5 number pickers left to right:
 *  hour, minute, second, milli, AM/PM.
 *
 * If is24hourFormat is true then AM/PM picker is not displayed and
 * hour range is 0..23. Otherwise hour range is 1..12.
 * The milli picker is not displayed if step >= SECOND_IN_MILLIS
 * The second picker is not displayed if step >= MINUTE_IN_MILLIS.
 */
class MultiFieldTimePickerDialog
    : public AlertDialog
    , public IDialogInterfaceOnClickListener
{
public:
    class OnMultiFieldTimeSetListener : public Object
    {
    public:
        virtual CARAPI OnTimeSet(
            /* [in] */ Int32 hourOfDay,
            /* [in] */ Int32 minute,
            /* [in] */ Int32 second,
            /* [in] */ Int32 milli) = 0;
    };

private:
    class NumberFormatter
        : public Object
        , public INumberPickerFormatter
    {
    public:
        CAR_INTERFACE_DECL();

        NumberFormatter(
            /* [in] */ const String& format);

        //@Override
        CARAPI Format(
            /* [in] */ Int32 value,
            /* [out] */ String* str);

    private:
        const String mFormat;
    };

public:
    CAR_INTERFACE_DECL();

    MultiFieldTimePickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [in] */ Int32 milli,
        /* [in] */ Int32 min,
        /* [in] */ Int32 max,
        /* [in] */ Int32 step,
        /* [in] */ Boolean is24hourFormat,
        /* [in] */ OnMultiFieldTimeSetListener* listener);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

private:
    CARAPI_(void) NotifyDateSet();

private:
    AutoPtr<INumberPicker> mHourSpinner;
    AutoPtr<INumberPicker> mMinuteSpinner;
    AutoPtr<INumberPicker> mSecSpinner;
    AutoPtr<INumberPicker> mMilliSpinner;
    AutoPtr<INumberPicker> mAmPmSpinner;
    AutoPtr<OnMultiFieldTimeSetListener> mListener;
    const Int32 mStep;
    Int32 mBaseMilli;
    const Boolean mIs24hourFormat;

    static const Int32 SECOND_IN_MILLIS = 1000;
    static const Int32 MINUTE_IN_MILLIS = 60 * SECOND_IN_MILLIS;
    static const Int32 HOUR_IN_MILLIS = 60 * MINUTE_IN_MILLIS;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_MULTIFIELDTIMEPICKERDIALOG_H__
