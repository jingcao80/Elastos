
#include "CStructTimevalHelper.h"
#include "CStructTimeval.h"

namespace Elastos {
namespace Droid {
namespace System {

CAR_SINGLETON_IMPL(CStructTimevalHelper)

CAR_INTERFACE_IMPL(CStructTimevalHelper, Singleton, IStructTimevalHelper)

ECode CStructTimevalHelper::FromMillis(
    /* [in] */ Int64 millis,
    /* [out] */ IStructTimeval** tv)
{
    return CStructTimeval::FromMillis(millis, tv);
}

} // namespace System
} // namespace Droid
} // namespace Elastos
