

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Security::ICredentials;
using Elastos::Droid::Security::KeyChain::IKeyChainConnection;
using Elastos::Droid::Security::IKeyChain;
using Elastos::Droid::Security::IKeyStore;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Org::Bouncycastle::Asn1::IASN1InputStream;
using Elastos::Droid::Org::Bouncycastle::Asn1::Pkcs::IPrivateKeyInfo;

using org::Apache::Harmony::Security::Utils::IAlgNameMapper;

using Elastos::IO::IByteArrayInputStream;

/**
 * CredentialStorage handles KeyStore reset, unlock, and install.
 *
 * CredentialStorage has a pretty convoluted state machine to migrate
 * from the old style separate keystore password to a new key guard
 * based password, as well as to deal with setting up the key guard if
 * necessary.
 *
 * KeyStore: UNINITALIZED
 * KeyGuard: OFF
 * Action:   set up key guard
 * Notes:    factory state
 *
 * KeyStore: UNINITALIZED
 * KeyGuard: ON
 * Action:   confirm key guard
 * Notes:    user had key guard but no keystore and upgraded from pre-ICS
 *           OR user had key guard and pre-ICS keystore password which was then reset
 *
 * KeyStore: LOCKED
 * KeyGuard: OFF/ON
 * Action:   old unlock dialog
 * Notes:    assume old password, need to use it to unlock.
 *           if unlock, ensure key guard before install.
 *           if reset, treat as UNINITALIZED/OFF
 *
 * KeyStore: UNLOCKED
 * KeyGuard: OFF
 * Action:   set up key guard
 * Notes:    ensure key guard, then proceed
 *
 * KeyStore: UNLOCKED
 * keyguard: ON
 * Action:   normal unlock/install
 * Notes:    this is the common case
 */
public class CredentialStorage extends Activity {

    private static const String TAG = "CredentialStorage";

    public static const String ACTION_UNLOCK = "com.android.credentials.UNLOCK";
    public static const String ACTION_INSTALL = "com.android.credentials.INSTALL";
    public static const String ACTION_RESET = "com.android.credentials.RESET";

    // This is the minimum acceptable password quality.  If the current password quality is
    // lower than this, keystore should not be activated.
    static const Int32 MIN_PASSWORD_QUALITY = DevicePolicyManager.PASSWORD_QUALITY_SOMETHING;

    private static const Int32 CONFIRM_KEY_GUARD_REQUEST = 1;

    private final KeyStore mKeyStore = KeyStore->GetInstance();

    /**
     * When non-NULL, the bundle containing credentials to install.
     */
    private Bundle mInstallBundle;

    /**
     * After unsuccessful KeyStore.unlock, the number of unlock
     * attempts remaining before the KeyStore will reset itself.
     *
     * Reset to -1 on successful unlock or reset.
     */
    private Int32 mRetriesRemaining = -1;

    //@Override
    protected void OnResume() {
        super->OnResume();

        Intent intent = GetIntent();
        String action = intent->GetAction();
        UserManager userManager = (UserManager) GetSystemService(Context.USER_SERVICE);
        if (!userManager->HasUserRestriction(UserManager.DISALLOW_CONFIG_CREDENTIALS)) {
            if (ACTION_RESET->Equals(action)) {
                new ResetDialog();
            } else {
                if (ACTION_INSTALL->Equals(action)
                        && "com.android.certinstaller".Equals(GetCallingPackage())) {
                    mInstallBundle = intent->GetExtras();
                }
                // ACTION_UNLOCK also handled here in addition to ACTION_INSTALL
                HandleUnlockOrInstall();
            }
        } else {
            Finish();
        }
    }

    /**
     * Based on the current state of the KeyStore and key guard, try to
     * make progress on unlocking or installing to the keystore.
     */
    private void HandleUnlockOrInstall() {
        // something already decided we are done, do not proceed
        if (IsFinishing()) {
            return;
        }
        switch (mKeyStore->State()) {
            case UNINITIALIZED: {
                EnsureKeyGuard();
                return;
            }
            case LOCKED: {
                new UnlockDialog();
                return;
            }
            case UNLOCKED: {
                if (!CheckKeyGuardQuality()) {
                    new ConfigureKeyGuardDialog();
                    return;
                }
                InstallIfAvailable();
                Finish();
                return;
            }
        }
    }

