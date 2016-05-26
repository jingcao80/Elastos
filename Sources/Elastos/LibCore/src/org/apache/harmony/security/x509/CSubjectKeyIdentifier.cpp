
#include "CSubjectKeyIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CSubjectKeyIdentifier)

CAR_INTERFACE_IMPL(CSubjectKeyIdentifier, Object, ISubjectKeyIdentifier)

ECode CSubjectKeyIdentifier::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectKeyIdentifier::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectKeyIdentifier::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectKeyIdentifier::GetKeyIdentifier(
    /* [out, callee] */ ArrayOf<Byte> ** ppKeyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectKeyIdentifier::constructor(
    /* [in] */ ArrayOf<Byte> * pKeyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

