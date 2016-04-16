#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class LauncherAnimatorUpdateListener
    : public Object
    , public IAnimatorUpdateListener
{
public:
    CAR_INTERFACE_DECL();

    CARAPI OnAnimationUpdate(
        /* [in] */ IValueAnimator* animation);

    virtual CARAPI OnAnimationUpdate(
        /* [in] */ Float a,
        /* [in] */ Float b) = 0;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMATORUPDATELISTENER_H__