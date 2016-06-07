
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BLUETOOTHCONTROLLERIMPL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BLUETOOTHCONTROLLERIMPL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothProfileServiceListener;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class BluetoothControllerImpl
    : public Object
    , public IBluetoothController
{
private:
    class Receiver
        : public BroadcastReceiver
    {
    public:

        TO_STRING_IMPL("BluetoothControllerImpl::Receiver")

        Receiver(
            /* [in] */ BluetoothControllerImpl* host);

        void Register();

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        BluetoothControllerImpl* mHost;
    };

    class DeviceInfo : public Object
    {
    public:
        DeviceInfo();

    public:
        Int32 mConnectionState;
        Boolean mBonded;  // per getBondedDevices
    };

    class ServiceListener
        : public Object
        , public IBluetoothProfileServiceListener
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceListener(
            /* [in] */ IBluetoothDevice* device,
            /* [in] */ const String& action,
            /* [in] */ Boolean connect);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ Int32 profile,
            /* [in] */ IBluetoothProfile* proxy);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ Int32 profile);

    private:
        AutoPtr<IBluetoothDevice> mDevice;
        String mAction;
        Boolean mConnect;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothControllerImpl(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI AddStateChangedCallback(
        /* [in] */ IBluetoothControllerCallback* cb);

    // @Override
    CARAPI RemoveStateChangedCallback(
        /* [in] */ IBluetoothControllerCallback* cb);

    // @Override
    CARAPI IsBluetoothEnabled(
        /* [out] */ Boolean* enabled);

    // @Override
    CARAPI IsBluetoothConnected(
        /* [out] */ Boolean* connected);

    // @Override
    CARAPI IsBluetoothConnecting(
        /* [out] */ Boolean* connecting);

    // @Override
    CARAPI SetBluetoothEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI IsBluetoothSupported(
        /* [out] */ Boolean* supported);

    // @Override
    CARAPI GetPairedDevices(
        /* [out] */ ISet** devices);

    // @Override
    CARAPI Connect(
        /* [in] */ IBluetoothControllerPairedDevice* pd);

    // @Override
    CARAPI Disconnect(
        /* [in] */ IBluetoothControllerPairedDevice* pd);

    // @Override
    CARAPI GetLastDeviceName(
        /* [out] */ String* name);

private:
    static CARAPI_(String) InfoToString(
        /* [in] */ DeviceInfo* info);

    static CARAPI_(Int32) ConnectionStateToPairedDeviceState(
        /* [in] */ Int32 state);

    CARAPI_(void) Connect(
        /* [in] */ IBluetoothControllerPairedDevice* pd,
        /* [in] */ Boolean connect);

    CARAPI_(void) UpdateBondedBluetoothDevices();

    CARAPI_(void) FirePairedDevicesChanged();

    CARAPI_(void) SetAdapterState(
        /* [in] */ Int32 adapterState);

    CARAPI_(void) SetConnecting(
        /* [in] */ Boolean connecting);

    CARAPI_(void) FireStateChange();

    CARAPI_(void) FireStateChange(
        /* [in] */ IBluetoothControllerCallback* cb);

    CARAPI_(AutoPtr<DeviceInfo>) UpdateInfo(
        /* [in] */ IBluetoothDevice* device);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IContext> mContext;
    AutoPtr<IArrayList> mCallbacks;  /*<Callback*/
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<Receiver> mReceiver;
    AutoPtr<IArrayMap> mDeviceInfo;  /*<BluetoothDevice, DeviceInfo*/

    Boolean mEnabled;
    Boolean mConnecting;
    AutoPtr<IBluetoothDevice> mLastDevice;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BLUETOOTHCONTROLLERIMPL_H__
