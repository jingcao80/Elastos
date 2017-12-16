
#include "elastos/droid/view/IWindowSessionCallback.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

const Boolean IWindowSessionCallbackStub::DEBUG = FALSE;
const String IWindowSessionCallbackStub::TAG("IWindowSessionCallbackStub");

IWindowSessionCallbackStub::IWindowSessionCallbackStub(
    /* [in] */ IIWindowSessionCallback* cb)
    : mCb(cb)
{}

void IWindowSessionCallbackStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IWindowSessionCallbackStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch (code) {
        default:
            assert(0);
    }
    return android::BBinder::onTransact(code, data, reply, flags);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
