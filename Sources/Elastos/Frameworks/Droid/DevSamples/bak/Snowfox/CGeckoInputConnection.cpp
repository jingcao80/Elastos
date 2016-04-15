
#include "GeckoApp.h"
#include "GeckoAppShell.h"
#include <elastos/coredef.h>
#include "CGeckoInputConnection.h"
#include "GeckoEvent.h"
#include <elastos/core/Math.h>
#include <stdlib.h>
#include <stdio.h>

using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::Style::ICharacterStyle;
using Elastos::Droid::Text::Style::EIID_ICharacterStyle;
using Elastos::Droid::Text::Style::IUnderlineSpan;
using Elastos::Droid::Text::Style::IForegroundColorSpan;
using Elastos::Droid::Text::Style::IBackgroundColorSpan;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CExtractedText;
using Elastos::Droid::View::InputMethod::IBaseInputConnection;
using Elastos::Droid::View::InputMethod::CBaseInputConnection;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

CGeckoInputConnection::ChangeNotification::ChangeNotification(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 oldEnd,
    /* [in] */ Int32 newEnd)
{
    DBG_LOG("\t + ChangeNotification::ChangeNotification()");
    mText   = text;
    mStart  = start;
    mEnd    = oldEnd;
    mNewEnd = newEnd;
    DBG_LOG("\t - ChangeNotification::ChangeNotification()");
}

CGeckoInputConnection::ChangeNotification::ChangeNotification(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    DBG_LOG("\t + ChangeNotification::ChangeNotification()");
    mText   = NULL;
    mStart  = start;
    mEnd    = end;
    mNewEnd = 0;
    DBG_LOG("\t - ChangeNotification::ChangeNotification()");
}

CGeckoInputConnection::CGeckoInputConnection()
    : mComposing(FALSE)
    , mComposingText("")
    , mCompositionStart(0)
    , mCompositionSelStart(0)
    , mCompositionSelLen(0)
    , mSelectionStart(0)
    , mSelectionLength(0)
{
/*    CCopyOnWriteArrayList< AutoPtr< ChangeNotification> >::New(
        (ICopyOnWriteArrayList< AutoPtr< ChangeNotification> >**)&mBatchChanges);*/
    DBG_LOG("\t ChangeNotification::CGeckoInputConnection()");
}

CGeckoInputConnection::~CGeckoInputConnection()
{
    DBG_LOG("\t + ChangeNotification::~CGeckoInputConnection()");
    pthread_cond_destroy(&mQueryResult);
    DBG_LOG("\t - ChangeNotification::~CGeckoInputConnection()");
}

ECode CGeckoInputConnection::GetTextBeforeCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence ** ppText)
{
    return GetTextAfterCursor(-length, flags, ppText);
}

ECode CGeckoInputConnection::GetTextAfterCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence ** ppText)
{
    DBG_LOG("\t + ChangeNotification::GetTextAfterCursor()");
    if (NULL == ppText) return E_INVALID_ARGUMENT;
    GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_GET_SELECTION, 0, 0));
    pthread_cond_wait(&mQueryResult, &mQueryMutex);
    //try {
    //      mQueryResult->take();
    //} catch (InterruptedException e) {
    //    Log.e("GeckoAppJava", "IME: getTextBefore/AfterCursor interrupted", e);
    //    return null;
    //}

    /* Compatible with both positive and negative length
        (no need for separate code for getTextBeforeCursor) */
    int textStart = mSelectionStart;
    int textLength = length;

    if (length < 0) {
      textStart += length;
      textLength = -length;
      if (textStart < 0) {
        textStart = 0;
        textLength = mSelectionStart;
      }
    }

    GeckoAppShell::SendEventToGecko(
        new GeckoEvent(GeckoEvent::IME_GET_TEXT, textStart, textLength));
    pthread_cond_wait(&mQueryResult, &mQueryMutex);
    //try {
        //*ppText = mQueryResult.take();
    //} catch (InterruptedException e) {
        //Log.e("GeckoAppJava", "IME: getTextBefore/AfterCursor: Interrupted!", e);
        //return null;
    //}

    DBG_LOG("\t - ChangeNotification::GetTextAfterCursor()");
    return NOERROR;
}

ECode CGeckoInputConnection::GetSelectedText(
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence ** ppText)
{
    DBG_LOG("\t + ChangeNotification::GetSelectedText()");
    return mBaseInputConnection->GetSelectedText(flags, ppText);
}

ECode CGeckoInputConnection::GetCursorCapsMode(
    /* [in] */ Int32 reqModes,
    /* [out] */ Int32 * pCapsMode)
{
    DBG_LOG("\t + ChangeNotification::GetCursorCapsMode()");
    if (NULL == pCapsMode) return E_INVALID_ARGUMENT;
    *pCapsMode = 0;
    return NOERROR;
}

ECode CGeckoInputConnection::GetExtractedText(
    /* [in] */ IExtractedTextRequest * pRequest,
    /* [in] */ Int32 flags,
    /* [out] */ IExtractedText ** ppExtractedText)
{
    DBG_LOG("\t + ChangeNotification::GetExtractedText()");
    if (pRequest == NULL || ppExtractedText == NULL)
        return E_INVALID_ARGUMENT;

    // Bail out here if gecko isn't running, otherwise we deadlock
    // below when waiting for the reply to IME_GET_SELECTION.
    if (!GeckoApp::CheckLaunchState(GeckoApp::LaunchState_GeckoRunning))
    {
        *ppExtractedText = NULL;
        return NOERROR;
    }

    AutoPtr<IExtractedText> extract;
//    ECode ec = CExtractedText::New((IExtractedText**)&extract);
//    if (FAILED(ec)) return ec;
    //extract->mFlags = 0;
    //extract->mPartialStartOffset = -1;
    //extract->mPartialEndOffset = -1;

    GeckoAppShell::SendEventToGecko(
        new GeckoEvent(GeckoEvent::IME_GET_SELECTION, 0, 0));
    pthread_cond_wait(&mQueryResult, &mQueryMutex);
    //try {
     //   mQueryResult->Take();
    //} catch (InterruptedException e) {
    //    Log.e("GeckoAppJava", "IME: getExtractedText interrupted", e);
    //    return null;
    //}
    //extract->mSelectionStart = mSelectionStart;
    //extract->mSelectionEnd = mSelectionStart + mSelectionLength;

    // bug 617298 - IME_GET_TEXT sometimes gives the wrong result due to
    // a stale cache. Use a set of three workarounds:
    // 1. Sleep for 20 milliseconds and hope the child updates us with the new text.
    //    Very evil and, consequentially, most effective.
    //try {
        Thread::Sleep(20);
    //} catch (InterruptedException e) {}
    GeckoAppShell::SendEventToGecko(
        new GeckoEvent(GeckoEvent::IME_GET_TEXT, 0, Elastos::Core::Math::INT32_MAX_VALUE));
    //try {
        extract->SetStartOffset(0);
        pthread_cond_wait(&mQueryResult, &mQueryMutex);
    //    extract->mText = mQueryResult.Take();

        // 2. Make a guess about what the text actually is
    //    if (mComposing && extract->mSelectionEnd > extract->mText->GetLength())
    //        extract->mText = extract->mText->SubSequence(0, Math::Min(extract->mText->GetLength(), mCompositionStart)) + mComposingText;

        // 3. If all else fails, make sure our selection indexes make sense
    //    extract->mSelectionStart = Math::Min(extract->mSelectionStart, extract->mText.GetLength());
    //    extract->mSelectionEnd = Math::Min(extract->mSelectionEnd, extract->mText->GetLength());

        if ((flags & IInputConnection::GET_EXTRACTED_TEXT_MONITOR) != 0) {
            mUpdateRequest = pRequest;
        }
        *ppExtractedText = extract;
        extract->AddRef();

    //} catch (InterruptedException e) {
    //    Log.e("GeckoAppJava", "IME: getExtractedText interrupted", e);
    //    return null;
    //}

    DBG_LOG("\t - ChangeNotification::GetExtractedText()");
    return NOERROR;
}

ECode CGeckoInputConnection::GetEditable(
    /* [out] */ IEditable** ppE)
{
    DBG_LOG("\t + ChangeNotification::GetEditable()");
    if (ppE) *ppE = NULL;
    return NOERROR;
}

