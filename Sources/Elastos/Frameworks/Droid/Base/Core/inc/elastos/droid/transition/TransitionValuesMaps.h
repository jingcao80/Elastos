
#ifndef __ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__
#define __ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Transition.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Transition {

class TransitionValuesMaps
    : public Object
    , public ITransitionValuesMaps
{
public:
    CAR_INTERFACE_DECL()

    TransitionValuesMaps();

public:
    AutoPtr<IArrayMap> mViewValues;
    AutoPtr<ISparseArray> mIdValues;
    AutoPtr<IInt64SparseArray> mItemIdValues;
    AutoPtr<IArrayMap> mNameValues;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_TRANSITIONVALUESMAPS_H__
