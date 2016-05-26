
#include "CName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CAR_OBJECT_IMPL(CName)

CAR_INTERFACE_IMPL(CName, Object, IName)

ECode CName::GetX500Principal(
    /* [out] */ Elastosx::Security::Auth::X500::IX500Principal ** ppX500Principal)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::GetName(
    /* [in] */ const String& format,
    /* [out] */ String * pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::constructor(
    /* [in] */ ArrayOf<Byte> * pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::constructor(
    /* [in] */ const String& name)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::constructor(
    /* [in] */ Elastos::Utility::IList * pRdn)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

