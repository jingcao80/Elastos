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

#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/CCryptKeeper.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::StatusBar::IStatusBarIcon;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::DisplayMode_Wrong;
using Elastos::Droid::Internal::Widget::EIID_IOnPatternListener;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsHelper;
using Elastos::Droid::Internal::Widget::CLockPatternUtilsHelper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CCryptKeeper::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_4(CCryptKeeper::InnerListener, Object, IOnEditorActionListener, IViewOnKeyListener, IViewOnTouchListener, ITextWatcher)

CCryptKeeper::InnerListener::InnerListener(
    /* [in] */ CCryptKeeper* host)
    : mHost(host)
{}

ECode CCryptKeeper::InnerListener::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnEditorAction(v, actionId, event, result);
}

ECode CCryptKeeper::InnerListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnKey(v, keyCode, event, result);
}

ECode CCryptKeeper::InnerListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnTouch(v, event, result);
}

ECode CCryptKeeper::InnerListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode CCryptKeeper::InnerListener::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return mHost->OnTextChanged(s, start, before, count);
}

ECode CCryptKeeper::InnerListener::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return mHost->AfterTextChanged(s);
}

//===============================================================================
//                  CCryptKeeper::MyRunnable
//===============================================================================

CCryptKeeper::MyRunnable::MyRunnable(
    /* [in] */ CCryptKeeper* host,
    /* [in] */ Int32 id,
    /* [in] */ IInputMethodManager* imm)
    : mHost(host)
    , mId(id)
    , mImm(imm)
{}

