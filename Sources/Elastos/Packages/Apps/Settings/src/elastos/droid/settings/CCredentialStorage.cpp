//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.KeyStore.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/CCredentialStorage.h"
#include "elastos/droid/settings/CChooseLockGenericFragment.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::KeyStore::Security::KeyStoreState;
using Elastos::Droid::KeyStore::Security::KeyStoreState_UNINITIALIZED;
using Elastos::Droid::KeyStore::Security::KeyStoreState_LOCKED;
using Elastos::Droid::KeyStore::Security::KeyStoreState_UNLOCKED;
using Elastos::Droid::KeyStore::Security::IIKeyChainService;
using Elastos::Droid::KeyStore::Security::IKeyChain;
using Elastos::Droid::KeyStore::Security::CKeyChain;
using Elastos::Droid::KeyStore::Security::IKeyChainConnection;
using Elastos::Droid::KeyStore::Security::ICredentials;
using Elastos::Droid::KeyStore::Security::CKeyStoreHelper;
using Elastos::Droid::KeyStore::Security::IKeyStoreHelper;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
// using Elastos::Droid::Org::Bouncycastle::Asn1::IASN1InputStream;
// using Elastos::Droid::Org::Bouncycastle::Asn1::Pkcs::IPrivateKeyInfo;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICloseable;
// using Elastos::IO::CByteArrayInputStream;
// using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;
// using org::Apache::Harmony::Security::Utils::IAlgNameMapper;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CCredentialStorage::ResetDialog
//===============================================================================

CAR_INTERFACE_IMPL_2(CCredentialStorage::ResetDialog, Object, IDialogInterfaceOnClickListener, IDialogInterfaceOnDismissListener)

CCredentialStorage::ResetDialog::ResetDialog(
    /* [in] */ CCredentialStorage* host)
    : mResetConfirmed(FALSE)
    , mHost(host)
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(Elastos::Droid::R::string::dialog_alert_title);
    builder->SetMessage(R::string::credentials_reset_hint);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, this);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, this);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    IDialog* _dialog = IDialog::Probe(dialog);
    _dialog->SetOnDismissListener(this);
    _dialog->Show();
}

ECode CCredentialStorage::ResetDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 button)
{
    mResetConfirmed = (button == IDialogInterface::BUTTON_POSITIVE);
    return NOERROR;
}

ECode CCredentialStorage::ResetDialog::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (mResetConfirmed) {
        mResetConfirmed = FALSE;
        AutoPtr<ResetKeyStoreAndKeyChain> task = new ResetKeyStoreAndKeyChain(mHost);
        AutoPtr< ArrayOf<IInterface*> > arg;
        task->Execute(arg);
        return NOERROR;
    }
    mHost->Finish();
    return NOERROR;
}

//===============================================================================
//                  CCredentialStorage::ResetKeyStoreAndKeyChain
//===============================================================================

CCredentialStorage::ResetKeyStoreAndKeyChain::ResetKeyStoreAndKeyChain(
    /* [in] */ CCredentialStorage* host)
    : mHost(host)
{}

