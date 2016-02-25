
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/inputmethod/CControlledInputConnectionWrapper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::View::EIID_IIInputContext;
using Elastos::Droid::Os::EIID_IBinder;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CControlledInputConnectionWrapper::
//========================================================================================
CAR_INTERFACE_IMPL_2(CControlledInputConnectionWrapper, Object, IIInputContext, IBinder)

CAR_OBJECT_IMPL(CControlledInputConnectionWrapper)

CControlledInputConnectionWrapper::CControlledInputConnectionWrapper()
    : mActive(FALSE)
{
}

ECode CControlledInputConnectionWrapper::constructor(
    /* [in] */ ILooper* mainLooper,
    /* [in] */ IInputConnection* conn,
    /* [in] */ IInputMethodManager* agmer)
{
    assert(0 && "TODO");
//    IInputConnectionWrapper::Init(mainLooper, conn);
    mParentInputMethodManager = (CInputMethodManager*)agmer;
    mActive = TRUE;
    return NOERROR;
}

ECode CControlledInputConnectionWrapper::GetTextAfterCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::GetTextAfterCursor(length, flags, seq, callback);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::GetTextBeforeCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::GetTextBeforeCursor(length, flags, seq, callback);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::GetSelectedText(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::GetSelectedText(flags, seq, callback);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::GetCursorCapsMode(
    /* [in] */ Int32 reqModes,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::GetCursorCapsMode(reqModes, seq, callback);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::GetExtractedText(request, flags, seq, callback);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::CommitText(text, newCursorPosition);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::CommitCompletion(
    /* [in] */ ICompletionInfo* text)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::CommitCompletion(text);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::CommitCorrection(
    /* [in] */ ICorrectionInfo* info)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::CommitCorrection(info);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::SetSelection(start, end);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::PerformEditorAction(
    /* [in] */ Int32 id)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::PerformEditorAction(id);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::PerformContextMenuAction(
    /* [in] */ Int32 id)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::PerformContextMenuAction(id);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::SetComposingRegion(start, end);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::SetComposingText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::SetComposingText(text, newCursorPosition);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::FinishComposingText()
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::FinishComposingText();
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::SendKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::SendKeyEvent(event);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::ClearMetaKeyStates(
    /* [in] */ Int32 states)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::ClearMetaKeyStates(states);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::DeleteSurroundingText(
    /* [in] */ Int32 leftLength,
    /* [in] */ Int32 rightLength)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::DeleteSurroundingText(leftLength, rightLength);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::BeginBatchEdit()
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::BeginBatchEdit();
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::EndBatchEdit()
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::EndBatchEdit();
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::ReportFullscreenMode(
    /* [in] */ Boolean enabled)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::ReportFullscreenMode(enabled);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::PerformPrivateCommand(action, data);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::GetDescription(
    /* [out] */ String* str)
{
    assert(0 && "TODO");
//    return IInputConnectionWrapper::GetDescription(str);
    return E_NOT_IMPLEMENTED;
}

ECode CControlledInputConnectionWrapper::RequestUpdateCursorAnchorInfo(
    /* [in] */ Int32 cursorUpdateMode,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* cb)
{
    assert(0 && "TODO");
    return E_NOT_IMPLEMENTED;
}

Boolean CControlledInputConnectionWrapper::IsActive()
{
    return mParentInputMethodManager->mActive && mActive;
}

void CControlledInputConnectionWrapper::Deactivate()
{
    mActive = FALSE;
}

ECode CControlledInputConnectionWrapper::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