ECode CCryptKeeper::MyRunnable::Run()
{
    switch(mId) {
        case 0 :
            mHost->HandleBadAttempt(CoreUtils::Convert(1 /* failedAttempt */));
            break;
        case 1 :
            mHost->mLockPatternView->ClearPattern();
            break;
        case 2 :
            mImm->ShowSoftInputUnchecked(0, NULL);

    }
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeper::NonConfigurationInstanceState
//===============================================================================

CAR_INTERFACE_IMPL(CCryptKeeper::NonConfigurationInstanceState, Object, INonConfigurationInstanceState)

CCryptKeeper::NonConfigurationInstanceState::NonConfigurationInstanceState(
    /* [in] */ IPowerManagerWakeLock* _wakelock)
{
    mWakelock = _wakelock;
}

//===============================================================================
//                  CCryptKeeper::DecryptTask
//===============================================================================

CCryptKeeper::DecryptTask::DecryptTask(
    /* [in] */ CCryptKeeper* host)
    : mHost(host)
{}

void CCryptKeeper::DecryptTask::Hide(
    /* [in] */ Int32 id)
{
    AutoPtr<IView> view;
    mHost->FindViewById(id, (IView**)&view);
    if (view != NULL) {
        view->SetVisibility(IView::GONE);
    }
}

ECode CCryptKeeper::DecryptTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params, //String...
    /* [out] */ IInterface** result) //Integer
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IIMountService> service = mHost->GetMountService();
    Int32 data;
    ECode ec = service->DecryptStorage(Object::ToString((*params)[0]), &data);
    if (SUCCEEDED(ec)) {
        AutoPtr<IInteger32> tmp = CoreUtils::Convert(data);
        *result = tmp;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        Logger::E("CCryptKeeper::DecryptTask", "Error while decrypting... 0x%08x", ec);
        AutoPtr<IInteger32> tmp = CoreUtils::Convert(-1);
        *result = tmp;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode CCryptKeeper::DecryptTask::OnPostExecute(
    /* [in] */ IInterface* failedAttempts)//Integer
{
    Int32 failedAttemptsData;
    IInteger32::Probe(failedAttempts)->GetValue(&failedAttemptsData);
    if (failedAttemptsData == 0) {
        // The password was entered successfully. Simply do nothing
        // and wait for the service restart to switch to surfacefligner
        if (mHost->mLockPatternView != NULL) {
            Boolean res;
            IView* lpv = IView::Probe(mHost->mLockPatternView);
            lpv->RemoveCallbacks(mHost->mClearPatternRunnable, &res);
            lpv->PostDelayed(mHost->mClearPatternRunnable, RIGHT_PATTERN_CLEAR_TIMEOUT_MS, &res);
        }
        Hide(R::id::passwordEntry);
        Hide(R::id::switch_ime_button);
        Hide(R::id::lockPattern);
        Hide(R::id::status);
        Hide(R::id::owner_info);
        Hide(R::id::emergencyCallButton);
    }
    else if (failedAttemptsData == MAX_FAILED_ATTEMPTS) {
        // Factory reset the device.
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MASTER_CLEAR, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtra(IIntent::EXTRA_REASON, String("CryptKeeper.MAX_FAILED_ATTEMPTS"));
        mHost->SendBroadcast(intent);
    }
    else if (failedAttemptsData == -1) {
        // Right password, but decryption failed. Tell user bad news ...
        mHost->SetContentView(R::layout::crypt_keeper_progress);
        mHost->ShowFactoryReset(TRUE);
        return NOERROR;
    }
    else {
        mHost->HandleBadAttempt(IInteger32::Probe(failedAttempts));
    }
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeper::ValidationTask
//===============================================================================

CCryptKeeper::ValidationTask::ValidationTask(
    /* [in] */ CCryptKeeper* host)
    : mState(0)
    , mHost(host)
{}

ECode CCryptKeeper::ValidationTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params, //Void...
    /* [out] */ IInterface** result)//Boolean
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IIMountService> service = mHost->GetMountService();
    Logger::D("CCryptKeeper::ValidationTask", "Validating encryption state.");
    ECode ec = service->GetEncryptionState(&mState);
    AutoPtr<IBoolean> tmp;
    if (FAILED(ec)) {
        Logger::W("CCryptKeeper::ValidationTask", "Unable to get encryption state properly");
        tmp = CoreUtils::Convert(TRUE);
        *result = tmp;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    if (mState == IIMountService::ENCRYPTION_STATE_NONE) {
        Logger::W("CCryptKeeper::ValidationTask", "Unexpectedly in CryptKeeper even though there is no encryption.");
        tmp = CoreUtils::Convert(TRUE); // Unexpected, but fine, I guess...
        *result = tmp;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    if (mState == IIMountService::ENCRYPTION_STATE_OK) {
        tmp = CoreUtils::Convert(TRUE);
        *result = tmp;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    tmp = CoreUtils::Convert(FALSE);
    *result = tmp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CCryptKeeper::ValidationTask::OnPostExecute(
    /* [in] */ IInterface* result)//Boolean
{
    mHost->mValidationComplete = TRUE;
    Boolean res;
    IBoolean::Probe(result)->GetValue(&res);
    if (FALSE == res) {
        Logger::W("CCryptKeeper::ValidationTask", "Incomplete, or corrupted encryption detected. Prompting user to wipe.");
        mHost->mEncryptionGoneBad = TRUE;
        mHost->mCorrupt = mState == IIMountService::ENCRYPTION_STATE_ERROR_CORRUPT;
    }
    else {
        Logger::D("CCryptKeeper::ValidationTask", "Encryption state validated. Proceeding to configure UI");
    }
    mHost->SetupUi();
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeper::MyHandler
//===============================================================================

CCryptKeeper::MyHandler::MyHandler(
    /* [in] */ CCryptKeeper* host)
    : mHost(host)
{}

ECode CCryptKeeper::MyHandler::constructor()
{
    return Handler::constructor();
}

ECode CCryptKeeper::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_UPDATE_PROGRESS:
            mHost->UpdateProgress();
            break;

        case MESSAGE_COOLDOWN:
            mHost->Cooldown();
            break;

        case MESSAGE_NOTIFY:
            mHost->NotifyUser();
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeper::LockPatternViewOnPatternListener
//===============================================================================

CAR_INTERFACE_IMPL(CCryptKeeper::LockPatternViewOnPatternListener, Object, IOnPatternListener)

CCryptKeeper::LockPatternViewOnPatternListener::LockPatternViewOnPatternListener(
    /* [in] */ CCryptKeeper* host)
    : mHost(host)
{}

ECode CCryptKeeper::LockPatternViewOnPatternListener::OnPatternStart()
{
    Boolean res;
    return IView::Probe(mHost->mLockPatternView)->RemoveCallbacks(mHost->mClearPatternRunnable, &res);
}

ECode CCryptKeeper::LockPatternViewOnPatternListener::OnPatternCleared()
{
    return NOERROR;
}

ECode CCryptKeeper::LockPatternViewOnPatternListener::OnPatternDetected(
    /* [in] */ IList* pattern) //List<LockPatternView.Cell>
{
    IView::Probe(mHost->mLockPatternView)->SetEnabled(FALSE);
    Int32 size;
    pattern->GetSize(&size);
    if (size >= MIN_LENGTH_BEFORE_REPORT) {
        AutoPtr<ILockPatternUtilsHelper> helper;
        CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
        Int32 lps;
        mHost->mLockPatternView->GetLockPatternSize(&lps);
        String str;
        helper->PatternToString(pattern, lps, &str);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(str));
        AutoPtr<DecryptTask> task = new DecryptTask(mHost);
        task->Execute(args);
    }
    else {
        // Allow user to make as many of these as they want.
        mHost->FakeUnlockAttempt(IView::Probe(mHost->mLockPatternView));
    }
    return NOERROR;
}

ECode CCryptKeeper::LockPatternViewOnPatternListener::OnPatternCellAdded(
    /* [in] */ IList* pattern)//List<Cell>
{
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeper::SetupUiAsyncTask
//===============================================================================

CCryptKeeper::SetupUiAsyncTask::SetupUiAsyncTask(
    /* [in] */ CCryptKeeper* host)
    : mType(IStorageManager::CRYPT_TYPE_PASSWORD)
    , mPatternVisible(FALSE)
    , mHost(host)
{}

ECode CCryptKeeper::SetupUiAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* v, //void...
    /* [out] */ IInterface** result) //void
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // try {
    AutoPtr<IIMountService> service = mHost->GetMountService();
    service->GetPasswordType(&mType);
    service->GetField(String("OwnerInfo"), &mOwnerInfo);
    String str;
    service->GetField(String("PatternVisible"), &str);
    mPatternVisible = !(String("0").Equals(str));
    // } catch (Exception e) {
    //     Logger::E(TAG, "Error calling mount service " + e);
    // }

    return NOERROR;
}

ECode CCryptKeeper::SetupUiAsyncTask::OnPostExecute(
    /* [in] */ IInterface* v)//java.lang.Void
{
    if (mType == IStorageManager::CRYPT_TYPE_PIN) {
        mHost->SetContentView(R::layout::crypt_keeper_pin_entry);
        mHost->mStatusString = R::string::enter_pin;
    }
    else if (mType == IStorageManager::CRYPT_TYPE_PATTERN) {
        mHost->SetContentView(R::layout::crypt_keeper_pattern_entry);
        mHost->SetBackFunctionality(FALSE);
        mHost->mStatusString = R::string::enter_pattern;
    }
    else {
        mHost->SetContentView(R::layout::crypt_keeper_password_entry);
        mHost->mStatusString = R::string::enter_password;
    }
    AutoPtr<IView> tmp;
    mHost->FindViewById(R::id::status, (IView**)&tmp);
    ITextView* status = ITextView::Probe(tmp);
    status->SetText(mHost->mStatusString);

    AutoPtr<IView> ownerInfoTmp;
    mHost->FindViewById(R::id::owner_info, (IView**)&ownerInfoTmp);
    AutoPtr<ITextView> ownerInfo = ITextView::Probe(ownerInfoTmp);
    ownerInfo->SetText(CoreUtils::Convert(mOwnerInfo));
    ownerInfoTmp->SetSelected(TRUE); // Required for marquee'ing to work

    mHost->PasswordEntryInit();

    if (mHost->mLockPatternView != NULL) {
        mHost->mLockPatternView->SetInStealthMode(!mPatternVisible);
    }

    if (mHost->mCooldown > 0) {
        mHost->SetBackFunctionality(FALSE);
        mHost->Cooldown(); // in case we are cooling down and coming back from emergency dialler
    }
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeper::MyOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CCryptKeeper::MyOnClickListener, Object, IViewOnClickListener)

CCryptKeeper::MyOnClickListener::MyOnClickListener(
    /* [in] */ CCryptKeeper* host,
    /* [in] */ Int32 id,
    /* [in] */ Boolean corrupt,
    /* [in] */ IInputMethodManager* imm)
    : mHost(host)
    , mId(id)
    , mCorrupt(corrupt)
    , mImm(imm)
{}

ECode CCryptKeeper::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    switch (mId) {
        case 0 : {
            // Factory reset the device.
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_MASTER_CLEAR, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);

            intent->PutExtra(IIntent::EXTRA_REASON,
                    String("CryptKeeper->ShowFactoryReset() corrupt=") + StringUtils::ToString(mCorrupt));
            mHost->SendBroadcast(intent);
            return NOERROR;
        }

        case 1 :
            mImm->ShowInputMethodPicker();
            return NOERROR;
        case 2 :
            mHost->TakeEmergencyCallAction();
            return NOERROR;
    }

    return NOERROR;
}

//===============================================================================
//                  CCryptKeeper
//===============================================================================

const String CCryptKeeper::ACTION_EMERGENCY_DIAL("com.android.phone.EmergencyDialer.DIAL");
const Int32 CCryptKeeper::MIN_LENGTH_BEFORE_REPORT = ILockPatternUtils::MIN_LOCK_PATTERN_SIZE;

const String CCryptKeeper::TAG("CCryptKeeper");
const String CCryptKeeper::DECRYPT_STATE("trigger_restart_framework");
const Int32 CCryptKeeper::MESSAGE_UPDATE_PROGRESS;
const Int32 CCryptKeeper::MESSAGE_COOLDOWN;
const Int32 CCryptKeeper::MESSAGE_NOTIFY;

const Int32 CCryptKeeper::MAX_FAILED_ATTEMPTS = 30;
const Int32 CCryptKeeper::COOL_DOWN_ATTEMPTS = 10;
const Int32 CCryptKeeper::COOL_DOWN_INTERVAL = 30; // 30 seconds

const String CCryptKeeper::EXTRA_FORCE_VIEW("com.android.settings.CryptKeeper.DEBUG_FORCE_VIEW");
const String CCryptKeeper::FORCE_VIEW_PROGRESS("progress");
const String CCryptKeeper::FORCE_VIEW_ERROR("error");
const String CCryptKeeper::FORCE_VIEW_PASSWORD("password");

const Int32 CCryptKeeper::WRONG_PATTERN_CLEAR_TIMEOUT_MS = 1500;

const Int32 CCryptKeeper::RIGHT_PATTERN_CLEAR_TIMEOUT_MS = 500;
const Int32 CCryptKeeper::FAKE_ATTEMPT_DELAY = 1000;

Int32 CCryptKeeper::sWidgetsToDisable = IStatusBarManager::DISABLE_EXPAND
        | IStatusBarManager::DISABLE_NOTIFICATION_ICONS
        | IStatusBarManager::DISABLE_NOTIFICATION_ALERTS
        | IStatusBarManager::DISABLE_SYSTEM_INFO
        | IStatusBarManager::DISABLE_HOME
        | IStatusBarManager::DISABLE_SEARCH
        | IStatusBarManager::DISABLE_RECENT;

CAR_OBJECT_IMPL(CCryptKeeper)

CCryptKeeper::CCryptKeeper()
    : mValidationComplete(FALSE)
    , mValidationRequested(FALSE)
    , mEncryptionGoneBad(FALSE)
    , mCorrupt(FALSE)
    , mIgnoreBack(FALSE)
    , mCooldown(0)
    , mNotificationCountdown(0)
    , mReleaseWakeLockCountdown(0)
    , mStatusString(R::string::enter_password)
{}

CCryptKeeper::~CCryptKeeper()
{}

ECode CCryptKeeper::constructor()
{
    mFakeUnlockAttemptRunnable = new MyRunnable(this, 0, NULL);
    mClearPatternRunnable = new MyRunnable(this, 1, NULL);

    mHandler = new MyHandler(this);
    mHandler->constructor();

    mChooseNewLockPatternListener = new LockPatternViewOnPatternListener(this);

    return Activity::constructor();
}

void CCryptKeeper::HandleBadAttempt(
    /* [in] */ IInteger32* failedAttempts)
{
    IView* lpv = IView::Probe(mLockPatternView);
    // Wrong entry. Handle pattern case.
    Boolean res;
    if (mLockPatternView != NULL) {
        mLockPatternView->SetDisplayMode(DisplayMode_Wrong);
        lpv->RemoveCallbacks(mClearPatternRunnable, &res);
        lpv->PostDelayed(mClearPatternRunnable, WRONG_PATTERN_CLEAR_TIMEOUT_MS, &res);
    }

    Int32 failedAttemptsData;
    failedAttempts->GetValue(&failedAttemptsData);
    if ((failedAttemptsData % COOL_DOWN_ATTEMPTS) == 0) {
        mCooldown = COOL_DOWN_INTERVAL;
        Cooldown();
    }
    else {
        AutoPtr<IView> tmp;
        FindViewById(R::id::status, (IView**)&tmp);
        ITextView* status = ITextView::Probe(tmp);

        Int32 remainingAttempts = MAX_FAILED_ATTEMPTS - failedAttemptsData;
        if (remainingAttempts < COOL_DOWN_ATTEMPTS) {
            AutoPtr<ICharSequence> warningTemplate;
            GetText(R::string::crypt_keeper_warn_wipe, (ICharSequence**)&warningTemplate);
            AutoPtr< ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(StringUtils::ToString(remainingAttempts)));
            AutoPtr<ICharSequence> warning = TextUtils::ExpandTemplate(warningTemplate, args);
            status->SetText(warning);
        }
        else {
            status->SetText(R::string::try_again);
        }

        if (mLockPatternView != NULL) {
            mLockPatternView->SetDisplayMode(DisplayMode_Wrong);
        }
        // Reenable the password entry
        if (mPasswordEntry != NULL) {
            IView* pe = IView::Probe(mPasswordEntry);
            pe->SetEnabled(TRUE);
            AutoPtr<IInterface> obj;
            GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
            IInputMethodManager* imm = IInputMethodManager::Probe(obj);
            imm->ShowSoftInput(pe, 0, &res);
            SetBackFunctionality(TRUE);
        }
        if (mLockPatternView != NULL) {
            lpv->SetEnabled(TRUE);
        }
    }
}

Boolean CCryptKeeper::IsDebugView()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    Boolean result;
    intent->HasExtra(EXTRA_FORCE_VIEW, &result);
    return result;
}

Boolean CCryptKeeper::IsDebugView(
    /* [in] */ const String& viewType /* non-nullable */)
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String value;
    intent->GetStringExtra(EXTRA_FORCE_VIEW, &value);
    return viewType.Equals(value);
}

void CCryptKeeper::NotifyUser()
{
    if (mNotificationCountdown > 0) {
        --mNotificationCountdown;
    }
    else if (mAudioManager != NULL) {
        // Play the standard keypress sound at full volume. This should be available on
        // every device. We cannot play a ringtone here because media services aren't
        // available yet. A DTMF-style tone is too soft to be noticed, and might not exist
        // on tablet devices. The idea is to alert the user that something is needed: this
        // does not have to be pleasing.
        ECode ec = mAudioManager->PlaySoundEffect(IAudioManager::FX_KEYPRESS_STANDARD, 100);
        if (FAILED(ec)) {
            Logger::W(TAG, "notifyUser: Exception while playing sound: 0x%08x", ec);
        }
    }
    // Notify the user again in 5 seconds.
    mHandler->RemoveMessages(MESSAGE_NOTIFY);
    Boolean res;
    mHandler->SendEmptyMessageDelayed(MESSAGE_NOTIFY, 5 * 1000, &res);

    if (mWakeLock->IsHeld(&res), res) {
        if (mReleaseWakeLockCountdown > 0) {
            --mReleaseWakeLockCountdown;
        }
        else {
            mWakeLock->ReleaseLock();
        }
    }
}

