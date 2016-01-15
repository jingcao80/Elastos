
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTCONNECTIONWRAPPER_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTCONNECTIONWRAPPER_H__

#include "_Elastos_Droid_View_InputMethod_CInputConnectionWrapper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

/**
 * <p>Wrapper class for proxying calls to another InputConnection.  Subclass
 * and have fun!
 */
CarClass(CInputConnectionWrapper)
    , public Object
    , public IInputConnection
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInputConnectionWrapper();

    CARAPI constructor(
        /* [in] */ IInputConnection* target,
        /* [in] */ Boolean mutble);

    CARAPI SetTarget(
        /* [in] */ IInputConnection* target);

    CARAPI GetTextBeforeCursor(
        /* [in] */ Int32 n,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** text);

    CARAPI GetTextAfterCursor(
        /* [in] */ Int32 n,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** text);

    CARAPI GetSelectedText(
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** text);

    CARAPI GetCursorCapsMode(
        /* [in] */ Int32 reqModes,
        /* [out] */ Int32* capsMode);

    CARAPI GetExtractedText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 flags,
        /* [out] */ IExtractedText** extractedText);

    CARAPI DeleteSurroundingText(
        /* [in] */ Int32 leftLength,
        /* [in] */ Int32 rightLength,
        /* [out] */ Boolean* flag);

    CARAPI SetComposingText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean* flag);

    CARAPI SetComposingRegion(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Boolean* flag);

    CARAPI FinishComposingText(
        /* [out] */ Boolean* flag);

    CARAPI CommitText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean* flag);

    CARAPI CommitCompletion(
        /* [in] */ ICompletionInfo* text,
        /* [out] */ Boolean* flag);

    CARAPI CommitCorrection(
        /* [in] */ ICorrectionInfo* info,
        /* [out] */ Boolean* flag);

    CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Boolean* flag);

    CARAPI PerformEditorAction(
        /* [in] */ Int32 editorAction,
        /* [out] */ Boolean* flag);

    CARAPI PerformContextMenuAction(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* flag);

    CARAPI BeginBatchEdit(
        /* [out] */ Boolean* flag);

    CARAPI EndBatchEdit(
        /* [out] */ Boolean* flag);

    CARAPI SendKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* flag);

    CARAPI ClearMetaKeyStates(
        /* [in] */ Int32 states,
        /* [out] */ Boolean* flag);

    CARAPI ReportFullscreenMode(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* flag);

    CARAPI PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data,
        /* [out] */ Boolean* flag);

    CARAPI RequestCursorUpdates(
        /* [in] */ Int32 cursorUpdateMode,
        /* [out] */ Boolean* result);

private:
    AutoPtr<IInputConnection> mTarget;
    Boolean mMutable;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTCONNECTIONWRAPPER_H__
