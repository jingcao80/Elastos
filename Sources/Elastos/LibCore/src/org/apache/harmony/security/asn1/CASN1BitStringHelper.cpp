
#include "CASN1BitStringHelper.h"
#include "CASN1BitString.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CASN1BitStringHelper)

CAR_INTERFACE_IMPL(CASN1BitStringHelper, Singleton, IASN1BitStringHelper)

ECode CASN1BitStringHelper::GetInstance(
    /* [out] */ IASN1BitString** instance)
{
    return CASN1BitString::GetInstance(instance);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

