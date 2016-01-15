
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/CRemoteInputHelper.h"
#include "elastos/droid/app/CRemoteInput.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CRemoteInputHelper, Singleton, IRemoteInputHelper)

CAR_SINGLETON_IMPL(CRemoteInputHelper)

ECode CRemoteInputHelper::GetResultsFromIntent(
    /* [in] */ IIntent* intent,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    AutoPtr<IBundle> b = CRemoteInput::GetResultsFromIntent(intent);
    *bundle = b;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

ECode CRemoteInputHelper::AddResultsToIntent(
    /* [in] */ ArrayOf<IRemoteInput*>* remoteInputs,
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* results)
{
    return AddResultsToIntent(remoteInputs, intent, results);
}

} // namespace App
} // namespace Droid
} // namespace Elastos