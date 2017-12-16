
#include "elastos/droid/internal/view/IInputMethodSession.h"
#include "elastos/droid/os/AndroidParcelUtils.h"

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

const String IInputMethodSessionProxy::DESCRIPTOR("com.android.internal.view.IInputMethodSession");
const Int32 IInputMethodSessionProxy::TRANSACTION_updateSelection = android::IBinder::FIRST_CALL_TRANSACTION + 2;

CAR_INTERFACE_IMPL(IInputMethodSessionProxy, Object, IIInputMethodSession);

IInputMethodSessionProxy::IInputMethodSessionProxy(
    /* [in] */ const android::sp<android::IBinder>& remote)
    : mRemote(remote)
{}

ECode IInputMethodSessionProxy::FinishInput()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodSessionProxy::UpdateExtractedText(
    /* [in] */ Int32 token,
    /* [in] */ IExtractedText* text)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodSessionProxy::UpdateSelection(
    /* [in] */ Int32 oldSelStart,
    /* [in] */ Int32 oldSelEnd,
    /* [in] */ Int32 newSelStart,
    /* [in] */ Int32 newSelEnd,
    /* [in] */ Int32 candidatesStart,
    /* [in] */ Int32 candidatesEnd)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteInt32(data, oldSelStart);
    AndroidParcelUtils::WriteInt32(data, oldSelEnd);
    AndroidParcelUtils::WriteInt32(data, newSelStart);
    AndroidParcelUtils::WriteInt32(data, newSelEnd);
    AndroidParcelUtils::WriteInt32(data, candidatesStart);
    AndroidParcelUtils::WriteInt32(data, candidatesEnd);
    mRemote->transact(TRANSACTION_updateSelection, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IInputMethodSessionProxy::ViewClicked(
    /* [in] */ Boolean focusChanged)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodSessionProxy::UpdateCursor(
    /* [in] */ IRect* newCursor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodSessionProxy::DisplayCompletions(
    /* [in] */ ArrayOf<ICompletionInfo*>* completions)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodSessionProxy::AppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodSessionProxy::ToggleSoftInput(
    /* [in] */ Int32 showFlags,
    /* [in] */ Int32 hideFlags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodSessionProxy::FinishSession()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodSessionProxy::UpdateCursorAnchorInfo(
    /* [in] */ ICursorAnchorInfo* cursorAnchorInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
