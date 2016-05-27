
#include "CServerKeyExchange.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CServerKeyExchange)

CAR_INTERFACE_IMPL(CServerKeyExchange, Object, IServerKeyExchange)

ECode CServerKeyExchange::GetLength(
    /* [out] */ Int32 * pLength)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CServerKeyExchange::GetRSAPublicKey(
    /* [out] */ Elastos::Security::Interfaces::IRSAPublicKey ** ppKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CServerKeyExchange::constructor(
    /* [in] */ Elastos::Math::IBigInteger * pPar1,
    /* [in] */ Elastos::Math::IBigInteger * pPar2,
    /* [in] */ Elastos::Math::IBigInteger * pPar3,
    /* [in] */ ArrayOf<Byte> * pHash)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CServerKeyExchange::constructor(
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
    /* [in] */ Int32 length,
    /* [in] */ Int32 keyExchange)
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