ECode CCredentialStorage::ResetKeyStoreAndKeyChain::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* unused,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Boolean res;
    mHost->mKeyStore->Reset(&res);

    AutoPtr<IKeyChain> kc;
    CKeyChain::AcquireSingleton((IKeyChain**)&kc);
    AutoPtr<IKeyChainConnection> keyChainConnection;
    ECode ec = kc->Bind(mHost, (IKeyChainConnection**)&keyChainConnection);
    if (FAILED(ec)) {
        Thread::GetCurrentThread()->Interrupt();
        AutoPtr<IBoolean> tmp = CoreUtils::Convert(FALSE);
        *result = tmp;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        AutoPtr<IIKeyChainService> kcs;
        ec = keyChainConnection->GetService((IIKeyChainService**)&kcs);
        if (FAILED(ec)) {
            AutoPtr<IBoolean> tmp = CoreUtils::Convert(FALSE);
            *result = tmp;
            REFCOUNT_ADD(*result)

            ICloseable::Probe(keyChainConnection)->Close();
            return NOERROR;
        }
        else {
            kcs->Reset(&res);
            AutoPtr<IBoolean> tmp = CoreUtils::Convert(res);
            *result = tmp;
            REFCOUNT_ADD(*result)

            ICloseable::Probe(keyChainConnection)->Close();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CCredentialStorage::ResetKeyStoreAndKeyChain::OnPostExecute(
    /* [in] */ IInterface* success)
{
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    Boolean res;
    IBoolean::Probe(success)->GetValue(&res);
    if (res) {
        helper->MakeText(mHost,
                R::string::credentials_erased, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    else {
        helper->MakeText(mHost,
                R::string::credentials_not_erased, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    return mHost->Finish();
}

//===============================================================================
//                  CCredentialStorage::ConfigureKeyGuardDialog
//===============================================================================

CAR_INTERFACE_IMPL_2(CCredentialStorage::ConfigureKeyGuardDialog, Object, IDialogInterfaceOnClickListener, IDialogInterfaceOnDismissListener)

CCredentialStorage::ConfigureKeyGuardDialog::ConfigureKeyGuardDialog(
    /* [in] */ CCredentialStorage* host)
    : mConfigureConfirmed(FALSE)
    , mHost(host)
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(Elastos::Droid::R::string::dialog_alert_title);
    builder->SetMessage(R::string::credentials_configure_lock_screen_hint);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, this);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, this);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    IDialog* _dialog = IDialog::Probe(dialog);
    _dialog->SetOnDismissListener(this);
    _dialog->Show();
}

ECode CCredentialStorage::ConfigureKeyGuardDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 button)
{
    mConfigureConfirmed = (button == IDialogInterface::BUTTON_POSITIVE);
    return NOERROR;
}

ECode CCredentialStorage::ConfigureKeyGuardDialog::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (mConfigureConfirmed) {
        mConfigureConfirmed = FALSE;
        AutoPtr<IIntent> intent;
        CIntent::New(IDevicePolicyManager::ACTION_SET_NEW_PASSWORD, (IIntent**)&intent);
        intent->PutExtra(CChooseLockGenericFragment::MINIMUM_QUALITY_KEY,
                        MIN_PASSWORD_QUALITY);
        mHost->StartActivity(intent);
        return NOERROR;
    }
    return mHost->Finish();
}

//===============================================================================
//                  CCredentialStorage::UnlockDialog::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CCredentialStorage::UnlockDialog::InnerListener, Object, ITextWatcher)

CCredentialStorage::UnlockDialog::InnerListener::InnerListener(
    /* [in] */ UnlockDialog* host)
    : mHost(host)
{}

ECode CCredentialStorage::UnlockDialog::InnerListener::AfterTextChanged(
    /* [in] */ IEditable* editable)
{
    return mHost->AfterTextChanged(editable);
}

ECode CCredentialStorage::UnlockDialog::InnerListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode CCredentialStorage::UnlockDialog::InnerListener::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return mHost->OnTextChanged(s, start, before, count);
}

//===============================================================================
//                  CCredentialStorage::UnlockDialog
//===============================================================================

CAR_INTERFACE_IMPL_2(CCredentialStorage::UnlockDialog, Object, IDialogInterfaceOnClickListener, IDialogInterfaceOnDismissListener)

CCredentialStorage::UnlockDialog::UnlockDialog(
    /* [in] */ CCredentialStorage* host)
    : mUnlockConfirmed(FALSE)
    , mHost(host)
{
    AutoPtr<IView> view;
    Elastos::Droid::View::View::Inflate(mHost, R::layout::credentials_dialog, NULL, (IView**)&view);

    AutoPtr<IResources> resources;
    mHost->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text;
    if (mHost->mRetriesRemaining == -1) {
        resources->GetText(R::string::credentials_unlock_hint, (ICharSequence**)&text);
    }
    else if (mHost->mRetriesRemaining > 3) {
        resources->GetText(R::string::credentials_wrong_password, (ICharSequence**)&text);
    }
    else if (mHost->mRetriesRemaining == 1) {
        resources->GetText(R::string::credentials_reset_warning, (ICharSequence**)&text);
    }
    else {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(mHost->mRetriesRemaining));
        String str;
        mHost->GetString(R::string::credentials_reset_warning_plural, args, &str);
        text = CoreUtils::Convert(str);
    }

    AutoPtr<IView> tmp;
    view->FindViewById(R::id::hint, (IView**)&tmp);
    ITextView::Probe(tmp)->SetText(text);
    AutoPtr<IView> oldPassword;
    view->FindViewById(R::id::old_password, (IView**)&oldPassword);
    mOldPassword = ITextView::Probe(oldPassword);
    oldPassword->SetVisibility(IView::VISIBLE);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mOldPassword->AddTextChangedListener(listener);
    tmp = NULL;
    view->FindViewById(R::id::error, (IView**)&tmp);
    mError = ITextView::Probe(tmp);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost, (IAlertDialogBuilder**)&builder);
    builder->SetView(view);
    builder->SetTitle(R::string::credentials_unlock);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, this);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, this);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    IDialog* _dialog = IDialog::Probe(dialog);
    _dialog->SetOnDismissListener(this);
    _dialog->Show();

    dialog->GetButton(IDialogInterface::BUTTON_POSITIVE, (IButton**)&mButton);
    IView::Probe(mButton)->SetEnabled(FALSE);
}

