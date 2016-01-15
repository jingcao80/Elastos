#include "elastos/droid/internal/policy/impl/CPolicyControl.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CAR_INTERFACE_IMPL(CPolicyControl, Singleton, IPolicyControl)

CAR_SINGLETON_IMPL(CPolicyControl)

ECode CPolicyControl::GetSystemUiVisibility(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Int32* res)
{
    return PolicyControl::GetSystemUiVisibility(win, attrs, res);
}

ECode CPolicyControl::GetWindowFlags(
    /* [in] */ IWindowState* win,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [out] */ Int32* res)
{
    return PolicyControl::GetWindowFlags(win, attrs, res);
}

ECode CPolicyControl::AdjustClearableFlags(
    /* [in] */ IWindowState* win,
    /* [in] */ Int32 clearableFlags,
    /* [out] */ Int32* res)
{
    return PolicyControl::AdjustClearableFlags(win, clearableFlags, res);
}

ECode CPolicyControl::DisableImmersiveConfirmation(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* res)
{
    return PolicyControl::DisableImmersiveConfirmation(pkg, res);
}

ECode CPolicyControl::ReloadFromSetting(
    /* [in] */ IContext* context)
{
    return PolicyControl::ReloadFromSetting(context);
}

ECode CPolicyControl::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    return PolicyControl::Dump(prefix, pw);
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
