
#ifndef __ELASTOS_DROID_APP_ALERTDIALOG_H__
#define __ELASTOS_DROID_APP_ALERTDIALOG_H__

#include <elastos/droid/app/Dialog.h>

using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListView;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC AlertDialog
    : public Dialog
    , public IAlertDialog
{
public:
    CAR_INTERFACE_DECL()

    AlertDialog();

    virtual ~AlertDialog();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ Boolean createThemeContextWrapper);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean cancelable,
        /* [in] */ IDialogInterfaceOnCancelListener* cancelListener);

    static CARAPI_(Int32) ResolveDialogTheme(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resid);

    /**
     * Gets one of the buttons used in the dialog.
     * <p>
     * If a button does not exist in the dialog, NULL will be returned.
     *
     * @param whichButton The identifier of the button that should be returned.
     *            For example, this can be
     *            {@link DialogInterface#BUTTON_POSITIVE}.
     * @return The button from the dialog, or NULL if a button does not exist.
     */
    CARAPI GetButton(
        /* [in] */ Int32 whichButton,
        /* [out] */ IButton** button);

    /**
     * Gets the list view used in the dialog.
     *
     * @return The {@link ListView} from the dialog.
     */
    CARAPI GetListView(
        /* [out] */ IListView** listView);

    using Dialog::SetTitle;

    //@Override
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    /**
     * @see Builder#setCustomTitle(View)
     */
    CARAPI SetCustomTitle(
        /* [in] */ IView* customTitleView);

    CARAPI SetMessage(
        /* [in] */ ICharSequence* message);

    /**
     * Set the view to display in that dialog.
     */
    CARAPI SetView(
        /* [in] */ IView* view);

    /**
     * Set the view to display in that dialog, specifying the spacing to appear around that
     * view.
     *
     * @param view The view to show in the content area of the dialog
     * @param viewSpacingLeft Extra space to appear to the left of {@code view}
     * @param viewSpacingTop Extra space to appear above {@code view}
     * @param viewSpacingRight Extra space to appear to the right of {@code view}
     * @param viewSpacingBottom Extra space to appear below {@code view}
     */
    CARAPI SetView(
        /* [in] */ IView* view,
        /* [in] */ Int32 viewSpacingLeft,
        /* [in] */ Int32 viewSpacingTop,
        /* [in] */ Int32 viewSpacingRight,
        /* [in] */ Int32 viewSpacingBottom);

    /**
     * Internal api to allow hinting for the best button panel layout.
     * @hide
     */
    CARAPI SetButtonPanelLayoutHint(
        /* [in] */ Int32 layoutHint);

    /**
     * Set a message to be sent when a button is pressed.
     *
     * @param whichButton Which button to set the message for, can be one of
     *            {@link DialogInterface#BUTTON_POSITIVE},
     *            {@link DialogInterface#BUTTON_NEGATIVE}, or
     *            {@link DialogInterface#BUTTON_NEUTRAL}
     * @param text The text to display in positive button.
     * @param msg The {@link Message} to be sent when clicked.
     */
    CARAPI SetButton(
        /* [in] */ Int32 whichButton,
        /* [in] */ ICharSequence* text,
        /* [in] */ IMessage* msg);

    /**
     * Set a listener to be invoked when the positive button of the dialog is pressed.
     *
     * @param whichButton Which button to set the listener on, can be one of
     *            {@link DialogInterface#BUTTON_POSITIVE},
     *            {@link DialogInterface#BUTTON_NEGATIVE}, or
     *            {@link DialogInterface#BUTTON_NEUTRAL}
     * @param text The text to display in positive button.
     * @param listener The {@link DialogInterface.OnClickListener} to use.
     */
    CARAPI SetButton(
        /* [in] */ Int32 whichButton,
        /* [in] */ ICharSequence* text,
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    /**
     * Set resId to 0 if you don't want an icon.
     * @param resId the resourceId of the drawable to use as the icon or 0
     * if you don't want an icon.
     */
    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    /**
     * Set an icon as supplied by a theme attribute. e.g. android.R.attr.alertDialogIcon
     *
     * @param attrId ID of a theme attribute that points to a drawable resource.
     */
    CARAPI SetIconAttribute(
        /* [in] */ Int32 attrId);

    CARAPI SetInverseBackgroundForced(
        /* [in] */ Boolean forceInverseBackground);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI GetAlertController(
        /* [out] */ IAlertController** alert);

    CARAPI SetAlertController(
        /* [in] */ IAlertController* alert);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    AutoPtr<IAlertController> mAlert;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ALERTDIALOG_H__
