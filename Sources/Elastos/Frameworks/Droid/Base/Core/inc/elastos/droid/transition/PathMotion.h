
#ifndef __ELASTOS_DROID_TRANSITION_PATHMOTION_H__
#define __ELASTOS_DROID_TRANSITION_PATHMOTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Transition.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;


namespace Elastos {
namespace Droid {
namespace Transition {

class PathMotion
    : public Object
    , public IPathMotion
{
public:
    CAR_INTERFACE_DECL()

    PathMotion();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_PATHMOTION_H__
