
#include "elastos/droid/utility/CPairHelper.h"
#include "elastos/droid/utility/CPair.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CPairHelper, Singleton, IPairHelper)

CAR_SINGLETON_IMPL(CPairHelper)

ECode CPairHelper::Create(
    /* [in] */ IInterface *A,
    /* [in] */ IInterface *B,
    /* [out] */ IPair **pair)
{
    return CPair::Create(A, B, pair);
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
