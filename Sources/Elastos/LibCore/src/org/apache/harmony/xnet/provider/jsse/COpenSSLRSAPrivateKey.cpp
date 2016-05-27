
#include "COpenSSLRSAPrivateKey.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLRSAPrivateKey)

CAR_INTERFACE_IMPL(COpenSSLRSAPrivateKey, Object, IOpenSSLRSAPrivateKey)

ECode COpenSSLRSAPrivateKey::GetPrivateExponent(
    /* [out] */ Elastos::Math::IBigInteger ** ppPrivateExponent)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPrivateKey::GetPkeyContext(
    /* [out] */ Int32 * pCtx)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPrivateKey::GetPkeyAlias(
    /* [out] */ String * pAlias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPrivateKey::Equals(
    /* [in] */ IInterface * pObj,
    /* [out] */ Boolean * pIsEqual)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPrivateKey::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPrivateKey::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLRSAPrivateKey::constructor(
    /* [in] */ Elastos::Security::Spec::IRSAPrivateKeySpec * pRsaKeySpec)
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

