
#include "org/apache/harmony/xml/dom/CDOMImplementationImplHelper.h"
#include "org/apache/harmony/xml/dom/CDOMImplementationImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_SINGLETON_IMPL(CDOMImplementationImplHelper)
CAR_INTERFACE_IMPL(CDOMImplementationImplHelper, Singleton, IDOMImplementationImplHelper)

AutoPtr<IDOMImplementation> CDOMImplementationImplHelper::instance;

ECode CDOMImplementationImplHelper::GetInstance(
    /* [out] */ Org::W3c::Dom::IDOMImplementation ** ppInstance)
{
    VALIDATE_NOT_NULL(ppInstance);
    if (instance == NULL) {
        instance = new CDOMImplementationImpl();
    }

    *ppInstance = instance;
    REFCOUNT_ADD(*ppInstance);
    return NOERROR;
}

}
}
}
}
}

