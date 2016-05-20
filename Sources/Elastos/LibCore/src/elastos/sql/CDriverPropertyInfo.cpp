
#include "CDriverPropertyInfo.h"

namespace Elastos {
namespace Sql {

CAR_OBJECT_IMPL(CDriverPropertyInfo)

CAR_INTERFACE_IMPL(CDriverPropertyInfo, Object, IDriverPropertyInfo)

CDriverPropertyInfo::CDriverPropertyInfo()
    : mRequired(FALSE)
{
}

ECode CDriverPropertyInfo::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    mName = name;
    mValue = value;
    mRequired = FALSE;
    return NOERROR;
}

} // namespace Sql
} // namespace Elastos

