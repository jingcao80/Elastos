
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CCONTROLLEDINPUTCONNECTIONWRAPPER_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CCONTROLLEDINPUTCONNECTIONWRAPPER_H__

#include "_Elastos_Droid_View_InputMethod_CControlledInputConnectionWrapper.h"
//#include "elastos/droid/view/IInputConnectionWrapper.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IIInputContextCallback;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CControlledInputConnectionWrapper)
    , public Object
    , public IIInputContext
    , public IBinder
//    , public IInputConnectionWrapper  // internal
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CControlledInputConnectionWrapper();

    CARAPI constructor(
        /* [in] */ ILooper* mainLooper,
        /* [in] */ IInputConnection* conn,
        /* [in] */ IInputMethodManager* agmer);

    CARAPI GetTextAfterCursor(
        /* [in] */ Int32 length,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI GetTextBeforeCursor(
        /* [in] */ Int32 length,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI GetSelectedText(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI GetCursorCapsMode(
        /* [in] */ Int32 reqModes,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI GetExtractedText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI CommitText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition);

    CARAPI CommitCompletion(
        /* [in] */ ICompletionInfo* text);

    CARAPI CommitCorrection(
        /* [in] */ ICorrectionInfo* info);

    CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI PerformEditorAction(
        /* [in] */ Int32 id);

    CARAPI PerformContextMenuAction(
        /* [in] */ Int32 id);

    CARAPI SetComposingRegion(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI SetComposingText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition);

    CARAPI FinishComposingText();

    CARAPI SendKeyEvent(
        /* [in] */ IKeyEvent* event);

    CARAPI ClearMetaKeyStates(
        /* [in] */ Int32 states);

    CARAPI DeleteSurroundingText(
        /* [in] */ Int32 leftLength,
        /* [in] */ Int32 rightLength);

    CARAPI BeginBatchEdit();

    CARAPI EndBatchEdit();

    CARAPI ReportFullscreenMode(
        /* [in] */ Boolean enabled);

    CARAPI PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    CARAPI GetDescription(
        /* [out] */ String* str);

    CARAPI RequestUpdateCursorAnchorInfo(
        /* [in] */ Int32 cursorUpdateMode,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* cb);

    CARAPI_(Boolean) IsActive();

    CARAPI_(void) Deactivate();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<CInputMethodManager> mParentInputMethodManager;
    Boolean mActive;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif   //__ELASTOS_DROID_VIEW_INPUTMETHOD_CCONTROLLEDINPUTCONNECTIONWRAPPER_H__
