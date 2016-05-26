
#include "CExtensionValue.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CExtensionValue)

CAR_INTERFACE_IMPL(CExtensionValue, Object, IExtensionValue)

ECode CExtensionValue::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensionValue::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensionValue::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensionValue::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CExtensionValue::constructor(
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

