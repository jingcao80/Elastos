
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CAccountManagerAmsResponse.h"

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Accounts {

ECode CAccountManagerAmsResponse::OnResult(
    /* [in] */ IBundle* bundle)
{
    AutoPtr<IParcelable> parcelable;
    FAIL_RETURN(bundle->GetParcelable(IAccountManager::KEY_INTENT,
            (IParcelable**)&parcelable));
    AutoPtr<IIntent> intent = (IIntent*)parcelable->Probe(Elastos::Droid::Content::EIID_IIntent);
    Boolean value;
    if (intent != NULL && mHost->mActivity != NULL) {
        // since the user provided an Activity we will silently start intents
        // that we see
        return mHost->mActivity->StartActivity(intent);
        // leave the Future running to wait for the real response to this request
    }
    else if (bundle->GetBoolean(String("retry"), &value), value) {
        // try {
        return mHost->DoWork();
        // } catch (RemoteException e) {
        //     // this will only happen if the system process is dead, which means
        //     // we will be dying ourselves
        // }
    }
    else {
        return mHost->Set(bundle);
    }
}

ECode CAccountManagerAmsResponse::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    Boolean result;
    if (errorCode == IAccountManager::ERROR_CODE_CANCELED) {
        // the authenticator indicated that this request was canceled, do so now
        return mHost->Cancel(TRUE /* mayInterruptIfRunning */, &result);
    }
    mHost->SetException(mHost->mHost->ConvertErrorToException(errorCode, errorMessage));
    return NOERROR;
}

ECode CAccountManagerAmsResponse::constructor (
    /* [in] */ Handle32 host)
{
    mHost = (CAccountManager::AmsTask*)host;
    return NOERROR;
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
