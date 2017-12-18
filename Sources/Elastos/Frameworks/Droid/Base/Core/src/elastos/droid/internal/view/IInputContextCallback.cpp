
#include "elastos/droid/internal/view/IInputContextCallback.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <binder/Parcel.h>

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

const String IInputContextCallbackProxy::DESCRIPTOR("com.android.internal.view.IInputContextCallback");
const Int32 IInputContextCallbackProxy::TRANSACTION_setTextBeforeCursor = android::IBinder::FIRST_CALL_TRANSACTION + 0;
const Int32 IInputContextCallbackProxy::TRANSACTION_setTextAfterCursor = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 IInputContextCallbackProxy::TRANSACTION_setExtractedText = android::IBinder::FIRST_CALL_TRANSACTION + 3;
const Int32 IInputContextCallbackProxy::TRANSACTION_setSelectedText = android::IBinder::FIRST_CALL_TRANSACTION + 4;
const Int32 IInputContextCallbackProxy::TRANSACTION_setRequestUpdateCursorAnchorInfoResult = android::IBinder::FIRST_CALL_TRANSACTION + 5;

CAR_INTERFACE_IMPL(IInputContextCallbackProxy, Object, IIInputContextCallback);

IInputContextCallbackProxy::IInputContextCallbackProxy(
    /* [in] */ const android::sp<android::IBinder>& remote)
    : mRemote(remote)
{}

ECode IInputContextCallbackProxy::SetTextBeforeCursor(
    /* [in] */ ICharSequence* textBeforeCursor,
    /* [in] */ Int32 seq)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    if (textBeforeCursor != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::Write_CHAR_SEQUENCE(data, textBeforeCursor);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, seq);
    mRemote->transact(TRANSACTION_setTextBeforeCursor, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IInputContextCallbackProxy::SetTextAfterCursor(
    /* [in] */ ICharSequence* textAfterCursor,
    /* [in] */ Int32 seq)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    if (textAfterCursor != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::Write_CHAR_SEQUENCE(data, textAfterCursor);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, seq);
    mRemote->transact(TRANSACTION_setTextAfterCursor, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IInputContextCallbackProxy::SetCursorCapsMode(
    /* [in] */ Int32 capsMode,
    /* [in] */ Int32 seq)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputContextCallbackProxy::SetExtractedText(
    /* [in] */ IExtractedText* extractedText,
    /* [in] */ Int32 seq)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    if (extractedText != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteExtractedText(data, extractedText);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, seq);
    mRemote->transact(TRANSACTION_setExtractedText, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IInputContextCallbackProxy::SetSelectedText(
    /* [in] */ ICharSequence* selectedText,
    /* [in] */ Int32 seq)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    if (selectedText != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::Write_CHAR_SEQUENCE(data, selectedText);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, seq);
    mRemote->transact(TRANSACTION_setSelectedText, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IInputContextCallbackProxy::SetRequestUpdateCursorAnchorInfoResult(
    /* [in] */ Boolean result,
    /* [in] */ Int32 seq)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteInt32(data, result ? 1 : 0);
    AndroidParcelUtils::WriteInt32(data, seq);
    mRemote->transact(TRANSACTION_setRequestUpdateCursorAnchorInfoResult, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