    /**
     * Make sure the user enters the key guard to set or change the
     * keystore password. This can be used in UNINITIALIZED to set the
     * keystore password or UNLOCKED to change the password (as is the
     * case after unlocking with an old-style password).
     */
    private void EnsureKeyGuard() {
        if (!CheckKeyGuardQuality()) {
            // key guard not setup, doing so will initialize keystore
            new ConfigureKeyGuardDialog();
            // will return to onResume after Activity
            return;
        }
        // force key guard confirmation
        if (ConfirmKeyGuard()) {
            // will return password value via onActivityResult
            return;
        }
        Finish();
    }

    /**
     * Returns TRUE if the currently set key guard matches our minimum quality requirements.
     */
    private Boolean CheckKeyGuardQuality() {
        Int32 quality = new LockPatternUtils(this).GetActivePasswordQuality();
        return (quality >= MIN_PASSWORD_QUALITY);
    }

    private Boolean IsHardwareBackedKey(Byte[] keyData) {
        try {
            ASN1InputStream bIn = new ASN1InputStream(new ByteArrayInputStream(keyData));
            PrivateKeyInfo pki = PrivateKeyInfo->GetInstance(bIn->ReadObject());
            String algId = pki->GetAlgorithmId()->GetAlgorithm().GetId();
            String algName = AlgNameMapper->Map2AlgName(algId);

            return KeyChain->IsBoundKeyAlgorithm(algName);
        } catch (IOException e) {
            Logger::E(TAG, "Failed to parse key data");
            return FALSE;
        }
    }

    /**
     * Install credentials if available, otherwise do nothing.
     */
    private void InstallIfAvailable() {
        if (mInstallBundle != NULL && !mInstallBundle->IsEmpty()) {
            Bundle bundle = mInstallBundle;
            mInstallBundle = NULL;

            final Int32 uid = bundle->GetInt(Credentials.EXTRA_INSTALL_AS_UID, -1);

            if (bundle->ContainsKey(Credentials.EXTRA_USER_PRIVATE_KEY_NAME)) {
                String key = bundle->GetString(Credentials.EXTRA_USER_PRIVATE_KEY_NAME);
                Byte[] value = bundle->GetByteArray(Credentials.EXTRA_USER_PRIVATE_KEY_DATA);

                Int32 flags = KeyStore.FLAG_ENCRYPTED;
                if (uid == Process.WIFI_UID && IsHardwareBackedKey(value)) {
                    // Hardware backed keystore is secure enough to allow for WIFI stack
                    // to enable access to secure networks without user intervention
                    Logger::D(TAG, "Saving private key with FLAG_NONE for WIFI_UID");
                    flags = KeyStore.FLAG_NONE;
                }

                if (!mKeyStore->ImportKey(key, value, uid, flags)) {
                    Logger::E(TAG, "Failed to install " + key + " as user " + uid);
                    return;
                }
            }

            Int32 flags = (uid == Process.WIFI_UID) ? KeyStore.FLAG_NONE : KeyStore.FLAG_ENCRYPTED;

            if (bundle->ContainsKey(Credentials.EXTRA_USER_CERTIFICATE_NAME)) {
                String certName = bundle->GetString(Credentials.EXTRA_USER_CERTIFICATE_NAME);
                Byte[] certData = bundle->GetByteArray(Credentials.EXTRA_USER_CERTIFICATE_DATA);

                if (!mKeyStore->Put(certName, certData, uid, flags)) {
                    Logger::E(TAG, "Failed to install " + certName + " as user " + uid);
                    return;
                }
            }

            if (bundle->ContainsKey(Credentials.EXTRA_CA_CERTIFICATES_NAME)) {
                String caListName = bundle->GetString(Credentials.EXTRA_CA_CERTIFICATES_NAME);
                Byte[] caListData = bundle->GetByteArray(Credentials.EXTRA_CA_CERTIFICATES_DATA);

                if (!mKeyStore->Put(caListName, caListData, uid, flags)) {
                    Logger::E(TAG, "Failed to install " + caListName + " as user " + uid);
                    return;
                }
            }

            SetResult(RESULT_OK);
        }
    }

