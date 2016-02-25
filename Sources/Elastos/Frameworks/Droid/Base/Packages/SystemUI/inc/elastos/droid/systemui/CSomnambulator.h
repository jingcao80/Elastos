
#ifndef  __ELASTOS_DROID_SYSTEMUI_CSOMNAMBULATOR_H__
#define  __ELASTOS_DROID_SYSTEMUI_CSOMNAMBULATOR_H__

#include "_Elastos_Droid_SystemUI_CSomnambulator.h"
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace SystemUI {

/**
 * A simple activity that launches a dream.
 * <p>
 * Note: This Activity is special.  If this class is moved to another package or
 * renamed, be sure to update the component name in {@link Sandman}.
 * </p>
 */
CarClass(CSomnambulator)
    , public Activity
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    // @Override
    CARAPI OnStart();
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CSOMNAMBULATOR_H__
