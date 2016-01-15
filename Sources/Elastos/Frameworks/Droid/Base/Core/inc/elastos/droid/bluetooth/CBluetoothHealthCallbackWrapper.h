
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHealthCallbackWrapper.h"

using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHealthCallbackWrapper)
{
public:
    CARAPI OnHealthAppConfigurationStatusChange(
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [in] */ Int32 status);

    CARAPI OnHealthChannelStateChange(
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 prevState,
        /* [in] */ Int32 newState,
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Int32 channelId);

    CARAPI constructor(
        /* [in] */ IBluetoothHealthCallback* cb);

private:
    AutoPtr<IBluetoothHealthCallback> mCallback;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__
