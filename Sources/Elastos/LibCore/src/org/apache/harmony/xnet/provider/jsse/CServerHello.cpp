
#include "CServerHello.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CServerHello)

CAR_INTERFACE_IMPL(CServerHello, Object, IServerHello)

ECode CServerHello::GetLength(
    /* [out] */ Int32 * pLength)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CServerHello::GetRandom(
    /* [out, callee] */ ArrayOf<Byte> ** ppRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CServerHello::constructor(
    /* [in] */ Elastos::Security::ISecureRandom * pSr,
    /* [in] */ ArrayOf<Byte> * pServer_version,
    /* [in] */ ArrayOf<Byte> * pSession_id,
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ICipherSuite * pCipher_suite,
    /* [in] */ Byte compression_method)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CServerHello::constructor(
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
    /* [in] */ Int32 length)
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

