
#ifndef __ELASTOS_DROID_TRANSITION_CTRANSITIONVALUES_H__
#define __ELASTOS_DROID_TRANSITION_CTRANSITIONVALUES_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Transition_CTransitionValues.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;

using Elastos::Utility::IMap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CTransitionValues)
    , public Object
    , public ITransitionValues
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hc);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    /**
     * The View with these values
     */
    AutoPtr<IView> mView;

    /**
     * The set of values tracked by transitions for this scene
     */
    AutoPtr<IMap> mValues;

    /**
     * The Transitions that targeted this view.
     */
    AutoPtr<IArrayList> mTargetedTransitions;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CTRANSITIONVALUES_H__
