
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CUpdateCredentialsSession.h"
#include "accounts/CAccountManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

UInt32 CUpdateCredentialsSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CUpdateCredentialsSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CUpdateCredentialsSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>((AccountManagerServiceSession*)this);
    }
    return _CUpdateCredentialsSession::Probe(riid);
}

ECode CUpdateCredentialsSession::Run()
{
    return mAuthenticator->UpdateCredentials((IIAccountAuthenticatorResponse*)this,
            mAccount, mAuthTokenType, mLoginOptions);
}

String CUpdateCredentialsSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    if (mLoginOptions != NULL) {
        AutoPtr<IObjectContainer> value;
        mLoginOptions->KeySet((IObjectContainer**)&value);
    }
    String accountStr, optionStr;
    mAccount->ToString(&accountStr);
    mLoginOptions->ToString(&optionStr);
    return AccountManagerServiceSession::ToDebugString(now) + String(", updateCredentials")
            + String(", ") + accountStr
            + String(", authTokenType ") + mAuthTokenType
            + String(", loginOptions ") + optionStr;
}

ECode CUpdateCredentialsSession::OnResult(
    /* [in] */ IBundle* result)
{
    return AccountManagerServiceSession::OnResult(result);
}

ECode CUpdateCredentialsSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CUpdateCredentialsSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CUpdateCredentialsSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CUpdateCredentialsSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CUpdateCredentialsSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

ECode CUpdateCredentialsSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IAccount* account,
    /* [in] */ const String& authTokenType,
    /* [in] */ IBundle* loginOptions,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Init((UserAccounts*)accounts, response,
            type, expectActivityLaunch, TRUE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host));
    mAccount = account;
    mAuthTokenType = authTokenType;
    mLoginOptions = loginOptions;
    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
