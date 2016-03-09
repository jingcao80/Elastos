
#ifndef __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/preference/Preference.h"

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * A base class for {@link Preference} objects that are
* dialog-based. These preferences will, when clicked, open a dialog showing the
* actual preference controls.
 *
* @attr ref android.R.styleable#DialogPreference_dialogTitle
* @attr ref android.R.styleable#DialogPreference_dialogMessage
* @attr ref android.R.styleable#DialogPreference_dialogIcon
* @attr ref android.R.styleable#DialogPreference_dialogLayout
* @attr ref android.R.styleable#DialogPreference_positiveButtonText
* @attr ref android.R.styleable#DialogPreference_negativeButtonText
 */
class DialogPreference
    : public Preference
    , public IDialogPreference
    , public IDialogInterfaceOnClickListener
    , public IDialogInterfaceOnDismissListener
    , public IPreferenceManagerOnActivityDestroyListener
{
public:
    DialogPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual ~DialogPreference() {}

    CAR_INTERFACE_DECL()

    /**
     * Sets the title of the dialog. This will be shown on subsequent dialogs.
     *
     * @param dialogTitle The title.
     */
    virtual CARAPI SetDialogTitle(
        /* [in] */ ICharSequence* dialogTitle);

    /**
     * @see #setDialogTitle(CharSequence)
     * @param dialogTitleResId The dialog title as a resource.
     */
    virtual CARAPI SetDialogTitle(
        /* [in] */ Int32 dialogTitleResId);

    /**
     * Returns the title to be shown on subsequent dialogs.
     * @return The title.
     */
    virtual CARAPI GetDialogTitle(
        /* [out] */ ICharSequence** title);

    /**
     * Sets the message of the dialog. This will be shown on subsequent dialogs.
     * <p>
     * This message forms the content View of the dialog and conflicts with
     * list-based dialogs, for example. If setting a custom View on a dialog via
     * {@link #setDialogLayoutResource(int)}, include a text View with ID
     * {@link android.R.id#message} and it will be populated with this message.
     *
     * @param dialogMessage The message.
     */
    virtual CARAPI SetDialogMessage(
        /* [in] */ ICharSequence* dialogMessage);

    /**
     * @see #setDialogMessage(CharSequence)
     * @param dialogMessageResId The dialog message as a resource.
     */
    virtual CARAPI SetDialogMessage(
        /* [in] */ Int32 dialogMessageResId);

    /**
     * Returns the message to be shown on subsequent dialogs.
     * @return The message.
     */
    virtual CARAPI GetDialogMessage(
        /* [out] */ ICharSequence** message);

    /**
     * Sets the icon of the dialog. This will be shown on subsequent dialogs.
     *
     * @param dialogIcon The icon, as a {@link Drawable}.
     */
    virtual CARAPI SetDialogIcon(
        /* [in] */ IDrawable* dialogIcon);

    /**
     * Sets the icon (resource ID) of the dialog. This will be shown on
     * subsequent dialogs.
     *
     * @param dialogIconRes The icon, as a resource ID.
     */
    virtual CARAPI SetDialogIcon(
        /* [in] */ Int32 dialogIconRes);

    /**
     * Returns the icon to be shown on subsequent dialogs.
     * @return The icon, as a {@link Drawable}.
     */
    virtual CARAPI GetDialogIcon(
        /* [out] */ IDrawable** icon);

    /**
     * Sets the text of the positive button of the dialog. This will be shown on
     * subsequent dialogs.
     *
     * @param positiveButtonText The text of the positive button.
     */
    virtual CARAPI SetPositiveButtonText(
        /* [in] */ ICharSequence* positiveButtonText);

    /**
     * @see #setPositiveButtonText(CharSequence)
     * @param positiveButtonTextResId The positive button text as a resource.
     */
    virtual CARAPI SetPositiveButtonText(
        /* [in] */ Int32 positiveButtonTextResId);

    /**
     * Returns the text of the positive button to be shown on subsequent
     * dialogs.
     *
     * @return The text of the positive button.
     */
    virtual CARAPI GetPositiveButtonText(
        /* [out] */ ICharSequence** text);

    /**
     * Sets the text of the negative button of the dialog. This will be shown on
     * subsequent dialogs.
     *
     * @param negativeButtonText The text of the negative button.
     */
    virtual CARAPI SetNegativeButtonText(
        /* [in] */ ICharSequence* negativeButtonText);

    /**
     * @see #setNegativeButtonText(CharSequence)
     * @param negativeButtonTextResId The negative button text as a resource.
     */
    virtual CARAPI SetNegativeButtonText(
        /* [in] */ Int32 negativeButtonTextResId);

    /**
     * Returns the text of the negative button to be shown on subsequent
     * dialogs.
     *
     * @return The text of the negative button.
     */
    virtual CARAPI GetNegativeButtonText(
        /* [out] */ ICharSequence** text);

    /**
     * Sets the layout resource that is inflated as the {@link View} to be shown
     * as the content View of subsequent dialogs.
     *
     * @param dialogLayoutResId The layout resource ID to be inflated.
     * @see #setDialogMessage(CharSequence)
     */
    virtual CARAPI SetDialogLayoutResource(
        /* [in] */ Int32 dialogLayoutResId);

    /**
     * Returns the layout resource that is used as the content View for
     * subsequent dialogs.
     *
     * @return The layout resource.
     */
    virtual CARAPI GetDialogLayoutResource(
        /* [out] */ Int32* layoutResId);

    virtual CARAPI OnClick();

    virtual CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    virtual CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    /**
     * Gets the dialog that is shown by this preference.
     *
     * @return The dialog, or null if a dialog is not being shown.
     */
    virtual CARAPI GetDialog(
        /* [out] */ IDialog** dialog);

    /**
     * {@inheritDoc}
     */
    virtual CARAPI OnActivityDestroy();

protected:
    /**
     * Prepares the dialog builder to be shown when the preference is clicked.
     * Use this to set custom properties on the dialog.
     * <p>
     * Do not {@link AlertDialog.Builder#create()} or
     * {@link AlertDialog.Builder#show()}.
     */
    virtual CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

    /**
     * Shows the dialog associated with this Preference. This is normally initiated
     * automatically on clicking on the preference. Call this method if you need to
     * show the dialog on some other event.
     *
     * @param state Optional instance state to restore on the dialog
     */
    virtual CARAPI ShowDialog(
        /* [in] */ IBundle* state);

    /**
     * @hide
     */
    virtual  CARAPI CreateDialog(
        /* [out] */ IDialog** isNeed);

    /**
     * Returns whether the preference needs to display a soft input method when the dialog
     * is displayed. Default is false. Subclasses should override this method if they need
     * the soft input method brought up automatically.
     * @hide
     */
    virtual CARAPI NeedInputMethod(
        /* [out] */ Boolean* isNeed);

    /**
     * Creates the content view for the dialog (if a custom content view is
     * required). By default, it inflates the dialog layout resource if it is
     * set.
     *
     * @return The content View for the dialog.
     * @see #setLayoutResource(int)
     */
    virtual CARAPI OnCreateDialogView(
        /* [out] */ IView** view);

    /**
     * Binds views in the content View of the dialog to data.
     * <p>
     * Make sure to call through to the superclass implementation.
     *
     * @param view The content View of the dialog, if it is custom.
     */
    virtual CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    /**
     * Called when the dialog is dismissed and should be used to save data to
     * the {@link SharedPreferences}.
     *
     * @param positiveResult Whether the positive button was clicked (true), or
     *            the negative button was clicked or the dialog was canceled (false).
     */
    virtual CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

    virtual CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** parcel);

    virtual CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    virtual CARAPI OnGetDefaultValue(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

private:
    /**
     * Sets the required flags on the dialog window to enable input method window to show up.
     */
    CARAPI RequestInputMethod(
        /* [in] */ IDialog* dialog);

private:
    AutoPtr<IAlertDialogBuilder> mBuilder;
    AutoPtr<ICharSequence> mDialogTitle;
    AutoPtr<ICharSequence> mDialogMessage;
    AutoPtr<IDrawable> mDialogIcon;
    AutoPtr<ICharSequence> mPositiveButtonText;
    AutoPtr<ICharSequence> mNegativeButtonText;
    Int32 mDialogLayoutResId;

    /** The dialog, if it is showing. */
    AutoPtr<IDialog> mDialog;

    /** Which button was clicked. */
    Int32 mWhichButtonClicked;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCE_H__
