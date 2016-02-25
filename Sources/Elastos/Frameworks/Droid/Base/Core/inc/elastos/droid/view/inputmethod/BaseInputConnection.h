
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_BASEINPUTCONNECTION_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_BASEINPUTCONNECTION_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CKeyCharacterMap.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::View::IIInputMethodManager;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::INoCopySpan;
using Elastos::Droid::Text::Style::ISuggestionSpan;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

/**
 * Base class for implementors of the InputConnection interface, taking care
 * of most of the common behavior for providing a connection to an Editable.
 * Implementors of this class will want to be sure to implement
 * {@link #getEditable} to provide access to their own editable object.
 */
class BaseInputConnection
    : public Object
    , public IBaseInputConnection
    , public IInputConnection
{
private:
    class ComposingText
        : public Object
        , public INoCopySpan
    {
    public:
        CAR_INTERFACE_DECL()
    };

public:
    CAR_INTERFACE_DECL()

    BaseInputConnection();

    CARAPI constructor(
        /* [in] */ IInputMethodManager* mgr,
        /* [in] */ Boolean fullEditor);

    CARAPI constructor(
        /* [in] */ IView* targetView,
        /* [in] */ Boolean fullEditor);

    ~BaseInputConnection();

    static CARAPI RemoveComposingSpans(
        /* [in] */ ISpannable* text);

    static CARAPI SetComposingSpans(
        /* [in] */ ISpannable* text);

    /** @hide */
    static CARAPI SetComposingSpans(
        /* [in] */ ISpannable* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    static CARAPI_(Int32) GetComposingSpanStart(
        /* [in] */ ISpannable* text);

    static CARAPI_(Int32) GetComposingSpanEnd(
        /* [in] */ ISpannable* text);

    /**
     * Return the target of edit operations.  The default implementation
     * returns its own fake editable that is just used for composing text;
     * subclasses that are real text editors should override this and
     * supply their own.
     */
    CARAPI GetEditable(
        /* [out] */ IEditable** result);

    /**
     * Default implementation does nothing.
     */
    CARAPI BeginBatchEdit(
        /* [out] */ Boolean* result);

    /**
     * Default implementation does nothing.
     */
    CARAPI EndBatchEdit(
        /* [out] */ Boolean* result);

    /**
     * Default implementation uses
     * {@link MetaKeyKeyListener#clearMetaKeyState(long, int)
     * MetaKeyKeyListener.clearMetaKeyState(long, int)} to clear the state.
     */
    CARAPI ClearMetaKeyStates(
        /* [in] */ Int32 states,
        /* [out] */ Boolean* result);

    /**
     * Default implementation does nothing and returns false.
     */
    CARAPI CommitCompletion(
        /* [in] */ ICompletionInfo* text,
        /* [out] */ Boolean* result);

    /**
     * Default implementation does nothing and returns false.
     */
    CARAPI CommitCorrection(
        /* [in] */ ICorrectionInfo* correctionInfo,
        /* [out] */ Boolean* result);

    /**
     * Default implementation replaces any existing composing text with
     * the given text.  In addition, only if dummy mode, a key event is
     * sent for the new text and the current editable buffer cleared.
     */
    CARAPI CommitText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean* result);

    /**
     * The default implementation performs the deletion around the current
     * selection position of the editable text.
     * @param beforeLength
     * @param afterLength
     */
    CARAPI DeleteSurroundingText(
        /* [in] */ Int32 beforeLength,
        /* [in] */ Int32 afterLength,
        /* [out] */ Boolean* result);

    /**
     * The default implementation removes the composing state from the
     * current editable text.  In addition, only if dummy mode, a key event is
     * sent for the new text and the current editable buffer cleared.
     */
    CARAPI FinishComposingText(
        /* [out] */ Boolean* result);

    /**
     * The default implementation uses TextUtils.getCapsMode to get the
     * cursor caps mode for the current selection position in the editable
     * text, unless in dummy mode in which case 0 is always returned.
     */
    CARAPI GetCursorCapsMode(
        /* [in] */ Int32 reqModes,
        /* [out] */ Int32* afterLength);

    /**
     * The default implementation always returns null.
     */
    CARAPI GetExtractedText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 flags,
        /* [out] */ IExtractedText** text);

    /**
     * The default implementation returns the given amount of text from the
     * current cursor position in the buffer.
     */
    CARAPI GetTextBeforeCursor(
        /* [in] */ Int32 length,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** seq);

    /**
     * The default implementation returns the text currently selected, or null if none is
     * selected.
     */
    CARAPI GetSelectedText(
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** seq);

    /**
     * The default implementation returns the given amount of text from the
     * current cursor position in the buffer.
     */
    CARAPI GetTextAfterCursor(
        /* [in] */ Int32 length,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** seq);

    /**
     * The default implementation turns this into the enter key.
     */
    CARAPI PerformEditorAction(
        /* [in] */ Int32 actionCode,
        /* [out] */ Boolean* result);

    /**
     * The default implementation does nothing.
     */
    CARAPI PerformContextMenuAction(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* result);

    /**
     * The default implementation does nothing.
     */
    CARAPI PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data,
        /* [out] */ Boolean* result);

    /**
     * The default implementation does nothing.
     */
    CARAPI RequestCursorUpdates(
        /* [in] */ Int32 cursorUpdateMode,
        /* [out] */ Boolean* result);

    /**
     * The default implementation places the given text into the editable,
     * replacing any existing composing text.  The new text is marked as
     * in a composing state with the composing style.
     */
    CARAPI SetComposingText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean* result);

    CARAPI SetComposingRegion(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Boolean* result);

    /**
     * The default implementation changes the selection position in the
     * current editable text.
     */
    CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Boolean* result);

    /**
     * Provides standard implementation for sending a key event to the window
     * attached to the input connection's view.
     */
    CARAPI SendKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Updates InputMethodManager with the current fullscreen mode.
     */
    CARAPI ReportFullscreenMode(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* result);

    /**
     * Called when this InputConnection is no longer used by the InputMethodManager.
     *
     * @hide
     */
    CARAPI_(void) ReportFinish();

private:
    CARAPI_(void) SendCurrentText();

    CARAPI_(void) EnsureDefaultComposingSpans();

    CARAPI_(void) ReplaceText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [in] */ Boolean composing);

protected:
    /** @hide */
    AutoPtr<IInputMethodManager> mIMM;

private:
    static const Boolean DEBUG;
    static const String TAG;
    static const AutoPtr<IInterface> COMPOSING;

    AutoPtr<IView> mTargetView;
    Boolean mDummyMode;

    AutoPtr<ArrayOf<IInterface*> > mDefaultComposingSpans;

    AutoPtr<IEditable> mEditable;
    AutoPtr<CKeyCharacterMap> mKeyCharacterMap;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_INPUTMETHOD_BASEINPUTCONNECTION_H__
