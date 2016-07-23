
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

    /**
     * Called to keep track of the overall UI state.
     */
    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_PROXIMITYSENSOR_H__
