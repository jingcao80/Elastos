
#include "elastos/droid/view/IWindow.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <binder/Parcel.h>

using Elastos::Droid::Os::AndroidParcelUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

const Boolean IWindowStub::DEBUG = FALSE;
const String IWindowStub::TAG("IWindowStub");
const String IWindowStub::DESCRIPTOR("android.view.IWindow");
const Int32 IWindowStub::TRANSACTION_resized = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 IWindowStub::TRANSACTION_moved = android::IBinder::FIRST_CALL_TRANSACTION + 2;
const Int32 IWindowStub::TRANSACTION_dispatchAppVisibility = android::IBinder::FIRST_CALL_TRANSACTION + 3;
const Int32 IWindowStub::TRANSACTION_windowFocusChanged = android::IBinder::FIRST_CALL_TRANSACTION + 5;
const Int32 IWindowStub::TRANSACTION_closeSystemDialogs = android::IBinder::FIRST_CALL_TRANSACTION + 6;

IWindowStub::IWindowStub(
    /* [in] */ IIWindow* window)
    : mWindow(window)
{}

void IWindowStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IWindowStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch(code) {
        case TRANSACTION_resized: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<IRect> frame;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                frame = AndroidParcelUtils::ReadRect(data);
            }
            AutoPtr<IRect> overscanInsets;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                overscanInsets = AndroidParcelUtils::ReadRect(data);
            }
            AutoPtr<IRect> contentInsets;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                contentInsets = AndroidParcelUtils::ReadRect(data);
            }
            AutoPtr<IRect> visibleInsets;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                visibleInsets = AndroidParcelUtils::ReadRect(data);
            }
            AutoPtr<IRect> stableInsets;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                stableInsets = AndroidParcelUtils::ReadRect(data);
            }
            AutoPtr<IRect> outsets;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                outsets = AndroidParcelUtils::ReadRect(data);
            }
            Boolean reportDraw = AndroidParcelUtils::ReadInt32(data) != 0;
            AutoPtr<IConfiguration> newConfig;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                String klass = AndroidParcelUtils::ReadString(data);
                AutoPtr<IConfiguration> globalConfig = AndroidParcelUtils::ReadConfiguration(data);
                klass = AndroidParcelUtils::ReadString(data);
                AutoPtr<IConfiguration> overrideConfig = AndroidParcelUtils::ReadConfiguration(data);
                klass = AndroidParcelUtils::ReadString(data);
                AutoPtr<IConfiguration> mergedConfig = AndroidParcelUtils::ReadConfiguration(data);
                newConfig = globalConfig;
            }
            AutoPtr<IRect> backDropFrame;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                backDropFrame = AndroidParcelUtils::ReadRect(data);
            }
            Boolean forceLayout = AndroidParcelUtils::ReadInt32(data) != 0;
            Boolean alwaysConsumeNavBar = AndroidParcelUtils::ReadInt32(data) != 0;
            Int32 displayId = AndroidParcelUtils::ReadInt32(data);
            mWindow->Resized(frame, overscanInsets, contentInsets,
                    visibleInsets, stableInsets, reportDraw, newConfig);
            return android::NO_ERROR;
        }

        case TRANSACTION_moved: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 newX = AndroidParcelUtils::ReadInt32(data);
            Int32 newY = AndroidParcelUtils::ReadInt32(data);
            mWindow->Moved(newX, newY);
            return android::NO_ERROR;
        }

        case TRANSACTION_dispatchAppVisibility: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Boolean visible = AndroidParcelUtils::ReadInt32(data) != 0;
            mWindow->DispatchAppVisibility(visible);
            return android::NO_ERROR;
        }

        case TRANSACTION_windowFocusChanged: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Boolean hasFocus = AndroidParcelUtils::ReadInt32(data) != 0;
            Boolean inTouchMode = AndroidParcelUtils::ReadInt32(data) != 0;
            mWindow->WindowFocusChanged(hasFocus, inTouchMode);
            return android::NO_ERROR;
        }

        case TRANSACTION_closeSystemDialogs: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            String reason = AndroidParcelUtils::ReadString(data);
            mWindow->CloseSystemDialogs(reason);
            return android::NO_ERROR;
        }

        default:
            ALOGD("==== IWindow::onTransact, code: %d ====", code);
            assert(0);
    }
    return android::BBinder::onTransact(code, data, reply, flags);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
