
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CConfirmCredentialsAsUserSession.h"
#include "accounts/CAccountManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

UInt32 CConfirmCredentialsAsUserSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CConfirmCredentialsAsUserSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CConfirmCredentialsAsUserSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>((AccountManagerServiceSession*)this);
    }
    return _CConfirmCredentialsAsUserSession::Probe(riid);
}

ECode CConfirmCredentialsAsUserSession::Run()
{
    return mAuthenticator->ConfirmCredentials((IIAccountAuthenticatorResponse*)this,
            mAccount, mOptions);
}

String CConfirmCredentialsAsUserSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    String accountStr;
    mAccount->ToString(&accountStr);
    return AccountManagerServiceSession::ToDebugString(now) + String(", confirmCredentials")
            + String(", ") + accountStr;
}

ECode CConfirmCredentialsAsUserSession::OnResult(
    /* [in] */ IBundle* result)
{
    return AccountManagerServiceSession::OnResult(result);
}

ECode CConfirmCredentialsAsUserSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CConfirmCredentialsAsUserSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CConfirmCredentialsAsUserSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CConfirmCredentialsAsUserSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CConfirmCredentialsAsUserSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

ECode CConfirmCredentialsAsUserSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IAccount* account,
    /* [in] */ IBundle* options,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Init((UserAccounts*)accounts, response,
            type, expectActivityLaunch, TRUE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host));
    mAccount = account;
    mOptions = options;
    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
