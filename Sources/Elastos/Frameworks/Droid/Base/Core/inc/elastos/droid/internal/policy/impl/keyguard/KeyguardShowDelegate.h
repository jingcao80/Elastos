#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSHOWDELEGATE_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSHOWDELEGATE_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Policy::IIKeyguardShowCallback;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

class KeyguardShowDelegate
    : public Object
    , public IBinder
    , public IIKeyguardShowCallback
{
public:
    CAR_INTERFACE_DECL()

    KeyguardShowDelegate();

    CARAPI constructor(
        /* [in] */ IKeyguardServiceDelegate* host,
        /* [in] */ IKeyguardServiceDelegateShowListener* showListener);

    CARAPI OnShown(
        /* [in] */ IBinder* windowToken);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    static const Boolean DEBUG;
    IKeyguardServiceDelegate* mHost;//TODO
    AutoPtr<IKeyguardServiceDelegateShowListener> mShowListener;
};

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSHOWDELEGATE_H__
