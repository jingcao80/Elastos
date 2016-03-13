
#include "elastos/droid/utility/CArrayMapHelper.h"
#include "elastos/droid/utility/CArrayMap.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_SINGLETON_IMPL(CArrayMapHelper);
CAR_INTERFACE_IMPL(CArrayMapHelper, Singleton, IArrayMapHelper);
ECode CArrayMapHelper::GetEMPTY(
    /* [out] */ IArrayMap** map)
{
    VALIDATE_NOT_NULL(map);
    *map = CArrayMap::EMPTY;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

} // Utility
} // Droid
} // Elastos
