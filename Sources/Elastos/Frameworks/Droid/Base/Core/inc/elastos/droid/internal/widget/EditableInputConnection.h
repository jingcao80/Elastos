//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_EDITABLEINPUTCONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_EDITABLEINPUTCONNECTION_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/inputmethod/BaseInputConnection.h"
#include "elastos/droid/view/inputmethod/CExtractedText.h"

using Elastos::Droid::View::InputMethod::BaseInputConnection;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class EditableInputConnection
    : public BaseInputConnection
    , public IEditableInputConnection
{
public:
    CAR_INTERFACE_DECL()

    EditableInputConnection();

    ~EditableInputConnection();

    CARAPI constructor(
        /* [in] */ ITextView* textview);

    // @Override
    CARAPI GetEditable(
        /* [out] */ IEditable** result);

    // @Override
    CARAPI BeginBatchEdit(
        /* [out] */ Boolean* res);

    // @Override
    CARAPI EndBatchEdit(
        /* [out] */ Boolean* res);

    // @Override
    CARAPI ClearMetaKeyStates(
        /* [in] */ Int32 states,
        /* [out] */ Boolean* res);

    /**
     * Calls the {@link TextView#onCommitCorrection} method of the associated TextView.
     */
    // @Override
    CARAPI CommitCompletion(
        /* [in] */ ICompletionInfo* text,
        /* [out] */ Boolean* res);

    /**
     * Calls the {@link TextView#onCommitCorrection} method of the associated TextView.
     */
    // @Override
    CARAPI CommitCorrection(
        /* [in] */ ICorrectionInfo* correctionInfo,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI PerformEditorAction(
        /* [in] */ Int32 actionCode,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI PerformContextMenuAction(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI GetExtractedText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 flags,
        /* [out] */ IExtractedText** extractedText);

    // @Override
    CARAPI PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI CommitText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI RequestCursorUpdates(
        /* [in] */ Int32 cursorUpdateMode,
        /* [out] */ Boolean* res);

protected:
    // @Override
    virtual CARAPI ReportFinish();

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

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_EDITABLEINPUTCONNECTION_H__