ECode CCryptKeeper::OnBackPressed()
{
    // In the rare case that something pressed back even though we were disabled.
    if (mIgnoreBack) {
        return NOERROR;
    }
    return Activity::OnBackPressed();
}

ECode CCryptKeeper::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    // If we are not encrypted or encrypting, get out quickly.
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String state;
    sp->Get(String("vold.decrypt"), &state);
    if (!IsDebugView() && (String("").Equals(state) || DECRYPT_STATE.Equals(state))) {
        // Disable the crypt keeper.
        AutoPtr<IPackageManager> pm;
        GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IComponentName> name;
        // CComponentName::New(this, ECLSID_CCryptKeeper, (IComponentName**)&name);
        CComponentName::New(this, String("Elastos.Droid.Settings.CCryptKeeper"), (IComponentName**)&name);
        pm->SetComponentEnabledSetting(name, IPackageManager::COMPONENT_ENABLED_STATE_DISABLED,
                IPackageManager::DONT_KILL_APP);
        // Typically CryptKeeper is launched as the home app.  We didn't
        // want to be running, so need to finish this activity.  We can count
        // on the activity manager re-launching the new home app upon finishing
        // this one, since this will leave the activity stack empty.
        // NOTE: This is really grungy.  I think it would be better for the
        // activity manager to explicitly launch the crypt keeper instead of
        // home in the situation where we need to decrypt the device
        Finish();
        return NOERROR;
    }

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Boolean success = FALSE;
    ECode ec = resources->GetBoolean(R::bool_::crypt_keeper_allow_rotation, &success);
    if (ec == (ECode) E_NOT_FOUND_EXCEPTION) {
    }
    else if (success) {
        SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED);
    }

    // Disable the status bar, but do NOT disable back because the user needs a way to go
    // from keyboard settings and back to the password screen.
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
    mStatusBar = IStatusBarManager::Probe(obj);
    mStatusBar->Disable(sWidgetsToDisable);

    SetAirplaneModeIfNecessary();
    obj = NULL;
    GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);
    // Check for (and recover) retained instance data
    AutoPtr<IInterface> lastInstance;
    GetLastNonConfigurationInstance((IInterface**)&lastInstance);
    if (INonConfigurationInstanceState::Probe(lastInstance) != NULL) {
        NonConfigurationInstanceState* retained = (NonConfigurationInstanceState*) INonConfigurationInstanceState::Probe(lastInstance);
        mWakeLock = retained->mWakelock;
        Logger::D(TAG, "Restoring wakelock from NonConfigurationInstanceState");
    }
    return NOERROR;
}

