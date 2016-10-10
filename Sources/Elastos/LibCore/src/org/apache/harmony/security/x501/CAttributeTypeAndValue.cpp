
#include "CAttributeTypeAndValue.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CAR_OBJECT_IMPL(CAttributeTypeAndValue)

CAR_INTERFACE_IMPL(CAttributeTypeAndValue, Object, IAttributeTypeAndValue)

ECode CAttributeTypeAndValue::GetType(
    /* [out] */ Org::Apache::Harmony::Security::Utils::IObjectIdentifier** ppIdentifier)
{
    VALIDATE_NOT_NULL(ppIdentifier);
    *ppIdentifier = mOld;
    REFCOUNT_ADD(*ppIdentifier);
    return NOERROR;
}


}
}
}
}
}

