#ifndef __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISECALLBACK_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * Bluetooth LE advertising callbacks, used to deliver advertising operation status.
  */
class AdvertiseCallback
    : public Object
    , public IAdvertiseCallback
{
public:

    CAR_INTERFACE_DECL();

    AdvertiseCallback();

    virtual ~AdvertiseCallback() = 0;

    /**
      * Callback triggered in response to {@link BluetoothLeAdvertiser#startAdvertising} indicating
      * that the advertising has been started successfully.
      *
      * @param settingsInEffect The actual settings used for advertising, which may be different from
      *            what has been requested.
      */
    CARAPI OnStartSuccess(
        /* [in] */ IAdvertiseSettings* settingsInEffect);

    /**
      * Callback when advertising could not be started.
      *
      * @param errorCode Error code (see ADVERTISE_FAILED_* constants) for advertising start
      *            failures.
      */
    CARAPI OnStartFailure(
        /* [in] */ Int32 errorCode);
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISECALLBACK_H__

