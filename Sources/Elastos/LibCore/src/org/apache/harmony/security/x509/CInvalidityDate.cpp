
#include "CInvalidityDate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CInvalidityDate)

CAR_INTERFACE_IMPL(CInvalidityDate, Object, IInvalidityDate)

ECode CInvalidityDate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInvalidityDate::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInvalidityDate::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInvalidityDate::GetDate(
    /* [out] */ Elastos::Utility::IDate ** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInvalidityDate::constructor(
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

