#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class LauncherAnimatorUpdateListener
    : public object
    , public IAnimatorUpdateListener
{
public:
    CAR_INTERFACE_DECL();

    CARAPI OnAnimationUpdate(
        /* [in] */ IValueAnimator* animation);

    virtual CARAPI onAnimationUpdate(
        /* [in] */ Float a,
        /* [in] */ Float b);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__