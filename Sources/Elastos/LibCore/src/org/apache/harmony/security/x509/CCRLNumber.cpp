
#include "CCRLNumber.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CCRLNumber)

CAR_INTERFACE_IMPL(CCRLNumber, Object, ICRLNumber)

ECode CCRLNumber::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumber::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumber::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumber::GetNumber(
    /* [out] */ Elastos::Math::IBigInteger ** ppNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCRLNumber::constructor(
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

