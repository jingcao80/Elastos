
#include "elastos/droid/app/COnKeyguardExitResult.h"

using Elastos::Droid::View::EIID_IOnKeyguardExitResult;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(COnKeyguardExitResult, Object, IOnKeyguardExitResult)

CAR_OBJECT_IMPL(COnKeyguardExitResult)

ECode COnKeyguardExitResult::constructor(
    /* [in] */ IKeyguardManagerOnKeyguardExitResult* cb)
{
    mCallback = cb;
    return NOERROR;
}

ECode COnKeyguardExitResult::OnKeyguardExitResult(
    /* [in] */ Boolean success)
{
    if (mCallback != NULL) {
        return mCallback->OnKeyguardExitResult(success);
    }

    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