ECode CCryptKeeper::OnStart()
{
    Activity::OnStart();
    SetupUi();
    return NOERROR;
}

void CCryptKeeper::SetupUi()
{
    if (mEncryptionGoneBad || IsDebugView(FORCE_VIEW_ERROR)) {
        SetContentView(R::layout::crypt_keeper_progress);
        ShowFactoryReset(mCorrupt);
        return;
    }

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String progress;
    sp->Get(String("vold.encrypt_progress"), &progress);
    if (!String("").Equals(progress) || IsDebugView(FORCE_VIEW_PROGRESS)) {
        SetContentView(R::layout::crypt_keeper_progress);
        EncryptionProgressInit();
    }
    else if (mValidationComplete || IsDebugView(FORCE_VIEW_PASSWORD)) {
        AutoPtr<SetupUiAsyncTask> task = new SetupUiAsyncTask(this);
        AutoPtr< ArrayOf<IInterface*> > args;
        task->Execute(args);
    }
    else if (!mValidationRequested) {
        // We're supposed to be encrypted, but no validation has been done.
        AutoPtr<ValidationTask> task = new ValidationTask(this);
        AutoPtr< ArrayOf<IInterface*> > args;
        task->Execute(args/*(Void[]) NULL*/);
        mValidationRequested = TRUE;
    }
}

ECode CCryptKeeper::OnStop()
{
    Activity::OnStop();
    mHandler->RemoveMessages(MESSAGE_COOLDOWN);
    mHandler->RemoveMessages(MESSAGE_UPDATE_PROGRESS);
    mHandler->RemoveMessages(MESSAGE_NOTIFY);
    return NOERROR;
}

ECode CCryptKeeper::OnRetainNonConfigurationInstance(
    /* [out] */ IInterface** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;

    AutoPtr<INonConfigurationInstanceState> state = new NonConfigurationInstanceState(mWakeLock);
    Logger::D(TAG, "Handing wakelock off to NonConfigurationInstanceState");
    mWakeLock = NULL;

    *instance = state;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode CCryptKeeper::OnDestroy()
{
    Activity::OnDestroy();

    if (mWakeLock != NULL) {
        Logger::D(TAG, "Releasing and destroying wakelock");
        mWakeLock->ReleaseLock();
        mWakeLock = NULL;
    }
    return NOERROR;
}

void CCryptKeeper::EncryptionProgressInit()
{
    // Accquire a partial wakelock to prevent the device from sleeping. Note
    // we never release this wakelock as we will be restarted after the device
    // is encrypted.
    Logger::D(TAG, "Encryption progress screen initializing.");
    if (mWakeLock == NULL) {
        Logger::D(TAG, "Acquiring wakelock.");
        AutoPtr<IInterface> obj;
        GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
        IPowerManager* pm = IPowerManager::Probe(obj);
        pm->NewWakeLock(IPowerManager::FULL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);
        mWakeLock->AcquireLock();
    }

    AutoPtr<IView> tmp;
    FindViewById(R::id::progress_bar, (IView**)&tmp);
    IProgressBar::Probe(tmp)->SetIndeterminate(TRUE);
    // Ignore all back presses from now, both hard and soft keys.
    SetBackFunctionality(FALSE);
    // Start the first run of progress manually. This method sets up messages to occur at
    // repeated intervals.
    UpdateProgress();
}

void CCryptKeeper::ShowFactoryReset(
    /* [in] */ Boolean corrupt)
{
    // Hide the encryption-bot to make room for the "factory reset" button
    AutoPtr<IView> tmp;
    FindViewById(R::id::encroid, (IView**)&tmp);
    tmp->SetVisibility(IView::GONE);

    // Show the reset button, failure text, and a divider
    AutoPtr<IView> buttonTmp;
    FindViewById(R::id::factory_reset, (IView**)&buttonTmp);
    // IButton* button = IButton::Probe(buttonTmp);
    buttonTmp->SetVisibility(IView::VISIBLE);
    AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 0, corrupt, NULL);
    buttonTmp->SetOnClickListener(listener);

    // Alert the user of the failure.
    if (corrupt) {
        AutoPtr<IView> tmp;
        FindViewById(R::id::title, (IView**)&tmp);
        ITextView::Probe(tmp)->SetText(R::string::crypt_keeper_data_corrupt_title);
        tmp = NULL;
        FindViewById(R::id::status, (IView**)&tmp);
        ITextView::Probe(tmp)->SetText(R::string::crypt_keeper_data_corrupt_summary);
    }
    else {
        AutoPtr<IView> tmp;
        FindViewById(R::id::title, (IView**)&tmp);
        ITextView::Probe(tmp)->SetText(R::string::crypt_keeper_failed_title);
        tmp = NULL;
        FindViewById(R::id::status, (IView**)&tmp);
        ITextView::Probe(tmp)->SetText(R::string::crypt_keeper_failed_summary);
    }

    AutoPtr<IView> view;
    FindViewById(R::id::bottom_divider, (IView**)&view);
    // TODO(viki): Why would the bottom divider be missing in certain layouts? Investigate.
    if (view != NULL) {
        view->SetVisibility(IView::VISIBLE);
    }
}

