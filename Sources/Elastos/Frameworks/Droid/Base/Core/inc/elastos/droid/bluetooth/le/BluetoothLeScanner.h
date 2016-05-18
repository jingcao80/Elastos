#ifndef __ELASTOS_DROID_BLUETOOTH_LE_BLUETOOTHLESCANNER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_BLUETOOTHLESCANNER_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/bluetooth/BluetoothGattCallbackWrapper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Bluetooth::BluetoothGattCallbackWrapper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IIBluetoothGatt;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * This class provides methods to perform scan related operations for Bluetooth LE devices. An
  * application can scan for a particular type of Bluetotoh LE devices using {@link ScanFilter}. It
  * can also request different types of callbacks for delivering the result.
  * <p>
  * Use {@link BluetoothAdapter#getBluetoothLeScanner()} to get an instance of
  * {@link BluetoothLeScanner}.
  * <p>
  * <b>Note:</b> Most of the scan methods here require
  * {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
  *
  * @see ScanFilter
  */
class BluetoothLeScanner
    : public Object
    , public IBluetoothLeScanner
{
private:
    /**
      * Bluetooth GATT interface callbacks
      */
    class BleScanCallbackWrapper
        : public BluetoothGattCallbackWrapper
    {
    private:
        class ScanResultRunnable
            : public Runnable
        {
        public:
            ScanResultRunnable(
                /* [in] */ IScanResult* scanResult,
                /* [in] */ BleScanCallbackWrapper* owner);

            // @Override
            CARAPI Run();

        private:
            AutoPtr<IScanResult> mScanResult;
            BleScanCallbackWrapper* mOwner;
        };

        class BatchScanResultsRunnable
            : public Runnable
        {
        public:
            BatchScanResultsRunnable(
                /* [in] */ IList* results,
                /* [in] */ BleScanCallbackWrapper* owner);

            // @Override
            CARAPI Run();

        private:
            AutoPtr<IList> mResults;
            BleScanCallbackWrapper* mOwner;
        };

        class FoundOrLostRunnable
            : public Runnable
        {
        public:
            FoundOrLostRunnable(
                /* [in] */ IScanResult* scanResult,
                /* [in] */ Boolean onFound,
                /* [in] */ BleScanCallbackWrapper* owner);

            // @Override
            CARAPI Run();

        private:
            AutoPtr<IScanResult> mScanResult;
            Boolean mOnFound;
            BleScanCallbackWrapper* mOwner;
        };

    public:
        BleScanCallbackWrapper(
            /* [in] */ IIBluetoothGatt* bluetoothGatt,
            /* [in] */ IList* filters,//IScanFilter
            /* [in] */ IScanSettings* settings,
            /* [in] */ IScanCallback* scanCallback,
            /* [in] */ IList* resultStorages, // IList<IResultStorageDescriptor>
            /* [in] */ BluetoothLeScanner* owner);

        virtual CARAPI StartRegisteration();

        virtual CARAPI StopLeScan();

        virtual CARAPI FlushPendingBatchResults();

        /**
          * Application interface registered - app is ready to go
          */
        // @Override
        CARAPI OnClientRegistered(
            /* [in] */ Int32 status,
            /* [in] */ Int32 clientIf);

        /**
          * Callback reporting an LE scan result.
          *
          * @hide
          */
        // @Override
        CARAPI OnScanResult(
            /* [in] */ IScanResult* scanResult);

        // @Override
        CARAPI OnBatchScanResults(
            /* [in] */ IList* results);// ScanResult

        // @Override
        CARAPI OnFoundOrLost(
            /* [in] */ Boolean onFound,
            /* [in] */ IScanResult* scanResult);

    private:
        static const Int32 REGISTRATION_CALLBACK_TIMEOUT_MILLIS = 2000;
        static Object sLock;
        AutoPtr<IScanCallback> mScanCallback;
        AutoPtr<IList> mFilters;// ScanFilter
        AutoPtr<IScanSettings> mSettings;// IScanSettings
        AutoPtr<IIBluetoothGatt> mBluetoothGatt;
        AutoPtr<IList> mResultStorages; // IList<ResultStorageDescriptor> >
        // mLeHandle 0: not registered
        // -1: scan stopped
        // > 0: registered and scan started
        Int32 mClientIf;
        BluetoothLeScanner* mOwner;
    };

    class CallbackErrorRunnable
        : public Runnable
    {
    public:
        CallbackErrorRunnable(
            /* [in] */ IScanCallback* callback,
            /* [in] */ Int32 errorCode,
            /* [in] */ BluetoothLeScanner* owner);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IScanCallback> mCallback;
        Int32 mErrorCode;
        BluetoothLeScanner* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothLeScanner();
    /**
      * Use {@link BluetoothAdapter#getBluetoothLeScanner()} instead.
      *
      * @param bluetoothManager BluetoothManager that conducts overall Bluetooth Management.
      * @hide
      */
    CARAPI constructor(
        /* [in] */ IIBluetoothManager* bluetoothManager);

    /**
      * Start Bluetooth LE scan with default parameters and no filters. The scan results will be
      * delivered through {@code callback}.
      * <p>
      * Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
      *
      * @param callback Callback used to deliver scan results.
      * @throws IllegalArgumentException If {@code callback} is null.
      */
    CARAPI StartScan(
        /* [in] */ IScanCallback* callback);

    /**
      * Start Bluetooth LE scan. The scan results will be delivered through {@code callback}.
      * <p>
      * Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
      *
      * @param filters {@link ScanFilter}s for finding exact BLE devices.
      * @param settings Settings for the scan.
      * @param callback Callback used to deliver scan results.
      * @throws IllegalArgumentException If {@code settings} or {@code callback} is null.
      */
    CARAPI StartScan(
        /* [in] */ IList* filters, // IScanFilter
        /* [in] */ IScanSettings* settings,
        /* [in] */ IScanCallback* callback);

    /**
      * Stops an ongoing Bluetooth LE scan.
      * <p>
      * Requires {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
      *
      * @param callback
      */
    CARAPI StopScan(
        /* [in] */ IScanCallback* callback);

    /**
      * Flush pending batch scan results stored in Bluetooth controller. This will return Bluetooth
      * LE scan results batched on bluetooth controller. Returns immediately, batch scan results data
      * will be delivered through the {@code callback}.
      *
      * @param callback Callback of the Bluetooth LE Scan, it has to be the same instance as the one
      *            used to start scan.
      */
    CARAPI FlushPendingScanResults(
        /* [in] */ IScanCallback* callback);

    /**
      * Start truncated scan.
      *
      * @hide
      */
    // @SystemApi
    CARAPI StartTruncatedScan(
        /* [in] */ IList* truncatedFilters,
        /* [in] */ IScanSettings* settings,
        /* [in] */ IScanCallback* callback);

    /**
      * Cleans up scan clients. Should be called when bluetooth is down.
      *
      * @hide
      */
    CARAPI Cleanup();

private:
    CARAPI StartScan(
        /* [in] */ IList* filters, // ScanFilter
        /* [in] */ IScanSettings* settings,
        /* [in] */ IScanCallback* callback,
        /* [in] */ IList* resultStorages); // IList<ResultStorageDescriptor*> >

    CARAPI_(void) PostCallbackError(
        /* [in] */ IScanCallback* callback,
        /* [in] */ Int32 errorCode);

    CARAPI_(Boolean) IsSettingsConfigAllowedForScan(
        /* [in] */ IScanSettings* settings);

private:
    static const String TAG;
    static const Boolean DBG;
    AutoPtr<IIBluetoothManager> mBluetoothManager;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IBluetoothAdapter> mBluetoothAdapter;
    Int32 mErrorCode;
    //<ScanCallback, BleScanCallbackWrapper>
    AutoPtr<IMap> mLeScanClients;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_BLUETOOTHLESCANNER_H__

