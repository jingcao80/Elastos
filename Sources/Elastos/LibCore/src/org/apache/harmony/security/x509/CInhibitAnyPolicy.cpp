
#include "CInhibitAnyPolicy.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CInhibitAnyPolicy)

CAR_INTERFACE_IMPL(CInhibitAnyPolicy, Object, IInhibitAnyPolicy)

ECode CInhibitAnyPolicy::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInhibitAnyPolicy::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInhibitAnyPolicy::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CInhibitAnyPolicy::constructor(
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

