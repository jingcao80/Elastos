
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Bluetooth_CBluetoothHealthCallbackWrapper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHealthCallbackWrapper)
    , public Object
    , public IIBluetoothHealthCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothHealthCallbackWrapper();

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

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
private:
    AutoPtr<IBluetoothHealthCallback> mCallback;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__
