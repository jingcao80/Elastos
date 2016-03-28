#ifndef __ELASTOS_DROID_BLUETOOTH_QBLUETOOTHADAPTER_H__
#define __ELASTOS_DROID_BLUETOOTH_QBLUETOOTHADAPTER_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;
using Elastos::Utility::Etl::HashMap;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Bluetooth::ILeLppCallback)

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class QBluetoothAdapter
    : public Object
    , public IQBluetoothAdapter
{
public:
    class LeLppClientWrapper
        : public Object
        , public ILeLppClientWrapper
        , public IIQBluetoothAdapterCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IQBluetoothAdapter* adapter,
            /* [in] */ IIQBluetooth* adapterService,
            /* [in] */ const String& address,
            /* [in] */ ILeLppCallback* callback);


        LeLppClientWrapper();

        CARAPI Register2(
            /* [in] */ Boolean add,
            /* [out] */ Boolean* result);

        CARAPI WriteRssiThreshold(
            /* [in] */ Byte min,
            /* [in] */ Byte max);

        CARAPI EnableMonitor(
            /* [in] */ Boolean enable);

        CARAPI ReadRssiThreshold();

        CARAPI OnWriteRssiThreshold(
            /* [in] */ const String& address,
            /* [in] */ Int32 status);

        CARAPI OnReadRssiThreshold(
            /* [in] */ const String& address,
            /* [in] */ Int32 low,
            /* [in] */ Int32 upper,
            /* [in] */ Int32 alert,
            /* [in] */ Int32 status);

        CARAPI OnEnableRssiMonitor(
            /* [in] */ const String& address,
            /* [in] */ Int32 e,
            /* [in] */ Int32 status);

        CARAPI OnRssiThresholdEvent(
            /* [in] */ const String& address,
            /* [in] */ Int32 evtType,
            /* [in] */ Int32 rssi);

        CARAPI OnUpdateLease(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IWeakReference> mAdapter;
        AutoPtr<IIQBluetooth> mQBluetoothAdapterService;
        String mDevice;
        AutoPtr<ILeLppCallback>  mClient;
    };

    class BluetoothManagerCallback
        : public Object
        , public IIBluetoothManagerCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IQBluetoothAdapter* host);

        CARAPI OnBluetoothServiceUp(
            /* [in] */ IIBluetooth* bluetoothService);

        CARAPI OnBluetoothServiceDown();

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        QBluetoothAdapter* mAdapter;
    };

    class QBluetoothManagerCallback
        : public Object
        , public IIQBluetoothManagerCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IQBluetoothAdapter* host);

        CARAPI OnQBluetoothServiceUp(
            /* [in] */ IIQBluetooth* qcbluetoothService);

        CARAPI OnQBluetoothServiceDown();

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        QBluetoothAdapter* mAdapter;
    };

public:
    CAR_INTERFACE_DECL()

    QBluetoothAdapter();

    virtual ~QBluetoothAdapter();

    CARAPI constructor(
        /* [in] */ IIBluetoothManager* managerService);

    /** @hide */
    CARAPI RegisterLppClient(
        /* [in] */ ILeLppCallback* client,
        /* [in] */ const String& address,
        /* [in] */ Boolean add,
        /* [out] */ Boolean* result);

   /**
     * Write the rssi threshold for a connected remote device.
     *
     * <p>The {@link BluetoothGattCallback#onWriteRssiThreshold} callback will be
     * invoked when the write request has been sent.
     *
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
     *
     * @param min    The lower limit of rssi threshold
     * @param max    The upper limit of rssi threshold
     * @return true, if the rssi threshold writing request has been sent tsuccessfully
     */
    /** @hide */
    CARAPI WriteRssiThreshold(
        /* [in] */ ILeLppCallback* client,
        /* [in] */ Int32 min,
        /* [in] */ Int32 max,
        /* [out] */ Boolean* result);

            /**
     * Enable rssi monitoring for a connected remote device.
     *
     * <p>The {@link BluetoothGattCallback#onEnableRssiMonitor} callback will be
     * invoked when the rssi monitor enable/disable request has been sent.
     *
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
     *
     * @param enable disable/enable rssi monitor
     * @return true, if the rssi monitoring request has been sent tsuccessfully
     */
     /** @hide */
    CARAPI EnableRssiMonitor(
        /* [in] */ ILeLppCallback* client,
        /* [in] */ Boolean e,
        /* [out] */ Boolean* result);

   /**
     * Read the rssi threshold for a connected remote device.
     *
     * <p>The {@link BluetoothGattCallback#onReadRssiThreshold} callback will be
     * invoked when the rssi threshold has been read.
     *
     * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
     *
     * @return true, if the rssi threshold has been requested successfully
     */
     /** @hide */
    CARAPI ReadRssiThreshold(
        /* [in] */ ILeLppCallback* client,
        /* [out] */ Boolean* result);

    static AutoPtr<QBluetoothAdapter> GetDefaultAdapter();

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;

    static AutoPtr<QBluetoothAdapter> sAdapter;
    static Object sLock;
    static AutoPtr<IBluetoothAdapter> mAdapter;

    AutoPtr<IIBluetoothManager> mManagerService;
    AutoPtr<IIBluetooth> mService;
    AutoPtr<IIQBluetooth> mQService;
    AutoPtr<IIBluetoothManagerCallback> mAdapterServiceCallback;
    AutoPtr<IIQBluetoothManagerCallback> mManagerCallback;

    AutoPtr<HashMap<AutoPtr<ILeLppCallback>, AutoPtr<ILeLppClientWrapper> > > mLppClients;
    Object mMapLock;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif