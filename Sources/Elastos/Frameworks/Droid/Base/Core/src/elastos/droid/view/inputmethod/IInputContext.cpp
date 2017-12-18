
#include "elastos/droid/view/inputmethod/IInputContext.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::AndroidParcelUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

const Boolean IInputContextStub::DEBUG = FALSE;
const String IInputContextStub::TAG("IInputContextStub");
const String IInputContextStub::DESCRIPTOR("com.android.internal.view.IInputContext");
const Int32 IInputContextStub::TRANSACTION_getTextBeforeCursor = android::IBinder::FIRST_CALL_TRANSACTION + 0;
const Int32 IInputContextStub::TRANSACTION_getTextAfterCursor = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 IInputContextStub::TRANSACTION_getExtractedText = android::IBinder::FIRST_CALL_TRANSACTION + 3;
const Int32 IInputContextStub::TRANSACTION_setComposingText = android::IBinder::FIRST_CALL_TRANSACTION + 6;
const Int32 IInputContextStub::TRANSACTION_finishComposingText = android::IBinder::FIRST_CALL_TRANSACTION + 7;
const Int32 IInputContextStub::TRANSACTION_commitText = android::IBinder::FIRST_CALL_TRANSACTION + 8;
const Int32 IInputContextStub::TRANSACTION_beginBatchEdit = android::IBinder::FIRST_CALL_TRANSACTION + 14;
const Int32 IInputContextStub::TRANSACTION_endBatchEdit = android::IBinder::FIRST_CALL_TRANSACTION + 15;
const Int32 IInputContextStub::TRANSACTION_setComposingRegion = android::IBinder::FIRST_CALL_TRANSACTION + 19;
const Int32 IInputContextStub::TRANSACTION_getSelectedText = android::IBinder::FIRST_CALL_TRANSACTION + 20;
const Int32 IInputContextStub::TRANSACTION_requestUpdateCursorAnchorInfo = android::IBinder::FIRST_CALL_TRANSACTION + 21;

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
        case TRANSACTION_getTextBeforeCursor: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 length = AndroidParcelUtils::ReadInt32(data);
            Int32 flags = AndroidParcelUtils::ReadInt32(data);
            Int32 seq = AndroidParcelUtils::ReadInt32(data);
            AutoPtr<IIInputContextCallback> cb =
                    AndroidParcelUtils::ReadIInputContextCallback(data);
            mContext->GetTextBeforeCursor(length, flags, seq, cb);
            return android::NO_ERROR;
        }

        case TRANSACTION_getTextAfterCursor: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 length = AndroidParcelUtils::ReadInt32(data);
            Int32 flags = AndroidParcelUtils::ReadInt32(data);
            Int32 seq = AndroidParcelUtils::ReadInt32(data);
            AutoPtr<IIInputContextCallback> cb =
                    AndroidParcelUtils::ReadIInputContextCallback(data);
            mContext->GetTextAfterCursor(length, flags, seq, cb);
            return android::NO_ERROR;
        }

        case TRANSACTION_getExtractedText: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<IExtractedTextRequest> request;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                request = AndroidParcelUtils::ReadExtractedTextRequest(data);
            }
            Int32 flags = AndroidParcelUtils::ReadInt32(data);
            Int32 seq = AndroidParcelUtils::ReadInt32(data);
            AutoPtr<IIInputContextCallback> cb =
                    AndroidParcelUtils::ReadIInputContextCallback(data);
            mContext->GetExtractedText(request, flags, seq, cb);
            return android::NO_ERROR;
        }

        case TRANSACTION_setComposingText: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<ICharSequence> text;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                text = AndroidParcelUtils::Read_CHAR_SEQUENCE(data);
            }
            Int32 newCursorPosition = AndroidParcelUtils::ReadInt32(data);
            mContext->SetComposingText(text, newCursorPosition);
            return android::NO_ERROR;
        }

        case TRANSACTION_finishComposingText: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            mContext->FinishComposingText();
            return android::NO_ERROR;
        }

        case TRANSACTION_commitText: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            AutoPtr<ICharSequence> text;
            if (AndroidParcelUtils::ReadInt32(data) != 0) {
                text = AndroidParcelUtils::Read_CHAR_SEQUENCE(data);
            }
            Int32 newCursorPosition = AndroidParcelUtils::ReadInt32(data);
            mContext->CommitText(text, newCursorPosition);
            return android::NO_ERROR;
        }

        case TRANSACTION_beginBatchEdit: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            mContext->BeginBatchEdit();
            return android::NO_ERROR;
        }

        case TRANSACTION_endBatchEdit: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            mContext->EndBatchEdit();
            return android::NO_ERROR;
        }

        case TRANSACTION_setComposingRegion: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 start = AndroidParcelUtils::ReadInt32(data);
            Int32 end = AndroidParcelUtils::ReadInt32(data);
            mContext->SetComposingRegion(start, end);
            return android::NO_ERROR;
        }

        case TRANSACTION_getSelectedText: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 flags = AndroidParcelUtils::ReadInt32(data);
            Int32 seq = AndroidParcelUtils::ReadInt32(data);
            AutoPtr<IIInputContextCallback> cb =
                    AndroidParcelUtils::ReadIInputContextCallback(data);
            mContext->GetSelectedText(flags, seq, cb);
            return android::NO_ERROR;
        }

        case TRANSACTION_requestUpdateCursorAnchorInfo: {
            data.enforceInterface(android::String16(DESCRIPTOR.string()));
            Int32 cursorUpdateMode = AndroidParcelUtils::ReadInt32(data);
            Int32 seq = AndroidParcelUtils::ReadInt32(data);
            AutoPtr<IIInputContextCallback> cb =
                    AndroidParcelUtils::ReadIInputContextCallback(data);
            mContext->RequestUpdateCursorAnchorInfo(cursorUpdateMode, seq, cb);
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
