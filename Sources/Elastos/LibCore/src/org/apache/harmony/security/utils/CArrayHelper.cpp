
#include "CArrayHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CAR_SINGLETON_IMPL(CArrayHelper)

CAR_INTERFACE_IMPL(CArrayHelper, Singleton, IArrayHelper)

ECode CArrayHelper::GetBytesAsString(
    /* [in] */ ArrayOf<Byte> * pData,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CArrayHelper::ToString(
    /* [in] */ ArrayOf<Byte> * pArray,
    /* [in] */ const String& prefix,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

