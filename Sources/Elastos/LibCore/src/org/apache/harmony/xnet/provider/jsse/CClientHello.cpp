
#include "CClientHello.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {


ECode CClientHello::GetLength(
    /* [out] */ Int32 * pLength)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientHello::GetRandom(
    /* [out, callee] */ ArrayOf<Byte> ** ppRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientHello::constructor(
    /* [in] */ Elastos::Security::ISecureRandom * pSr,
    /* [in] */ ArrayOf<Byte> * pVer,
    /* [in] */ ArrayOf<Byte> * pSes_id,
    /* [in] */ ArrayOf<Org::Apache::Harmony::Xnet::Provider::Jsse::ICipherSuite *> * pCipher_suite)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientHello::constructor(
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
    /* [in] */ Int32 length)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientHello::constructor(
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds)
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

