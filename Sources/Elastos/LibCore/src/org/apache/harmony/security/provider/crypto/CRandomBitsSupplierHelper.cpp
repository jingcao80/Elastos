
#include "CRandomBitsSupplierHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_SINGLETON_IMPL(CRandomBitsSupplierHelper)

CAR_INTERFACE_IMPL(CRandomBitsSupplierHelper, Singleton, IRandomBitsSupplierHelper)

ECode CRandomBitsSupplierHelper::IsServiceAvailable(
    /* [out] */ Boolean * pAvailable)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRandomBitsSupplierHelper::GetRandomBits(
    /* [in] */ Int32 numBytes,
    /* [out, callee] */ ArrayOf<Byte> ** ppRandomBits)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

