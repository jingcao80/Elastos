
#include "org/alljoyn/bus/CMutableInteger32Value.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(CMutableInteger32Value, Object, IMutableInteger32Value)

CAR_OBJECT_IMPL(CMutableInteger32Value)

ECode CMutableInteger32Value::constructor()
{
    mValue = 0;
    return NOERROR;
}

ECode CMutableInteger32Value::constructor(
    /* [in] */ Int32 v)
{
    mValue = v;
    return NOERROR;
}

ECode CMutableInteger32Value::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CMutableInteger32Value::SetValue(
    /* [in] */ Int32 value)
{
    mValue = value;
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
