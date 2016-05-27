
#include "CProtocolVersion.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CProtocolVersion)

CAR_INTERFACE_IMPL(CProtocolVersion, Object, IProtocolVersion)

ECode CProtocolVersion::GetName(
    /* [out] */ String * pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CProtocolVersion::GetVersion(
    /* [out, callee] */ ArrayOf<Byte> ** ppVer)
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

