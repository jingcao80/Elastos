
#include "org/apache/harmony/security/x509/CPolicyInformationHelper.h"
#include "org/apache/harmony/security/x509/CPolicyInformation.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CPolicyInformationHelper)

CAR_INTERFACE_IMPL(CPolicyInformationHelper, Singleton, IPolicyInformationHelper)

ECode CPolicyInformationHelper::GetASN1(
    /* [out] */ IASN1Sequence** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CPolicyInformation::ASN1;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org