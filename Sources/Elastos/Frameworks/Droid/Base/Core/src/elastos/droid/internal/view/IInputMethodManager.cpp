
#include "elastos/droid/internal/view/IInputMethodManager.h"
#include "elastos/droid/internal/view/IInputConnectionWrapper.h"
#include "elastos/droid/os/AndroidParcelUtils.h"

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

const String IInputMethodManagerProxy::DESCRIPTOR("com.android.internal.view.IInputMethodManager");
const Int32 IInputMethodManagerProxy::TRANSACTION_finishInput = android::IBinder::FIRST_CALL_TRANSACTION + 7;
const Int32 IInputMethodManagerProxy::TRANSACTION_startInputOrWindowGainedFocus = android::IBinder::FIRST_CALL_TRANSACTION + 10;

CAR_INTERFACE_IMPL(IInputMethodManagerProxy, Object, IIInputMethodManager);

IInputMethodManagerProxy::IInputMethodManagerProxy(
    /* [in] */ android::sp<android::IBinder>& remote)
    : mRemote(remote)
{}

ECode IInputMethodManagerProxy::GetInputMethodList(
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::GetEnabledInputMethodList(
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::GetEnabledInputMethodSubtypeList(
    /* [in] */ const String& imiId,
    /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::GetLastInputMethodSubtype(
    /* [out] */ IInputMethodSubtype** subType)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::GetShortcutInputMethodsAndSubtypes(
    /* [out] */ IList** subtypes)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::AddClient(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::RemoveClient(
    /* [in] */ IInputMethodClient* client)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::FinishInput(
    /* [in] */ IInputMethodClient* client)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR));
    AndroidParcelUtils::WriteIInputMethodClient(data, client);
    mRemote->transact(TRANSACTION_finishInput, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IInputMethodManagerProxy::ShowSoftInput(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver,
    /* [out] */ Boolean* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::HideSoftInput(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver,
    /* [out] */ Boolean* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::StartInputOrWindowGainedFocus(
    /* [in] */ Int32 startInputReason,
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 controlFlags,
    /* [in] */ Int32 softInputMode,
    /* [in] */ Int32 windowFlags,
    /* [in] */ IEditorInfo* attribute,
    /* [in] */ IIInputContext* inputContext,
    /* [out] */ IInputBindResult** result)
{
    VALIDATE_NOT_NULL(result);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR));
    AndroidParcelUtils::WriteInt32(data, startInputReason);
    AndroidParcelUtils::WriteIInputMethodClient(data, client);

    AndroidParcelUtils::WriteIIWindow(data, IIWindow::Probe(windowToken));
    AndroidParcelUtils::WriteInt32(data, controlFlags);
    AndroidParcelUtils::WriteInt32(data, softInputMode);
    AndroidParcelUtils::WriteInt32(data, windowFlags);
    if (attribute != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteEditorInfo(data, attribute);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteIIInputContext(data, inputContext);
    Int32 missingMethodFlags = 0;
    AndroidParcelUtils::WriteInt32(data, 0);
    mRemote->transact(TRANSACTION_startInputOrWindowGainedFocus, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IInputBindResult> ibr = AndroidParcelUtils::ReadInputBindResult(reply);
        *result = ibr;
        REFCOUNT_ADD(*result);
    }
    else {
        *result = NULL;
    }

    return ec;
}

ECode IInputMethodManagerProxy::ShowInputMethodPickerFromClient(
    /* [in] */ IInputMethodClient* client)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::ShowInputMethodAndSubtypeEnablerFromClient(
    /* [in] */ IInputMethodClient* client,
    /* [in] */ const String& topId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::SetInputMethod(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::SetInputMethodAndSubtype(
    /* [in] */ IBinder* token,
    /* [in] */ const String& id,
    /* [in] */ IInputMethodSubtype* subtype)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::HideMySoftInput(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::ShowMySoftInput(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::UpdateStatusIcon(
    /* [in] */ IBinder* token,
    /* [in] */ const String& capsuleName,
    /* [in] */ Int32 iconId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::RegisterSuggestionSpansForNotification(
    /* [in] */ ArrayOf<ISuggestionSpan*>* spans)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::NotifySuggestionPicked(
    /* [in] */ ISuggestionSpan* span,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 index,
    /* [out] */ Boolean* picked)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::GetCurrentInputMethodSubtype(
    /* [out] */ IInputMethodSubtype** subType)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::SetCurrentInputMethodSubtype(
    /* [in] */ IInputMethodSubtype* subtype,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::SwitchToLastInputMethod(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::SwitchToNextInputMethod(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean onlyCurrentIme,
    /* [out] */ Boolean* switched)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::ShouldOfferSwitchingToNextInputMethod(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::SetInputMethodEnabled(
    /* [in] */ const String& id,
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::SetAdditionalInputMethodSubtypes(
    /* [in] */ const String& id,
    /* [in] */ ArrayOf<IInputMethodSubtype*>* subtypes)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::GetInputMethodWindowVisibleHeight(
    /* [out] */ Int32* height)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInputMethodManagerProxy::NotifyUserAction(
    /* [in] */ Int32 sequenceNumber)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
