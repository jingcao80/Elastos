
#include "elastos/droid/bluetooth/CBluetoothHealthCallbackWrapper.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothHealthCallbackWrapper, Object, IIBluetoothHealthCallback, IBinder);

CAR_OBJECT_IMPL(CBluetoothHealthCallbackWrapper);

CBluetoothHealthCallbackWrapper::CBluetoothHealthCallbackWrapper()
{
}

ECode CBluetoothHealthCallbackWrapper::OnHealthAppConfigurationStatusChange(
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [in] */ Int32 status)
{
    return mCallback->OnHealthAppConfigurationStatusChange(config, status);
}

ECode CBluetoothHealthCallbackWrapper::OnHealthChannelStateChange(
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 prevState,
    /* [in] */ Int32 newState,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ Int32 channelId)
{
    return mCallback->OnHealthChannelStateChange(config, device, prevState, newState,
            fd, channelId);
}

ECode CBluetoothHealthCallbackWrapper::constructor(
    /* [in] */ IBluetoothHealthCallback* cb)
{
    mCallback = cb;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
