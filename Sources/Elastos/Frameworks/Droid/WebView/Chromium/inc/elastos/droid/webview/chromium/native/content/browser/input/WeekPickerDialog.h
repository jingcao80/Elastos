
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_WEEKPICKERDIALOG_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_WEEKPICKERDIALOG_H__

#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/TwoFieldDatePickerDialog.h"

// import android.content.Context;

// import org.chromium.content.R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

class WeekPicker;

class WeekPickerDialog
    : public TwoFieldDatePickerDialog
{
public:
    /**
     * @param context The context the dialog is to run in.
     * @param callBack How the parent is notified that the date is set.
     * @param year The initial year of the dialog.
     * @param weekOfYear The initial week of the dialog.
     */
    WeekPickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ OnValueSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 weekOfYear,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    /**
     * @param context The context the dialog is to run in.
     * @param theme the theme to apply to this dialog
     * @param callBack How the parent is notified that the date is set.
     * @param year The initial year of the dialog.
     * @param weekOfYear The initial week of the dialog.
     */
    WeekPickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ OnValueSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 weekOfYear,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    /**
     * Gets the {@link WeekPicker} contained in this dialog.
     *
     * @return The calendar view.
     */
    CARAPI_(AutoPtr<WeekPicker>) GetWeekPicker();

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

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_WEEKPICKERDIALOG_H__
