#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CLayoutDirections.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CLayoutDirections, Object, ILayoutDirections)

CAR_OBJECT_IMPL(CLayoutDirections)

ECode CLayoutDirections::constructor(
    /* [in] */ ArrayOf<Int32>* directions)
{
    mDirections = directions;
    return NOERROR;
}

ECode CLayoutDirections::SetDirections(
    /* [in] */ ArrayOf<Int32>* directions)
{
    mDirections = directions;
    return NOERROR;
}

ECode CLayoutDirections::GetDirections(
    /* [out, callee] */ ArrayOf<Int32>** directions)
{
    VALIDATE_NOT_NULL(directions);
    *directions = mDirections;
    REFCOUNT_ADD(*directions);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
