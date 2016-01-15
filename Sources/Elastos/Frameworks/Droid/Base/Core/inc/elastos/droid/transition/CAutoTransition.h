
#ifndef __ELASTOS_DROID_TRANSITION_CAUTOTRANSITION_H__
#define __ELASTOS_DROID_TRANSITION_CAUTOTRANSITION_H__

#include "_Elastos_Droid_Transition_CAutoTransition.h"
#include "elastos/droid/transition/TransitionSet.h"

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * Utility class for creating a default transition that automatically fades,
 * moves, and resizes views during a scene change.
 *
 * <p>An AutoTransition can be described in a resource file by using the
 * tag <code>autoTransition</code>, along with the other standard
 * attributes of {@link android.R.styleable#Transition}.</p>
 */
CarClass(CAutoTransition)
    , public TransitionSet
{
public:
    CAR_OBJECT_DECL()

    /**
     * Constructs an AutoTransition object, which is a TransitionSet which
     * first fades out disappearing targets, then moves and resizes existing
     * targets, and finally fades in appearing targets.
     *
     */
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

private:
    CARAPI_(void) Init();
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CAUTOTRANSITION_H__
