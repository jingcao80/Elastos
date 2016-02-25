

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;

using Elastos::Droid::Internal::Os::Storage::IExternalStorageFormatter;

/**
 * Confirm and execute a format of the sdcard.
 * Multiple confirmations are required: first, a general "are you sure
 * you want to do this?" prompt, followed by a keyguard pattern trace if the user
 * has defined one, followed by a final strongly-worded "THIS WILL ERASE EVERYTHING
 * ON THE SD CARD" prompt.  If at any time the phone is allowed to go to sleep, is
 * locked, et cetera, then the confirmation sequence is abandoned.
 */
public class MediaFormat extends Activity {

    private static const Int32 KEYGUARD_REQUEST = 55;

    private LayoutInflater mInflater;

    private View mInitialView;
    private Button mInitiateButton;

    private View mFinalView;
    private Button mFinalButton;

    /**
     * The user has gone through the multiple confirmation, so now we go ahead
     * and invoke the Mount Service to format the SD card.
     */
    private Button.OnClickListener mFinalClickListener = new Button->OnClickListener() {
            CARAPI OnClick(View v) {

                if (Utils->IsMonkeyRunning()) {
                    return;
                }
                Intent intent = new Intent(ExternalStorageFormatter.FORMAT_ONLY);
                intent->SetComponent(ExternalStorageFormatter.COMPONENT_NAME);
                // Transfer the storage volume to the new intent
                final StorageVolume storageVolume = GetIntent()->GetParcelableExtra(
                        StorageVolume.EXTRA_STORAGE_VOLUME);
                intent->PutExtra(StorageVolume.EXTRA_STORAGE_VOLUME, storageVolume);
                StartService(intent);
                Finish();
            }
        };

    /**
     *  Keyguard validation is run using the standard {@link ConfirmLockPattern}
     * component as a subactivity
     */
    private Boolean RunKeyguardConfirmation(Int32 request) {
        return new ChooseLockSettingsHelper(this)
                .LaunchConfirmationActivity(request,
                        GetText(R::string::media_format_gesture_prompt),
                        GetText(R::string::media_format_gesture_explanation));
    }

    //@Override
    protected void OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);

        if (requestCode != KEYGUARD_REQUEST) {
            return;
        }

        // If the user entered a valid keyguard trace, present the final
        // confirmation prompt; otherwise, go back to the initial state.
        if (resultCode == Activity.RESULT_OK) {
            EstablishFinalConfirmationState();
        } else if (resultCode == Activity.RESULT_CANCELED) {
            Finish();
        } else {
            EstablishInitialState();
        }
    }

    /**
     * If the user clicks to begin the reset sequence, we next require a
     * keyguard confirmation if the user has currently enabled one.  If there
     * is no keyguard available, we simply go to the final confirmation prompt.
     */
    private Button.OnClickListener mInitiateListener = new Button->OnClickListener() {
            CARAPI OnClick(View v) {
                if (!RunKeyguardConfirmation(KEYGUARD_REQUEST)) {
                    EstablishFinalConfirmationState();
                }
            }
        };

    /**
     * Configure the UI for the final confirmation interaction
     */
    private void EstablishFinalConfirmationState() {
        if (mFinalView == NULL) {
            mFinalView = mInflater->Inflate(R.layout.media_format_final, NULL);
            mFinalButton =
                    (Button) mFinalView->FindViewById(R.id.execute_media_format);
            mFinalButton->SetOnClickListener(mFinalClickListener);
        }

        SetContentView(mFinalView);
    }

    /**
     * In its initial state, the activity presents a button for the user to
     * click in order to initiate a confirmation sequence.  This method is
     * called from various other points in the code to reset the activity to
     * this base state.
     *
     * <p>Reinflating views from resources is expensive and prevents us from
     * caching widget pointers, so we use a single-inflate pattern:  we lazy-
     * inflate each view, caching all of the widget pointers we'll need at the
     * time, then simply reuse the inflated views directly whenever we need
     * to change contents.
     */
    private void EstablishInitialState() {
        if (mInitialView == NULL) {
            mInitialView = mInflater->Inflate(R.layout.media_format_primary, NULL);
            mInitiateButton =
                    (Button) mInitialView->FindViewById(R.id.initiate_media_format);
            mInitiateButton->SetOnClickListener(mInitiateListener);
        }

        SetContentView(mInitialView);
    }

    //@Override
    protected void OnCreate(Bundle savedState) {
        super->OnCreate(savedState);

        mInitialView = NULL;
        mFinalView = NULL;
        mInflater = LayoutInflater->From(this);

        EstablishInitialState();
    }

    /** Abandon all progress through the confirmation sequence by returning
     * to the initial view any time the activity is interrupted (e.g. by
     * idle timeout).
     */
    //@Override
    CARAPI OnPause() {
        super->OnPause();

        if (!IsFinishing()) {
            EstablishInitialState();
        }
    }
}
