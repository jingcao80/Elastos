
#ifndef __ELASTOS_DROID_INCALLUI_PROXIMITYSENSOR_H__
#define __ELASTOS_DROID_INCALLUI_PROXIMITYSENSOR_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/AudioModeProvider.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class ProximitySensor
    : public Object
    , public IInCallStateListener
{
public:
    CAR_INTERFACE_DECL();

    ProximitySensor(
        /* [in] */ IContext* context,
        /* [in] */ AudioModeProvider* audioModeProvider);

    CARAPI_(void) TearDown();

    /**
     * Called to keep track of the overall UI state.
     */
    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    /**
     * Used to save when the UI goes in and out of the foreground.
     */
    CARAPI_(void) OnInCallShowing(
        /* [in] */ Boolean showing);

    /**
     * TODO: There is no way to determine if a screen is off due to proximity or if it is
     * legitimately off, but if ever we can do that in the future, it would be useful here.
     * Until then, this function will simply return true of the screen is off.
     */
    CARAPI_(Boolean) IsScreenReallyOff();
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_PROXIMITYSENSOR_H__