ECode CCredentialStorage::UnlockDialog::AfterTextChanged(
    /* [in] */ IEditable* editable)
{
    AutoPtr<ICharSequence> cs;
    Int32 len;
    IView::Probe(mButton)->SetEnabled(mOldPassword == NULL ||
            ((mOldPassword->GetText((ICharSequence**)&cs), cs)->GetLength(&len), len) > 0);
    return NOERROR;
}

ECode CCredentialStorage::UnlockDialog::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode CCredentialStorage::UnlockDialog::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return NOERROR;
}

ECode CCredentialStorage::UnlockDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 button)
{
    mUnlockConfirmed = (button == IDialogInterface::BUTTON_POSITIVE);
    return NOERROR;
}

ECode CCredentialStorage::UnlockDialog::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (mUnlockConfirmed) {
        mUnlockConfirmed = FALSE;
        IView::Probe(mError)->SetVisibility(IView::VISIBLE);
        AutoPtr<ICharSequence> cs;
        mOldPassword->GetText((ICharSequence**)&cs);
        Boolean res;
        mHost->mKeyStore->Unlock(CoreUtils::Unbox(cs), &res);
        Int32 error;
        mHost->mKeyStore->GetLastError(&error);
        if (error == IKeyStore::NO_ERROR) {
            mHost->mRetriesRemaining = -1;
            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            helper->MakeText(mHost, R::string::credentials_enabled, IToast::LENGTH_SHORT, (IToast**)&toast);
            toast->Show();
            // aha, now we are unlocked, switch to key guard.
            // we'll end up back in onResume to install
            mHost->EnsureKeyGuard();
        }
        else if (error == IKeyStore::UNINITIALIZED) {
            mHost->mRetriesRemaining = -1;
            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            helper->MakeText(mHost, R::string::credentials_erased, IToast::LENGTH_SHORT, (IToast**)&toast);
            toast->Show();
            // we are reset, we can now set new password with key guard
            mHost->HandleUnlockOrInstall();
        }
        else if (error >= IKeyStore::WRONG_PASSWORD) {
            // we need to try again
            mHost->mRetriesRemaining = error - IKeyStore::WRONG_PASSWORD + 1;
            mHost->HandleUnlockOrInstall();
        }
        return NOERROR;
    }
    mHost->Finish();
    return NOERROR;
}

//===============================================================================
//                  CCredentialStorage
//===============================================================================

const String CCredentialStorage::ACTION_UNLOCK("com.android.credentials.UNLOCK");
const String CCredentialStorage::ACTION_INSTALL("com.android.credentials.INSTALL");
const String CCredentialStorage::ACTION_RESET("com.android.credentials.RESET");

const Int32 CCredentialStorage::MIN_PASSWORD_QUALITY = IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING;

const String CCredentialStorage::TAG("CCredentialStorage");

const Int32 CCredentialStorage::CONFIRM_KEY_GUARD_REQUEST = 1;


CAR_OBJECT_IMPL(CCredentialStorage)

CCredentialStorage::CCredentialStorage()
    : mRetriesRemaining(-1)
{}

CCredentialStorage::~CCredentialStorage()
{}

ECode CCredentialStorage::constructor()
{
    AutoPtr<IKeyStoreHelper> helper;
    CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
    helper->GetInstance((IKeyStore**)&mKeyStore);
    return Activity::constructor();
}

