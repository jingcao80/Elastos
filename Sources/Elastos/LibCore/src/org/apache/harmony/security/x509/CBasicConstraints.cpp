
#include "CBasicConstraints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CBasicConstraints)

CAR_INTERFACE_IMPL(CBasicConstraints, Object, IBasicConstraints)

ECode CBasicConstraints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::GetPathLenConstraint(
    /* [out] */ Int32 * pPathLenConstraint)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CBasicConstraints::constructor(
    /* [in] */ ArrayOf<Byte> * pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