    /**
     * Prompt for reset confirmation, resetting on confirmation, finishing otherwise.
     */
    private class ResetDialog
            implements DialogInterface.OnClickListener, DialogInterface.OnDismissListener
    {
        private Boolean mResetConfirmed;

        private ResetDialog() {
            AlertDialog dialog = new AlertDialog->Builder(CredentialStorage.this)
                    .SetTitle(android.R::string::dialog_alert_title)
                    .SetMessage(R::string::credentials_reset_hint)
                    .SetPositiveButton(android.R::string::ok, this)
                    .SetNegativeButton(android.R::string::cancel, this)
                    .Create();
            dialog->SetOnDismissListener(this);
            dialog->Show();
        }

        //@Override CARAPI OnClick(DialogInterface dialog, Int32 button) {
            mResetConfirmed = (button == DialogInterface.BUTTON_POSITIVE);
        }

        //@Override CARAPI OnDismiss(DialogInterface dialog) {
            if (mResetConfirmed) {
                mResetConfirmed = FALSE;
                new ResetKeyStoreAndKeyChain()->Execute();
                return;
            }
            Finish();
        }
    }

    /**
     * Background task to handle reset of both keystore and user installed CAs.
     */
    private class ResetKeyStoreAndKeyChain extends AsyncTask<Void, Void, Boolean> {

        //@Override protected Boolean DoInBackground(Void... unused) {

            mKeyStore->Reset();

            try {
                KeyChainConnection keyChainConnection = KeyChain->Bind(CredentialStorage.this);
                try {
                    return keyChainConnection->GetService()->Reset();
                } catch (RemoteException e) {
                    return FALSE;
                } finally {
                    keyChainConnection->Close();
                }
            } catch (InterruptedException e) {
                Thread->CurrentThread()->Interrupt();
                return FALSE;
            }
        }

        //@Override protected void OnPostExecute(Boolean success) {
            if (success) {
                Toast->MakeText(CredentialStorage.this,
                               R::string::credentials_erased, Toast.LENGTH_SHORT).Show();
            } else {
                Toast->MakeText(CredentialStorage.this,
                               R::string::credentials_not_erased, Toast.LENGTH_SHORT).Show();
            }
            Finish();
        }
    }

    /**
     * Prompt for key guard configuration confirmation.
     */
    private class ConfigureKeyGuardDialog
            implements DialogInterface.OnClickListener, DialogInterface.OnDismissListener
    {
        private Boolean mConfigureConfirmed;

        private ConfigureKeyGuardDialog() {
            AlertDialog dialog = new AlertDialog->Builder(CredentialStorage.this)
                    .SetTitle(android.R::string::dialog_alert_title)
                    .SetMessage(R::string::credentials_configure_lock_screen_hint)
                    .SetPositiveButton(android.R::string::ok, this)
                    .SetNegativeButton(android.R::string::cancel, this)
                    .Create();
            dialog->SetOnDismissListener(this);
            dialog->Show();
        }

        //@Override CARAPI OnClick(DialogInterface dialog, Int32 button) {
            mConfigureConfirmed = (button == DialogInterface.BUTTON_POSITIVE);
        }

        //@Override CARAPI OnDismiss(DialogInterface dialog) {
            if (mConfigureConfirmed) {
                mConfigureConfirmed = FALSE;
                Intent intent = new Intent(DevicePolicyManager.ACTION_SET_NEW_PASSWORD);
                intent->PutExtra(ChooseLockGeneric.ChooseLockGenericFragment.MINIMUM_QUALITY_KEY,
                                MIN_PASSWORD_QUALITY);
                StartActivity(intent);
                return;
            }
            Finish();
        }
    }

    /**
     * Confirm existing key guard, returning password via onActivityResult.
     */
    private Boolean ConfirmKeyGuard() {
        Resources res = GetResources();
        Boolean launched = new ChooseLockSettingsHelper(this)
                .LaunchConfirmationActivity(CONFIRM_KEY_GUARD_REQUEST,
                                            res->GetText(R::string::credentials_install_gesture_prompt),
                                            res->GetText(R::string::credentials_install_gesture_explanation),
                                            TRUE);
        return launched;
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);

        /**
         * Receive key guard password initiated by confirmKeyGuard.
         */
        if (requestCode == CONFIRM_KEY_GUARD_REQUEST) {
            if (resultCode == Activity.RESULT_OK) {
                String password = data->GetStringExtra(ChooseLockSettingsHelper.EXTRA_KEY_PASSWORD);
                if (!TextUtils->IsEmpty(password)) {
                    // success
                    mKeyStore->Password(password);
                    // return to onResume
                    return;
                }
            }
            // failed confirmation, bail
            Finish();
        }
    }

    /**
     * Prompt for unlock with old-style password.
     *
     * On successful unlock, ensure migration to key guard before continuing.
     * On unsuccessful unlock, retry by calling handleUnlockOrInstall.
     */
    private class UnlockDialog implements TextWatcher,
            DialogInterface.OnClickListener, DialogInterface.OnDismissListener
    {
        private Boolean mUnlockConfirmed;

        private final Button mButton;
        private final TextView mOldPassword;
        private final TextView mError;

        private UnlockDialog() {
            View view = View->Inflate(CredentialStorage.this, R.layout.credentials_dialog, NULL);

            CharSequence text;
            if (mRetriesRemaining == -1) {
                text = GetResources()->GetText(R::string::credentials_unlock_hint);
            } else if (mRetriesRemaining > 3) {
                text = GetResources()->GetText(R::string::credentials_wrong_password);
            } else if (mRetriesRemaining == 1) {
                text = GetResources()->GetText(R::string::credentials_reset_warning);
            } else {
                text = GetString(R::string::credentials_reset_warning_plural, mRetriesRemaining);
            }

            ((TextView) view->FindViewById(R.id.hint)).SetText(text);
            mOldPassword = (TextView) view->FindViewById(R.id.old_password);
            mOldPassword->SetVisibility(View.VISIBLE);
            mOldPassword->AddTextChangedListener(this);
            mError = (TextView) view->FindViewById(R.id.error);

            AlertDialog dialog = new AlertDialog->Builder(CredentialStorage.this)
                    .SetView(view)
                    .SetTitle(R::string::credentials_unlock)
                    .SetPositiveButton(android.R::string::ok, this)
                    .SetNegativeButton(android.R::string::cancel, this)
                    .Create();
            dialog->SetOnDismissListener(this);
            dialog->Show();
            mButton = dialog->GetButton(DialogInterface.BUTTON_POSITIVE);
            mButton->SetEnabled(FALSE);
        }

        //@Override CARAPI AfterTextChanged(Editable editable) {
            mButton->SetEnabled(mOldPassword == NULL || mOldPassword->GetText()->Length() > 0);
        }

        //@Override CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {
        }

        //@Override CARAPI OnTextChanged(CharSequence s,Int32 start, Int32 before, Int32 count) {
        }

        //@Override CARAPI OnClick(DialogInterface dialog, Int32 button) {
            mUnlockConfirmed = (button == DialogInterface.BUTTON_POSITIVE);
        }

        //@Override CARAPI OnDismiss(DialogInterface dialog) {
            if (mUnlockConfirmed) {
                mUnlockConfirmed = FALSE;
                mError->SetVisibility(View.VISIBLE);
                mKeyStore->Unlock(mOldPassword->GetText()->ToString());
                Int32 error = mKeyStore->GetLastError();
                if (error == KeyStore.NO_ERROR) {
                    mRetriesRemaining = -1;
                    Toast->MakeText(CredentialStorage.this,
                                   R::string::credentials_enabled,
                                   Toast.LENGTH_SHORT).Show();
                    // aha, now we are unlocked, switch to key guard.
                    // we'll end up back in onResume to install
                    EnsureKeyGuard();
                } else if (error == KeyStore.UNINITIALIZED) {
                    mRetriesRemaining = -1;
                    Toast->MakeText(CredentialStorage.this,
                                   R::string::credentials_erased,
                                   Toast.LENGTH_SHORT).Show();
                    // we are reset, we can now set new password with key guard
                    HandleUnlockOrInstall();
                } else if (error >= KeyStore.WRONG_PASSWORD) {
                    // we need to try again
                    mRetriesRemaining = error - KeyStore.WRONG_PASSWORD + 1;
                    HandleUnlockOrInstall();
                }
                return;
            }
            Finish();
        }
    }
}
