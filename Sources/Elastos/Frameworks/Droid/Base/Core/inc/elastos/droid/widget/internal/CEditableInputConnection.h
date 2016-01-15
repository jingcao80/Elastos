#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_CEIDTABLEINPUTCONNECTION_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_CEIDTABLEINPUTCONNECTION_H__

#include "_Elastos_Droid_Widget_Internal_CEditableInputConnection.h"
#include "elastos/droid/widget/internal/EditableInputConnection.h"

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::InputMethod::EIID_BaseInputConnection;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

CarClass(CEditableInputConnection) , public EditableInputConnection
{
public:

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ ITextView* textview);

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
        /* [in] */ Int32 beforeLength,
        /* [in] */ Int32 afterLength,
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
        /* [in] */ ICorrectionInfo* correctionInfo,
        /* [out] */ Boolean* successed);

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
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_INTERNAL_CEIDTABLEINPUTCONNECTION_H__
