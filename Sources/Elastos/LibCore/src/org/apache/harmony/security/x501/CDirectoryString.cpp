
#include "CDirectoryString.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CAR_OBJECT_IMPL(CDirectoryString)

CAR_INTERFACE_IMPL(CDirectoryString, Object, IDirectoryString)

ECode CDirectoryString::GetASN1(
    /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Choice ** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDirectoryString::SetASN1(
    /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Choice * pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

