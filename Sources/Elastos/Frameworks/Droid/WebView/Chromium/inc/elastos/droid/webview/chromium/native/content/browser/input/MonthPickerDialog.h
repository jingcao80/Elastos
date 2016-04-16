
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_MONTHPICKERDIALOG_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_MONTHPICKERDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/TwoFieldDatePickerDialog.h"

using Elastos::Droid::Content::IContext;

// import org.chromium.content.R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

class MonthPicker;

class MonthPickerDialog : public TwoFieldDatePickerDialog
{
public:
    /**
     * @param context The context the dialog is to run in.
     * @param callBack How the parent is notified that the date is set.
     * @param year The initial year of the dialog.
     * @param monthOfYear The initial month of the dialog.
     */
    MonthPickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ OnValueSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Double minMonth,
        /* [in] */ Double maxMonth);

    /**
     * Gets the {@link MonthPicker} contained in this dialog.
     *
     * @return The calendar view.
     */
    CARAPI_(AutoPtr<MonthPicker>) GetMonthPicker();

protected:
    //@Override
    CARAPI_(AutoPtr<TwoFieldDatePicker>) CreatePicker(
        /* [in] */ IContext* context,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_MONTHPICKERDIALOG_H__
