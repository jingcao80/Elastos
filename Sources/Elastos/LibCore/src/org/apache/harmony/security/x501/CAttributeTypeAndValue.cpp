
#include "CAttributeTypeAndValue.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CAR_OBJECT_IMPL(CAttributeTypeAndValue)

CAR_INTERFACE_IMPL(CAttributeTypeAndValue, Object, IAttributeTypeAndValue)

ECode CAttributeTypeAndValue::AppendName(
    /* [in] */ const String& attrFormat,
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttributeTypeAndValue::GetType(
    /* [out] */ Org::Apache::Harmony::Security::Utils::IObjectIdentifierInUtils** ppIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttributeTypeAndValue::constructor(
    /* [in] */ const String& sOid,
    /* [in] */ Org::Apache::Harmony::Security::X501::IAttributeValue * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

