
#include "org/apache/harmony/security/x509/CGeneralNames.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CGeneralNames)

CAR_INTERFACE_IMPL(CGeneralNames, Object, IGeneralNames)

ECode CGeneralNames::GetNames(
    /* [out] */ IList** ppNames)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNames::GetPairsList(
    /* [out] */ ICollection** ppPairslist)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNames::AddName(
    /* [in] */ IGeneralName* pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNames::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNames::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNames::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralNames::constructor(
    /* [in] */ IList* pGeneralNames)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org