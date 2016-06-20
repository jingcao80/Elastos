#ifndef  __ELASTOS_DROID_PHONE_BLUETOOTHMANAGER_H__
#define  __ELASTOS_DROID_PHONE_BLUETOOTHMANAGER_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Phone {

class BluetoothManager
    : public Object
    , public IBluetoothManager
{
public:
    TO_STRING_IMPL("BluetoothManager")

    CAR_INTERFACE_DECL()

    BluetoothManager() {}

    CARAPI IsBluetoothHeadsetAudioOn(
        /* [out] */ Boolean* result);

    CARAPI IsBluetoothAvailable(
        /* [out] */ Boolean* result);

    CARAPI IsBluetoothAudioConnected(
        /* [out] */ Boolean* result);

    CARAPI IsBluetoothAudioConnectedOrPending(
        /* [out] */ Boolean* result);

    CARAPI ShowBluetoothIndication(
        /* [out] */ Boolean* result);

    CARAPI UpdateBluetoothIndication();

    CARAPI AddBluetoothIndicatorListener(
        /* [in] */ IBluetoothManagerBluetoothIndicatorListener* listener);

    CARAPI RemoveBluetoothIndicatorListener(
        /* [in] */ IBluetoothManagerBluetoothIndicatorListener* listener);

    CARAPI ConnectBluetoothAudio();

    CARAPI DisconnectBluetoothAudio();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_BLUETOOTHMANAGER_H__