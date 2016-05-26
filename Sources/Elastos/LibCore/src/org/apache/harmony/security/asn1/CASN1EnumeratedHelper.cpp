
#include "CASN1EnumeratedHelper.h"
#include "CASN1Enumerated.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CASN1EnumeratedHelper)

CAR_INTERFACE_IMPL(CASN1EnumeratedHelper, Singleton, IASN1EnumeratedHelper)

ECode CASN1EnumeratedHelper::GetInstance(
    /* [out] */ IASN1Enumerated** instance)
{
    CASN1Enumerated::GetInstance(instance);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