ECode CGeckoInputConnection::DeleteSurroundingText(
    /* [in] */ Int32 leftLength,
    /* [in] */ Int32 rightLength,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::DeleteSurroundingText()");
    /* deleteSurroundingText is supposed to ignore the composing text,
    so we cancel any pending composition, delete the text, and then
    restart the composition */
    if (leftLength == 0 && rightLength == 0) {
        *pFlag = TRUE;
        return NOERROR;
    }

    if (mComposing) {
        String s;
        // Cancel current composition
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(0, 0, 0, 0, 0, 0, s));
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_COMPOSITION_END, 0, 0));
    }

    // Select text to be deleted
    Int32 delStart, delLen;
    GeckoAppShell::SendEventToGecko(
        new GeckoEvent(GeckoEvent::IME_GET_SELECTION, 0, 0));
    pthread_cond_wait(&mQueryResult, &mQueryMutex);
    //try {
    //    mQueryResult->take();
    //} catch (InterruptedException e) {
    //    Log.e("GeckoAppJava", "IME: deleteSurroundingText interrupted", e);
    //    return false;
    //}
    delStart = mSelectionStart > leftLength ?
                mSelectionStart - leftLength : 0;
    delLen = mSelectionStart + rightLength - delStart;
    GeckoAppShell::SendEventToGecko(
        new GeckoEvent(GeckoEvent::IME_SET_SELECTION, delStart, delLen));

    // Restore composition / delete text
    if (mComposing) {
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_COMPOSITION_BEGIN, 0, 0));
        if (!mComposingText.IsNullOrEmpty()) {
            /* IME_SET_TEXT doesn't work well with empty strings */
            GeckoAppShell::SendEventToGecko(
                new GeckoEvent(
                   0,
                   mComposingText.GetLength(),
                   GeckoEvent::IME_RANGE_RAWINPUT,
                   GeckoEvent::IME_RANGE_UNDERLINE,
                   0,
                   0,
                   mComposingText));
        }
    } else {
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_DELETE_TEXT, 0, 0));
    }

    if (pFlag) *pFlag = TRUE;
    DBG_LOG("\t - ChangeNotification::DeleteSurroundingText()");
    return NOERROR;
}

