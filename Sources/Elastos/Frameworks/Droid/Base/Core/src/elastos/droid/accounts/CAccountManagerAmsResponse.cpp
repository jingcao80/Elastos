
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CAccountManagerAmsResponse.h"

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_OBJECT_IMPL(CAccountManagerAmsResponse)

CAR_INTERFACE_IMPL(CAccountManagerAmsResponse, Object, IAccountManagerResponse)

ECode CAccountManagerAmsResponse::OnResult(
    /* [in] */ IBundle* bundle)
{
    AutoPtr<IParcelable> parcelable;
    FAIL_RETURN(bundle->GetParcelable(IAccountManager::KEY_INTENT,
            (IParcelable**)&parcelable));
    AutoPtr<IIntent> intent = IIntent::Probe(parcelable);
    Boolean value = FALSE;
    if (intent != NULL && mHost->mActivity != NULL) {
        // since the user provided an Activity we will silently start intents
        // that we see
        return IContext::Probe(mHost->mActivity)->StartActivity(intent);
        // leave the Future running to wait for the real response to this request
    }
    else if (bundle->GetBoolean(String("retry"), &value), value) {
        return mHost->DoWork();
    }
    else {
        return mHost->Set(bundle);
    }
    return NOERROR;
}

ECode CAccountManagerAmsResponse::OnError(
    /* [in] */ Int32 code,
    /* [in] */ const String& errorMessage)
{
    Boolean result = FALSE;
    if (code == IAccountManager::ERROR_CODE_CANCELED || code == IAccountManager::ERROR_CODE_USER_RESTRICTED
            || code == IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE) {
        // the authenticator indicated that this request was canceled or we were
        // forbidden to fulfill; cancel now
        return mHost->Cancel(TRUE /* mayInterruptIfRunning */, &result);
    }
    mHost->SetException(mHost->mHost->ConvertErrorToException(code, errorMessage));
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
