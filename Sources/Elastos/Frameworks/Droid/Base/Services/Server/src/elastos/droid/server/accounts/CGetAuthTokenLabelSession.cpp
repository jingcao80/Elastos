
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CGetAuthTokenLabelSession.h"
#include "accounts/CAccountManagerService.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Accounts::IAccountManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

UInt32 CGetAuthTokenLabelSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CGetAuthTokenLabelSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CGetAuthTokenLabelSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CGetAuthTokenLabelSession::Probe(riid);
}

String CGetAuthTokenLabelSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    return AccountManagerServiceSession::ToDebugString(now)
            + String(", getAuthTokenLabel") + String(", ")
            + mAccountType + String(", authTokenType ") + mAuthTokenType;;
}

ECode CGetAuthTokenLabelSession::Run()
{
    return mAuthenticator->GetAuthTokenLabel(this,
            mAuthTokenType);
}

ECode CGetAuthTokenLabelSession::OnResult(
    /* [in] */ IBundle* result)
{
    if (result != NULL) {
        String label;
        result->GetString(IAccountManager::KEY_AUTH_TOKEN_LABEL, &label);
        AutoPtr<IBundle> bundle;
        FAIL_RETURN(CBundle::New((IBundle**)&bundle));
        bundle->PutString(IAccountManager::KEY_AUTH_TOKEN_LABEL, label);
        return AccountManagerServiceSession::OnResult(bundle);
    }
    else {
        return AccountManagerServiceSession::OnResult(result);
    }
}

ECode CGetAuthTokenLabelSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CGetAuthTokenLabelSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CGetAuthTokenLabelSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CGetAuthTokenLabelSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CGetAuthTokenLabelSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

ECode CGetAuthTokenLabelSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& type,
    /* [in] */ const String& authTokenType,
    /* [in] */ IIAccountManager* host)
{
    FAIL_RETURN(Init((UserAccounts*)accounts, response,
            type, FALSE, FALSE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host));
    mAuthTokenType = authTokenType;
    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
