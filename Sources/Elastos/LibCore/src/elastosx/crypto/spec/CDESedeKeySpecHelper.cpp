#include "CDESedeKeySpecHelper.h"
#include "CDESedeKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_INTERFACE_IMPL(CDESedeKeySpecHelper, Singleton, IDESedeKeySpecHelper)

CAR_SINGLETON_IMPL(CDESedeKeySpecHelper)

ECode CDESedeKeySpecHelper::IsParityAdjusted(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    return CDESedeKeySpec::IsParityAdjusted(key, offset, result);
}

} // Spec
} // Crypto
} // Elastosx