
#include "elastos/droid/view/CWindowInfoHelper.h"
#include "elastos/droid/view/CWindowInfo.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CWindowInfoHelper);
CAR_INTERFACE_IMPL(CWindowInfoHelper, Singleton, IWindowInfoHelper);
ECode CWindowInfoHelper::Obtain(
    /* [out] */ IWindowInfo** info)
{
    return CWindowInfo::Obtain(info);
}

ECode CWindowInfoHelper::Obtain(
    /* [in] */ IWindowInfo *other,
    /* [out] */ IWindowInfo** info)
{
    return CWindowInfo::Obtain(other, info);
}

} // namespace View
} // namespace Droid
} // namespace Elastos