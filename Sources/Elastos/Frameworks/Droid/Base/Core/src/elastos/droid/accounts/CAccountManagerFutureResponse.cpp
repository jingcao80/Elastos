
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CAccountManagerFutureResponse.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String TAG("CAccountManagerFutureResponse");

CAR_OBJECT_IMPL(CAccountManagerFutureResponse)

CAR_INTERFACE_IMPL(CAccountManagerFutureResponse, Object, IAccountManagerResponse)

ECode CAccountManagerFutureResponse::OnResult(
    /* [in] */ IBundle* bundle)
{
    // try {
    AutoPtr<IInterface> result;
    ECode ec = mHost->BundleToResult(bundle, (IInterface**)&result);

    if (FAILED(ec)) {
        Slogger::D(TAG, "AccountManagerFutureResponse::OnResult failed here, we will set exception below");
        return OnError(IAccountManager::ERROR_CODE_INVALID_RESPONSE,
                    String("no result in response"));
    }
    if (result == NULL) {
        return NOERROR;
    }

    mHost->Set(result);
    return NOERROR;
    // } catch (ClassCastException e) {
    //     // we will set the exception below
    // } catch (AuthenticatorException e) {
    //     // we will set the exception below
    // }
}

ECode CAccountManagerFutureResponse::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    Boolean result = FALSE;
    if (errorCode == IAccountManager::ERROR_CODE_CANCELED || errorCode == IAccountManager::ERROR_CODE_USER_RESTRICTED
            || errorCode == IAccountManager::ERROR_CODE_MANAGEMENT_DISABLED_FOR_ACCOUNT_TYPE) {
        // the authenticator indicated that this request was canceled or we were
        // forbidden to fulfill; cancel now
        return mHost->Cancel(TRUE /* mayInterruptIfRunning */, &result);
    }
    mHost->SetException(mHost->mHost->ConvertErrorToException(errorCode, errorMessage));
    return NOERROR;
}

ECode CAccountManagerFutureResponse::constructor (
    /* [in] */ Handle32 host)
{
    mHost = (CAccountManager::BaseFutureTask*)host;
    return NOERROR;
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
