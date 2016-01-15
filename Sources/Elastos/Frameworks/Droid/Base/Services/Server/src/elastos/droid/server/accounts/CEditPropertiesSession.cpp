
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CEditPropertiesSession.h"
#include "accounts/CAccountManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

UInt32 CEditPropertiesSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CEditPropertiesSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CEditPropertiesSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>((AccountManagerServiceSession*)this);
    }
    return _CEditPropertiesSession::Probe(riid);
}

ECode CEditPropertiesSession::Run()
{
    return mAuthenticator->EditProperties((IIAccountAuthenticatorResponse*)this,
            mAccountType);
}

String CEditPropertiesSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    return AccountManagerServiceSession::ToDebugString(now) + String(", editProperties")
            + String(", accountType") + mAccountType;
}

ECode CEditPropertiesSession::OnResult(
    /* [in] */ IBundle* result)
{
    return AccountManagerServiceSession::OnResult(result);
}

ECode CEditPropertiesSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CEditPropertiesSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CEditPropertiesSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CEditPropertiesSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CEditPropertiesSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

ECode CEditPropertiesSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String accountType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ IIAccountManager* host)
{
    return Init((UserAccounts*)accounts, response,
            accountType, expectActivityLaunch, TRUE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host);
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
