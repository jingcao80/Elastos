
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CTestFeaturesSession.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Core::CString;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

UInt32 CTestFeaturesSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CTestFeaturesSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CTestFeaturesSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>((AccountManagerServiceSession*)this);
    }
    return _CTestFeaturesSession::Probe(riid);
}

ECode CTestFeaturesSession::Run()
{
    // try {
    ECode ec = mAuthenticator->HasFeatures((IIAccountAuthenticatorResponse*)this,
            mAccount, *mFeatures);
    if (FAILED(ec)) {
        OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION, String("remote exception"));
        return ec;
    }
    // } catch (RemoteException e) {
    //     onError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION, "remote exception");
    // }
    return NOERROR;
}

ECode CTestFeaturesSession::OnResult(
    /* [in] */ IBundle* result)
{
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // try {
        if (result == NULL) {
            return response->OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE,
                    String("null bundle"));
        }
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, getClass().getSimpleName() + " calling onResult() on response "
        //             + response);
        // }
        AutoPtr<IBundle> newResult;
        CBundle::New((IBundle**)&newResult);
        Boolean value;
        result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, FALSE, &value);
        newResult->PutBoolean(IAccountManager::KEY_BOOLEAN_RESULT, value);
        return response->OnResult(newResult);
        // } catch (RemoteException e) {
        //     // if the caller is dead then there is no one to care about remote exceptions
        //     if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //         Log.v(TAG, "failure while notifying response", e);
        //     }
        // }
    }
    return NOERROR;
}

ECode CTestFeaturesSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CTestFeaturesSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CTestFeaturesSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CTestFeaturesSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CTestFeaturesSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

String CTestFeaturesSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    String accountStr;
    mAccount->ToString(&accountStr);
    AutoPtr<ICharSequence> cs;
    CString::New(String(","), (ICharSequence**)&cs);
    AutoPtr<IObjectContainer> features;
    for (Int32 i = 0; i < mFeatures->GetLength(); i ++) {
        String s = (*mFeatures)[i];
        AutoPtr<ICharSequence> cs;
        CString::New(s, (ICharSequence**)&cs);
        features->Add(cs);
    }
    return AccountManagerServiceSession::ToDebugString(now) + String(", hasFeatures")
            + String(", ") + accountStr
            + String(", ") + (mFeatures != NULL ? TextUtils::Join(cs, features) : String(NULL));
}

ECode CTestFeaturesSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ IAccount* account,
    /* [in] */ const ArrayOf<String>& features,
    /* [in] */ IIAccountManager* host)
{
    String type;
    account->GetType(&type);
    FAIL_RETURN(Init((UserAccounts*)accounts, response,
            type, FALSE /* expectActivityLaunch */,
            TRUE /* stripAuthTokenFromResult */, (CAccountManagerService*)host));
    mFeatures = features.Clone();
    mAccount = account;
    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
