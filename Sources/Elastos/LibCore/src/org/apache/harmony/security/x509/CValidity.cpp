
#include "CValidity.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CValidity)

CAR_INTERFACE_IMPL(CValidity, Object, IValidity)

ECode CValidity::GetNotBefore(
    /* [out] */ Elastos::Utility::IDate ** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CValidity::GetNotAfter(
    /* [out] */ Elastos::Utility::IDate ** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CValidity::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CValidity::constructor(
    /* [in] */ Elastos::Utility::IDate * pNotBefore,
    /* [in] */ Elastos::Utility::IDate * pNotAfter)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

