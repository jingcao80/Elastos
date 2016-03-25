
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/CAddAcountSession.h"
#include "accounts/CAccountManagerService.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Core::CString;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

UInt32 CAddAcountSession::AddRef()
{
    return AccountManagerServiceSession::AddRef();
}

UInt32 CAddAcountSession::Release()
{
    return AccountManagerServiceSession::Release();
}

PInterface CAddAcountSession::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_AccountManagerServiceSession) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CAddAcountSession::Probe(riid);
}

ECode CAddAcountSession::Run()
{
    return mAuthenticator->AddAccount(this,
            mAccountType, mAuthTokenType, *mRequiredFeatures, mOptions);
}

String CAddAcountSession::ToDebugString(
    /* [in]  */ Int64 now)
{
    AutoPtr<IObjectContainer> features;
    for (Int32 i = 0; i < mRequiredFeatures->GetLength(); i++) {
        String s = (*mRequiredFeatures)[i];
        AutoPtr<ICharSequence> cs;
        CString::New(s, (ICharSequence**)&cs);
        features->Add(cs);
    }
    AutoPtr<ICharSequence> str;
    CString::New(String(","), (ICharSequence**)&str);
    return AccountManagerServiceSession::ToDebugString(now) + String(", addAccount")
            + String(", accountType ") + mAccountType
            + String(", requiredFeatures ")
            + (mRequiredFeatures != NULL ? TextUtils::Join(str, features) : String(NULL));
}

ECode CAddAcountSession::OnResult(
    /* [in] */ IBundle* result)
{
    return AccountManagerServiceSession::OnResult(result);
}

ECode CAddAcountSession::OnRequestContinued()
{
    return AccountManagerServiceSession::OnRequestContinued();
}

ECode CAddAcountSession::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    return AccountManagerServiceSession::OnError(errorCode, errorMessage);
}

ECode CAddAcountSession::ProxyDied()
{
    return AccountManagerServiceSession::ProxyDied();
}

ECode CAddAcountSession::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    return AccountManagerServiceSession::OnServiceConnected(name, service);
}

ECode CAddAcountSession::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return AccountManagerServiceSession::OnServiceDisconnected(name);
}

ECode CAddAcountSession::constructor(
    /* [in] */ Handle32 accounts,
    /* [in] */ IAccountManagerResponse* response,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean expectActivityLaunch,
    /* [in] */ const String& authTokenType,
    /* [in] */ const ArrayOf<String>& requiredFeatures,
    /* [in] */ IBundle* options,
    /* [in] */ IIAccountManager* host)
{
    FAIL_RETURN(Init((UserAccounts*)accounts, response,
            accountType, expectActivityLaunch, TRUE /* stripAuthTokenFromResult */,
            (CAccountManagerService*)host));
    mAuthTokenType = authTokenType;
    mRequiredFeatures = requiredFeatures.Clone();
    mOptions = options;
    return NOERROR;
}

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
