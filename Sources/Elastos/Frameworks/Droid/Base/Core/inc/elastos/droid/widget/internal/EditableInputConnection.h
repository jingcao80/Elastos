
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_EDITABLEINPUTCONNECTION_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_EDITABLEINPUTCONNECTION_H__

#include "elastos/droid/view/inputmethod/BaseInputConnection.h"
#include "elastos/droid/view/inputmethod/CExtractedText.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::EIID_ISpanned;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::Text::Style::EIID_ISuggestionSpan;
using Elastos::Droid::View::InputMethod::BaseInputConnection;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::CExtractedText;
using Elastos::Droid::View::InputMethod::IInputConnection;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

class EditableInputConnection : public BaseInputConnection
{
public:

    EditableInputConnection();

    EditableInputConnection(
        /* [in] */ ITextView* textview);

    virtual CARAPI_(AutoPtr<IEditable>) GetEditable();

    virtual CARAPI_(Boolean) BeginBatchEdit();

    virtual CARAPI_(Boolean) EndBatchEdit();

    virtual CARAPI_(Boolean) ClearMetaKeyStates(
        /* [in] */ Int32 states);

    virtual CARAPI_(Boolean) CommitCompletion(
        /* [in] */ ICompletionInfo* text);

    /**
     * Calls the {@link TextView#onCommitCorrection} method of the associated TextView.
     */
    virtual CARAPI_(Boolean) CommitCorrection(
        /* [in] */ ICorrectionInfo* correctionInfo);

    virtual CARAPI_(Boolean) PerformEditorAction(
        /* [in] */ Int32 actionCode);

    virtual CARAPI_(Boolean) PerformContextMenuAction(
        /* [in] */ Int32 id);

    virtual CARAPI_(AutoPtr<IExtractedText>) GetExtractedText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 flags);

    virtual CARAPI_(Boolean) PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    virtual CARAPI_(Boolean) CommitText(ICharSequence* text, Int32 newCursorPosition);

protected:
    virtual CARAPI ReportFinish();

    CARAPI Init(
        /* [in] */ ITextView* textview);

private:
    static const Boolean DEBUG = FALSE;
    static const String TAG;
    AutoPtr<ITextView> mTextView;

    // Keeps track of nested begin/end batch edit to ensure this connection always has a
    // balanced impact on its associated TextView.
    // A negative value means that this connection has been finished by the InputMethodManager.
    Int32 mBatchEditNesting;

    Object mLock;
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif
