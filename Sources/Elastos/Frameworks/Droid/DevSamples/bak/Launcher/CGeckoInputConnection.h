
#ifndef __CGECKOINPUTCONNECTION_H__
#define __CGECKOINPUTCONNECTION_H__

#include "_CGeckoInputConnection.h"
//#include <elastos/ElRefBase.h>
#include <elastos.h>
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Core;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::Text::IEditable;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

CarClass(CGeckoInputConnection)
{
public:
    CGeckoInputConnection();

    ~CGeckoInputConnection();

    CARAPI GetTextBeforeCursor(
        /* [in] */ Int32 n,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence ** ppText);

    CARAPI GetTextAfterCursor(
        /* [in] */ Int32 n,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence ** ppText);

    CARAPI GetSelectedText(
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence ** ppText);

    CARAPI GetCursorCapsMode(
        /* [in] */ Int32 reqModes,
        /* [out] */ Int32 * pCapsMode);

    CARAPI GetExtractedText(
        /* [in] */ IExtractedTextRequest * pRequest,
        /* [in] */ Int32 flags,
        /* [out] */ IExtractedText ** ppExtractedText);

    CARAPI DeleteSurroundingText(
        /* [in] */ Int32 leftLength,
        /* [in] */ Int32 rightLength,
        /* [out] */ Boolean * pFlag);

    CARAPI SetComposingText(
        /* [in] */ ICharSequence * pText,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean * pFlag);

    CARAPI SetComposingRegion(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Boolean * pFlag);

    CARAPI FinishComposingText(
        /* [out] */ Boolean * pFlag);

    CARAPI CommitText(
        /* [in] */ ICharSequence * pText,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean * pFlag);

    CARAPI CommitCompletion(
        /* [in] */ ICompletionInfo * pText,
        /* [out] */ Boolean * pFlag);

    CARAPI CommitCorrection(
        /* [in] */ ICorrectionInfo* correctionInfo,
        /* [out] */ Boolean* successed);

    CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Boolean * pFlag);

    CARAPI PerformEditorAction(
        /* [in] */ Int32 editorAction,
        /* [out] */ Boolean * pFlag);

    CARAPI PerformContextMenuAction(
        /* [in] */ Int32 id,
        /* [out] */ Boolean * pFlag);

    CARAPI BeginBatchEdit(
        /* [out] */ Boolean * pFlag);

    CARAPI EndBatchEdit(
        /* [out] */ Boolean * pFlag);

    CARAPI SendKeyEvent(
        /* [in] */ IKeyEvent * pEvent,
        /* [out] */ Boolean * pFlag);

    CARAPI ClearMetaKeyStates(
        /* [in] */ Int32 states,
        /* [out] */ Boolean * pFlag);

    CARAPI ReportFullscreenMode(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean * pFlag);

    CARAPI PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle * pData,
        /* [out] */ Boolean * pFlag);

    CARAPI constructor(
        /* [in] */ IView * pTargetView);

    CARAPI GetEditable(IEditable** ppE);

    CARAPI OnKeyDel(
        /* [out] */ Boolean * pFlag);

    CARAPI NotifyTextChange(
        /* [in] */ IInputMethodManager* imm,
        /* [in] */ const String& text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 oldEnd,
        /* [in] */ Int32 newEnd);

    CARAPI NotifySelectionChange(
        /* [in] */ IInputMethodManager* imm,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI Reset();

    CARAPI SetSelectionStart(
        /* [in] */ Int32 selectionStart);

    CARAPI SetSelectionLength(
        /* [in] */ Int32 selectionLength);

    CARAPI PutResult(
        /* [in] */ const String& result);

public:
    //For ITextWatcher interface.
    /**
     * This method is called to notify you that, within <code>s</code>,
     * the <code>count</code> characters beginning at <code>start</code>
     * are about to be replaced by new text with length <code>after</code>.
     * It is an error to attempt to make changes to <code>s</code> from
     * this callback.
     */
    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    /**
     * This method is called to notify you that, within <code>s</code>,
     * the <code>count</code> characters beginning at <code>start</code>
     * have just replaced old text that had length <code>before</code>.
     * It is an error to attempt to make changes to <code>s</code> from
     * this callback.
     */
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    /**
     * This method is called to notify you that, somewhere within
     * <code>s</code>, the text has been changed.
     * It is legitimate to make further changes to <code>s</code> from
     * this callback, but be careful not to get yourself into an infinite
     * loop, because any changes you make will cause this method to be
     * called again recursively.
     * (You are not told where the change took place because other
     * afterTextChanged() methods may already have made other changes
     * and invalidated the offsets.  But if you need to know here,
     * you can use {@link Spannable#setSpan} in {@link #onTextChanged}
     * to mark your place and then look up from here where the span
     * ended up.
     */
    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

private:
    class ChangeNotification:public ElRefBase {
    public:
        String mText;
        Int32 mStart;
        Int32 mEnd;
        Int32 mNewEnd;

        ChangeNotification(String text, Int32 start, Int32 oldEnd, Int32 newEnd);

        ChangeNotification(Int32 start, Int32 end);
    };
        // Is a composition active?
    Boolean mComposing;
    // Composition text when a composition is active
    String mComposingText;
    // Start index of the composition within the text body
    Int32 mCompositionStart;
    /* During a composition, we should not alter the real selection,
        therefore we keep our own offsets to emulate selection */
    // Start of fake selection, relative to start of composition
    Int32 mCompositionSelStart;
    // Length of fake selection
    Int32 mCompositionSelLen;

    AutoPtr<IExtractedTextRequest> mUpdateRequest;
    AutoPtr<IExtractedText> mUpdateExtract;

    Int32 mSelectionStart;
    Int32 mSelectionLength;
    //SynchronousQueue<String> mQueryResult;
    AutoPtr<IInputConnection> mBaseInputConnection;

    // Number of in flight changes
    Int32 mNumPendingChanges;

    Boolean mBatchMode;
    pthread_cond_t mQueryResult;
    pthread_mutex_t mQueryMutex;
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CGECKOINPUTCONNECTION_H__

