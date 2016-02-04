
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEALTHCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEALTHCALLBACK_H__

//#include "Elastos.Droid.Core_server.h"
#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
 * This abstract class is used to implement {@link BluetoothHealth} callbacks.
 */
class BluetoothHealthCallback
    : public Object
    , public IBluetoothHealthCallback
{
public:
    CAR_INTERFACE_DECL();

    BluetoothHealthCallback();

    virtual ~BluetoothHealthCallback() = 0;

    /**
     * Callback to inform change in registration state of the health
     * application.
     * <p> This callback is called on the binder thread (not on the UI thread)
     *
     * @param config Bluetooth Health app configuration
     * @param status Success or failure of the registration or unregistration
     *            calls. Can be one of
     *            {@link BluetoothHealth#APP_CONFIG_REGISTRATION_SUCCESS} or
     *            {@link BluetoothHealth#APP_CONFIG_REGISTRATION_FAILURE} or
     *            {@link BluetoothHealth#APP_CONFIG_UNREGISTRATION_SUCCESS} or
     *            {@link BluetoothHealth#APP_CONFIG_UNREGISTRATION_FAILURE}
     */
    CARAPI OnHealthAppConfigurationStatusChange(
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [in] */ Int32 status);

    /**
     * Callback to inform change in channel state.
     * <p> Its the responsibility of the implementor of this callback to close the
     * parcel file descriptor when done. This callback is called on the Binder
     * thread (not the UI thread)
     *
     * @param config The Health app configutation
     * @param device The Bluetooth Device
     * @param prevState The previous state of the channel
     * @param newState The new state of the channel.
     * @param fd The Parcel File Descriptor when the channel state is connected.
     * @param channelId The id associated with the channel. This id will be used
     *            in future calls like when disconnecting the channel.
     */
    CARAPI OnHealthChannelStateChange(
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 prevState,
        /* [in] */ Int32 newState,
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Int32 channelId);

private:
    const static String TAG;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEALTHCALLBACK_H__
