
#include "CASN1BooleanHelper.h"
#include "CASN1Boolean.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CASN1BooleanHelper)

CAR_INTERFACE_IMPL(CASN1BooleanHelper, Singleton, IASN1BooleanHelper)

ECode CASN1BooleanHelper::GetInstance(
    /* [out] */ IASN1Type** instance)
{
    return CASN1Boolean::GetInstance(instance);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

