#include "accounts/AccountManagerServiceSession.h"
#include "accounts/CAccountManagerService.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::CAccount;
using Elastos::Droid::Accounts::IAuthenticatorDescriptionHelper;
using Elastos::Droid::Accounts::CAuthenticatorDescriptionHelper;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

// d5ac45fe-be36-4a17-9a22-48da9f639a69
extern "C" const InterfaceID EIID_AccountManagerServiceSession =
    { 0xd5ac45fe, 0xbe36, 0x4a17, { 0x9a, 0x22, 0x48, 0xda, 0x9f, 0x63, 0x9a, 0x69 } };

const String AccountManagerServiceSession::TAG("AccountManagerServiceSession");

AccountManagerServiceSession::AccountManagerServiceSession()
    : mExpectActivityLaunch(FALSE)
    , mCreationTime(0)
    , mNumResults(0)
    , mNumRequestContinued(0)
    , mNumErrors(0)
    , mStripAuthTokenFromResult(FALSE)
{}

AutoPtr<IAccountManagerResponse> AccountManagerServiceSession::GetResponseAndClose()
{
    if (mResponse == NULL) {
        // this session has already been closed
        return NULL;
    }
    AutoPtr<IAccountManagerResponse> response = mResponse;
    Close(); // this clears mResponse so we need to save the response before this call
    return response;
}

void AccountManagerServiceSession::Close()
{
    {
        AutoLock lock(mHost->mSessionsLock);
        HashMap<String, AutoPtr<AccountManagerServiceSession> >::Iterator it = mHost->mSessions->Find(ToString());
        if (it == mHost->mSessions->End()) {
            // the session was already closed, so bail out now
            return;
        } else {
            mHost->mSessions->Erase(it);
        }
    }

    if (mResponse != NULL) {
        // stop listening for response deaths
        AutoPtr<IProxy> proxy = (IProxy*)mResponse->Probe(EIID_IProxy);
        assert(proxy != NULL);
        Boolean res;
        proxy->UnlinkToDeath(
            (IProxyDeathRecipient*)this->Probe(EIID_IProxyDeathRecipient), 0/* flags */, &res);

        // clear this so that we don't accidentally send any further results
        mResponse = NULL;
    }
    CancelTimeout();
    Unbind();
}

ECode AccountManagerServiceSession::ProxyDied()
{
    PFL_EX("ProxyDied()");
    mResponse = NULL;
    Close();
    return NOERROR;
}

String AccountManagerServiceSession::ToDebugString()
{
    return ToDebugString(SystemClock::GetElapsedRealtime());
}

String AccountManagerServiceSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    return String("Session: expectLaunch ") + StringUtils::BooleanToString(mExpectActivityLaunch)
            + String(", connected ") + StringUtils::BooleanToString(mAuthenticator != NULL)
            + String(", stats (") + StringUtils::Int32ToString(mNumResults) + String("/")
            + StringUtils::Int32ToString(mNumRequestContinued) + String("/")
            + StringUtils::Int32ToString(mNumErrors) + String(")")
            + String(", lifetime ") + StringUtils::Int64ToString((now - mCreationTime) / 1000.0);
}

ECode AccountManagerServiceSession::Bind()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "initiating bind to authenticator type " + mAccountType);
    // }
    if (!BindToAuthenticator(mAccountType)) {
        Slogger::D(TAG, "bind attempt failed for %s", (const char*)ToDebugString());
        return OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                String("bind failure"));
    }
    return NOERROR;
}

void AccountManagerServiceSession::Unbind()
{
    if (mAuthenticator != NULL) {
        mAuthenticator = NULL;
        mHost->mContext->UnbindService((IServiceConnection*)this);
    }
}

void AccountManagerServiceSession::ScheduleTimeout()
{
    AutoPtr<IMessage> msg;
    mHost->mMessageHandler->ObtainMessage(
        CAccountManagerService::MESSAGE_TIMED_OUT,
        this->Probe(EIID_IInterface), (IMessage**)&msg);
    Boolean result;
    mHost->mMessageHandler->SendMessageDelayed(msg,
        CAccountManagerService::TIMEOUT_DELAY_MS, &result);
}

void AccountManagerServiceSession::CancelTimeout()
{
    mHost->mMessageHandler->RemoveMessages(
        CAccountManagerService::MESSAGE_TIMED_OUT, (IInterface*)this);
}

ECode AccountManagerServiceSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    mAuthenticator = IIAccountAuthenticator::Probe(service);
    // try {
    ECode ec = Run();
    if (FAILED(ec)) {
        return OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                String("remote failure"));
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     onError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
    //             "remote exception");
    // }
}

ECode AccountManagerServiceSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    mAuthenticator = NULL;
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // try {
        return response->OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                String("disconnected"));
        // } catch (RemoteException e) {
        //     if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //         Log.v(TAG, "Session.onServiceDisconnected: "
        //                 + "caught RemoteException while responding", e);
        //     }
        // }
    }
    return NOERROR;
}

ECode AccountManagerServiceSession::OnTimedOut()
{
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // try {
        return response->OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION,
                String("timeout"));
        // } catch (RemoteException e) {
        //     if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //         Log.v(TAG, "Session.onTimedOut: caught RemoteException while responding",
        //                 e);
        //     }
        // }
    }
    return NOERROR;
}

