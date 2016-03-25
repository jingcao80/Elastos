
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CRemoveAccountSession.h"
#include "accounts/CAccountManagerService.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Accounts::IAccountManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

UInt32 CRemoveAccountSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CRemoveAccountSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CRemoveAccountSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CRemoveAccountSession::Probe(riid);
}

String CRemoveAccountSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    String accountStr;
    mAccount->ToString(&accountStr);
    return AccountManagerServiceSession::ToDebugString(now) + String(", removeAccount")
            + String(", account ") + accountStr;
}

ECode CRemoveAccountSession::Run()
{
    return mAuthenticator->GetAccountRemovalAllowed(
            this, mAccount);
}

ECode CRemoveAccountSession::OnResult(
    /* [in] */ IBundle* result)
{
    Boolean res1, res2;
    if (result != NULL &&
            (result->ContainsKey(IAccountManager::KEY_BOOLEAN_RESULT, &res1), res1) &&
            (result->ContainsKey(IAccountManager::KEY_INTENT, &res2), !res2)) {
        Boolean removalAllowed;
        result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, &removalAllowed);
        if (removalAllowed) {
            mHost->RemoveAccountInternal(mAccounts, mAccount);
        }
        AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
        if (response != NULL) {
            // if (Log.isLoggable(TAG, Log.VERBOSE)) {
            //     Log.v(TAG, getClass().getSimpleName() + " calling onResult() on response "
            //             + response);
            // }
            AutoPtr<IBundle> result2;
            CBundle::New((IBundle**)&result2);
            result2->PutBoolean(IAccountManager::KEY_BOOLEAN_RESULT, removalAllowed);
            // try {
            response->OnResult(result2);
            // } catch (RemoteException e) {
            //     // ignore
            // }
        }
    }
    return AccountManagerServiceSession::OnResult(result);
}

ECode CRemoveAccountSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CRemoveAccountSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CRemoveAccountSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CRemoveAccountSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CRemoveAccountSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

ECode CRemoveAccountSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Init((UserAccounts*)accounts, response,
            type, FALSE /* expectActivityLaunch */,
            TRUE /* stripAuthTokenFromResult */, (CAccountManagerService*)host));
    mAccount = account;
    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