ECode CGeckoInputConnection::SetComposingText(
    /* [in] */ ICharSequence * text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::SetComposingText()");
    if (NULL == pFlag) return E_INVALID_ARGUMENT;
    // Set new composing text
    if (text != NULL) text->ToString(&mComposingText);
    else  mComposingText = "";
    ECode ec = NOERROR;

    if (!mComposing) {
        if (mComposingText.IsNullOrEmpty()) {
            // Some IMEs such as iWnn sometimes call with empty composing
            // text.  (See bug 664364)
            // If composing text is empty, ignore this and don't start
            // compositing.
            *pFlag = TRUE;
            return NOERROR;
        }

        // Get current selection
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_GET_SELECTION, 0, 0));
        pthread_cond_wait(&mQueryResult, &mQueryMutex);
        //try {
        //    mQueryResult->take();
        //} catch (InterruptedException e) {
        //    Log.e("GeckoAppJava", "IME: setComposingText interrupted", e);
        //    return false;
        //}
        // Make sure we are in a composition
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_COMPOSITION_BEGIN, 0, 0));
        mComposing = TRUE;
        mCompositionStart = mSelectionLength >= 0 ?
            mSelectionStart : mSelectionStart + mSelectionLength;
    }

    Int32 nLen = 0;
    // Set new selection
    // New selection should be within the composition
    mCompositionSelStart = newCursorPosition > 0 ? mComposingText.GetLength() : 0;
    mCompositionSelLen = 0;

    AutoPtr<ISpanned> span = ISpanned::Probe(text);
    // Handle composition text styles
    if (text != NULL && span) {

        Int32 spanStart = 0, spanEnd = 0;
        Boolean pastSelStart = FALSE, pastSelEnd = FALSE;

        InterfaceID id = EIID_ICharacterStyle;

        do {
            Int32 rangeType = GeckoEvent::IME_RANGE_CONVERTEDTEXT;
            Int32 rangeStyles = 0, rangeForeColor = 0, rangeBackColor = 0;
            text->GetLength(&nLen);

            // Find next offset where there is a style transition
            ec = span->NextSpanTransition(spanStart + 1, nLen,
                id, &spanEnd);
            if (FAILED(ec)) return ec;

            // We need to count the selection as a transition
            if (mCompositionSelLen >= 0) {
                if (!pastSelStart && spanEnd >= mCompositionSelStart) {
                    spanEnd = mCompositionSelStart;
                    pastSelStart = TRUE;
                } else if (!pastSelEnd && spanEnd >=
                        mCompositionSelStart + mCompositionSelLen) {
                    spanEnd = mCompositionSelStart + mCompositionSelLen;
                    pastSelEnd = TRUE;
                    rangeType = GeckoEvent::IME_RANGE_SELECTEDRAWTEXT;
                }
            } else {
                if (!pastSelEnd && spanEnd >=
                        mCompositionSelStart + mCompositionSelLen) {
                    spanEnd = mCompositionSelStart + mCompositionSelLen;
                    pastSelEnd = TRUE;
                } else if (!pastSelStart &&
                        spanEnd >= mCompositionSelStart) {
                    spanEnd = mCompositionSelStart;
                    pastSelStart = TRUE;
                    rangeType = GeckoEvent::IME_RANGE_SELECTEDRAWTEXT;
                }
            }
            // Empty range, continue
            if (spanEnd <= spanStart)
                continue;

            // Get and iterate through list of span objects within range
            AutoPtr<ArrayOf<ICharacterStyle*> > styles;
            ec  = span->GetSpans(
                spanStart, spanEnd, id, (ArrayOf<IInterface*>**)&styles);

            for (Int32 i = 0; i < styles->GetLength(); i++) {
                 ICharacterStyle* style = (*styles)[i];

                if (IUnderlineSpan::Probe(style)) {
                    // Text should be underlined
                    rangeStyles |= GeckoEvent::IME_RANGE_UNDERLINE;

                }
                else if (IForegroundColorSpan::Probe(style)) {
                    // Text should be of a different foreground color
                    rangeStyles |= GeckoEvent::IME_RANGE_FORECOLOR;
                    IForegroundColorSpan::Probe(style)->GetForegroundColor(&rangeForeColor);

                }
                else if (IBackgroundColorSpan::Probe(style)) {
                    // Text should be of a different background color
                    rangeStyles |= GeckoEvent::IME_RANGE_BACKCOLOR;
                    IBackgroundColorSpan::Probe(style)->GetBackgroundColor(&rangeBackColor);
                }
            }

            // Add range to array, the actual styles are
            //  applied when IME_SET_TEXT is sent
            GeckoAppShell::SendEventToGecko(
                new GeckoEvent(spanStart, spanEnd - spanStart,
                               rangeType, rangeStyles,
                               rangeForeColor, rangeBackColor));

            spanStart = spanEnd;
            text->GetLength(&nLen);
        } while (spanStart < nLen);
    } else {
        text->GetLength(&nLen);
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(0, text == NULL ? 0 : nLen,
                           GeckoEvent::IME_RANGE_RAWINPUT,
                           GeckoEvent::IME_RANGE_UNDERLINE, 0, 0));
    }

    // Change composition (treating selection end as where the caret is)
    GeckoAppShell::SendEventToGecko(
        new GeckoEvent(mCompositionSelStart + mCompositionSelLen, 0,
                       GeckoEvent::IME_RANGE_CARETPOSITION, 0, 0, 0,
                       mComposingText));
    *pFlag = TRUE;
    DBG_LOG("\t - ChangeNotification::SetComposingText()");
    return ec;
}

