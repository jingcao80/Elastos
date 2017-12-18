
#include "elastos/droid/hardware/input/IInputDevicesChangedListener.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::AndroidParcelUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

const Boolean IInputDevicesChangedListenerStub::DEBUG = TRUE;
const String IInputDevicesChangedListenerStub::TAG("IInputDevicesChangedListenerStub");
const String IInputDevicesChangedListenerStub::DESCRIPTOR("android.hardware.input.IInputDevicesChangedListener");
const Int32 IInputDevicesChangedListenerStub::TRANSACTION_onInputDevicesChanged = android::IBinder::FIRST_CALL_TRANSACTION + 0;

IInputDevicesChangedListenerStub::IInputDevicesChangedListenerStub(
    /* [in] */ IInputDevicesChangedListener* listener)
    : mListener(listener)
{}

void IInputDevicesChangedListenerStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IInputDevicesChangedListenerStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch (code) {
        case TRANSACTION_onInputDevicesChanged: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr< ArrayOf<Int32> > deviceIdAndGeneration =
                    AndroidParcelUtils::CreateInt32Array(data);
            mListener->OnInputDevicesChanged(deviceIdAndGeneration);
            return android::NO_ERROR;
        }
    }
    return android::BBinder::onTransact(code, data, reply, flags);
}

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