void CCryptKeeper::UpdateProgress()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String state;
    sp->Get(String("vold.encrypt_progress"), &state);

    if (String("error_partially_encrypted").Equals(state)) {
        ShowFactoryReset(FALSE);
        return;
    }

    // Get status as percentage first
    AutoPtr<ICharSequence> status;
    GetText(R::string::crypt_keeper_setup_description, (ICharSequence**)&status);
    Int32 percent = 0;
    // try {
    // Force a 50% progress state when debugging the view.
    percent = IsDebugView() ? 50 : StringUtils::ParseInt32(state);
    // } catch (Exception e) {
    //     Logger::W(TAG, "Error parsing progress: " + e->ToString());
    // }
    String progress = StringUtils::ToString(percent);

    // Now try to get status as time remaining and replace as appropriate
    Logger::V(TAG, "Encryption progress: %s", progress.string());
    // try {
    String timeProperty;
    sp->Get(String("vold.encrypt_time_remaining"), &timeProperty);
    Int32 time = StringUtils::ParseInt32(timeProperty);
    if (time >= 0) {
        // Round up to multiple of 10 - this way display is less jerky
        time = (time + 9) / 10 * 10;
        progress = DateUtils::FormatElapsedTime(time);
        status = NULL;
        GetText(R::string::crypt_keeper_setup_time_remaining, (ICharSequence**)&status);
    }
    // } catch (Exception e) {
    //     // Will happen if no time etc - show percentage
    // }

    AutoPtr<IView> tmp;
    FindViewById(R::id::status, (IView**)&tmp);
    ITextView* tv = ITextView::Probe(tmp);
    if (tv != NULL) {
        AutoPtr< ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(progress));
        tv->SetText(TextUtils::ExpandTemplate(status, args));
    }

    // Check the progress every 1 seconds
    mHandler->RemoveMessages(MESSAGE_UPDATE_PROGRESS);
    Boolean res;
    mHandler->SendEmptyMessageDelayed(MESSAGE_UPDATE_PROGRESS, 1000, &res);
}

void CCryptKeeper::Cooldown()
{
    AutoPtr<IView> tmp;
    FindViewById(R::id::status, (IView**)&tmp);
    ITextView* status = ITextView::Probe(tmp);

    Boolean res;
    if (mCooldown <= 0) {
        // Re-enable the password entry and back presses.
        if (mPasswordEntry != NULL) {
            IView* pe = IView::Probe(mPasswordEntry);
            pe->SetEnabled(TRUE);
            AutoPtr<IInterface> obj;
            GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
            IInputMethodManager* imm = IInputMethodManager::Probe(obj);
            imm->ShowSoftInput(pe, 0, &res);
            SetBackFunctionality(TRUE);
        }
        if (mLockPatternView != NULL) {
            IView::Probe(mLockPatternView)->SetEnabled(TRUE);
        }
        status->SetText(mStatusString);
    }
    else {
        // Disable the password entry and back presses.
        if (mPasswordEntry != NULL) {
            IView::Probe(mPasswordEntry)->SetEnabled(FALSE);
        }
        if (mLockPatternView != NULL) {
            IView::Probe(mLockPatternView)->SetEnabled(FALSE);
        }

        AutoPtr<ICharSequence> templateCs;
        GetText(R::string::crypt_keeper_cooldown, (ICharSequence**)&templateCs);
        AutoPtr< ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(StringUtils::ToString(mCooldown)));
        status->SetText(TextUtils::ExpandTemplate(templateCs, args));

        mCooldown--;
        mHandler->RemoveMessages(MESSAGE_COOLDOWN);
        mHandler->SendEmptyMessageDelayed(MESSAGE_COOLDOWN, 1000, &res); // Tick every second
    }
}

void CCryptKeeper::SetBackFunctionality(
    /* [in] */ Boolean isEnabled)
{
    mIgnoreBack = !isEnabled;
    if (isEnabled) {
        mStatusBar->Disable(sWidgetsToDisable);
    }
    else {
        mStatusBar->Disable(sWidgetsToDisable | IStatusBarManager::DISABLE_BACK);
    }
}