ECode CGeckoInputConnection::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean * successed)
{
    DBG_LOG("\t + ChangeNotification::SetComposingRegion()");
    if (start < 0 || end < start)
    {
        *successed = TRUE;
        return NOERROR;
    }

    AutoPtr<ICharSequence> text;
    if (start == mCompositionStart && end - start == (Int32)mComposingText.GetLength()) {
        // Use mComposingText to avoid extra call to Gecko
        //return E_NOT_IMPLEMENTED;
        //text =mComposingText;
        CStringWrapper::New(mComposingText, (ICharSequence**)&text);
    }

    FAIL_RETURN(FinishComposingText(successed));

    if (text == NULL && start < end) {
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_GET_TEXT, start, end - start));
        return E_NOT_IMPLEMENTED;
        /*
        ec = mQueryResult->Take(&text);
        if (ec == InterruptedException) {
            printf("GeckoAppJava"  "IME: setComposingRegion interrupted");
            *successed = FALSE;
            return NOERROR;
        };
        */
    }

    GeckoAppShell::SendEventToGecko(
                new GeckoEvent(GeckoEvent::IME_SET_SELECTION, start, end - start));

    // Call setComposingText with the same text to start composition and let Gecko know about new composing region
    FAIL_RETURN(SetComposingText(text, 1, successed));

    *successed = TRUE;
    DBG_LOG("\t - ChangeNotification::SetComposingRegion()");
    return NOERROR;
}



ECode CGeckoInputConnection::FinishComposingText(
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::FinishComposingText()");
    if (mComposing) {
        // Set style to none
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(0, mComposingText.GetLength(),
                GeckoEvent::IME_RANGE_RAWINPUT, 0, 0, 0,
                mComposingText));
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_COMPOSITION_END, 0, 0));
        mComposing = FALSE;
        mComposingText = "";

        if (!mBatchMode) {
            // Make sure caret stays at the same position
            GeckoAppShell::SendEventToGecko(
                new GeckoEvent(GeckoEvent::IME_SET_SELECTION,
                               mCompositionStart + mCompositionSelStart, 0));
        }
    }
    *pFlag = TRUE;
    DBG_LOG("\t - ChangeNotification::FinishComposingText()");
    return NOERROR;
}

ECode CGeckoInputConnection::CommitText(
    /* [in] */ ICharSequence * pText,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::CommitText()");
    Boolean bRet = FALSE;
    SetComposingText(pText, newCursorPosition, &bRet);
    FinishComposingText(&bRet);

    if (pFlag) *pFlag = TRUE;
    DBG_LOG("\t - ChangeNotification::CommitText()");
    return NOERROR;
}

ECode CGeckoInputConnection::CommitCompletion(
    /* [in] */ ICompletionInfo * pText,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::CommitCompletion()");
    String text;
    //pText->GetText(&text);
    AutoPtr<ICharSequence> pCharS;
    ECode ec = pText->GetText((ICharSequence**)&pCharS);
    if (FAILED(ec)) return ec;
    ec = CommitText(pCharS, 1, pFlag);
    DBG_LOG("\t - ChangeNotification::CommitCompletion()");
    return ec;
}

ECode CGeckoInputConnection::CommitCorrection(
    /* [in] */ ICorrectionInfo* correctionInfo,
    /* [out] */ Boolean* successed)
{
    DBG_LOG("\t + ChangeNotification::CommitCorrection()");
    return mBaseInputConnection->CommitCorrection(correctionInfo, successed);
}


ECode CGeckoInputConnection::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::SetSelection()");
    if (NULL == pFlag) return E_INVALID_ARGUMENT;

    if (mComposing) {
        /* Translate to fake selection positions */
        start -= mCompositionStart;
        end -= mCompositionStart;

        if (start < 0) {
            start = 0;
        }
        else if (start > (Int32)mComposingText.GetLength()) {
            start = (Int32)mComposingText.GetLength();
        }

        if (end < 0) {
            end = 0;
        }
        else if (end > (Int32)mComposingText.GetLength()) {
            end = (Int32)mComposingText.GetLength();
        }

        mCompositionSelStart = start;
        mCompositionSelLen = end - start;
    }
    else {
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_SET_SELECTION,
                           start, end - start));
    }
    *pFlag = true;
    DBG_LOG("\t - ChangeNotification::SetSelection()");
    return NOERROR;
}

ECode CGeckoInputConnection::PerformEditorAction(
    /* [in] */ Int32 editorAction,
    /* [out] */ Boolean * pFlag)
{
    // TODO: Add your code here
    DBG_LOG("\t + ChangeNotification::PerformEditorAction()");
    return mBaseInputConnection->PerformEditorAction(editorAction, pFlag);
}

ECode CGeckoInputConnection::PerformContextMenuAction(
    /* [in] */ Int32 id,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::PerformContextMenuAction()");
    // First we need to ask Gecko to tell us the full contents of the
    // text field we're about to operate on.
    String text;
    GeckoAppShell::SendEventToGecko(
        new GeckoEvent(GeckoEvent::IME_GET_TEXT, 0, Elastos::Core::Math::INT32_MAX_VALUE));
    pthread_cond_wait(&mQueryResult, &mQueryMutex);
    //try {
    //    text = mQueryResult->Take();
    //} catch (InterruptedException e) {
    //    Log.e("GeckoAppJava", "IME: performContextMenuAction interrupted", e);
    //    return false;
    //}
    Boolean bResult = FALSE;
    switch (id) {
        case 0x0102001f://R->id->selectAll
            SetSelection(0, text.GetLength(), &bResult);
            break;
        case 0x01020020://R->id->cut
            // Fill the clipboard
            GeckoAppShell::SetClipboardText(text);
            // If GET_TEXT returned an empty selection, we'll select everything
            if (mSelectionLength <= 0)
                GeckoAppShell::SendEventToGecko(
                    new GeckoEvent(GeckoEvent::IME_SET_SELECTION, 0, text.GetLength()));
            GeckoAppShell::SendEventToGecko(
                new GeckoEvent(GeckoEvent::IME_DELETE_TEXT, 0, 0));
            break;
        case 0x01020022://R->id->paste
        {
            String clipboardText;
            GeckoAppShell::GetClipboardText(&clipboardText);
            AutoPtr<ICharSequence> pCharS;
            ECode ec = CStringWrapper::New(clipboardText, (ICharSequence**)&pCharS);
            if (FAILED(ec)) return ec;
            CommitText(pCharS, 1, &bResult);
            break;
        }
        case 0x01020021://R->id->copy
            // If there is no selection set, we must be doing "Copy All",
            // otherwise, we need to get the selection from Gecko
            if (mSelectionLength > 0) {
                GeckoAppShell::SendEventToGecko(
                    new GeckoEvent(GeckoEvent::IME_GET_SELECTION, 0, 0));
                pthread_cond_wait(&mQueryResult, &mQueryMutex);
                //try {
                //    text = mQueryResult.take();
                //} catch (InterruptedException e) {
                //    Log.e("GeckoAppJava", "IME: performContextMenuAction interrupted", e);
                //    return false;
                //}
            }
            GeckoAppShell::SetClipboardText(text);
            break;
    }
    if (pFlag) *pFlag = TRUE;
    DBG_LOG("\t - ChangeNotification::PerformContextMenuAction()");
    return NOERROR;
}

ECode CGeckoInputConnection::BeginBatchEdit(
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::BeginBatchEdit()");
    if (NULL == pFlag) return E_INVALID_ARGUMENT;
    *pFlag = TRUE;
    DBG_LOG("\t - ChangeNotification::BeginBatchEdit()");
    return NOERROR;
}

ECode CGeckoInputConnection::EndBatchEdit(
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::EndBatchEdit()");
    return E_NOT_IMPLEMENTED;
    /*
    if (NULL == pFlag) return E_INVALID_ARGUMENT;
    mBatchMode = FALSE;
    Boolean isEmpty;
    mBatchChanges->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<IContext> context;
        GeckoApp::sSurfaceView->GetContext((IContext**)&context);
        AutoPtr<IInputMethodManager> imm;
        context->GetSystemService(Context_INPUT_METHOD_SERVICE, (IInterface**)&imm);
        if (imm != NULL) {
            for (Int32 i = o; mBatchChanges->GetSize(); i++) {
                ChangeNotification * n;
                mBatchChanges->Get(&n);
                if (n->mText != NULL)
                    NotifyTextChange(imm, n->mText, n->mStart, n->mEnd, n->mNewEnd);
                else
                    NotifySelectionChange(imm, n->mStart, n->mEnd);
            }
        }
        mBatchChanges->Clear();
    }

    *pFlag = TRUE;
    return NOERROR;*/
}

ECode CGeckoInputConnection::SendKeyEvent(
    /* [in] */ IKeyEvent * pEvent,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::SendKeyEvent()");
    return mBaseInputConnection->SendKeyEvent(pEvent, pFlag);
}

ECode CGeckoInputConnection::ClearMetaKeyStates(
    /* [in] */ Int32 states,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::ClearMetaKeyStates()");
    return mBaseInputConnection->ClearMetaKeyStates(states, pFlag);
}

ECode CGeckoInputConnection::ReportFullscreenMode(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::ReportFullscreenMode()");
    return mBaseInputConnection->ReportFullscreenMode(enabled, pFlag);
}

ECode CGeckoInputConnection::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle * pData,
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::PerformPrivateCommand()");
    return mBaseInputConnection->PerformPrivateCommand(action, pData, pFlag);
}

ECode CGeckoInputConnection::constructor(
     IView * targetView)
{
    DBG_LOG("\t + ChangeNotification::constructor()");
    pthread_cond_init(&mQueryResult, NULL);
    //mQueryResult = new SynchronousQueue<String>();
    FAIL_RETURN(CExtractedText::New((IExtractedText**)&mUpdateExtract));
    DBG_LOG("\t - ChangeNotification::constructor()");
    return CBaseInputConnection::New(
        targetView, TRUE, (IBaseInputConnection**)&mBaseInputConnection);
}

ECode CGeckoInputConnection::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    DBG_LOG("\t + ChangeNotification::BeforeTextChanged()");
    return E_NOT_IMPLEMENTED;
}

ECode CGeckoInputConnection::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    DBG_LOG("\t + ChangeNotification::OnTextChanged()");
    mNumPendingChanges++;
    GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_SET_SELECTION, start, before));

    if (count == 0) {
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_DELETE_TEXT, 0, 0));
    }
    else {
        Boolean bRet;
        // Start and stop composition to force UI updates.
        FinishComposingText(&bRet);
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_COMPOSITION_BEGIN, 0, 0));

        AutoPtr<ICharSequence> subStr;
        s->SubSequence(start, start + count, (ICharSequence**)&subStr);
        String subString;
        subStr->ToString(&subString);
        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(0, count,
                           GeckoEvent::IME_RANGE_RAWINPUT, 0, 0, 0,
                           subString));

        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_COMPOSITION_END, 0, 0));

        GeckoAppShell::SendEventToGecko(
            new GeckoEvent(GeckoEvent::IME_SET_SELECTION, start + count, 0));
    }

    // Block this thread until all pending events are processed
    GeckoAppShell::GeckoEventSync();
    DBG_LOG("\t - ChangeNotification::OnTextChanged()");
    return NOERROR;
}

ECode CGeckoInputConnection::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    DBG_LOG("\t + ChangeNotification::AfterTextChanged()");
    return E_NOT_IMPLEMENTED;
}

ECode CGeckoInputConnection::OnKeyDel(
    /* [out] */ Boolean * pFlag)
{
    DBG_LOG("\t + ChangeNotification::OnKeyDel()");
    // Some IMEs don't update us on deletions
    // In that case we are not updated when a composition
    // is destroyed, and Bad Things happen
    if (NULL == pFlag) return E_INVALID_ARGUMENT;

    if (!mComposing) {
        *pFlag = FALSE;
        return NOERROR;
    }

    if (!mComposingText.IsNullOrEmpty()) {
         mComposingText = mComposingText.Substring(
            0, mComposingText.GetLength() - 1);

        if (!mComposingText.IsNullOrEmpty()) {
            *pFlag = FALSE;
            return NOERROR;
        }
    }

    Boolean bResult;
    CommitText(NULL, 1, &bResult);

    *pFlag = TRUE;
    DBG_LOG("\t - ChangeNotification::OnKeyDel()");
    return NOERROR;
}

