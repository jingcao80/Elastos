
#include "CDOMImplementationImplHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_SINGLETON_IMPL(CDOMImplementationImplHelper)

CAR_INTERFACE_IMPL(CDOMImplementationImplHelper, Singleton, IDOMImplementationImplHelper)

ECode CDOMImplementationImplHelper::GetInstance(
    /* [out] */ Org::W3c::Dom::IDOMImplementation ** ppInstance)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

