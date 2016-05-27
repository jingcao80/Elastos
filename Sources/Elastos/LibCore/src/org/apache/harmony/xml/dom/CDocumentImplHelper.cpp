
#include "CDocumentImplHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_SINGLETON_IMPL(CDocumentImplHelper)

CAR_INTERFACE_IMPL(CDocumentImplHelper, Singleton, IDocumentImplHelper)

ECode CDocumentImplHelper::IsXMLIdentifier(
    /* [in] */ const String& s,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

