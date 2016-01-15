
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CActivityInfoHelper.h"
#include "elastos/droid/content/pm/ActivityInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CActivityInfoHelper, Singleton, IActivityInfoHelper)

CAR_SINGLETON_IMPL(CActivityInfoHelper)

ECode CActivityInfoHelper::GetCONFIG_NATIVE_BITS(
    /* [out, callee] */ ArrayOf<Int32>** bits)
{
    VALIDATE_NOT_NULL(bits);
    *bits = ActivityInfo::CONFIG_NATIVE_BITS;
    REFCOUNT_ADD(*bits);
    return NOERROR;
}

ECode CActivityInfoHelper::ActivityInfoConfigToNative(
    /* [in] */ Int32 input,
    /* [out] */ Int32* native)
{
    VALIDATE_NOT_NULL(native);
    *native = ActivityInfo::ActivityInfoConfigToNative(input);
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