ECode CCredentialStorage::OnResume()
{
    Activity::OnResume();

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
    Boolean res;
    if (userManager->HasUserRestriction(IUserManager::DISALLOW_CONFIG_CREDENTIALS, &res), !res) {
        if (ACTION_RESET.Equals(action)) {
            AutoPtr<ResetDialog> dialog = new ResetDialog(this);
        }
        else {
            String package;
            if (ACTION_INSTALL.Equals(action)
                    && String("com.android.certinstaller").Equals((GetCallingPackage(&package), package))) {
                intent->GetExtras((IBundle**)&mInstallBundle);
            }
            // ACTION_UNLOCK also handled here in addition to ACTION_INSTALL
            HandleUnlockOrInstall();
        }
    }
    else {
        Finish();
    }
    return NOERROR;
}

void CCredentialStorage::HandleUnlockOrInstall()
{
    // something already decided we are done, do not proceed
    Boolean isFinished;
    if (IsFinishing(&isFinished), isFinished) {
        return;
    }
    KeyStoreState kss;
    mKeyStore->State(&kss);
    switch (kss) {
        case KeyStoreState_UNINITIALIZED: {
            EnsureKeyGuard();
            return;
        }
        case KeyStoreState_LOCKED: {
            AutoPtr<UnlockDialog> dialog = new UnlockDialog(this);
            return;
        }
        case KeyStoreState_UNLOCKED: {
            if (!CheckKeyGuardQuality()) {
                AutoPtr<ConfigureKeyGuardDialog> dialog = new ConfigureKeyGuardDialog(this);
                return;
            }
            InstallIfAvailable();
            Finish();
            return;
        }
    }
}

