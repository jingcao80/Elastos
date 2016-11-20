
#include "org/alljoyn/bus/CMutableInteger16Value.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(CMutableInteger16Value, Object, IMutableInteger16Value)

CAR_OBJECT_IMPL(CMutableInteger16Value)

ECode CMutableInteger16Value::constructor()
{
    mValue = 0;
    return NOERROR;
}

ECode CMutableInteger16Value::constructor(
    /* [in] */ Int16 v)
{
    mValue = v;
    return NOERROR;
}

ECode CMutableInteger16Value::GetValue(
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CMutableInteger16Value::SetValue(
    /* [in] */ Int16 value)
{
    mValue = value;
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