ECode AccountManagerServiceSession::OnResult(
    /* [in] */ IBundle* result)
{
    mNumResults++;
    if (result != NULL) {
        String s;
        result->GetString(IAccountManager::KEY_AUTHTOKEN, &s);
        AutoPtr<ICharSequence> cs;
        CString::New(s, (ICharSequence**)&cs);
        if (!TextUtils::IsEmpty(cs)) {
            String accountName, accountType;
            result->GetString(IAccountManager::KEY_ACCOUNT_NAME, &accountName);
            result->GetString(IAccountManager::KEY_ACCOUNT_TYPE, &accountType);
            AutoPtr<ICharSequence> accountNameCs, accountTypeCs;
            CString::New(accountName, (ICharSequence**)&accountNameCs);
            CString::New(accountType, (ICharSequence**)&accountTypeCs);
            if (!TextUtils::IsEmpty(accountNameCs) && !TextUtils::IsEmpty(accountTypeCs)) {
                AutoPtr<IAccount> account;
                FAIL_RETURN(CAccount::New(accountName, accountType, (IAccount**)&account));
                AutoPtr<IUserHandle> userH;
                FAIL_RETURN(CUserHandle::New(mAccounts->mUserId, (IUserHandle**)&userH));
                mHost->CancelNotification(
                        mHost->GetSigninRequiredNotificationId(mAccounts, account), userH);
            }
        }
    }

    AutoPtr<IAccountManagerResponse> response;
    Boolean isContains = FALSE;
    if (mExpectActivityLaunch && result != NULL
            && (result->ContainsKey(IAccountManager::KEY_INTENT, &isContains), isContains)) {
        response = mResponse;
    }
    else {
        response = GetResponseAndClose();
    }
    if (response != NULL) {
        // try {
        if (result == NULL) {
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Log.v(TAG, getClass().getSimpleName()
            //             + " calling onError() on response " + response);
            // }
            return response->OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE,
                    String("null bundle returned"));
        }
        else {
            if (mStripAuthTokenFromResult) {
                result->Remove(IAccountManager::KEY_AUTHTOKEN);
            }
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Log.v(TAG, getClass().getSimpleName()
            //             + " calling onResult() on response " + response);
            // }
            return response->OnResult(result);
        }
        // } catch (RemoteException e) {
        //     // if the caller is dead then there is no one to care about remote exceptions
        //     if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //         Log.v(TAG, "failure while notifying response", e);
        //     }
        // }
    }
    return NOERROR;
}

ECode AccountManagerServiceSession::OnRequestContinued()
{
    mNumRequestContinued++;
    return NOERROR;
}

ECode AccountManagerServiceSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    mNumErrors++;
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, getClass().getSimpleName()
        //             + " calling onError() on response " + response);
        // }
        // try {
        return response->OnError(errorCode, errorMessage);
        // } catch (RemoteException e) {
        //     if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //         Log.v(TAG, "Session.onError: caught RemoteException while responding", e);
        //     }
        // }
    }
    else {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "Session.onError: already closed");
        // }
    }

    return NOERROR;
}

Boolean AccountManagerServiceSession::BindToAuthenticator(
    /* [in]  */ const String& authenticatorType)
{
    AutoPtr<IAuthenticatorDescription> value;
    AutoPtr<IAuthenticatorDescriptionHelper> helper;
    ASSERT_SUCCEEDED(CAuthenticatorDescriptionHelper::AcquireSingleton(
            (IAuthenticatorDescriptionHelper**)&helper));
    helper->NewKey(authenticatorType, (IAuthenticatorDescription**)&value);
    AutoPtr<IRegisteredServicesCacheServiceInfo> authenticatorInfo =
            mHost->mAuthenticatorCache->GetServiceInfo(value, mAccounts->mUserId);
    if (authenticatorInfo == NULL) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "there is no authenticator for " + authenticatorType
        //             + ", bailing out");
        // }
        return FALSE;
    }

    AutoPtr<IIntent> intent;
    ASSERT_SUCCEEDED(CIntent::New((IIntent**)&intent));
    intent->SetAction(IAccountManager::ACTION_AUTHENTICATOR_INTENT);
    intent->SetComponent(authenticatorInfo->mComponentName);
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "performing bindService to " + authenticatorInfo.componentName);
    // }
    Boolean result = FALSE;
    if (mHost->mContext->BindService(intent, (IServiceConnection*)this,
            IContext::BIND_AUTO_CREATE, mAccounts->mUserId, &result), !result) {
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "bindService to " + authenticatorInfo.componentName + " failed");
        // }
        return FALSE;
    }

    return TRUE;
}

String AccountManagerServiceSession::ToString()
{
    return TAG + StringUtils::Int32ToString((Handle32)this);
}

ECode AccountManagerServiceSession::Init(
    /* [in]  */ UserAccounts* accounts,
    /* [in]  */ IAccountManagerResponse* response,
    /* [in]  */ const String& accountType,
    /* [in]  */ Boolean expectActivityLaunch,
    /* [in]  */ Boolean stripAuthTokenFromResult,
    /* [in]  */ CAccountManagerService* host)
{
    // super();
    if (response == NULL) {
        Slogger::E(TAG, "response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("response is null");
    }
    if (accountType.IsNull()) {
        Slogger::E(TAG, "accountType is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("accountType is null");
    }
    mAccounts = accounts;
    mStripAuthTokenFromResult = stripAuthTokenFromResult;
    mResponse = response;
    mAccountType = accountType;
    mExpectActivityLaunch = expectActivityLaunch;
    mCreationTime = SystemClock::GetElapsedRealtime();
    mHost = host;
    {
        AutoLock lock(mHost->mSessionsLock);
        (*mHost->mSessions)[ToString()] = this;
    }
    AutoPtr<IProxy> proxy = (IProxy*)response->Probe(EIID_IProxy);
    assert(proxy != NULL);
    if (FAILED(proxy->LinkToDeath(
        (IProxyDeathRecipient*)this->Probe(EIID_IProxyDeathRecipient), 0/* flags */))) {
        mResponse = NULL;
        ProxyDied();
    }

    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
