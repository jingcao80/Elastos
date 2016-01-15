
#include "CNameConstraints.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {


ECode CNameConstraints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::IsAcceptable(
    /* [in] */ Elastos::Security::Cert::IX509Certificate * pCert,
    /* [out] */ Boolean * pIsAcceptable)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::IsAcceptableEx(
    /* [in] */ Elastos::Utility::IList * pNames,
    /* [out] */ Boolean * pIsAcceptable)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CNameConstraints::constructor(
    /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralSubtrees * pPermittedSubtrees,
    /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralSubtrees * pExcludedSubtrees)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

