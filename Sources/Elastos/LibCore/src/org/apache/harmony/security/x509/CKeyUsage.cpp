
#include "CKeyUsage.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CKeyUsage)

CAR_INTERFACE_IMPL(CKeyUsage, Object, IKeyUsage)

ECode CKeyUsage::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyUsage::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyUsage::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyUsage::GetKeyUsage(
    /* [out, callee] */ ArrayOf<Boolean> ** ppKeyUsage)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyUsage::constructor(
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

