#ifndef __ELASTOS_DROID_BLUETOOTH_LE_BLUETOOTHLEADVERTISER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_BLUETOOTHLEADVERTISER_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/bluetooth/BluetoothGattCallbackWrapper.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Bluetooth::BluetoothGattCallbackWrapper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Utility::IMap;

using Elastos::Droid::Bluetooth::IIBluetoothGatt;
using Elastos::Droid::Bluetooth::IIBluetoothManager;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * This class provides a way to perform Bluetooth LE advertise operations, such as starting and
  * stopping advertising. An advertiser can broadcast up to 31 bytes of advertisement data
  * represented by {@link AdvertiseData}.
  * <p>
  * To get an instance of {@link BluetoothLeAdvertiser}, call the
  * {@link BluetoothAdapter#getBluetoothLeAdvertiser()} method.
  * <p>
  * <b>Note:</b> Most of the methods here require {@link android.Manifest.permission#BLUETOOTH_ADMIN}
  * permission.
  *
  * @see AdvertiseData
  */
class BluetoothLeAdvertiser
    : public Object
    , public IBluetoothLeAdvertiser
{
private:
    /**
      * Bluetooth GATT interface callbacks for advertising.
      */
    class AdvertiseCallbackWrapper
        : public BluetoothGattCallbackWrapper
    {
    public:
        AdvertiseCallbackWrapper(
            /* [in] */ BluetoothLeAdvertiser* owner);

        AdvertiseCallbackWrapper(
            /* [in] */ IAdvertiseCallback* advertiseCallback,
            /* [in] */ IAdvertiseData* advertiseData,
            /* [in] */ IAdvertiseData* scanResponse,
            /* [in] */ IAdvertiseSettings* settings,
            /* [in] */ IIBluetoothGatt* bluetoothGatt,
            /* [in] */ BluetoothLeAdvertiser* owner);

        virtual CARAPI StartRegisteration();

        virtual CARAPI StopAdvertising();

        /**
          * Application interface registered - app is ready to go
          */
        // @Override
        CARAPI OnClientRegistered(
            /* [in] */ Int32 status,
            /* [in] */ Int32 clientIf);

        // @Override
        CARAPI OnMultiAdvertiseCallback(
            /* [in] */ Int32 status,
            /* [in] */ Boolean isStart,
            /* [in] */ IAdvertiseSettings* settings);

    private:
        static const Int32 LE_CALLBACK_TIMEOUT_MILLIS = 2000;
        static Object sLock;
        AutoPtr<IAdvertiseCallback> mAdvertiseCallback;
        AutoPtr<IAdvertiseData> mAdvertisement;
        AutoPtr<IAdvertiseData> mScanResponse;
        AutoPtr<IAdvertiseSettings> mSettings;
        AutoPtr<IIBluetoothGatt> mBluetoothGatt;
        // mClientIf 0: not registered
        // -1: scan stopped
        // >0: registered and scan started
        Int32 mClientIf;
        Boolean mIsAdvertising;
        BluetoothLeAdvertiser* mOwner;
    };

    class StartFailureRunnable
        : public Runnable
    {
    public:
        StartFailureRunnable(
            /* [in] */ IAdvertiseCallback* callback,
            /* [in] */ Int32 error,
            /* [in] */ BluetoothLeAdvertiser* owner);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IAdvertiseCallback> mCallback;
        Int32 mError;
        BluetoothLeAdvertiser* mOwner;
    };

    class StartSuccessRunnable
        : public Runnable
    {
    public:
        StartSuccessRunnable(
            /* [in] */ IAdvertiseCallback* callback,
            /* [in] */ IAdvertiseSettings* settings,
            /* [in] */ BluetoothLeAdvertiser* owner);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IAdvertiseCallback> mCallback;
        AutoPtr<IAdvertiseSettings> mSettings;
        BluetoothLeAdvertiser* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    BluetoothLeAdvertiser();

    /**
      * Use BluetoothAdapter.getLeAdvertiser() instead.
      *
      * @param bluetoothManager BluetoothManager that conducts overall Bluetooth Management
      * @hide
      */
    CARAPI constructor(
        /* [in] */ IIBluetoothManager* bluetoothManager);

    /**
      * Start Bluetooth LE Advertising. On success, the {@code advertiseData} will be broadcasted.
      * Returns immediately, the operation status is delivered through {@code callback}.
      * <p>
      * Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
      *
      * @param settings Settings for Bluetooth LE advertising.
      * @param advertiseData Advertisement data to be broadcasted.
      * @param callback Callback for advertising status.
      */
    CARAPI StartAdvertising(
        /* [in] */ IAdvertiseSettings* settings,
        /* [in] */ IAdvertiseData* advertiseData,
        /* [in] */ IAdvertiseCallback* callback);

    /**
      * Start Bluetooth LE Advertising. The {@code advertiseData} will be broadcasted if the
      * operation succeeds. The {@code scanResponse} is returned when a scanning device sends an
      * active scan request. This method returns immediately, the operation status is delivered
      * through {@code callback}.
      * <p>
      * Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN}
      *
      * @param settings Settings for Bluetooth LE advertising.
      * @param advertiseData Advertisement data to be advertised in advertisement packet.
      * @param scanResponse Scan response associated with the advertisement data.
      * @param callback Callback for advertising status.
      */
    CARAPI StartAdvertising(
        /* [in] */ IAdvertiseSettings* settings,
        /* [in] */ IAdvertiseData* advertiseData,
        /* [in] */ IAdvertiseData* scanResponse,
        /* [in] */ IAdvertiseCallback* callback);

    /**
      * Stop Bluetooth LE advertising. The {@code callback} must be the same one use in
      * {@link BluetoothLeAdvertiser#startAdvertising}.
      * <p>
      * Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
      *
      * @param callback {@link AdvertiseCallback} identifies the advertising instance to stop.
      */
    CARAPI StopAdvertising(
        /* [in] */ IAdvertiseCallback* callback);

    /**
      * Cleans up advertise clients. Should be called when bluetooth is down.
      *
      * @hide
      */
    CARAPI Cleanup();

private:
    // Compute the size of the advertise data.
    CARAPI_(Int32) TotalBytes(
        /* [in] */ IAdvertiseData* data);

    CARAPI_(Int32) ByteLength(
        /* [in] */ ArrayOf<Byte>* array);

    CARAPI_(void) PostStartFailure(
        /* [in] */ IAdvertiseCallback* callback,
        /* [in] */ Int32 error);

    CARAPI_(void) PostStartSuccess(
        /* [in] */ IAdvertiseCallback* callback,
        /* [in] */ IAdvertiseSettings* settings);

private:
    static const String TAG;
    static const Int32 MAX_ADVERTISING_DATA_BYTES = 31;
    // Each fields need one byte for field length and another byte for field type.
    static const Int32 OVERHEAD_BYTES_PER_FIELD = 2;
    // Flags field will be set by system.
    static const Int32 FLAGS_FIELD_BYTES = 3;
    static const Int32 MANUFACTURER_SPECIFIC_DATA_LENGTH = 2;
    static const Int32 SERVICE_DATA_UUID_LENGTH = 2;

    AutoPtr<IIBluetoothManager> mBluetoothManager;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IBluetoothAdapter> mBluetoothAdapter;
    AutoPtr<IMap> mLeAdvertisers;// IAdvertiseCallback, AdvertiseCallbackWrapper
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_BLUETOOTHLEADVERTISER_H__

