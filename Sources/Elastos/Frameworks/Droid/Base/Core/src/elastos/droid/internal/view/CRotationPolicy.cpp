#include "elastos/droid/internal/view/CRotationPolicy.h"
#include "elastos/droid/internal/view/RotationPolicy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CAR_INTERFACE_IMPL(CRotationPolicy, Singleton, IRotationPolicy)

CAR_SINGLETON_IMPL(CRotationPolicy)

ECode CRotationPolicy::IsRotationSupported(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = RotationPolicy::IsRotationSupported(context);
    return NOERROR;
}

ECode CRotationPolicy::GetRotationLockOrientation(
    /* [in] */ IContext* context,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = RotationPolicy::GetRotationLockOrientation(context);
    return NOERROR;
}

ECode CRotationPolicy::IsRotationLockToggleVisible(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = RotationPolicy::IsRotationLockToggleVisible(context);
    return NOERROR;
}

ECode CRotationPolicy::IsRotationLocked(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = RotationPolicy::IsRotationLocked(context);
    return NOERROR;
}

ECode CRotationPolicy::SetRotationLock(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    RotationPolicy::SetRotationLock(context, enabled);
    return NOERROR;
}

ECode CRotationPolicy::SetRotationLockForAccessibility(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    RotationPolicy::SetRotationLockForAccessibility(context, enabled);
    return NOERROR;
}

ECode CRotationPolicy::RegisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* listener)
{
    RotationPolicy::RegisterRotationPolicyListener(context, listener);
    return NOERROR;
}

ECode CRotationPolicy::RegisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* listener,
    /* [in] */ Int32 userHandle)
{
    RotationPolicy::RegisterRotationPolicyListener(context, listener, userHandle);
    return NOERROR;
}

ECode CRotationPolicy::UnregisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* listener)
{
    RotationPolicy::UnregisterRotationPolicyListener(context, listener);
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
