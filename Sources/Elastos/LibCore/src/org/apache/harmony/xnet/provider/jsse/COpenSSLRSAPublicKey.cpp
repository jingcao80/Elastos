
#include "COpenSSLRSAPublicKey.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLRSAPublicKey)

CAR_INTERFACE_IMPL(COpenSSLRSAPublicKey, Object, IOpenSSLRSAPublicKey)

ECode COpenSSLRSAPublicKey::GetPublicExponent(
    /* [out] */ Elastos::Math::IBigInteger ** ppPublicExponent)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPublicKey::Equals(
    /* [in] */ IInterface * pObj,
    /* [out] */ Boolean * pIsEqual)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPublicKey::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPublicKey::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

