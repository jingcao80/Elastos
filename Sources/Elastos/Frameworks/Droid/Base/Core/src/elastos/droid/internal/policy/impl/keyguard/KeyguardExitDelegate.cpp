#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/policy/impl/keyguard/KeyguardExitDelegate.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

CAR_INTERFACE_IMPL_2(KeyguardExitDelegate, Object, IBinder, IIKeyguardExitCallback)

const Boolean KeyguardExitDelegate::DEBUG = TRUE;

KeyguardExitDelegate::KeyguardExitDelegate()
{
}

ECode KeyguardExitDelegate::constructor(
    /* [in] */ IKeyguardServiceDelegate* host,
    /* [in] */ IOnKeyguardExitResult* onKeyguardExitResult)
{
    mHost = host;
    mOnKeyguardExitResult = onKeyguardExitResult;
    return NOERROR;
}

//@Override
ECode KeyguardExitDelegate::OnKeyguardExitResult(
    /* [in] */ Boolean success)
{
    if (DEBUG) Logger::V("KeyguardExitDelegate", "**** onKeyguardExitResult(%d) CALLED ****", success);
    if (mOnKeyguardExitResult != NULL) {
        return mOnKeyguardExitResult->OnKeyguardExitResult(success);
    }
    return NOERROR;
}

ECode KeyguardExitDelegate::ToString(
    /* [out] */ String* info)
{
    return Object::ToString(info);
}

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
