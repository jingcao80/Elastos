
#include "CPublicKeyImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CAR_OBJECT_IMPL(CPublicKeyImpl)

CAR_INTERFACE_IMPL(CPublicKeyImpl, Object, IPublicKeyImpl)

ECode CPublicKeyImpl::GetAlgorithm(
    /* [out] */ String * pAlgorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPublicKeyImpl::GetFormat(
    /* [out] */ String * pFormat)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPublicKeyImpl::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPublicKeyImpl::SetAlgorithm(
    /* [in] */ const String& algorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPublicKeyImpl::SetEncoding(
    /* [in] */ ArrayOf<Byte> * pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPublicKeyImpl::constructor(
    /* [in] */ const String& algorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

