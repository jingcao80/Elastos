
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CGetAuthTokenSession.h"
#include "accounts/CAccountManagerService.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::CAccount;
using Elastos::Droid::Accounts::CAccountAuthenticatorResponse;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CGetAuthTokenSession::CGetAuthTokenSession()
    : mNotifyOnAuthFailure(FALSE)
    , mPermissionGranted(FALSE)
{}

UInt32 CGetAuthTokenSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CGetAuthTokenSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CGetAuthTokenSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CGetAuthTokenSession::Probe(riid);
}

String CGetAuthTokenSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    if (mLoginOptions != NULL) {
        AutoPtr<IObjectContainer> value;
        mLoginOptions->KeySet((IObjectContainer**)&value);
    }
    String accountStr, optionStr;
    mAccount->ToString(&accountStr);
    mLoginOptions->ToString(&optionStr);
    return AccountManagerServiceSession::ToDebugString(now) + String(", getAuthToken")
            + String(", ") + accountStr
            + String(", authTokenType ") + mAuthTokenType
            + String(", loginOptions ") + optionStr
            + String(", notifyOnAuthFailure ") + StringUtils::BooleanToString(mNotifyOnAuthFailure);
}

ECode CGetAuthTokenSession::Run()
{
    // If the caller doesn't have permission then create and return the
    // "grant permission" intent instead of the "getAuthToken" intent.
    if (!mPermissionGranted) {
        return mAuthenticator->GetAuthTokenLabel(
                this, mAuthTokenType);
    }
    else {
        return mAuthenticator->GetAuthToken(
                this, mAccount,
                mAuthTokenType, mLoginOptions);
    }
}

ECode CGetAuthTokenSession::OnResult(
    /* [in] */ IBundle* result)
{
    if (result != NULL) {
        Boolean isContains;
        if (result->ContainsKey(IAccountManager::KEY_AUTH_TOKEN_LABEL, &isContains),
                isContains) {
            AutoPtr<IAccountAuthenticatorResponse> response;
            CAccountAuthenticatorResponse::New(this,
                    (IAccountAuthenticatorResponse**)&response);
            String value;
            result->GetString(IAccountManager::KEY_AUTH_TOKEN_LABEL, &value);
            AutoPtr<IIntent> intent = mHost->NewGrantCredentialsPermissionIntent(mAccount,
                    Binder::GetCallingUid(), response, mAuthTokenType, value);
            AutoPtr<IBundle> bundle;
            CBundle::New((IBundle**)&bundle);
            bundle->PutParcelable(IAccountManager::KEY_INTENT,
                    (IParcelable*)intent->Probe(EIID_IParcelable));
            return OnResult(bundle);
        }
        String authToken;
        result->GetString(IAccountManager::KEY_AUTHTOKEN, &authToken);
        if (!authToken.IsNull()) {
            String name, type;
            result->GetString(IAccountManager::KEY_ACCOUNT_NAME, &name);
            result->GetString(IAccountManager::KEY_ACCOUNT_TYPE, &type);
            AutoPtr<ICharSequence> nameCsq, typeCsq;
            CString::New(name, (ICharSequence**)&nameCsq);
            CString::New(type, (ICharSequence**)&typeCsq);
            if (TextUtils::IsEmpty(typeCsq) || TextUtils::IsEmpty(nameCsq)) {
                return OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE,
                        String("the type and name should not be empty"));
            }
            if (!mCustomTokens) {
                AutoPtr<IAccount> account;
                CAccount::New(name, type, (IAccount**)&account);
                mHost->SaveAuthTokenToDatabase(mAccounts, mAccount, mAuthTokenType, authToken);
            }
        }

        AutoPtr<IParcelable> p;
        result->GetParcelable(IAccountManager::KEY_INTENT, (IParcelable**)&p);
        AutoPtr<IIntent> intent = (IIntent*)p->Probe(Elastos::Droid::Content::EIID_IIntent);
        if (intent != NULL && mNotifyOnAuthFailure && !mCustomTokens) {
            String s;
            result->GetString(IAccountManager::KEY_AUTH_FAILED_MESSAGE, &s);
            AutoPtr<ICharSequence> csq;
            CString::New(s, (ICharSequence**)&csq);
            mHost->DoNotification(mAccounts, mAccount, csq, intent, mAccounts->mUserId);
        }
    }
    return AccountManagerServiceSession::OnResult(result);
}

ECode CGetAuthTokenSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CGetAuthTokenSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CGetAuthTokenSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CGetAuthTokenSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CGetAuthTokenSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

ECode CGetAuthTokenSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IBundle* loginOptions,
    /* [in] */ Boolean notifyOnAuthFailure,
    /* [in] */ Boolean permissionGranted,
    /* [in] */ Boolean customTokens,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Init((UserAccounts*)accounts, response,
            type, expectActivityLaunch, FALSE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host));
    mAccount = account;
    mAuthTokenType = authTokenType;
    mLoginOptions = loginOptions;
    mNotifyOnAuthFailure = notifyOnAuthFailure;
    mPermissionGranted = permissionGranted;
    mCustomTokens = customTokens;
    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
