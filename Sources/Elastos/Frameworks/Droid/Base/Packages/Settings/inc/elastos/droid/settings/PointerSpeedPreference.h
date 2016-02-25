

package com.android.settings;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Preference::ISeekBarDialogPreference;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ISeekBar;

public class PointerSpeedPreference extends SeekBarDialogPreference implements
        SeekBar.OnSeekBarChangeListener {
    private final InputManager mIm;
    private SeekBar mSeekBar;

    private Int32 mOldSpeed;
    private Boolean mRestoredOldState;

    private Boolean mTouchInProgress;

    private ContentObserver mSpeedObserver = new ContentObserver(new Handler()) {
        //@Override
        CARAPI OnChange(Boolean selfChange) {
            OnSpeedChanged();
        }
    };

    public PointerSpeedPreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
        mIm = (InputManager)GetContext()->GetSystemService(Context.INPUT_SERVICE);
    }

    //@Override
    protected void ShowDialog(Bundle state) {
        super->ShowDialog(state);

        GetContext()->GetContentResolver().RegisterContentObserver(
                Settings.System->GetUriFor(Settings.System.POINTER_SPEED), TRUE,
                mSpeedObserver);

        mRestoredOldState = FALSE;
    }

    //@Override
    protected void OnBindDialogView(View view) {
        super->OnBindDialogView(view);

        mSeekBar = GetSeekBar(view);
        mSeekBar->SetMax(InputManager.MAX_POINTER_SPEED - InputManager.MIN_POINTER_SPEED);
        mOldSpeed = mIm->GetPointerSpeed(GetContext());
        mSeekBar->SetProgress(mOldSpeed - InputManager.MIN_POINTER_SPEED);
        mSeekBar->SetOnSeekBarChangeListener(this);
    }

    CARAPI OnProgressChanged(SeekBar seekBar, Int32 progress, Boolean fromTouch) {
        if (!mTouchInProgress) {
            mIm->TryPointerSpeed(progress + InputManager.MIN_POINTER_SPEED);
        }
    }

    CARAPI OnStartTrackingTouch(SeekBar seekBar) {
        mTouchInProgress = TRUE;
    }

    CARAPI OnStopTrackingTouch(SeekBar seekBar) {
        mTouchInProgress = FALSE;
        mIm->TryPointerSpeed(seekBar->GetProgress() + InputManager.MIN_POINTER_SPEED);
    }

    private void OnSpeedChanged() {
        Int32 speed = mIm->GetPointerSpeed(GetContext());
        mSeekBar->SetProgress(speed - InputManager.MIN_POINTER_SPEED);
    }

    //@Override
    protected void OnDialogClosed(Boolean positiveResult) {
        super->OnDialogClosed(positiveResult);

        final ContentResolver resolver = GetContext()->GetContentResolver();

        if (positiveResult) {
            mIm->SetPointerSpeed(GetContext(),
                    mSeekBar->GetProgress() + InputManager.MIN_POINTER_SPEED);
        } else {
            RestoreOldState();
        }

        resolver->UnregisterContentObserver(mSpeedObserver);
    }

    private void RestoreOldState() {
        if (mRestoredOldState) return;

        mIm->TryPointerSpeed(mOldSpeed);
        mRestoredOldState = TRUE;
    }

    //@Override
    protected Parcelable OnSaveInstanceState() {
        final Parcelable superState = super->OnSaveInstanceState();
        if (GetDialog() == NULL || !GetDialog()->IsShowing()) return superState;

        // Save the dialog state
        final SavedState myState = new SavedState(superState);
        myState.progress = mSeekBar->GetProgress();
        myState.oldSpeed = mOldSpeed;

        // Restore the old state when the activity or dialog is being paused
        RestoreOldState();
        return myState;
    }

    //@Override
    protected void OnRestoreInstanceState(Parcelable state) {
        if (state == NULL || !state->GetClass()->Equals(SavedState.class)) {
            // Didn't save state for us in onSaveInstanceState
            super->OnRestoreInstanceState(state);
            return;
        }

        SavedState myState = (SavedState) state;
        super->OnRestoreInstanceState(myState->GetSuperState());
        mOldSpeed = myState.oldSpeed;
        mSeekBar->SetProgress(myState.progress);
        mIm->TryPointerSpeed(myState.progress + InputManager.MIN_POINTER_SPEED);
    }

    private static class SavedState extends BaseSavedState {
        Int32 progress;
        Int32 oldSpeed;

        public SavedState(Parcel source) {
            Super(source);
            progress = source->ReadInt();
            oldSpeed = source->ReadInt();
        }

        //@Override
        CARAPI WriteToParcel(Parcel dest, Int32 flags) {
            super->WriteToParcel(dest, flags);
            dest->WriteInt(progress);
            dest->WriteInt(oldSpeed);
        }

        public SavedState(Parcelable superState) {
            Super(superState);
        }

        public static const Parcelable.Creator<SavedState> CREATOR =
                new Parcelable.Creator<SavedState>() {

            public SavedState CreateFromParcel(Parcel in) {
                return new SavedState(in);
            }

            public SavedState[] NewArray(Int32 size) {
                return new SavedState[size];
            }
        };
    }
}

