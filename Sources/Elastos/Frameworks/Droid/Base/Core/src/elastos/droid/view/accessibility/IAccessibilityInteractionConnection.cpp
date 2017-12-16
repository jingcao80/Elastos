
#include "elastos/droid/view/accessibility/IAccessibilityInteractionConnection.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const Boolean IAccessibilityInteractionConnectionStub::DEBUG = FALSE;
const String IAccessibilityInteractionConnectionStub::TAG("IAccessibilityInteractionConnectionStub");

IAccessibilityInteractionConnectionStub::IAccessibilityInteractionConnectionStub(
    /* [in] */ IIAccessibilityInteractionConnection* connection)
    : mConnection(connection)
{}

void IAccessibilityInteractionConnectionStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IAccessibilityInteractionConnectionStub::onTransact(
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
} // namespace Accessibility
