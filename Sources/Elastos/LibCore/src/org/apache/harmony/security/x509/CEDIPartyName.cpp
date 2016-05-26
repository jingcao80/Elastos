
#include "CEDIPartyName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CEDIPartyName)

CAR_INTERFACE_IMPL(CEDIPartyName, Object, IEDIPartyName)

ECode CEDIPartyName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

