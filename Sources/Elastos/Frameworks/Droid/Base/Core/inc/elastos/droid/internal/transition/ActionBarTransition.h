
#ifndef __ELASTOS_DROID_INTERNAL_TRANSITION_HTTPDATETIME_H__
#define __ELASTOS_DROID_INTERNAL_TRANSITION_HTTPDATETIME_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Transition {

class ActionBarTransition
{
public:
    static void BeginDelayedTransition(
        /* [in] */ IViewGroup* sceneRoot);

private:
    ActionBarTransition();

    static AutoPtr<ITransition> InitTransition();

private:
    static Boolean TRANSITIONS_ENABLED;

    static const Int32 TRANSITION_DURATION; // ms

    static const AutoPtr<ITransition> sTransition;
};

} // namespace Transition
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TRANSITION_HTTPDATETIME_H__