void CCredentialStorage::EnsureKeyGuard()
{
    if (!CheckKeyGuardQuality()) {
        // key guard not setup, doing so will initialize keystore
        AutoPtr<ConfigureKeyGuardDialog> dialog = new ConfigureKeyGuardDialog(this);
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

Boolean CCredentialStorage::CheckKeyGuardQuality()
{
    AutoPtr<ILockPatternUtils> lpu;
    CLockPatternUtils::New(this, (ILockPatternUtils**)&lpu);
    Int32 quality;
    lpu->GetActivePasswordQuality(&quality);
    return (quality >= MIN_PASSWORD_QUALITY);
}

Boolean CCredentialStorage::IsHardwareBackedKey(
    /* [in] */ ArrayOf<Byte>* keyData)
{
    assert(0 && "TODO");
    Logger::E(TAG, "IsHardwareBackedKey, CASN1InputStream is TODO");
    return FALSE;

    // AutoPtr<IInputStream> is;
    // CByteArrayInputStream::New(keyData, (IInputStream**)&is);
    // AutoPtr<IASN1InputStream> bin;
    // CASN1InputStream::New(is, (IASN1InputStream**)&bin);
    // AutoPtr<IASN1Primitive> pri;
    // ECode ec = bIn->ReadObject((IASN1Primitive**)&pri);
    // if (ec == (ECode)E_IO_EXCEPTION) {
    //     Logger::E(TAG, "Failed to parse key data");
    //     return FALSE;

    // }
    // AutoPtr<IPrivateKeyInfoHelper> helper;
    // CPrivateKeyInfoHelper::AcquireSingleton((IPrivateKeyInfoHelper**)&helper);
    // AutoPtr<IPrivateKeyInfo> pki;
    // helper->GetInstance(pri, (IPrivateKeyInfo**)&pki);
    // // String algId = pki.getAlgorithmId().getAlgorithm().getId();
    // AutoPtr<IAlgorithmIdentifier> ai;
    // pki->GetAlgorithmId((IAlgorithmIdentifier**)&ai);
    // String algId;
    // ai->GetAlgorithm(&algId);
    // AutoPtr<IAlgNameMapperHelper> anmHelper;
    // CAlgNameMapperHelper::AcquireSingleton((IAlgNameMapperHelper**)&anmHelper);
    // String algName;
    // anmHelper->Map2AlgName(algId, &algName);

    // AutoPtr<IKeyChain> helper;
    // CKeyChain::AcquireSingleton((IKeyChain**)&helper);
    // Boolean res;
    // helper->IsBoundKeyAlgorithm(algName, &res);
    // return res;
}

void CCredentialStorage::InstallIfAvailable()
{
    Boolean res;
    if (mInstallBundle != NULL && (mInstallBundle->IsEmpty(&res), !res)) {
        AutoPtr<IBundle> bundle = mInstallBundle;
        mInstallBundle = NULL;

        Int32 uid;
        bundle->GetInt32(ICredentials::EXTRA_INSTALL_AS_UID, -1, &uid);

        if (bundle->ContainsKey(ICredentials::EXTRA_USER_PRIVATE_KEY_NAME, &res), res) {
            String key;
            bundle->GetString(ICredentials::EXTRA_USER_PRIVATE_KEY_NAME, &key);

            AutoPtr< ArrayOf<Byte> > value;
            bundle->GetByteArray(ICredentials::EXTRA_USER_PRIVATE_KEY_DATA, (ArrayOf<Byte>**)&value);

            Int32 flags = IKeyStore::FLAG_ENCRYPTED;
            if (uid == IProcess::WIFI_UID && IsHardwareBackedKey(value)) {
                // Hardware backed keystore is secure enough to allow for WIFI stack
                // to enable access to secure networks without user intervention
                Logger::D(TAG, "Saving private key with FLAG_NONE for WIFI_UID");
                flags = IKeyStore::FLAG_NONE;
            }

            if (mKeyStore->ImportKey(key, value, uid, flags, &res), !res) {
                Logger::E(TAG, "Failed to install %s as user %d", key.string(), uid);
                return;
            }
        }

        Int32 flags = (uid == IProcess::WIFI_UID) ? IKeyStore::FLAG_NONE : IKeyStore::FLAG_ENCRYPTED;

        if (bundle->ContainsKey(ICredentials::EXTRA_USER_CERTIFICATE_NAME, &res), res) {
            String certName;
            bundle->GetString(ICredentials::EXTRA_USER_CERTIFICATE_NAME, &certName);
            AutoPtr< ArrayOf<Byte> > certData;
            bundle->GetByteArray(ICredentials::EXTRA_USER_CERTIFICATE_DATA, (ArrayOf<Byte>**)&certData);

            if (mKeyStore->Put(certName, certData, uid, flags, &res), !res) {
                Logger::E(TAG, "Failed to install %s as user %d", certName.string(), uid);
                return;
            }
        }

        if (bundle->ContainsKey(ICredentials::EXTRA_CA_CERTIFICATES_NAME, &res), res) {
            String caListName;
            bundle->GetString(ICredentials::EXTRA_CA_CERTIFICATES_NAME, &caListName);
            AutoPtr< ArrayOf<Byte> > caListData;
            bundle->GetByteArray(ICredentials::EXTRA_CA_CERTIFICATES_DATA, (ArrayOf<Byte>**)&caListData);

            if (mKeyStore->Put(caListName, caListData, uid, flags, &res), !res) {
                Logger::E(TAG, "Failed to install %s as user %d", caListName.string(), uid);
                return;
            }
        }

        SetResult(RESULT_OK);
    }
}

Boolean CCredentialStorage::ConfirmKeyGuard()
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<ChooseLockSettingsHelper> clsh = new ChooseLockSettingsHelper();
    clsh->constructor(this);

    AutoPtr<ICharSequence> cs1, cs2;
    res->GetText(R::string::credentials_install_gesture_prompt, (ICharSequence**)&cs1);
    res->GetText(R::string::credentials_install_gesture_explanation, (ICharSequence**)&cs2);
    Boolean launched = clsh->LaunchConfirmationActivity(
            CONFIRM_KEY_GUARD_REQUEST, cs1, cs2, TRUE);

    return launched;
}

ECode CCredentialStorage::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    Activity::OnActivityResult(requestCode, resultCode, data);

    /**
     * Receive key guard password initiated by confirmKeyGuard.
     */
    if (requestCode == CONFIRM_KEY_GUARD_REQUEST) {
        if (resultCode == IActivity::RESULT_OK) {
            String password;
            data->GetStringExtra(ChooseLockSettingsHelper::EXTRA_KEY_PASSWORD, &password);
            if (!TextUtils::IsEmpty(password)) {
                // success
                Boolean res;
                mKeyStore->Password(password, &res);
                // return to onResume
                return NOERROR;
            }
        }
        // failed confirmation, bail
        Finish();
    }
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos