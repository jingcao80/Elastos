

package com.android.settings;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::IEditTextPreference;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IEditText;

/**
 * TODO: Add a soft dialpad for PIN entry.
 */
class EditPinPreference extends EditTextPreference {

    interface OnPinEnteredListener {
        void OnPinEntered(EditPinPreference preference, Boolean positiveResult);
    }

    private OnPinEnteredListener mPinListener;

    public EditPinPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    public EditPinPreference(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);
    }

    CARAPI SetOnPinEnteredListener(OnPinEnteredListener listener) {
        mPinListener = listener;
    }

    //@Override
    protected void OnBindDialogView(View view) {
        super->OnBindDialogView(view);

        final EditText editText = GetEditText();

        if (editText != NULL) {
            editText->SetInputType(InputType.TYPE_CLASS_NUMBER |
                InputType.TYPE_NUMBER_VARIATION_PASSWORD);
        }
    }

    public Boolean IsDialogOpen() {
        Dialog dialog = GetDialog();
        return dialog != NULL && dialog->IsShowing();
    }

    //@Override
    protected void OnDialogClosed(Boolean positiveResult) {
        super->OnDialogClosed(positiveResult);
        if (mPinListener != NULL) {
            mPinListener->OnPinEntered(this, positiveResult);
        }
    }

    CARAPI ShowPinDialog() {
        Dialog dialog = GetDialog();
        if (dialog == NULL || !dialog->IsShowing()) {
            ShowDialog(NULL);
        }
    }
}
