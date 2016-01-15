
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CAccountManagerFutureResponse.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String TAG("CAccountManagerFutureResponse");

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
    Boolean result;
    if (errorCode == IAccountManager::ERROR_CODE_CANCELED) {
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
