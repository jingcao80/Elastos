
#include "elastos/droid/view/inputmethod/IInputContext.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

const Boolean IInputContextStub::DEBUG = FALSE;
const String IInputContextStub::TAG("IInputContextStub");
const String IInputContextStub::DESCRIPTOR("com.android.internal.view.IInputContext");
const Int32 IInputContextStub::TRANSACTION_finishComposingText = android::IBinder::FIRST_CALL_TRANSACTION + 7;

IInputContextStub::IInputContextStub(
    /* [in] */ IIInputContext* context)
    : mContext(context)
{}

void IInputContextStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IInputContextStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch (code) {
        case TRANSACTION_finishComposingText: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            mContext->FinishComposingText();
            return android::NO_ERROR;
        }

        default:
            assert(0);
    }
    return android::BBinder::onTransact(code, data, reply, flags);
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