ECode CGeckoInputConnection::NotifyTextChange(
    /* [in] */ IInputMethodManager* imm,
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 oldEnd,
    /* [in] */ Int32 newEnd)
{
    DBG_LOG("\t + ChangeNotification::NotifyTextChange()");
    if (mBatchMode) {
        return E_NOT_IMPLEMENTED;
        //mBatchChanges->Add(new ChangeNotification(text, start, oldEnd, newEnd));
        return NOERROR;
    }

    mNumPendingChanges = Elastos::Core::Math::Max(mNumPendingChanges - 1, 0);

    String editable;
    GeckoApp::sSurfaceView->mEditable->ToString(&editable);
    if (mNumPendingChanges == 0 && !text.Equals(editable)) {
        AutoPtr<ICharSequence> charS;
        ECode ec = CStringWrapper::New(text, (ICharSequence**)&charS);
        if (FAILED(ec)) return ec;
        GeckoApp::sSurfaceView->SetEditable(charS);
    }

    if (mUpdateRequest == NULL){
        return E_INVALID_OPERATION;
    }


    //mUpdateExtract->mFlags = 0;

    // We update from (0, oldEnd) to (0, newEnd) because some Android IMEs
    // assume that updates start at zero, according to jchen.
    //mUpdateExtract->mPartialStartOffset = 0;
    //mUpdateExtract->mPartialEndOffset = oldEnd;

    // Faster to not query for selection
    //mUpdateExtract->mSelectionStart = newEnd;
    //mUpdateExtract->mSelectionEnd = newEnd;

    //mUpdateExtract->mText = text.Substring(0, newEnd);
    mUpdateExtract->SetStartOffset(0);

    Int32 token;
    mUpdateRequest->GetToken(&token);
    DBG_LOG("\t - ChangeNotification::NotifyTextChange()");
    return imm->UpdateExtractedText(GeckoApp::sSurfaceView, token, mUpdateExtract);
}


ECode CGeckoInputConnection::NotifySelectionChange(
    /* [in] */ IInputMethodManager* imm,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    DBG_LOG("\t + ChangeNotification::NotifySelectionChange()");
    if (mBatchMode) {
        return E_NOT_IMPLEMENTED;
        /*
        mBatchChanges->Add(new ChangeNotification(start, end));
        return NOERROR;*/
    }

    if (mComposing) {
        imm->UpdateSelection(GeckoApp::sSurfaceView,
            mCompositionStart + mCompositionSelStart,
            mCompositionStart + mCompositionSelStart + mCompositionSelLen,
            mCompositionStart,
            mCompositionStart + mComposingText.GetLength());
    }
    else {
        imm->UpdateSelection(GeckoApp::sSurfaceView, start, end, -1, -1);
    }

    // We only change the selection if we are relatively sure that the text we have is
    // up-to-date.  Bail out if we are stil expecting changes.
    if (mNumPendingChanges > 0)
        return NOERROR;

    Int32 maxLen = 0;
    GeckoApp::sSurfaceView->mEditable->GetLength(&maxLen);
    //Selection::SetSelection(GeckoApp::surfaceView->mEditable,
    //                       Math::Min(start, maxLen),
     //                      Math::Min(end, maxLen));
    DBG_LOG("\t - ChangeNotification::NotifySelectionChange()");
    return NOERROR;
}

ECode CGeckoInputConnection::Reset()
{
    DBG_LOG("\t + ChangeNotification::Reset()");
    mComposing = FALSE;
    mComposingText = "";

    if (mUpdateRequest)
    {
        mUpdateRequest = NULL;
    }
    DBG_LOG("\t - ChangeNotification::Reset()");
    return NOERROR;
}

ECode CGeckoInputConnection::SetSelectionStart(
    /* [in] */ Int32 selectionStart)
{
    DBG_LOG("\t + ChangeNotification::SetSelectionStart()");
    mSelectionStart = selectionStart;
    DBG_LOG("\t - ChangeNotification::SetSelectionStart()");
    return NOERROR;
}

ECode CGeckoInputConnection::SetSelectionLength(
    /* [in] */ Int32 selectionLength)
{
    DBG_LOG("\t + ChangeNotification::SetSelectionLength()");
    mSelectionLength = selectionLength;
    DBG_LOG("\t - ChangeNotification::SetSelectionLength()");
    return NOERROR;
}

ECode CGeckoInputConnection::PutResult(
    /* [in] */ const String& result)
{
    DBG_LOG("\t + ChangeNotification::PutResult()");
    //mQueryResult.Put(result);
    pthread_cond_signal(&mQueryResult);
    DBG_LOG("\t - ChangeNotification::PutResult()");
    return NOERROR;
}

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
