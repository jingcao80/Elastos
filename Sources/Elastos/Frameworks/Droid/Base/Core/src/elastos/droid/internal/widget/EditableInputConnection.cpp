
#include "elastos/droid/internal/widget/EditableInputConnection.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Style::EIID_ISuggestionSpan;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::CExtractedText;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const String EditableInputConnection::TAG("EditableInputConnection");

CAR_INTERFACE_IMPL(EditableInputConnection, BaseInputConnection, IEditableInputConnection);

EditableInputConnection::EditableInputConnection()
   : mBatchEditNesting(0)
{}

EditableInputConnection::~EditableInputConnection()
{}

ECode EditableInputConnection::constructor(
    /* [in] */ ITextView* textview)
{
    FAIL_RETURN(BaseInputConnection::constructor(IView::Probe(textview), TRUE));
    mTextView = textview;
    return NOERROR;
}

ECode EditableInputConnection::GetEditable(
    /* [out] */ IEditable** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ITextView> tv = mTextView;
    if (tv != NULL) {
        AutoPtr<IEditable> e;
        tv->GetEditableText((IEditable**)&e);
        *result = e;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    return NOERROR;
}

ECode EditableInputConnection::BeginBatchEdit(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    {    AutoLock syncLock(mLock);
        if (mBatchEditNesting >= 0) {
            mTextView->BeginBatchEdit();
            mBatchEditNesting++;
            *res = TRUE;
            return NOERROR;
        }
    }
    *res = FALSE;
    return NOERROR;
}

ECode EditableInputConnection::EndBatchEdit(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    {    AutoLock syncLock(mLock);
        if (mBatchEditNesting > 0) {
            // When the connection is reset by the InputMethodManager and reportFinish
            // is called, some endBatchEdit calls may still be asynchronously received from the
            // IME. Do not take these into account, thus ensuring that this IC's final
            // contribution to mTextView's nested batch edit count is zero.
            mTextView->EndBatchEdit();
            mBatchEditNesting--;
            *res = TRUE;
            return NOERROR;
        }
    }
    *res = FALSE;
    return NOERROR;
}

ECode EditableInputConnection::ReportFinish()
{
    BaseInputConnection::ReportFinish();

    {    AutoLock syncLock(mLock);
        while (mBatchEditNesting > 0) {
            Boolean res;
            EndBatchEdit(&res);
        }
        // Will prevent any further calls to begin or endBatchEdit
        mBatchEditNesting = -1;
        return NOERROR;
    }
    return NOERROR;
}

ECode EditableInputConnection::ClearMetaKeyStates(
    /* [in] */ Int32 states,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    AutoPtr<IEditable> content;
    GetEditable((IEditable**)&content);
    if (content == NULL) {
        return NOERROR;
    }
    AutoPtr<IKeyListener> kl;
    mTextView->GetKeyListener((IKeyListener**)&kl);
    if (kl != NULL) {
        //try {
            kl->ClearMetaKeyState(IView::Probe(mTextView), content, states);
        //} catch (AbstractMethodError e) {
            // This is an old listener that doesn't implement the
            // new method.
        //}
    }
    *res = TRUE;
    return NOERROR;
}

ECode EditableInputConnection::CommitCompletion(
    /* [in] */ ICompletionInfo* text,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (DEBUG) {
        Logger::V(TAG, "commitCompletion, text: %p", text);
    }
    mTextView->BeginBatchEdit();
    mTextView->OnCommitCompletion(text);
    mTextView->EndBatchEdit();
    *res = TRUE;
    return NOERROR;
}

ECode EditableInputConnection::CommitCorrection(
    /* [in] */ ICorrectionInfo* correctionInfo,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (DEBUG) {
        Logger::V(TAG, "commitCorrection, correctionInfo: %p", correctionInfo);
    }
    mTextView->BeginBatchEdit();
    mTextView->OnCommitCorrection(correctionInfo);
    mTextView->EndBatchEdit();
    *res = TRUE;
    return NOERROR;
}

ECode EditableInputConnection::PerformEditorAction(
    /* [in] */ Int32 actionCode,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (DEBUG) {
        Logger::V(TAG, "performEditorAction, actionCode: %d", actionCode);
    }
    mTextView->OnEditorAction(actionCode);
    *res = TRUE;
    return NOERROR;
}

ECode EditableInputConnection::PerformContextMenuAction(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (DEBUG) {
        Logger::V(TAG, "performContextMenuAction, id: %d", id);
    }
    mTextView->BeginBatchEdit();
    Boolean result;
    mTextView->OnTextContextMenuItem(id, &result);
    mTextView->EndBatchEdit();
    *res = TRUE;
    return NOERROR;
}

ECode EditableInputConnection::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [out] */ IExtractedText** extractedText)
{
    VALIDATE_NOT_NULL(extractedText);
    *extractedText = NULL;

    if (mTextView != NULL) {
        AutoPtr<IExtractedText> et;
        CExtractedText::New((IExtractedText**)&et);

        Boolean res;
        mTextView->ExtractText(request, et, &res);
        if (res) {
            if ((flags & IInputConnection::GET_EXTRACTED_TEXT_MONITOR) != 0) {
                mTextView->SetExtracting(request);
            }
            *extractedText = et;
            REFCOUNT_ADD(*extractedText);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode EditableInputConnection::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean result;
    mTextView->OnPrivateIMECommand(action, data, &result);
    *res = TRUE;
    return NOERROR;
}

ECode EditableInputConnection::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (mTextView == NULL) {
        return BaseInputConnection::CommitText(text, newCursorPosition, res);
    }
    if (ISpanned::Probe(text)) {
        AutoPtr<ISpanned> spanned = ISpanned::Probe(text);
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
    Boolean success;
    BaseInputConnection::CommitText(text, newCursorPosition, &success);
    mTextView->HideErrorIfUnchanged();

    *res = success;
    return NOERROR;
}

ECode EditableInputConnection::RequestCursorUpdates(
    /* [in] */ Int32 cursorUpdateMode,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    if (DEBUG) {
        Logger::V(TAG, "requestUpdateCursorAnchorInfo: %d ", cursorUpdateMode);
    }

    // It is possible that any other bit is used as a valid flag in a future release.
    // We should reject the entire request in such a case.
    const Int32 KNOWN_FLAGS_MASK = IInputConnection::CURSOR_UPDATE_IMMEDIATE |
            IInputConnection::CURSOR_UPDATE_MONITOR;
    const Int32 unknownFlags = cursorUpdateMode & ~KNOWN_FLAGS_MASK;
    if (unknownFlags != 0) {
        if (DEBUG) {
            Logger::D(TAG, "Rejecting requestUpdateCursorAnchorInfo due to unknown flags.cursorUpdateMode= %d, unknownFlags= %d",
            cursorUpdateMode, unknownFlags);
        }
        return NOERROR;
    }

    if (mIMM == NULL) {
        // In this case, TYPE_CURSOR_ANCHOR_INFO is not handled.
        // TODO: Return some notification code rather than false to indicate method that
        // CursorAnchorInfo is temporarily unavailable.
        return NOERROR;
    }
    mIMM->SetUpdateCursorAnchorInfoMode(cursorUpdateMode);
    Boolean suc;
    if ((cursorUpdateMode & IInputConnection::CURSOR_UPDATE_IMMEDIATE) != 0) {
        if (mTextView == NULL) {
            // In this case, FLAG_CURSOR_ANCHOR_INFO_IMMEDIATE is silently ignored.
            // TODO: Return some notification code for the input method that indicates
            // FLAG_CURSOR_ANCHOR_INFO_IMMEDIATE is ignored.
        }
        else if (IView::Probe(mTextView)->IsInLayout(&suc), suc) {
            // In this case, the view hierarchy is currently undergoing a layout pass.
            // IMM#updateCursorAnchorInfo is supposed to be called soon after the layout
            // pass is finished.
        }
        else {
            // This will schedule a layout pass of the view tree, and the layout event
            // eventually triggers IMM#updateCursorAnchorInfo.
            IView::Probe(mTextView)->RequestLayout();
        }
    }
    *res = TRUE;
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
