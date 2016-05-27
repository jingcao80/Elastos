
#include "CClientKeyExchange.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CClientKeyExchange)

CAR_INTERFACE_IMPL(CClientKeyExchange, Object, IClientKeyExchange)

ECode CClientKeyExchange::GetLength(
    /* [out] */ Int32 * pLength)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientKeyExchange::IsEmpty(
    /* [out] */ Boolean * pIsEmpty)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientKeyExchange::constructor(
    /* [in] */ ArrayOf<Byte> * pEncrypted_pre_master_secret,
    /* [in] */ Boolean isTLS)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientKeyExchange::constructor(
    /* [in] */ Elastos::Math::IBigInteger * pDh_Yc)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientKeyExchange::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CClientKeyExchange::constructor(
    /* [in] */ Org::Apache::Harmony::Xnet::Provider::Jsse::IHandshakeIODataStream * pIds,
    /* [in] */ Int32 length,
    /* [in] */ Boolean isTLS,
    /* [in] */ Boolean isRSA)
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

