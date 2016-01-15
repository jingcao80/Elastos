
#include "elastos/droid/internal/EditableInputConnection.h"

using Elastos::Droid::Text::Method::IKeyListener;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

const String EditableInputConnection::TAG("EditableInputConnection");

EditableInputConnection::EditableInputConnection()
   : mBatchEditNesting(0)
{}

EditableInputConnection::EditableInputConnection(
    /* [in] */ ITextView* textview)
   : BaseInputConnection(textview, TRUE)
   , mTextView(textview)
   , mBatchEditNesting(0)
{}

ECode EditableInputConnection::Init(
    /* [in] */ ITextView* textview)
{
    FAIL_RETURN(BaseInputConnection::Init(textview, TRUE));
    mTextView = textview;
    return NOERROR;
}

AutoPtr<IEditable> EditableInputConnection::GetEditable()
{
    AutoPtr<ITextView> tv = mTextView;
    if (tv != NULL) {
        AutoPtr<IEditable> e;
        tv->GetEditableText((IEditable**)&e);
        return e;
    }
    return NULL;
}

Boolean EditableInputConnection::BeginBatchEdit()
{
    AutoLock lock(mLock);
    if (mBatchEditNesting >= 0) {
        mTextView->BeginBatchEdit();
        mBatchEditNesting++;
        return TRUE;
    }
    return FALSE;
}

Boolean EditableInputConnection::EndBatchEdit()
{
    AutoLock lock(mLock);
    if (mBatchEditNesting > 0) {
        // When the connection is reset by the InputMethodManager and reportFinish
        // is called, some endBatchEdit calls may still be asynchronously received from the
        // IME. Do not take these into account, thus ensuring that this IC's final
        // contribution to mTextView's nested batch edit count is zero.
        mTextView->EndBatchEdit();
        mBatchEditNesting--;
        return TRUE;
    }
    return FALSE;
}

ECode EditableInputConnection::ReportFinish()
{
    BaseInputConnection::ReportFinish();

    AutoLock lock(mLock);
    while (mBatchEditNesting > 0) {
        EndBatchEdit();
    }
    // Will prevent any further calls to begin or endBatchEdit
    mBatchEditNesting = -1;
    return NOERROR;
}

Boolean EditableInputConnection::ClearMetaKeyStates(
    /* [in] */ Int32 states)
{
    AutoPtr<IEditable> content = GetEditable();
    if (content == NULL) return FALSE;
    AutoPtr<IKeyListener> kl;
    mTextView->GetKeyListener((IKeyListener**)&kl);
    if (kl != NULL) {
        //try {
            kl->ClearMetaKeyState(mTextView, content, states);
        //} catch (AbstractMethodError e) {
            // This is an old listener that doesn't implement the
            // new method.
        //}
    }
    return TRUE;
}

Boolean EditableInputConnection::CommitCompletion(
    /* [in] */ ICompletionInfo* text)
{
    //if (DEBUG) Log.v(TAG, "commitCompletion " + text);
    mTextView->BeginBatchEdit();
    mTextView->OnCommitCompletion(text);
    mTextView->EndBatchEdit();
    return TRUE;
}

/**
 * Calls the {@link TextView#onCommitCorrection} method of the associated TextView.
 */
Boolean EditableInputConnection::CommitCorrection(
    /* [in] */ ICorrectionInfo* correctionInfo)
{
    //if (DEBUG) Log.v(TAG, "commitCorrection" + correctionInfo);
    mTextView->BeginBatchEdit();
    mTextView->OnCommitCorrection(correctionInfo);
    mTextView->EndBatchEdit();
    return TRUE;
}

Boolean EditableInputConnection::PerformEditorAction(
    /* [in] */ Int32 actionCode)
{
    //if (DEBUG) Log.v(TAG, "performEditorAction " + actionCode);
    mTextView->OnEditorAction(actionCode);
    return TRUE;
}

Boolean EditableInputConnection::PerformContextMenuAction(
    /* [in] */ Int32 id)
{
    //if (DEBUG) Log.v(TAG, "performContextMenuAction " + id);
    mTextView->BeginBatchEdit();
    Boolean result;
    mTextView->OnTextContextMenuItem(id, &result);
    mTextView->EndBatchEdit();
    return TRUE;
}

AutoPtr<IExtractedText> EditableInputConnection::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags)
{
    if (mTextView != NULL) {
        AutoPtr<IExtractedText> et;
        CExtractedText::New((IExtractedText**)&et);

        Boolean res;
        mTextView->ExtractText(request, et, &res);
        if (res) {
            if ((flags & IInputConnection::GET_EXTRACTED_TEXT_MONITOR) != 0) {
                mTextView->SetExtracting(request);
            }
            return et;
        }
    }
    return NULL;
}

Boolean EditableInputConnection::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    Boolean result;
    mTextView->OnPrivateIMECommand(action, data, &result);
    return TRUE;
}

Boolean EditableInputConnection::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition)
{
    if (mTextView == NULL) {
        return BaseInputConnection::CommitText(text, newCursorPosition);
    }
    if (text->Probe(EIID_ISpanned)) {
        AutoPtr<ISpanned> spanned = ((ISpanned*) text->Probe(EIID_ISpanned));
        Int32 length;
        text->GetLength(&length);
        AutoPtr<ArrayOf<IInterface*> > temp;
        spanned->GetSpans(0, length, EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&temp);
        AutoPtr<ArrayOf<ISuggestionSpan*> > spans = ArrayOf<ISuggestionSpan*>::Alloc(temp->GetLength());
        for (Int32 i = 0; i < temp->GetLength(); i++) {
            spans->Set(i, ISuggestionSpan::Probe((*temp)[i]));
        }

        mIMM->RegisterSuggestionSpansForNotification(spans);
    }

    mTextView->ResetErrorChangedFlag();
    Boolean success = BaseInputConnection::CommitText(text, newCursorPosition);
    mTextView->HideErrorIfUnchanged();

    return success;
}
}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
