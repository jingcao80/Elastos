#ifndef __ELASTOS_DROID_SETTINGS_CCREDENTIALSTORAGE_H__
#define __ELASTOS_DROID_SETTINGS_CCREDENTIALSTORAGE_H__

#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_CCredentialStorage.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::KeyStore::Security::IKeyStore;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;

namespace Elastos {
namespace Droid {
namespace Settings {

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
CarClass(CCredentialStorage)
    , public Activity
{
private:
    /**
     * Prompt for reset confirmation, resetting on confirmation, finishing otherwise.
     */
    class ResetDialog
        : public Object
        , public IDialogInterfaceOnClickListener
        , public IDialogInterfaceOnDismissListener
    {
    public:
        TO_STRING_IMPL("CCredentialStorage::ResetDialog")

        CAR_INTERFACE_DECL()

        ResetDialog(
            /* [in] */ CCredentialStorage* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 button);

        //@Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        Boolean mResetConfirmed;
        CCredentialStorage* mHost;
    };

    /**
     * Background task to handle reset of both keystore and user installed CAs.
     */
    class ResetKeyStoreAndKeyChain
        // AsyncTask<Void, Void, Boolean>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("CCredentialStorage::ResetKeyStoreAndKeyChain")

        ResetKeyStoreAndKeyChain(
            /* [in] */ CCredentialStorage* host);

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* unused,
            /* [out] */ IInterface** result); //Boolean

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* success); // Boolean

    private:
        CCredentialStorage* mHost;
    };

    /**
     * Prompt for key guard configuration confirmation.
     */
    class ConfigureKeyGuardDialog
        : public Object
        , public IDialogInterfaceOnClickListener
        , public IDialogInterfaceOnDismissListener
    {
    public:
        TO_STRING_IMPL("CCredentialStorage::ConfigureKeyGuardDialog")

        CAR_INTERFACE_DECL()

        ConfigureKeyGuardDialog(
            /* [in] */ CCredentialStorage* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 button);

        //@Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        Boolean mConfigureConfirmed;
        CCredentialStorage* mHost;
    };

    /**
     * Prompt for unlock with old-style password.
     *
     * On successful unlock, ensure migration to key guard before continuing.
     * On unsuccessful unlock, retry by calling handleUnlockOrInstall.
     */
    class UnlockDialog
        : public Object
        , public IDialogInterfaceOnClickListener
        , public IDialogInterfaceOnDismissListener
    {
    private:
        class InnerListener
            : public Object
            , public ITextWatcher
        {
        public:
            TO_STRING_IMPL("CCredentialStorage::UnlockDialog::InnerListener")

            CAR_INTERFACE_DECL()

            InnerListener(
                /* [in] */ UnlockDialog* host);

            //@Override
            CARAPI AfterTextChanged(
                /* [in] */ IEditable* editable);

            //@Override
            CARAPI BeforeTextChanged(
                /* [in] */ ICharSequence* s,
                /* [in] */ Int32 start,
                /* [in] */ Int32 count,
                /* [in] */ Int32 after);

            //@Override
            CARAPI OnTextChanged(
                /* [in] */ ICharSequence* s,
                /* [in] */ Int32 start,
                /* [in] */ Int32 before,
                /* [in] */ Int32 count);

        private:
            UnlockDialog* mHost;
        };

    public:
        TO_STRING_IMPL("CCredentialStorage::UnlockDialog")

        CAR_INTERFACE_DECL()

        UnlockDialog(
            /* [in] */ CCredentialStorage* host);

        //@Override
        CARAPI AfterTextChanged(
            /* [in] */ IEditable* editable);

        //@Override
        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        //@Override
        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 button);

        //@Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        Boolean mUnlockConfirmed;

        AutoPtr<IButton> mButton;
        AutoPtr<ITextView> mOldPassword;
        AutoPtr<ITextView> mError;
        CCredentialStorage* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CCredentialStorage();

    virtual ~CCredentialStorage();

    CARAPI constructor();

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

protected:
    //@Override
    CARAPI OnResume();

private:
    /**
     * Based on the current state of the KeyStore and key guard, try to
     * make progress on unlocking or installing to the keystore.
     */
    CARAPI_(void) HandleUnlockOrInstall();

    /**
     * Make sure the user enters the key guard to set or change the
     * keystore password. This can be used in UNINITIALIZED to set the
     * keystore password or UNLOCKED to change the password (as is the
     * case after unlocking with an old-style password).
     */
    CARAPI_(void) EnsureKeyGuard();

    /**
     * Returns TRUE if the currently set key guard matches our minimum quality requirements.
     */
    CARAPI_(Boolean) CheckKeyGuardQuality();

    CARAPI_(Boolean) IsHardwareBackedKey(
        /* [in] */ ArrayOf<Byte>* keyData);

    /**
     * Install credentials if available, otherwise do nothing.
     */
    CARAPI_(void) InstallIfAvailable();

    /**
     * Confirm existing key guard, returning password via onActivityResult.
     */
    CARAPI_(Boolean) ConfirmKeyGuard();

public:
    static const String ACTION_UNLOCK;// = "com.android.credentials.UNLOCK";
    static const String ACTION_INSTALL;// = "com.android.credentials.INSTALL";
    static const String ACTION_RESET;// = "com.android.credentials.RESET";

    // This is the minimum acceptable password quality.  If the current password quality is
    // lower than this, keystore should not be activated.
    static const Int32 MIN_PASSWORD_QUALITY;// = IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING;

private:
    static const String TAG;// = "CCredentialStorage";

    static const Int32 CONFIRM_KEY_GUARD_REQUEST;// = 1;

    AutoPtr<IKeyStore> mKeyStore;

    /**
     * When non-NULL, the bundle containing credentials to install.
     */
    AutoPtr<IBundle> mInstallBundle;

    /**
     * After unsuccessful KeyStore.unlock, the number of unlock
     * attempts remaining before the KeyStore will reset itself.
     *
     * Reset to -1 on successful unlock or reset.
     */
    Int32 mRetriesRemaining;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCREDENTIALSTORAGE_H__