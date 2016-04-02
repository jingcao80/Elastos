
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_VELOCITYTRACKERFACTOR_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_VELOCITYTRACKERFACTOR_H__

#include "_SystemUI.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A class to generate {@link VelocityTrackerInterface}, depending on the configuration.
 */
class VelocityTrackerFactory
{
public:
    static CARAPI_(AutoPtr<IVelocityTrackerInterface>) Obtain(
        /* [in] */ IContext* ctx);

public:
    static const String PLATFORM_IMPL;
    static const String NOISY_IMPL;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_VELOCITYTRACKERFACTOR_H__
