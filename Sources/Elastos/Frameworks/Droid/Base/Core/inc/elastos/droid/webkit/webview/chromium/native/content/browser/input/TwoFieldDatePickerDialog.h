
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKERDIALOG_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKERDIALOG_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/TwoFieldDatePicker.h"
#include "elastos/droid/app/AlertDialog.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;

// import org.chromium.content.R;
// import org.chromium.content.browser.input.TwoFieldDatePicker.OnMonthOrWeekChangedListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

class TwoFieldDatePickerDialog
    : public AlertDialog
    , public IDialogInterfaceOnClickListener
    , public TwoFieldDatePicker::OnMonthOrWeekChangedListener
{
public:
    /**
     * The callback used to indicate the user is done filling in the date.
     */
    class OnValueSetListener : public Object
    {
    public:
        /**
         * @param year The year that was set.
         * @param positionInYear The position in the year that was set.
         */
        virtual CARAPI_(void) OnValueSet(
            /* [in] */ Int32 year,
            /* [in] */ Int32 positionInYear) = 0;
    };

public:
    CAR_INTERFACE_DECL();

    TwoFieldDatePickerDialog();

    /**
     * @param context The context the dialog is to run in.
     * @param callBack How the parent is notified that the date is set.
     * @param year The initial year of the dialog.
     * @param weekOfYear The initial week of the dialog.
     */
    TwoFieldDatePickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ OnValueSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 positionInYear,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    /**
     * @param context The context the dialog is to run in.
     * @param theme the theme to apply to this dialog
     * @param callBack How the parent is notified that the date is set.
     * @param year The initial year of the dialog.
     * @param weekOfYear The initial week of the dialog.
     */
    TwoFieldDatePickerDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ OnValueSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 positionInYear,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    //@Override
    CARAPI OnMonthOrWeekChanged(
        /* [in] */ TwoFieldDatePicker* view,
        /* [in] */ Int32 year,
        /* [in] */ Int32 positionInYear);

    /**
     * Sets the current date.
     *
     * @param year The date week year.
     * @param weekOfYear The date week.
     */
    CARAPI_(void) UpdateDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 weekOfYear);

protected:
    CARAPI_(AutoPtr<TwoFieldDatePicker>) CreatePicker(
        /* [in] */ IContext* context,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    /**
     * Notifies the listener, if such, that a date has been set.
     */
    CARAPI_(void) TryNotifyDateSet();

protected:
    /*const*/ AutoPtr<TwoFieldDatePicker> mPicker;
    /*const*/ AutoPtr<OnValueSetListener> mCallBack;

private:
    void Init(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ OnValueSetListener* callBack,
        /* [in] */ Int32 year,
        /* [in] */ Int32 positionInYear,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

private:
    static const String YEAR;
    static const String POSITION_IN_YEAR;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKERDIALOG_H__
