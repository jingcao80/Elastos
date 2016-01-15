
#include "elastos/droid/transition/PathMotion.h"

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// PathMotion::
//===============================================================
CAR_INTERFACE_IMPL(PathMotion, Object, IPathMotion)

PathMotion::PathMotion()
{}

ECode PathMotion::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos