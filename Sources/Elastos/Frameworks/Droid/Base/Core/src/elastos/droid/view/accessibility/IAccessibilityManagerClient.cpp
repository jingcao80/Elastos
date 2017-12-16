
#include "elastos/droid/view/accessibility/IAccessibilityManagerClient.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const Boolean IAccessibilityManagerClientStub::DEBUG = FALSE;
const String IAccessibilityManagerClientStub::TAG("IAccessibilityManagerClientStub");

IAccessibilityManagerClientStub::IAccessibilityManagerClientStub(
    /* [in] */ IIAccessibilityManagerClient* client)
    : mClient(client)
{}

void IAccessibilityManagerClientStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IAccessibilityManagerClientStub::onTransact(
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

} // namespace Accessibility
} // namespace View
} // namespace Droid
} // namespace Elastos
