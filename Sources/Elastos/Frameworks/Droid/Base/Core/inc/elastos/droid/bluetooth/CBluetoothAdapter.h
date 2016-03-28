
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTER_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothAdapter.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>
#include "elastos/droid/bluetooth/CBluetoothAdapterManagerCallback.h"
#include "elastos/droid/bluetooth/le/ScanCallback.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Bluetooth::LE::IBluetoothLeAdvertiser;
using Elastos::Droid::Bluetooth::LE::IBluetoothLeScanner;
using Elastos::Droid::Bluetooth::LE::IScanResult;
using Elastos::Droid::Bluetooth::LE::ScanCallback;
using Elastos::Droid::Os::IHandler;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IMap;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
 * Represents the local device Bluetooth adapter. The {@link BluetoothAdapter}
 * lets you perform fundamental Bluetooth tasks, such as initiate
 * device discovery, query a list of bonded (paired) devices,
 * instantiate a {@link BluetoothDevice} using a known MAC address, and create
 * a {@link BluetoothServerSocket} to listen for connection requests from other
 * devices.
 *
 * <p>To get a {@link BluetoothAdapter} representing the local Bluetooth
 * adapter, call the static {@link #getDefaultAdapter} method.
 * Fundamentally, this is your starting point for all
 * Bluetooth actions. Once you have the local adapter, you can get a set of
 * {@link BluetoothDevice} objects representing all paired devices with
 * {@link #getBondedDevices()}; start device discovery with
 * {@link #startDiscovery()}; or create a {@link BluetoothServerSocket} to
 * listen for incoming connection requests with
 * {@link #listenUsingRfcommWithServiceRecord(String,UUID)}.
 *
 * <p class="note"><strong>Note:</strong>
 * Most methods require the {@link android.Manifest.permission#BLUETOOTH}
 * permission and some also require the
 * {@link android.Manifest.permission#BLUETOOTH_ADMIN} permission.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about using Bluetooth, read the
 * <a href="{@docRoot}guide/topics/wireless/bluetooth.html">Bluetooth</a> developer guide.</p>
 * </div>
 *
 * {@see BluetoothDevice}
 * {@see BluetoothServerSocket}
 */
CarClass(CBluetoothAdapter)
    , public Object
    , public IBluetoothAdapter
{
//public:
//    interface IBluetoothStateChangeCallback : public IInterface
//    {
//    public:
//        virtual CARAPI_(void) OnBluetoothStateChange(
//            /* [in] */ Boolean on) = 0;
//    };
public:
    class BluetoothAdapterScanCallback
        : public ScanCallback
    {
    public:
        ~BluetoothAdapterScanCallback(){};

        BluetoothAdapterScanCallback(
            /* [in] */ CBluetoothAdapter* owner,
            /* [in] */ ArrayOf<IUUID*>* serviceUuids,
            /* [in] */ IBluetoothAdapterLeScanCallback* callback);

        CARAPI OnScanResult(
            /* [in] */ Int32 callbackType,
            /* [in] */ IScanResult* result);

    private:
        CBluetoothAdapter* mOwner;
        AutoPtr<ArrayOf<IUUID*> > mServiceUuids;
        AutoPtr<IBluetoothAdapterLeScanCallback> mCallback;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CBluetoothAdapter();

    ~CBluetoothAdapter();

    static CARAPI_(AutoPtr<IBluetoothAdapter>) GetDefaultAdapter();

    CARAPI GetRemoteDevice(
        /* [in] */ const String& address,
        /* [out] */ IBluetoothDevice** device);

    CARAPI GetRemoteDevice(
        /* [in] */ ArrayOf<Byte>* address,
        /* [out] */ IBluetoothDevice** device);

    CARAPI GetBluetoothLeAdvertiser(
        /* [out] */ IBluetoothLeAdvertiser** btAdvertiser);

    CARAPI GetBluetoothLeScanner(
        /* [out] */ IBluetoothLeScanner** btLeScanner);

    CARAPI IsEnabled(
        /* [out] */ Boolean* isEnable);

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI Enable(
        /* [out] */ Boolean* result);

    CARAPI Disable(
        /* [out] */ Boolean* result);

    CARAPI Disable(
        /* [in] */ Boolean persist,
        /* [out] */ Boolean* result);

    CARAPI GetAddress(
        /* [out] */ String* address);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI ConfigHciSnoopLog(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* result);

    CARAPI GetUuids(
        /* [out, callee] */ ArrayOf<Elastos::Droid::Os::IParcelUuid *>** uuids);

    CARAPI SetName(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    CARAPI GetScanMode(
        /* [out] */ Int32* mode);

    CARAPI SetScanMode(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 duration,
        /* [out] */ Boolean* result);

    CARAPI SetScanMode(
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* result);

    CARAPI GetDiscoverableTimeout(
        /* [out] */ Int32* timeout);

    CARAPI SetDiscoverableTimeout(
        /* [in] */ Int32 timeout);

    CARAPI StartDiscovery(
        /* [out] */ Boolean* result);

    CARAPI CancelDiscovery(
        /* [out] */ Boolean* result);

    CARAPI IsDiscovering(
        /* [out] */ Boolean* isDiscovering);

    CARAPI IsMultipleAdvertisementSupported(
        /* [out] */ Boolean* result);

    CARAPI IsOffloadedFilteringSupported(
        /* [out] */ Boolean* result);

    CARAPI IsOffloadedScanBatchingSupported(
        /* [out] */ Boolean* result);

    CARAPI GetControllerActivityEnergyInfo(
        /* [in] */ Int32 updateType,
        /* [out] */ IBluetoothActivityEnergyInfo** btActivityEnergyInfo);

    CARAPI GetBondedDevices(
        /* [out, callee] */ ArrayOf<IBluetoothDevice*>** device);

    CARAPI GetConnectionState(
        /* [out] */ Int32* state);

    CARAPI GetProfileConnectionState(
        /* [in] */ Int32 profile,
        /* [out] */ Int32* state);

    CARAPI ListenUsingRfcommOn(
        /* [in] */ Int32 channel,
        /* [out] */ IBluetoothServerSocket** socket);

    CARAPI ListenUsingRfcommWithServiceRecord(
        /* [in] */ const String& name,
        /* [in] */ Elastos::Utility::IUUID* uuid,
        /* [out] */ IBluetoothServerSocket** socket);

    CARAPI ListenUsingInsecureL2capWithServiceRecord(
        /* [in] */ const String& name,
        /* [in] */ Elastos::Utility::IUUID* uuid,
        /* [out] */ IBluetoothServerSocket** socket);

    CARAPI ListenUsingL2capWithServiceRecord(
        /* [in] */ const String& name,
        /* [in] */ Elastos::Utility::IUUID* uuid,
        /* [out] */ IBluetoothServerSocket** socket);

    CARAPI ListenUsingInsecureRfcommWithServiceRecord(
        /* [in] */ const String& name,
        /* [in] */ Elastos::Utility::IUUID* uuid,
        /* [out] */ IBluetoothServerSocket** socket);

    CARAPI ListenUsingEncryptedRfcommWithServiceRecord(
        /* [in] */ const String& name,
        /* [in] */ Elastos::Utility::IUUID* uuid,
        /* [out] */ IBluetoothServerSocket** socket);

    CARAPI ListenUsingInsecureRfcommOn(
        /* [in] */ Int32 port,
        /* [out] */ IBluetoothServerSocket** socket);

    CARAPI ListenUsingEncryptedRfcommOn(
        /* [in] */ Int32 port,
        /* [out] */ IBluetoothServerSocket** socket);

    static CARAPI ListenUsingScoOn(
        /** [out] */ IBluetoothServerSocket** socket);

    CARAPI ReadOutOfBandData(
        /* [out] */ IInterface** data);

    CARAPI GetProfileProxy(
        /* [in] */ Elastos::Droid::Content::IContext* context,
        /* [in] */ IBluetoothProfileServiceListener* listener,
        /* [in] */ Int32 profile,
        /* [out] */ Boolean* result);

    CARAPI CloseProfileProxy(
        /* [in] */ Int32 profile,
        /* [in] */ IBluetoothProfile* proxy);

    CARAPI EnableNoAutoConnect(
        /* [out] */ Boolean* result);

    CARAPI ChangeApplicationBluetoothState(
        /* [in] */ Boolean on,
        /* [in] */ IIBluetoothStateChangeCallback* stateChangeCallback,
        /* [out] */ Boolean* result);

    CARAPI StartLeScan(
        /* [in] */ IBluetoothAdapterLeScanCallback* cb,
        /* [out] */ Boolean* result);

    CARAPI StartLeScan(
        /* [in] */ ArrayOf<IUUID*>* serviceUuids,
        /* [in] */ IBluetoothAdapterLeScanCallback* cb,
        /* [out] */ Boolean* result);

    CARAPI StopLeScan(
        /* [in] */ IBluetoothAdapterLeScanCallback* cb);

    static CARAPI_(Boolean) CheckBluetoothAddress(
        /* [in] */ const String& address);

    CARAPI_(AutoPtr<IIBluetoothManager>) GetBluetoothManager();

    CARAPI_(AutoPtr<IIBluetooth>) GetBluetoothService(
        /* [in] */ IIBluetoothManagerCallback* cb);

    CARAPI_(void) RemoveServiceStateCallback(
        /* [in] */ IIBluetoothManagerCallback* cb);

    CARAPI constructor(
        /* [in] */ IIBluetoothManager* managerService);

private:
    CARAPI CreateNewRfcommSocketAndRecord(
        /* [in] */ const String& name,
        /* [in] */ IUUID* uuid,
        /* [in] */ Boolean auth,
        /* [in] */ Boolean encrypt,
        /* [out] */ IBluetoothServerSocket** socket);

    CARAPI CreateNewL2capSocketAndRecord(
        /* [in] */ const String& name,
        /* [in] */ IUUID* uuid,
        /* [in] */ Boolean auth,
        /* [in] */ Boolean encrypt,
        /* [out] */ IBluetoothServerSocket** socket);

private:
    const static String TAG;
    const static Boolean DBG;
    const static Boolean VDBG;
    const static Int32 ADDRESS_LENGTH = 17;
    const static Int32 CONTROLLER_ENERGY_UPDATE_TIMEOUT_MILLIS = 30;

    /**
     * Lazily initialized singleton. Guaranteed final after first object
     * constructed.
     */
    static AutoPtr<IBluetoothAdapter> sAdapter;

    static AutoPtr<IBluetoothLeScanner> sBluetoothLeScanner;
    static AutoPtr<IBluetoothLeAdvertiser> sBluetoothLeAdvertiser;

    AutoPtr<IIBluetoothManager> mManagerService;
    AutoPtr<IIBluetooth> mService;

    Object mLock;
    AutoPtr<IMap> mLeScanClients;//<LeScanCallback, ScanCallback>

    //AutoPtr<IHandler> mServiceRecordHandler;
    AutoPtr<IIBluetoothManagerCallback> mManagerCallback;
    Object mManagerCallbackLock;
    List<AutoPtr<IIBluetoothManagerCallback> > mProxyServiceStateCallbacks;
    static Object sLock;

    friend class CBluetoothAdapterManagerCallback;
};

}
}
}

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHADAPTER_H__
