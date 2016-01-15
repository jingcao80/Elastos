
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CUidRange.h"
#include "elastos/droid/net/CUidRangeHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CUidRangeHelper, Singleton, IUidRangeHelper)

CAR_SINGLETON_IMPL(CUidRangeHelper)

ECode CUidRangeHelper::CreateForUser(
    /* [in] */ Int32 userId,
    /* [out] */ IUidRange** result)
{
    return CUidRange::CreateForUser(userId, result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
