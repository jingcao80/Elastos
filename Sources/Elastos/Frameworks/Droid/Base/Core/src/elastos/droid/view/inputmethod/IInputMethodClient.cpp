
#include "elastos/droid/view/inputmethod/IInputMethodClient.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::AndroidParcelUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

const Boolean IInputMethodClientStub::DEBUG = FALSE;
const String IInputMethodClientStub::TAG("IInputMethodClientStub");
const String IInputMethodClientStub::DESCRIPTOR("com.android.internal.view.IInputMethodClient");
const Int32 IInputMethodClientStub::TRANSACTION_onBindMethod = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 IInputMethodClientStub::TRANSACTION_onUnbindMethod = android::IBinder::FIRST_CALL_TRANSACTION + 2;
const Int32 IInputMethodClientStub::TRANSACTION_setActive = android::IBinder::FIRST_CALL_TRANSACTION + 3;
const Int32 IInputMethodClientStub::TRANSACTION_reportFullscreenMode = android::IBinder::FIRST_CALL_TRANSACTION + 5;

IInputMethodClientStub::IInputMethodClientStub(
    /* [in] */ IInputMethodClient* client)
    : mClient(client)
{}

void IInputMethodClientStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IInputMethodClientStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch (code) {
        case TRANSACTION_onBindMethod: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<IInputBindResult> br;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                br = AndroidParcelUtils::ReadInputBindResult(data);
            }
            mClient->OnBindMethod(br);
            return android::NO_ERROR;
        }

        case TRANSACTION_onUnbindMethod: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 sequence = AndroidParcelUtils::ReadInt32(data);
            Int32 unbindReason = AndroidParcelUtils::ReadInt32(data);
            mClient->OnUnbindMethod(sequence);
            return android::NO_ERROR;
        }

        case TRANSACTION_setActive: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Boolean active = AndroidParcelUtils::ReadInt32(data) != 0;
            Boolean fullscreen = AndroidParcelUtils::ReadInt32(data) != 0;
            mClient->SetActive(active);
            return android::NO_ERROR;
        }

        case TRANSACTION_reportFullscreenMode: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Boolean fullscreen = AndroidParcelUtils::ReadInt32(data) != 0;
            return android::NO_ERROR;
        }
        default:
            ALOGD("==== IInputMethodClient::onTransact, code: %d ====", code);
            assert(0);
    }
    return android::BBinder::onTransact(code, data, reply, flags);
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
