#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/policy/impl/keyguard/KeyguardShowDelegate.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

CAR_INTERFACE_IMPL_2(KeyguardShowDelegate, Object, IBinder, IIKeyguardShowCallback)

const Boolean KeyguardShowDelegate::DEBUG = FALSE;

KeyguardShowDelegate::KeyguardShowDelegate()
{
}

ECode KeyguardShowDelegate::constructor(
    /* [in] */ IKeyguardServiceDelegate* host,
    /* [in] */ IKeyguardServiceDelegateShowListener* showListener)
{
    mHost = host;
    mShowListener = showListener;
    return NOERROR;
}

//@Override
ECode KeyguardShowDelegate::OnShown(
    /* [in] */ IBinder* windowToken)
{
    if (DEBUG) Logger::V("KeyguardShowDelegate", "**** SHOWN CALLED ****");
    if (mShowListener != NULL) {
        mShowListener->OnShown(windowToken);
    }
    mHost->HideScrim();
    return NOERROR;
}

ECode KeyguardShowDelegate::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    return Object::ToString(info);
}

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
