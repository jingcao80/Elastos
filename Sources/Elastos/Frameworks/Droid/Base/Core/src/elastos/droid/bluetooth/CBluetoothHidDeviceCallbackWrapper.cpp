#include "elastos/droid/bluetooth/CBluetoothHidDeviceCallbackWrapper.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_OBJECT_IMPL(CBluetoothHidDeviceCallbackWrapper)

CAR_INTERFACE_IMPL_2(CBluetoothHidDeviceCallbackWrapper, Object, IIBluetoothHidDeviceCallback, IBinder)

CBluetoothHidDeviceCallbackWrapper::CBluetoothHidDeviceCallbackWrapper()
{}

ECode CBluetoothHidDeviceCallbackWrapper::constructor(
    /* [in] */ IBluetoothHidDeviceCallback* cb)
{
    mCallback = cb;
    return NOERROR;
}

ECode CBluetoothHidDeviceCallbackWrapper::OnAppStatusChanged(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IBluetoothHidDeviceAppConfiguration* config,
    /* [in] */ Boolean registered)
{
    return mCallback->OnAppStatusChanged(device, config, registered);
}

ECode CBluetoothHidDeviceCallbackWrapper::OnConnectionStateChanged(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 state)
{
    return mCallback->OnConnectionStateChanged(device, state);
}

ECode CBluetoothHidDeviceCallbackWrapper::OnGetReport(
    /* [in] */ Byte type,
    /* [in] */ Byte id,
    /* [in] */ Int32 bufferSize)
{
    return mCallback->OnGetReport(type, id, bufferSize);
}

ECode CBluetoothHidDeviceCallbackWrapper::OnSetReport(
    /* [in] */ Byte type,
    /* [in] */ Byte id,
    /* [in] */ ArrayOf<Byte>* data)
{
    return mCallback->OnSetReport(type, id, data);
}

ECode CBluetoothHidDeviceCallbackWrapper::OnSetProtocol(
    /* [in] */ Byte protocol)
{
    return mCallback->OnSetProtocol(protocol);
}

ECode CBluetoothHidDeviceCallbackWrapper::OnIntrData(
    /* [in] */ Byte reportId,
    /* [in] */ ArrayOf<Byte>* data)
{
    return mCallback->OnIntrData(reportId, data);
}

ECode CBluetoothHidDeviceCallbackWrapper::OnVirtualCableUnplug()
{
    return mCallback->OnVirtualCableUnplug();
}

ECode CBluetoothHidDeviceCallbackWrapper::ToString(
    /* [out] */ String* str)
{
    *str = "CBluetoothHidDeviceCallbackWrapper";
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos