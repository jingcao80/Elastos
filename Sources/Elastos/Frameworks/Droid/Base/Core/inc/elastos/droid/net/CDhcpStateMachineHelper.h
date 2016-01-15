
#ifndef __ELASTOS_DROID_NET_CDHCPSTATEMACHINEHELPER_H__
#define __ELASTOS_DROID_NET_CDHCPSTATEMACHINEHELPER_H__

#include "_Elastos_Droid_Net_CDhcpStateMachineHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IStateMachine;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * StateMachine that interacts with the native DHCP client and can talk to
 * a controller that also needs to be a StateMachine
 *
 * The Dhcp state machine provides the following features:
 * - Wakeup and renewal using the native DHCP client  (which will not renew
 *   on its own when the device is in suspend state and this can lead to device
 *   holding IP address beyond expiry)
 * - A notification right before DHCP request or renewal is started. This
 *   can be used for any additional setup before DHCP. For example, wifi sets
 *   BT-Wifi coex settings right before DHCP is initiated
 *
 * @hide
 */
CarClass(CDhcpStateMachineHelper)
    , public Singleton
    , public IDhcpStateMachineHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI MakeDhcpStateMachine(
        /* [in] */ IContext* context,
        /* [in] */ IStateMachine* controller,
        /* [in] */ const String& intf,
        /* [out] */ IDhcpStateMachine** result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CDHCPSTATEMACHINEHELPER_H__
