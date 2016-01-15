#ifndef __ELASTOS_DROID_INTERNAL_VIEW_CROTATIONPOLICY_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_CROTATIONPOLICY_H__

#include "_Elastos_Droid_Internal_View_CRotationPolicy.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CarClass(CRotationPolicy)
    , public Singleton
    , public IRotationPolicy
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsRotationSupported(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI GetRotationLockOrientation(
        /* [in] */ IContext* context,
        /* [out] */ Int32* result);

    CARAPI IsRotationLockToggleVisible(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsRotationLocked(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI SetRotationLock(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);

    CARAPI SetRotationLockForAccessibility(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);

    CARAPI RegisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener);

    CARAPI RegisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener,
        /* [in] */ Int32 userHandle);

    CARAPI UnregisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener);
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_CROTATIONPOLICY_H__
