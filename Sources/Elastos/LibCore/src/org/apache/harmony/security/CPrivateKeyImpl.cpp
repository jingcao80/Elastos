
#include "CPrivateKeyImpl.h"

using Elastos::Security::EIID_IPrivateKey;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CAR_OBJECT_IMPL(CPrivateKeyImpl)

CAR_INTERFACE_IMPL_2(CPrivateKeyImpl, Object, IPrivateKeyImpl, IPrivateKey)

ECode CPrivateKeyImpl::GetAlgorithm(
    /* [out] */ String * pAlgorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivateKeyImpl::GetFormat(
    /* [out] */ String * pFormat)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivateKeyImpl::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivateKeyImpl::SetAlgorithm(
    /* [in] */ const String& algorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivateKeyImpl::SetEncoding(
    /* [in] */ ArrayOf<Byte> * pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivateKeyImpl::constructor(
    /* [in] */ const String& algorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org