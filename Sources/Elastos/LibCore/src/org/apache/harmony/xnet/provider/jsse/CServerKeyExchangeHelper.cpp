
#include "CServerKeyExchangeHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CServerKeyExchangeHelper)

CAR_INTERFACE_IMPL(CServerKeyExchangeHelper, Singleton, IServerKeyExchangeHelper)

ECode CServerKeyExchangeHelper::ToUnsignedByteArray(
    /* [in] */ Elastos::Math::IBigInteger * pBi,
    /* [out, callee] */ ArrayOf<Byte> ** ppByteArray)
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