void CCryptKeeper::FakeUnlockAttempt(
    /* [in] */ IView* postingView)
{
    Boolean res;
    postingView->PostDelayed(mFakeUnlockAttemptRunnable, FAKE_ATTEMPT_DELAY, &res);
}

void CCryptKeeper::PasswordEntryInit()
{
    // Password/pin case
    AutoPtr<IView> tmp;
    FindViewById(R::id::passwordEntry, (IView**)&tmp);
    mPasswordEntry = IEditText::Probe(tmp);
    Boolean res;
    if (mPasswordEntry != NULL){
        AutoPtr<InnerListener> listener = new InnerListener(this);
        ITextView* tvpe = ITextView::Probe(mPasswordEntry);
        tvpe->SetOnEditorActionListener(listener);
        IView* pe = IView::Probe(mPasswordEntry);
        pe->RequestFocus(&res);
        // Become quiet when the user interacts with the Edit text screen.
        pe->SetOnKeyListener(listener);
        pe->SetOnTouchListener(listener);
        tvpe->AddTextChangedListener(listener);
    }

    // Pattern case
    tmp = NULL;
    FindViewById(R::id::lockPattern, (IView**)&tmp);
    mLockPatternView = ILockPatternView::Probe(tmp);
    if (mLockPatternView != NULL) {
        mLockPatternView->SetOnPatternListener(mChooseNewLockPatternListener);
    }

    // Disable the Emergency call button if the device has no voice telephone capability
    if (GetTelephonyManager()->IsVoiceCapable(&res), !res) {
        AutoPtr<IView> emergencyCall;
        FindViewById(R::id::emergencyCallButton, (IView**)&emergencyCall);
        if (emergencyCall != NULL) {
            Logger::D(TAG, "Removing the emergency Call button");
            emergencyCall->SetVisibility(IView::GONE);
        }
    }

    AutoPtr<IView> imeSwitcher;
    FindViewById(R::id::switch_ime_button, (IView**)&imeSwitcher);
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    IInputMethodManager* imm = IInputMethodManager::Probe(obj);
    if (imeSwitcher != NULL && HasMultipleEnabledIMEsOrSubtypes(imm, FALSE)) {
        imeSwitcher->SetVisibility(IView::VISIBLE);
        AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 1, FALSE, imm);
        imeSwitcher->SetOnClickListener(listener);
    }

    // We want to keep the screen on while waiting for input. In minimal boot mode, the device
    // is completely non-functional, and we want the user to notice the device and enter a
    // password.
    if (mWakeLock == NULL) {
        Logger::D(TAG, "Acquiring wakelock.");
        AutoPtr<IInterface> obj;
        GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
        IPowerManager* pm = IPowerManager::Probe(obj);
        if (pm != NULL) {
            mWakeLock = NULL;
            pm->NewWakeLock(IPowerManager::FULL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);
            mWakeLock->AcquireLock();
            // Keep awake for 10 minutes - if the user hasn't been alerted by then
            // best not to just drain their battery
            mReleaseWakeLockCountdown = 96; // 96 * 5 secs per click + 120 secs before we show this = 600
        }
    }

    // Asynchronously throw up the IME, since there are issues with requesting it to be shown
    // immediately.
    if (mLockPatternView == NULL && mCooldown <= 0) {
        AutoPtr<MyRunnable> runnable = new MyRunnable(this, 2, imm);
        mHandler->PostDelayed(runnable, 0, &res);
    }

    UpdateEmergencyCallButtonState();
    // Notify the user in 120 seconds that we are waiting for him to enter the password.
    mHandler->RemoveMessages(MESSAGE_NOTIFY);
    mHandler->SendEmptyMessageDelayed(MESSAGE_NOTIFY, 120 * 1000, &res);

    // Dismiss secure & non-secure keyguards while this screen is showing.
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD
            | IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED);
}

Boolean CCryptKeeper::HasMultipleEnabledIMEsOrSubtypes(
    /* [in] */ IInputMethodManager* imm,
    /* [in] */ Boolean shouldIncludeAuxiliarySubtypes)
{
    AutoPtr<IList> enabledImis;//List<InputMethodInfo>
    imm->GetEnabledInputMethodList((IList**)&enabledImis);

    // Number of the filtered IMEs
    Int32 filteredImisCount = 0;

    Int32 size;
    enabledImis->GetSize(&size);
    Boolean res;
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        enabledImis->Get(i, (IInterface**)&obj);
        IInputMethodInfo* imi = IInputMethodInfo::Probe(obj);

        // We can return TRUE immediately after we find two or more filtered IMEs.
        if (filteredImisCount > 1) return TRUE;
        AutoPtr<IList> subtypes;//List<InputMethodSubtype>
        imm->GetEnabledInputMethodSubtypeList(imi, TRUE, (IList**)&subtypes);
        // IMEs that have no subtypes should be counted.
        if (subtypes->IsEmpty(&res), res) {
            ++filteredImisCount;
            continue;
        }

        Int32 auxCount = 0;
        Int32 size2;
        subtypes->GetSize(&size2);
        for (Int32 j = 0; j < size2; ++j) {
            AutoPtr<IInterface> tmp;
            subtypes->Get(j, (IInterface**)&tmp);
            IInputMethodSubtype* subtype = IInputMethodSubtype::Probe(tmp);

            if (subtype->IsAuxiliary(&res), res) {
                ++auxCount;
            }
        }
        const Int32 nonAuxCount = size2 - auxCount;

        // IMEs that have one or more non-auxiliary subtypes should be counted.
        // If shouldIncludeAuxiliarySubtypes is TRUE, IMEs that have two or more auxiliary
        // subtypes should be counted as well.
        if (nonAuxCount > 0 || (shouldIncludeAuxiliarySubtypes && auxCount > 1)) {
            ++filteredImisCount;
            continue;
        }
    }

    AutoPtr<IList> list;
    Int32 count;

    return filteredImisCount > 1
    // imm->GetEnabledInputMethodSubtypeList(NULL, FALSE) will return the current IME's enabled
    // input method subtype (The current IME should be LatinIME.)
            || ((imm->GetEnabledInputMethodSubtypeList(NULL, FALSE, (IList**)&list), list)->GetSize(&count), count) > 1;
}

