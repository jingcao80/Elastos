
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_CHROMEDATEPICKERDIALOG_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_CHROMEDATEPICKERDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IDatePickerDialog;
using Elastos::Droid::App::IDatePickerDialogOnDateSetListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Widget::IDatePicker;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * The behavior of the DatePickerDialog changed after JellyBean so it now calls
 * OndateSetListener.onDateSet() even when the dialog is dismissed (e.g. back button, tap
 * outside). This class will call the listener instead of the DatePickerDialog only when the
 * BUTTON_POSITIVE has been clicked.
 */
class ChromeDatePickerDialog
    : public Object
    // TODO , public IDatePickerDialog;
{
public:
    ChromeDatePickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ IDatePickerDialogOnDateSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth);

    /**
     * The superclass DatePickerDialog has null for OnDateSetListener so we need to call the
     * listener manually.
     */
    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

private:
    const AutoPtr<IDatePickerDialogOnDateSetListener> mCallBack;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_CHROMEDATEPICKERDIALOG_H__
