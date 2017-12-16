
#include "elastos/droid/hardware/display/IDisplayManagerCallback.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::AndroidParcelUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

const Boolean IDisplayManagerCallbackStub::DEBUG = TRUE;
const String IDisplayManagerCallbackStub::TAG("IDisplayManagerCallbackStub");
const String IDisplayManagerCallbackStub::DESCRIPTOR("android.hardware.display.IDisplayManagerCallback");
const Int32 IDisplayManagerCallbackStub::TRANSACTION_onDisplayEvent = android::IBinder::FIRST_CALL_TRANSACTION + 0;

IDisplayManagerCallbackStub::IDisplayManagerCallbackStub(
    /* [in] */ IIDisplayManagerCallback* callback)
    : mCallback(callback)
{}

void IDisplayManagerCallbackStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IDisplayManagerCallbackStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch (code) {
        case TRANSACTION_onDisplayEvent: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 displayId = AndroidParcelUtils::ReadInt32(data);
            Int32 event = AndroidParcelUtils::ReadInt32(data);
            mCallback->OnDisplayEvent(displayId, event);
            return android::NO_ERROR;
        }

        default: {
            assert(0);
        }
    }
    return android::BBinder::onTransact(code, data, reply, flags);
}

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
