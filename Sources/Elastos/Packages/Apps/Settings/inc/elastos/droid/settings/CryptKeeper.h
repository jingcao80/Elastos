

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::View::IOnKeyListener;
using Elastos::Droid::View::View::IOnTouchListener;
using Elastos::Droid::View::Inputmethod::IEditorInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::View::Inputmethod::IInputMethodSubtype;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Statusbar::IStatusBarIcon;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternView;
using Elastos::Droid::Internal::Widget::LockPatternView::ICell;

using static com::Android::Internal::Widget::LockPatternView::IDisplayMode;

using Elastos::Utility::IList;

/**
 * Settings screens to show the UI flows for encrypting/decrypting the device.
 *
 * This may be started via adb for debugging the UI layout, without having to go through
 * encryption flows everytime. It should be noted that starting the activity in this manner
 * is only useful for verifying UI-correctness - the behavior will not be identical.
 * <pre>
 * $ adb shell pm enable com.android.settings/.CryptKeeper
 * $ adb shell am start \
 *     -e "com.android.settings.CryptKeeper.DEBUG_FORCE_VIEW" "progress" \
 *     -n com.android.settings/.CryptKeeper
 * </pre>
 */
public class CryptKeeper extends Activity implements TextView.OnEditorActionListener,
        OnKeyListener, OnTouchListener, TextWatcher {
    private static const String TAG = "CryptKeeper";

    private static const String DECRYPT_STATE = "trigger_restart_framework";
    /** Message sent to us to indicate encryption update progress. */
    private static const Int32 MESSAGE_UPDATE_PROGRESS = 1;
    /** Message sent to us to cool-down (waste user's time between password attempts) */
    private static const Int32 MESSAGE_COOLDOWN = 2;
    /** Message sent to us to indicate alerting the user that we are waiting for password entry */
    private static const Int32 MESSAGE_NOTIFY = 3;

    // Constants used to control policy.
    private static const Int32 MAX_FAILED_ATTEMPTS = 30;
    private static const Int32 COOL_DOWN_ATTEMPTS = 10;
    private static const Int32 COOL_DOWN_INTERVAL = 30; // 30 seconds

    // Intent action for launching the Emergency Dialer activity.
    static const String ACTION_EMERGENCY_DIAL = "com.android.phone.EmergencyDialer.DIAL";

    // Debug Intent extras so that this Activity may be started via adb for debugging UI layouts
    private static const String EXTRA_FORCE_VIEW =
            "com.android.settings.CryptKeeper.DEBUG_FORCE_VIEW";
    private static const String FORCE_VIEW_PROGRESS = "progress";
    private static const String FORCE_VIEW_ERROR = "error";
    private static const String FORCE_VIEW_PASSWORD = "password";

    /** When encryption is detected, this flag indicates whether or not we've checked for errors. */
    private Boolean mValidationComplete;
    private Boolean mValidationRequested;
    /** A flag to indicate that the volume is in a bad state (e.g. partially encrypted). */
    private Boolean mEncryptionGoneBad;
    /** If gone bad, should we show encryption failed (FALSE) or corrupt (TRUE)*/
    private Boolean mCorrupt;
    /** A flag to indicate when the back event should be ignored */
    private Boolean mIgnoreBack = FALSE;
    private Int32 mCooldown;
    PowerManager.WakeLock mWakeLock;
    private EditText mPasswordEntry;
    private LockPatternView mLockPatternView;
    /** Number of calls to {@link #NotifyUser()} to ignore before notifying. */
    private Int32 mNotificationCountdown = 0;
    /** Number of calls to {@link #NotifyUser()} before we release the wakelock */
    private Int32 mReleaseWakeLockCountdown = 0;
    private Int32 mStatusString = R::string::enter_password;

    // how Int64 we wait to clear a wrong pattern
    private static const Int32 WRONG_PATTERN_CLEAR_TIMEOUT_MS = 1500;

    // how Int64 we wait to clear a right pattern
    private static const Int32 RIGHT_PATTERN_CLEAR_TIMEOUT_MS = 500;

    // When the user enters a short pin/password, run this to show an error,
    // but don't count it against attempts.
    private final Runnable mFakeUnlockAttemptRunnable = new Runnable() {
        CARAPI Run() {
            HandleBadAttempt(1 /* failedAttempt */);
        }
    };

    // TODO: this should be tuned to match minimum decryption timeout
    private static const Int32 FAKE_ATTEMPT_DELAY = 1000;

    private Runnable mClearPatternRunnable = new Runnable() {
        CARAPI Run() {
            mLockPatternView->ClearPattern();
        }
    };

    /**
     * Used to propagate state through configuration changes (e.g. screen rotation)
     */
    private static class NonConfigurationInstanceState {
        final PowerManager.WakeLock wakelock;

        NonConfigurationInstanceState(PowerManager.WakeLock _wakelock) {
            wakelock = _wakelock;
        }
    }

    private class DecryptTask extends AsyncTask<String, Void, Integer> {
        private void Hide(Int32 id) {
            View view = FindViewById(id);
            if (view != NULL) {
                view->SetVisibility(View.GONE);
            }
        }

        //@Override
        protected Integer DoInBackground(String... params) {
            final IMountService service = GetMountService();
            try {
                return service->DecryptStorage(params[0]);
            } catch (Exception e) {
                Logger::E(TAG, "Error while decrypting...", e);
                return -1;
            }
        }

        //@Override
        protected void OnPostExecute(Integer failedAttempts) {
            if (failedAttempts == 0) {
                // The password was entered successfully. Simply do nothing
                // and wait for the service restart to switch to surfacefligner
                if (mLockPatternView != NULL) {
                    mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
                    mLockPatternView->PostDelayed(mClearPatternRunnable, RIGHT_PATTERN_CLEAR_TIMEOUT_MS);
                }
                Hide(R.id.passwordEntry);
                Hide(R.id.switch_ime_button);
                Hide(R.id.lockPattern);
                Hide(R.id.status);
                Hide(R.id.owner_info);
                Hide(R.id.emergencyCallButton);
            } else if (failedAttempts == MAX_FAILED_ATTEMPTS) {
                // Factory reset the device.
                Intent intent = new Intent(IIntent::ACTION_MASTER_CLEAR);
                intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
                intent->PutExtra(Intent.EXTRA_REASON, "CryptKeeper.MAX_FAILED_ATTEMPTS");
                SendBroadcast(intent);
            } else if (failedAttempts == -1) {
                // Right password, but decryption failed. Tell user bad news ...
                SetContentView(R.layout.crypt_keeper_progress);
                ShowFactoryReset(TRUE);
                return;
            } else {
                HandleBadAttempt(failedAttempts);
            }
        }
    }

    private void HandleBadAttempt(Integer failedAttempts) {
        // Wrong entry. Handle pattern case.
        if (mLockPatternView != NULL) {
            mLockPatternView->SetDisplayMode(DisplayMode.Wrong);
            mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
            mLockPatternView->PostDelayed(mClearPatternRunnable, WRONG_PATTERN_CLEAR_TIMEOUT_MS);
        }
        if ((failedAttempts % COOL_DOWN_ATTEMPTS) == 0) {
            mCooldown = COOL_DOWN_INTERVAL;
            Cooldown();
        } else {
            final TextView status = (TextView) FindViewById(R.id.status);

            Int32 remainingAttempts = MAX_FAILED_ATTEMPTS - failedAttempts;
            if (remainingAttempts < COOL_DOWN_ATTEMPTS) {
                CharSequence warningTemplate = GetText(R::string::crypt_keeper_warn_wipe);
                CharSequence warning = TextUtils->ExpandTemplate(warningTemplate,
                        Integer->ToString(remainingAttempts));
                status->SetText(warning);
            } else {
                status->SetText(R::string::try_again);
            }

            if (mLockPatternView != NULL) {
                mLockPatternView->SetDisplayMode(DisplayMode.Wrong);
            }
            // Reenable the password entry
            if (mPasswordEntry != NULL) {
                mPasswordEntry->SetEnabled(TRUE);
                final InputMethodManager imm = (InputMethodManager) GetSystemService(
                        Context.INPUT_METHOD_SERVICE);
                imm->ShowSoftInput(mPasswordEntry, 0);
                SetBackFunctionality(TRUE);
            }
            if (mLockPatternView != NULL) {
                mLockPatternView->SetEnabled(TRUE);
            }
        }
    }

    private class ValidationTask extends AsyncTask<Void, Void, Boolean> {
        Int32 state;

        //@Override
        protected Boolean DoInBackground(Void... params) {
            final IMountService service = GetMountService();
            try {
                Logger::D(TAG, "Validating encryption state.");
                state = service->GetEncryptionState();
                if (state == IMountService.ENCRYPTION_STATE_NONE) {
                    Logger::W(TAG, "Unexpectedly in CryptKeeper even though there is no encryption.");
                    return TRUE; // Unexpected, but fine, I guess...
                }
                return state == IMountService.ENCRYPTION_STATE_OK;
            } catch (RemoteException e) {
                Logger::W(TAG, "Unable to get encryption state properly");
                return TRUE;
            }
        }

        //@Override
        protected void OnPostExecute(Boolean result) {
            mValidationComplete = TRUE;
            if (Boolean.FALSE->Equals(result)) {
                Logger::W(TAG, "Incomplete, or corrupted encryption detected. Prompting user to wipe.");
                mEncryptionGoneBad = TRUE;
                mCorrupt = state == IMountService.ENCRYPTION_STATE_ERROR_CORRUPT;
            } else {
                Logger::D(TAG, "Encryption state validated. Proceeding to configure UI");
            }
            SetupUi();
        }
    }

    private final Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_UPDATE_PROGRESS:
                UpdateProgress();
                break;

            case MESSAGE_COOLDOWN:
                Cooldown();
                break;

            case MESSAGE_NOTIFY:
                NotifyUser();
                break;
            }
        }
    };

    private AudioManager mAudioManager;
    /** The status bar where back/home/recent buttons are shown. */
    private StatusBarManager mStatusBar;

    /** All the widgets to disable in the status bar */
    final private static Int32 sWidgetsToDisable = StatusBarManager.DISABLE_EXPAND
            | StatusBarManager.DISABLE_NOTIFICATION_ICONS
            | StatusBarManager.DISABLE_NOTIFICATION_ALERTS
            | StatusBarManager.DISABLE_SYSTEM_INFO
            | StatusBarManager.DISABLE_HOME
            | StatusBarManager.DISABLE_SEARCH
            | StatusBarManager.DISABLE_RECENT;

    protected static const Int32 MIN_LENGTH_BEFORE_REPORT = LockPatternUtils.MIN_LOCK_PATTERN_SIZE;

    /** @return whether or not this Activity was started for debugging the UI only. */
    private Boolean IsDebugView() {
        return GetIntent()->HasExtra(EXTRA_FORCE_VIEW);
    }

    /** @return whether or not this Activity was started for debugging the specific UI view only. */
    private Boolean IsDebugView(String viewType /* non-nullable */) {
        return viewType->Equals(GetIntent()->GetStringExtra(EXTRA_FORCE_VIEW));
    }

    /**
     * Notify the user that we are awaiting input. Currently this sends an audio alert.
     */
    private void NotifyUser() {
        if (mNotificationCountdown > 0) {
            --mNotificationCountdown;
        } else if (mAudioManager != NULL) {
            try {
                // Play the standard keypress sound at full volume. This should be available on
                // every device. We cannot play a ringtone here because media services aren't
                // available yet. A DTMF-style tone is too soft to be noticed, and might not exist
                // on tablet devices. The idea is to alert the user that something is needed: this
                // does not have to be pleasing.
                mAudioManager->PlaySoundEffect(AudioManager.FX_KEYPRESS_STANDARD, 100);
            } catch (Exception e) {
                Logger::W(TAG, "notifyUser: Exception while playing sound: " + e);
            }
        }
        // Notify the user again in 5 seconds.
        mHandler->RemoveMessages(MESSAGE_NOTIFY);
        mHandler->SendEmptyMessageDelayed(MESSAGE_NOTIFY, 5 * 1000);

        if (mWakeLock->IsHeld()) {
            if (mReleaseWakeLockCountdown > 0) {
                --mReleaseWakeLockCountdown;
            } else {
                mWakeLock->Release();
            }
        }
    }

    /**
     * Ignore back events after the user has entered the decrypt screen and while the device is
     * encrypting.
     */
    //@Override
    CARAPI OnBackPressed() {
        // In the rare case that something pressed back even though we were disabled.
        if (mIgnoreBack)
            return;
        super->OnBackPressed();
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        // If we are not encrypted or encrypting, get out quickly.
        final String state = SystemProperties->Get("vold.decrypt");
        if (!IsDebugView() && ("".Equals(state) || DECRYPT_STATE->Equals(state))) {
            // Disable the crypt keeper.
            PackageManager pm = GetPackageManager();
            ComponentName name = new ComponentName(this, CryptKeeper.class);
            pm->SetComponentEnabledSetting(name, PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                    PackageManager.DONT_KILL_APP);
            // Typically CryptKeeper is launched as the home app.  We didn't
            // want to be running, so need to finish this activity.  We can count
            // on the activity manager re-launching the new home app upon finishing
            // this one, since this will leave the activity stack empty.
            // NOTE: This is really grungy.  I think it would be better for the
            // activity manager to explicitly launch the crypt keeper instead of
            // home in the situation where we need to decrypt the device
            Finish();
            return;
        }

        try {
            if (GetResources()->GetBoolean(R.bool.crypt_keeper_allow_rotation)) {
                SetRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
            }
        } catch (NotFoundException e) {
        }

        // Disable the status bar, but do NOT disable back because the user needs a way to go
        // from keyboard settings and back to the password screen.
        mStatusBar = (StatusBarManager) GetSystemService(Context.STATUS_BAR_SERVICE);
        mStatusBar->Disable(sWidgetsToDisable);

        SetAirplaneModeIfNecessary();
        mAudioManager = (AudioManager) GetSystemService(Context.AUDIO_SERVICE);
        // Check for (and recover) retained instance data
        final Object lastInstance = GetLastNonConfigurationInstance();
        if (lastInstance instanceof NonConfigurationInstanceState) {
            NonConfigurationInstanceState retained = (NonConfigurationInstanceState) lastInstance;
            mWakeLock = retained.wakelock;
            Logger::D(TAG, "Restoring wakelock from NonConfigurationInstanceState");
        }
    }

    /**
     * Note, we defer the state check and screen setup to OnStart() because this will be
     * re-run if the user clicks the power button (sleeping/waking the screen), and this is
     * especially important if we were to lose the wakelock for any reason.
     */
    //@Override
    CARAPI OnStart() {
        super->OnStart();
        SetupUi();
    }

    /**
     * Initializes the UI based on the current state of encryption.
     * This is idempotent - calling repeatedly will simply re-initialize the UI.
     */
    private void SetupUi() {
        if (mEncryptionGoneBad || IsDebugView(FORCE_VIEW_ERROR)) {
            SetContentView(R.layout.crypt_keeper_progress);
            ShowFactoryReset(mCorrupt);
            return;
        }

        final String progress = SystemProperties->Get("vold.encrypt_progress");
        if (!"".Equals(progress) || IsDebugView(FORCE_VIEW_PROGRESS)) {
            SetContentView(R.layout.crypt_keeper_progress);
            EncryptionProgressInit();
        } else if (mValidationComplete || IsDebugView(FORCE_VIEW_PASSWORD)) {
            new AsyncTask<Void, Void, Void>() {
                Int32 type = StorageManager.CRYPT_TYPE_PASSWORD;
                String owner_info;
                Boolean pattern_visible;

                //@Override
                public Void DoInBackground(Void... v) {
                    try {
                        final IMountService service = GetMountService();
                        type = service->GetPasswordType();
                        owner_info = service->GetField("OwnerInfo");
                        pattern_visible = !("0".Equals(service->GetField("PatternVisible")));
                    } catch (Exception e) {
                        Logger::E(TAG, "Error calling mount service " + e);
                    }

                    return NULL;
                }

                //@Override
                CARAPI OnPostExecute(java.lang.Void v) {
                    If(type == StorageManager.CRYPT_TYPE_PIN) {
                        SetContentView(R.layout.crypt_keeper_pin_entry);
                        mStatusString = R::string::enter_pin;
                    } else if (type == StorageManager.CRYPT_TYPE_PATTERN) {
                        SetContentView(R.layout.crypt_keeper_pattern_entry);
                        SetBackFunctionality(FALSE);
                        mStatusString = R::string::enter_pattern;
                    } else {
                        SetContentView(R.layout.crypt_keeper_password_entry);
                        mStatusString = R::string::enter_password;
                    }
                    final TextView status = (TextView) FindViewById(R.id.status);
                    status->SetText(mStatusString);

                    final TextView ownerInfo = (TextView) FindViewById(R.id.owner_info);
                    ownerInfo->SetText(owner_info);
                    ownerInfo->SetSelected(TRUE); // Required for marquee'ing to work

                    PasswordEntryInit();

                    if (mLockPatternView != NULL) {
                        mLockPatternView->SetInStealthMode(!pattern_visible);
                    }

                    if (mCooldown > 0) {
                        SetBackFunctionality(FALSE);
                        Cooldown(); // in case we are cooling down and coming back from emergency dialler
                    }
                }
            }.Execute();
        } else if (!mValidationRequested) {
            // We're supposed to be encrypted, but no validation has been done.
            new ValidationTask()->Execute((Void[]) NULL);
            mValidationRequested = TRUE;
        }
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();
        mHandler->RemoveMessages(MESSAGE_COOLDOWN);
        mHandler->RemoveMessages(MESSAGE_UPDATE_PROGRESS);
        mHandler->RemoveMessages(MESSAGE_NOTIFY);
    }

    /**
     * Reconfiguring, so propagate the wakelock to the next instance.  This runs between OnStop()
     * and OnDestroy() and only if we are changing configuration (e.g. rotation).  Also clears
     * mWakeLock so the subsequent call to onDestroy does not release it.
     */
    //@Override
    public Object OnRetainNonConfigurationInstance() {
        NonConfigurationInstanceState state = new NonConfigurationInstanceState(mWakeLock);
        Logger::D(TAG, "Handing wakelock off to NonConfigurationInstanceState");
        mWakeLock = NULL;
        return state;
    }

    //@Override
    CARAPI OnDestroy() {
        super->OnDestroy();

        if (mWakeLock != NULL) {
            Logger::D(TAG, "Releasing and destroying wakelock");
            mWakeLock->Release();
            mWakeLock = NULL;
        }
    }

    /**
     * Start encrypting the device.
     */
    private void EncryptionProgressInit() {
        // Accquire a partial wakelock to prevent the device from sleeping. Note
        // we never release this wakelock as we will be restarted after the device
        // is encrypted.
        Logger::D(TAG, "Encryption progress screen initializing.");
        if (mWakeLock == NULL) {
            Logger::D(TAG, "Acquiring wakelock.");
            PowerManager pm = (PowerManager) GetSystemService(Context.POWER_SERVICE);
            mWakeLock = pm->NewWakeLock(PowerManager.FULL_WAKE_LOCK, TAG);
            mWakeLock->Acquire();
        }

        ((ProgressBar) FindViewById(R.id.progress_bar)).SetIndeterminate(TRUE);
        // Ignore all back presses from now, both hard and soft keys.
        SetBackFunctionality(FALSE);
        // Start the first run of progress manually. This method sets up messages to occur at
        // repeated intervals.
        UpdateProgress();
    }

    /**
     * Show factory reset screen allowing the user to reset their phone when
     * there is nothing else we can do
     * @param corrupt TRUE if userdata is corrupt, FALSE if encryption failed
     *        partway through
     */
    private void ShowFactoryReset(final Boolean corrupt) {
        // Hide the encryption-bot to make room for the "factory reset" button
        FindViewById(R.id.encroid).SetVisibility(View.GONE);

        // Show the reset button, failure text, and a divider
        final Button button = (Button) FindViewById(R.id.factory_reset);
        button->SetVisibility(View.VISIBLE);
        button->SetOnClickListener(new OnClickListener() {
                //@Override
            CARAPI OnClick(View v) {
                // Factory reset the device.
                Intent intent = new Intent(IIntent::ACTION_MASTER_CLEAR);
                intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
                intent->PutExtra(Intent.EXTRA_REASON,
                        "CryptKeeper->ShowFactoryReset() corrupt=" + corrupt);
                SendBroadcast(intent);
            }
        });

        // Alert the user of the failure.
        if (corrupt) {
            ((TextView) FindViewById(R.id.title)).SetText(R::string::crypt_keeper_data_corrupt_title);
            ((TextView) FindViewById(R.id.status)).SetText(R::string::crypt_keeper_data_corrupt_summary);
        } else {
            ((TextView) FindViewById(R.id.title)).SetText(R::string::crypt_keeper_failed_title);
            ((TextView) FindViewById(R.id.status)).SetText(R::string::crypt_keeper_failed_summary);
        }

        final View view = FindViewById(R.id.bottom_divider);
        // TODO(viki): Why would the bottom divider be missing in certain layouts? Investigate.
        if (view != NULL) {
            view->SetVisibility(View.VISIBLE);
        }
    }

    private void UpdateProgress() {
        final String state = SystemProperties->Get("vold.encrypt_progress");

        if ("error_partially_encrypted".Equals(state)) {
            ShowFactoryReset(FALSE);
            return;
        }

        // Get status as percentage first
        CharSequence status = GetText(R::string::crypt_keeper_setup_description);
        Int32 percent = 0;
        try {
            // Force a 50% progress state when debugging the view.
            percent = IsDebugView() ? 50 : Integer->ParseInt(state);
        } catch (Exception e) {
            Logger::W(TAG, "Error parsing progress: " + e->ToString());
        }
        String progress = Integer->ToString(percent);

        // Now try to get status as time remaining and replace as appropriate
        Logger::V(TAG, "Encryption progress: " + progress);
        try {
            final String timeProperty = SystemProperties->Get("vold.encrypt_time_remaining");
            Int32 time = Integer->ParseInt(timeProperty);
            if (time >= 0) {
                // Round up to multiple of 10 - this way display is less jerky
                time = (time + 9) / 10 * 10;
                progress = DateUtils->FormatElapsedTime(time);
                status = GetText(R::string::crypt_keeper_setup_time_remaining);
            }
        } catch (Exception e) {
            // Will happen if no time etc - show percentage
        }

        final TextView tv = (TextView) FindViewById(R.id.status);
        if (tv != NULL) {
            tv->SetText(TextUtils->ExpandTemplate(status, progress));
        }

        // Check the progress every 1 seconds
        mHandler->RemoveMessages(MESSAGE_UPDATE_PROGRESS);
        mHandler->SendEmptyMessageDelayed(MESSAGE_UPDATE_PROGRESS, 1000);
    }

    /** Disable password input for a while to force the user to waste time between retries */
    private void Cooldown() {
        final TextView status = (TextView) FindViewById(R.id.status);

        if (mCooldown <= 0) {
            // Re-enable the password entry and back presses.
            if (mPasswordEntry != NULL) {
                mPasswordEntry->SetEnabled(TRUE);
                final InputMethodManager imm = (InputMethodManager) GetSystemService(
                                          Context.INPUT_METHOD_SERVICE);
                imm->ShowSoftInput(mPasswordEntry, 0);
                SetBackFunctionality(TRUE);
            }
            if (mLockPatternView != NULL) {
                mLockPatternView->SetEnabled(TRUE);
            }
            status->SetText(mStatusString);
        } else {
            // Disable the password entry and back presses.
            if (mPasswordEntry != NULL) {
                mPasswordEntry->SetEnabled(FALSE);
            }
            if (mLockPatternView != NULL) {
                mLockPatternView->SetEnabled(FALSE);
            }

            CharSequence template = GetText(R::string::crypt_keeper_cooldown);
            status->SetText(TextUtils->ExpandTemplate(template, Integer->ToString(mCooldown)));

            mCooldown--;
            mHandler->RemoveMessages(MESSAGE_COOLDOWN);
            mHandler->SendEmptyMessageDelayed(MESSAGE_COOLDOWN, 1000); // Tick every second
        }
    }

    /**
     * Sets the back status: enabled or disabled according to the parameter.
     * @param isEnabled TRUE if back is enabled, FALSE otherwise.
     */
    private final void SetBackFunctionality(Boolean isEnabled) {
        mIgnoreBack = !isEnabled;
        if (isEnabled) {
            mStatusBar->Disable(sWidgetsToDisable);
        } else {
            mStatusBar->Disable(sWidgetsToDisable | StatusBarManager.DISABLE_BACK);
        }
    }

    private void FakeUnlockAttempt(View postingView) {
        postingView->PostDelayed(mFakeUnlockAttemptRunnable, FAKE_ATTEMPT_DELAY);
    }

    protected LockPatternView.OnPatternListener mChooseNewLockPatternListener =
        new LockPatternView->OnPatternListener() {

        //@Override
        CARAPI OnPatternStart() {
            mLockPatternView->RemoveCallbacks(mClearPatternRunnable);
        }

        //@Override
        CARAPI OnPatternCleared() {
        }

        //@Override
        CARAPI OnPatternDetected(List<LockPatternView.Cell> pattern) {
            mLockPatternView->SetEnabled(FALSE);
            if (pattern->Size() >= MIN_LENGTH_BEFORE_REPORT) {
                new DecryptTask()->Execute(LockPatternUtils->PatternToString(pattern));
            } else {
                // Allow user to make as many of these as they want.
                FakeUnlockAttempt(mLockPatternView);
            }
        }

        //@Override
        CARAPI OnPatternCellAdded(List<Cell> pattern) {
        }
     };

     private void PasswordEntryInit() {
        // Password/pin case
        mPasswordEntry = (EditText) FindViewById(R.id.passwordEntry);
        if (mPasswordEntry != NULL){
            mPasswordEntry->SetOnEditorActionListener(this);
            mPasswordEntry->RequestFocus();
            // Become quiet when the user interacts with the Edit text screen.
            mPasswordEntry->SetOnKeyListener(this);
            mPasswordEntry->SetOnTouchListener(this);
            mPasswordEntry->AddTextChangedListener(this);
        }

        // Pattern case
        mLockPatternView = (LockPatternView) FindViewById(R.id.lockPattern);
        if (mLockPatternView != NULL) {
            mLockPatternView->SetOnPatternListener(mChooseNewLockPatternListener);
        }

        // Disable the Emergency call button if the device has no voice telephone capability
        if (!GetTelephonyManager()->IsVoiceCapable()) {
            final View emergencyCall = FindViewById(R.id.emergencyCallButton);
            if (emergencyCall != NULL) {
                Logger::D(TAG, "Removing the emergency Call button");
                emergencyCall->SetVisibility(View.GONE);
            }
        }

        final View imeSwitcher = FindViewById(R.id.switch_ime_button);
        final InputMethodManager imm = (InputMethodManager) GetSystemService(
                Context.INPUT_METHOD_SERVICE);
        if (imeSwitcher != NULL && HasMultipleEnabledIMEsOrSubtypes(imm, FALSE)) {
            imeSwitcher->SetVisibility(View.VISIBLE);
            imeSwitcher->SetOnClickListener(new OnClickListener() {
                    //@Override
                CARAPI OnClick(View v) {
                    imm->ShowInputMethodPicker();
                }
            });
        }

        // We want to keep the screen on while waiting for input. In minimal boot mode, the device
        // is completely non-functional, and we want the user to notice the device and enter a
        // password.
        if (mWakeLock == NULL) {
            Logger::D(TAG, "Acquiring wakelock.");
            final PowerManager pm = (PowerManager) GetSystemService(Context.POWER_SERVICE);
            if (pm != NULL) {
                mWakeLock = pm->NewWakeLock(PowerManager.FULL_WAKE_LOCK, TAG);
                mWakeLock->Acquire();
                // Keep awake for 10 minutes - if the user hasn't been alerted by then
                // best not to just drain their battery
                mReleaseWakeLockCountdown = 96; // 96 * 5 secs per click + 120 secs before we show this = 600
            }
        }

        // Asynchronously throw up the IME, since there are issues with requesting it to be shown
        // immediately.
        if (mLockPatternView == NULL && mCooldown <= 0) {
            mHandler->PostDelayed(new Runnable() {
                //@Override CARAPI Run() {
                    imm->ShowSoftInputUnchecked(0, NULL);
                }
            }, 0);
        }

        UpdateEmergencyCallButtonState();
        // Notify the user in 120 seconds that we are waiting for him to enter the password.
        mHandler->RemoveMessages(MESSAGE_NOTIFY);
        mHandler->SendEmptyMessageDelayed(MESSAGE_NOTIFY, 120 * 1000);

        // Dismiss secure & non-secure keyguards while this screen is showing.
        GetWindow()->AddFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD
                | WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
    }

    /**
     * Method adapted from com.android.inputmethod.latin.Utils
     *
     * @param imm The input method manager
     * @param shouldIncludeAuxiliarySubtypes
     * @return TRUE if we have multiple IMEs to choose from
     */
    private Boolean HasMultipleEnabledIMEsOrSubtypes(InputMethodManager imm,
            final Boolean shouldIncludeAuxiliarySubtypes) {
        final List<InputMethodInfo> enabledImis = imm->GetEnabledInputMethodList();

        // Number of the filtered IMEs
        Int32 filteredImisCount = 0;

        for (InputMethodInfo imi : enabledImis) {
            // We can return TRUE immediately after we find two or more filtered IMEs.
            if (filteredImisCount > 1) return TRUE;
            final List<InputMethodSubtype> subtypes =
                    imm->GetEnabledInputMethodSubtypeList(imi, TRUE);
            // IMEs that have no subtypes should be counted.
            if (subtypes->IsEmpty()) {
                ++filteredImisCount;
                continue;
            }

            Int32 auxCount = 0;
            for (InputMethodSubtype subtype : subtypes) {
                if (subtype->IsAuxiliary()) {
                    ++auxCount;
                }
            }
            final Int32 nonAuxCount = subtypes->Size() - auxCount;

            // IMEs that have one or more non-auxiliary subtypes should be counted.
            // If shouldIncludeAuxiliarySubtypes is TRUE, IMEs that have two or more auxiliary
            // subtypes should be counted as well.
            if (nonAuxCount > 0 || (shouldIncludeAuxiliarySubtypes && auxCount > 1)) {
                ++filteredImisCount;
                continue;
            }
        }

        return filteredImisCount > 1
        // imm->GetEnabledInputMethodSubtypeList(NULL, FALSE) will return the current IME's enabled
        // input method subtype (The current IME should be LatinIME.)
                || imm->GetEnabledInputMethodSubtypeList(NULL, FALSE).Size() > 1;
    }

    private IMountService GetMountService() {
        final IBinder service = ServiceManager->GetService("mount");
        if (service != NULL) {
            return IMountService.Stub->AsInterface(service);
        }
        return NULL;
    }

    //@Override
    public Boolean OnEditorAction(TextView v, Int32 actionId, KeyEvent event) {
        if (actionId == EditorInfo.IME_NULL || actionId == EditorInfo.IME_ACTION_DONE) {
            // Get the password
            final String password = v->GetText()->ToString();

            if (TextUtils->IsEmpty(password)) {
                return TRUE;
            }

            // Now that we have the password clear the password field.
            v->SetText(NULL);

            // Disable the password entry and back keypress while checking the password. These
            // we either be re-enabled if the password was wrong or after the cooldown period.
            mPasswordEntry->SetEnabled(FALSE);
            SetBackFunctionality(FALSE);

            if (password->Length() >= LockPatternUtils.MIN_LOCK_PATTERN_SIZE) {
                Logger::D(TAG, "Attempting to send command to decrypt");
                new DecryptTask()->Execute(password);
            } else {
                // Allow user to make as many of these as they want.
                FakeUnlockAttempt(mPasswordEntry);
            }

            return TRUE;
        }
        return FALSE;
    }

    /**
     * Set airplane mode on the device if it isn't an LTE device.
     * Full story: In minimal boot mode, we cannot save any state. In particular, we cannot save
     * any incoming SMS's. So SMSs that are received here will be silently dropped to the floor.
     * That is bad. Also, we cannot receive any telephone calls in this state. So to avoid
     * both these problems, we turn the radio off. However, on certain networks turning on and
     * off the radio takes a Int64 time. In such cases, we are better off leaving the radio
     * running so the latency of an E911 call is short.
     * The behavior after this is:
     * 1. Emergency dialing: the emergency dialer has logic to force the device out of
     *    airplane mode and restart the radio.
     * 2. Full boot: we read the persistent settings from the previous boot and restore the
     *    radio to whatever it was before it restarted. This also happens when rebooting a
     *    phone that has no encryption.
     */
    private final void SetAirplaneModeIfNecessary() {
        final Boolean isLteDevice =
                GetTelephonyManager()->GetLteOnCdmaMode() == PhoneConstants.LTE_ON_CDMA_TRUE;
        if (!isLteDevice) {
            Logger::D(TAG, "Going into airplane mode.");
            Settings::Global::>PutInt(GetContentResolver(), Settings::Global::AIRPLANE_MODE_ON, 1);
            final Intent intent = new Intent(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
            intent->PutExtra("state", TRUE);
            SendBroadcastAsUser(intent, UserHandle.ALL);
        }
    }

    /**
     * Code to update the state of, and handle clicks from, the "Emergency call" button.
     *
     * This code is mostly duplicated from the corresponding code in
     * LockPatternUtils and LockPatternKeyguardView under frameworks/base.
     */
    private void UpdateEmergencyCallButtonState() {
        final Button emergencyCall = (Button) FindViewById(R.id.emergencyCallButton);
        // The button isn't present at all in some configurations.
        if (emergencyCall == NULL)
            return;

        if (IsEmergencyCallCapable()) {
            emergencyCall->SetVisibility(View.VISIBLE);
            emergencyCall->SetOnClickListener(new View->OnClickListener() {
                    //@Override

                    CARAPI OnClick(View v) {
                        TakeEmergencyCallAction();
                    }
                });
        } else {
            emergencyCall->SetVisibility(View.GONE);
            return;
        }

        Int32 textId;
        if (GetTelecomManager()->IsInCall()) {
            // Show "return to call"
            textId = R::string::cryptkeeper_return_to_call;
        } else {
            textId = R::string::cryptkeeper_emergency_call;
        }
        emergencyCall->SetText(textId);
    }

    private Boolean IsEmergencyCallCapable() {
        return GetResources()->GetBoolean(R.bool.config_voice_capable);
    }

    private void TakeEmergencyCallAction() {
        TelecomManager telecomManager = GetTelecomManager();
        if (telecomManager->IsInCall()) {
            telecomManager->ShowInCallScreen(FALSE /* showDialpad */);
        } else {
            LaunchEmergencyDialer();
        }
    }


    private void LaunchEmergencyDialer() {
        final Intent intent = new IIntent::ACTION_EMERGENCY_DIAL);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                        | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        SetBackFunctionality(TRUE);
        StartActivity(intent);
    }

    private TelephonyManager GetTelephonyManager() {
        return (TelephonyManager) GetSystemService(Context.TELEPHONY_SERVICE);
    }

    private TelecomManager GetTelecomManager() {
        return (TelecomManager) GetSystemService(Context.TELECOM_SERVICE);
    }

    /**
     * Listen to key events so we can disable sounds when we get a keyinput in EditText.
     */
    private void DelayAudioNotification() {
        mNotificationCountdown = 20;
    }

    //@Override
    public Boolean OnKey(View v, Int32 keyCode, KeyEvent event) {
        DelayAudioNotification();
        return FALSE;
    }

    //@Override
    public Boolean OnTouch(View v, MotionEvent event) {
        DelayAudioNotification();
        return FALSE;
    }

    //@Override
    CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {
        return;
    }

    //@Override
    CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {
        DelayAudioNotification();
    }

    //@Override
    CARAPI AfterTextChanged(Editable s) {
        return;
    }
}
