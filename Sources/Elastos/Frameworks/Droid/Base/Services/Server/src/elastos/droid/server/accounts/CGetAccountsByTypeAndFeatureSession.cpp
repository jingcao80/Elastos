
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CGetAccountsByTypeAndFeatureSession.h"
#include "accounts/CAccountManagerService.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Core::CString;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CGetAccountsByTypeAndFeatureSession::CGetAccountsByTypeAndFeatureSession()
    : mCurrentAccount(0)
{}

UInt32 CGetAccountsByTypeAndFeatureSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CGetAccountsByTypeAndFeatureSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CGetAccountsByTypeAndFeatureSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>((AccountManagerServiceSession*)this);
    }
    return _CGetAccountsByTypeAndFeatureSession::Probe(riid);
}

ECode CGetAccountsByTypeAndFeatureSession::Run()
{
    {
        AutoLock lock(mAccounts->mCacheLock);
        mAccountsOfType = mHost->GetAccountsFromCacheLocked(mAccounts, mAccountType);
    }
    // check whether each account matches the requested features
    mAccountsWithFeatures.Resize(mAccountsOfType->GetLength());
    mCurrentAccount = 0;

    return CheckAccount();
}

ECode CGetAccountsByTypeAndFeatureSession::CheckAccount()
{
    if (mCurrentAccount >= mAccountsOfType->GetLength()) {
        return SendResult();
    }

    AutoPtr<IIAccountAuthenticator> accountAuthenticator = mAuthenticator;
    if (accountAuthenticator == NULL) {
        // It is possible that the authenticator has died, which is indicated by
        // mAuthenticator being set to null. If this happens then just abort.
        // There is no need to send back a result or error in this case since
        // that already happened when mAuthenticator was cleared.
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "checkAccount: aborting session since we are no longer"
        //             + " connected to the authenticator, " + toDebugString());
        // }
        return NOERROR;
    }
    // try {
    ECode ec = accountAuthenticator->HasFeatures((IIAccountAuthenticatorResponse*)this,
            (*mAccountsOfType)[mCurrentAccount], *mFeatures);
    if (FAILED(ec)) {
        OnError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION, String("remote exception"));
        return ec;
    }
    // } catch (RemoteException e) {
    //     onError(IAccountManager::ERROR_CODE_REMOTE_EXCEPTION, "remote exception");
    // }
    return NOERROR;
}

ECode CGetAccountsByTypeAndFeatureSession::OnResult(
    /* [in] */ IBundle* result)
{
    mNumResults++;
    if (result == NULL) {
        return OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE, String("null bundle"));
    }
    Boolean value;
    if (result->GetBoolean(IAccountManager::KEY_BOOLEAN_RESULT, FALSE, &value)) {
        mAccountsWithFeatures.PushBack((*mAccountsOfType)[mCurrentAccount]);
    }
    mCurrentAccount++;
    return CheckAccount();
}

ECode CGetAccountsByTypeAndFeatureSession::SendResult()
{
    AutoPtr<IAccountManagerResponse> response = GetResponseAndClose();
    if (response != NULL) {
        // try {
        AutoPtr< ArrayOf<IParcelable*> > accounts = ArrayOf<IParcelable*>::Alloc(
                mAccountsWithFeatures.GetSize());
        List<AutoPtr<IAccount> >::Iterator it = mAccountsWithFeatures.Begin();
        for (Int32 i = 0; i < accounts->GetLength(); ++i, ++it) {
            accounts->Set(i, (IParcelable*)(*it)->Probe(EIID_IParcelable));
        }
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, getClass().getSimpleName() + " calling onResult() on response "
        //             + response);
        // }
        AutoPtr<IBundle> result;
        FAIL_RETURN(CBundle::New((IBundle**)&result));
        FAIL_RETURN(result->PutParcelableArray(IAccountManager::KEY_ACCOUNTS, accounts));
        return response->OnResult(result);
        // } catch (RemoteException e) {
        //     // if the caller is dead then there is no one to care about remote exceptions
        //     if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //         Log.v(TAG, "failure while notifying response", e);
        //     }
        // }
    }
    return NOERROR;
}

String CGetAccountsByTypeAndFeatureSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    AutoPtr<ICharSequence> cs;
    CString::New(String(","), (ICharSequence**)&cs);
    AutoPtr<IObjectContainer> features;
    for (Int32 i = 0; i< mFeatures->GetLength(); i++) {
        String s = (*mFeatures)[i];
        AutoPtr<ICharSequence> str;
        CString::New(s, (ICharSequence**)&str);
        features->Add(str);
    }
    return AccountManagerServiceSession::ToDebugString(now)
            + String(", getAccountsByTypeAndFeatures") + String(", ")
            + (mFeatures != NULL ? TextUtils::Join(cs, features) : String(NULL));
}

ECode CGetAccountsByTypeAndFeatureSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CGetAccountsByTypeAndFeatureSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CGetAccountsByTypeAndFeatureSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CGetAccountsByTypeAndFeatureSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CGetAccountsByTypeAndFeatureSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

ECode CGetAccountsByTypeAndFeatureSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& type,
    /* [in] */ const ArrayOf<String>& features,
    /* [in] */ IIAccountManager* host)
{
    FAIL_RETURN(Init((UserAccounts*)accounts, response,
            type, FALSE /* expectActivityLaunch */,
            TRUE /* stripAuthTokenFromResult */, (CAccountManagerService*)host));
    mFeatures = features.Clone();
    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
