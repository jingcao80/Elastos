
#include "CEditableInputConnection.h"

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

ECode CEditableInputConnection::constructor(
        /* [in] */ ITextView* textview)
{
    return EditableInputConnection::Init(textview);
}

PInterface CEditableInputConnection::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_BaseInputConnection) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CEditableInputConnection::Probe(riid);
}


ECode CEditableInputConnection::GetTextBeforeCursor(
    /* [in] */ Int32 n,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    AutoPtr<ICharSequence> cs = EditableInputConnection::GetTextBeforeCursor(n, flags);
    *text = cs.Get();
    REFCOUNT_ADD(*text);
    return NOERROR;
}


ECode CEditableInputConnection::GetTextAfterCursor(
    /* [in] */ Int32 n,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    AutoPtr<ICharSequence> cs = EditableInputConnection::GetTextAfterCursor(n, flags);
    *text = cs.Get();
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CEditableInputConnection::GetSelectedText(
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    AutoPtr<ICharSequence> cs = EditableInputConnection::GetSelectedText(flags);
    *text = cs.Get();
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CEditableInputConnection::GetCursorCapsMode(
    /* [in] */ Int32 reqModes,
    /* [out] */ Int32* capsMode)
{
    *capsMode = EditableInputConnection::GetCursorCapsMode(reqModes);
    return NOERROR;
}

ECode CEditableInputConnection::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [out] */ IExtractedText** extractedText)
{
    VALIDATE_NOT_NULL(extractedText);
    AutoPtr<IExtractedText> et = EditableInputConnection::GetExtractedText(request, flags);
    *extractedText = et.Get();
    REFCOUNT_ADD(*extractedText);
    return NOERROR;
}

ECode CEditableInputConnection::DeleteSurroundingText(
    /* [in] */ Int32 beforeLength,
    /* [in] */ Int32 afterLength,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::DeleteSurroundingText(beforeLength, afterLength);
    return NOERROR;
}

ECode CEditableInputConnection::SetComposingText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::SetComposingText(text, newCursorPosition);
    return NOERROR;
}

ECode CEditableInputConnection::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::SetComposingRegion(start, end);
    return NOERROR;
}

ECode CEditableInputConnection::FinishComposingText(
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::FinishComposingText();
    return NOERROR;
}

ECode CEditableInputConnection::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::CommitText(text, newCursorPosition);
    return NOERROR;
}

ECode CEditableInputConnection::CommitCompletion(
    /* [in] */ ICompletionInfo* text,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::CommitCompletion(text);
    return NOERROR;
}

ECode CEditableInputConnection::CommitCorrection(
    /* [in] */ ICorrectionInfo* correctionInfo,
    /* [out] */ Boolean* successed)
{
    *successed = EditableInputConnection::CommitCorrection(correctionInfo);
    return NOERROR;
}

ECode CEditableInputConnection::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::SetSelection(start, end);
    return NOERROR;
}

ECode CEditableInputConnection::PerformEditorAction(
    /* [in] */ Int32 editorAction,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::PerformEditorAction(editorAction);
    return NOERROR;
}

ECode CEditableInputConnection::PerformContextMenuAction(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::PerformContextMenuAction(id);
    return NOERROR;
}

ECode CEditableInputConnection::BeginBatchEdit(
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::BeginBatchEdit();
    return NOERROR;
}

ECode CEditableInputConnection::EndBatchEdit(
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::EndBatchEdit();
    return NOERROR;
}

ECode CEditableInputConnection::SendKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::SendKeyEvent(event);
    return NOERROR;
}

ECode CEditableInputConnection::ClearMetaKeyStates(
    /* [in] */ Int32 states,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::ClearMetaKeyStates(states);
    return NOERROR;
}

ECode CEditableInputConnection::ReportFullscreenMode(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::ReportFullscreenMode(enabled);
    return NOERROR;
}

ECode CEditableInputConnection::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data,
    /* [out] */ Boolean* flag)
{
    *flag = EditableInputConnection::PerformPrivateCommand(action, data);
    return NOERROR;
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