AutoPtr<IIMountService> CCryptKeeper::GetMountService()
{
    AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
    if (service != NULL) {
        return IIMountService::Probe(service);
    }
    return NULL;
}

ECode CCryptKeeper::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (actionId == IEditorInfo::IME_NULL || actionId == IEditorInfo::IME_ACTION_DONE) {
        // Get the password
        AutoPtr<ICharSequence> cs;
        v->GetText((ICharSequence**)&cs);
        String password;
        cs->ToString(&password);

        if (TextUtils::IsEmpty(password)) {
            *result = TRUE;
            return NOERROR;
        }

        // Now that we have the password clear the password field.
        v->SetText((ICharSequence*)NULL);

        // Disable the password entry and back keypress while checking the password. These
        // we either be re-enabled if the password was wrong or after the cooldown period.
        IView::Probe(mPasswordEntry)->SetEnabled(FALSE);
        SetBackFunctionality(FALSE);

        if (password.GetLength() >= ILockPatternUtils::MIN_LOCK_PATTERN_SIZE) {
            Logger::D(TAG, "Attempting to send command to decrypt");
            AutoPtr<DecryptTask> task = new DecryptTask(this);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(password));
            task->Execute(args);
        }
        else {
            // Allow user to make as many of these as they want.
            FakeUnlockAttempt(IView::Probe(mPasswordEntry));
        }

        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

void CCryptKeeper::SetAirplaneModeIfNecessary()
{
    Int32 mode;
    GetTelephonyManager()->GetLteOnCdmaMode(&mode);
    const Boolean isLteDevice = mode == IPhoneConstants::LTE_ON_CDMA_TRUE;
    if (!isLteDevice) {
        Logger::D(TAG, "Going into airplane mode.");
        AutoPtr<IContentResolver> resolver;
        GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
        Boolean res;
        global->PutInt32(resolver, ISettingsGlobal::AIRPLANE_MODE_ON, 1, &res);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
        intent->PutBooleanExtra(String("state"), TRUE);
        SendBroadcastAsUser(intent, UserHandle::ALL);
    }
}

void CCryptKeeper::UpdateEmergencyCallButtonState()
{
    AutoPtr<IView> emergencyCallTmp;
    FindViewById(R::id::emergencyCallButton, (IView**)&emergencyCallTmp);
    IButton* emergencyCall = IButton::Probe(emergencyCallTmp);
    // The button isn't present at all in some configurations.
    if (emergencyCall == NULL)
        return;

    if (IsEmergencyCallCapable()) {
        emergencyCallTmp->SetVisibility(IView::VISIBLE);
        AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 2, FALSE, NULL);
        emergencyCallTmp->SetOnClickListener(listener);
    }
    else {
        emergencyCallTmp->SetVisibility(IView::GONE);
        return;
    }

    Int32 textId;
    Boolean res;
    if (GetTelecomManager()->IsInCall(&res), res) {
        // Show "return to call"
        textId = R::string::cryptkeeper_return_to_call;
    }
    else {
        textId = R::string::cryptkeeper_emergency_call;
    }
    ITextView::Probe(emergencyCall)->SetText(textId);
}

Boolean CCryptKeeper::IsEmergencyCallCapable()
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(Elastos::Droid::R::bool_::config_voice_capable, &res);
    return res;
}

void CCryptKeeper::TakeEmergencyCallAction()
{
    AutoPtr<ITelecomManager> telecomManager = GetTelecomManager();
    Boolean res;
    if (telecomManager->IsInCall(&res), res) {
        telecomManager->ShowInCallScreen(FALSE /* showDialpad */);
    }
    else {
        LaunchEmergencyDialer();
    }
}

void CCryptKeeper::LaunchEmergencyDialer()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_EMERGENCY_DIAL, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                    | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    SetBackFunctionality(TRUE);
    StartActivity(intent);
}

AutoPtr<ITelephonyManager> CCryptKeeper::GetTelephonyManager()
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    return ITelephonyManager::Probe(obj);
}

AutoPtr<ITelecomManager> CCryptKeeper::GetTelecomManager()
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&obj);
    return ITelecomManager::Probe(obj);
}

void CCryptKeeper::DelayAudioNotification()
{
    mNotificationCountdown = 20;
}

ECode CCryptKeeper::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    DelayAudioNotification();
    *result = FALSE;
    return NOERROR;
}

ECode CCryptKeeper::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    DelayAudioNotification();
    *result = FALSE;
    return NOERROR;
}

ECode CCryptKeeper::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode CCryptKeeper::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    DelayAudioNotification();
    return NOERROR;
}

ECode CCryptKeeper::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos