#include "CDESKeySpecHelper.h"
#include "CDESKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_INTERFACE_IMPL(CDESKeySpecHelper, Singleton, IDESKeySpecHelper)

CAR_SINGLETON_IMPL(CDESKeySpecHelper)

ECode CDESKeySpecHelper::IsParityAdjusted(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    return CDESKeySpec::IsParityAdjusted(key, offset, result);
}

ECode CDESKeySpecHelper::IsWeak(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    return CDESKeySpec::IsWeak(key, offset, result);
}

} // Spec
} // Crypto
} // Elastosx