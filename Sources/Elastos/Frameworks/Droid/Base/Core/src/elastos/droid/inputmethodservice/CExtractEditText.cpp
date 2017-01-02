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

#include "elastos/droid/inputmethodservice/CExtractEditText.h"
#include "elastos/droid/R.h"

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CAR_OBJECT_IMPL(CExtractEditText);
CAR_INTERFACE_IMPL(CExtractEditText, EditText, IExtractEditText);
CExtractEditText::CExtractEditText()
    : mSettingExtractedText(0)
{
}

ECode CExtractEditText::SetExtractedText(
    /* [in] */ IExtractedText* text)
{
    mSettingExtractedText++;
    EditText::SetExtractedText(text);

    mSettingExtractedText--;
    return NOERROR;
}

ECode CExtractEditText::PerformClick(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if ((EditText::PerformClick(&tmp), !tmp) && mIME != NULL) {
        mIME->OnExtractedTextClicked();
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode CExtractEditText::OnTextContextMenuItem(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean state = FALSE;
    if (mIME != NULL && (mIME->OnExtractTextContextMenuItem(id, &state), state)) {
        // Mode was started on Extracted, needs to be stopped here.
        // Cut and paste will change the text, which stops selection mode.
        if (id == R::id::copy) {
            StopSelectionActionMode();
        }

        *result = TRUE;
        return NOERROR;
    }

    return EditText::OnTextContextMenuItem(id, result);
}

ECode CExtractEditText::IsInputMethodTarget(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CExtractEditText::HasWindowFocus(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return EditText::IsEnabled(result);
}

ECode CExtractEditText::IsFocused(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return EditText::IsEnabled(result);
}

ECode CExtractEditText::HasFocus(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return EditText::IsEnabled(result);
}

void CExtractEditText::OnSelectionChanged(
    /* [in] */ Int32 selStart,
    /* [in] */ Int32 selEnd)
{
    if (mSettingExtractedText == 0 && mIME != NULL && selStart >= 0 && selEnd >= 0) {
        mIME->OnExtractedSelectionChanged(selStart, selEnd);
    }
}

void CExtractEditText::ViewClicked(
    /* [in] */ IInputMethodManager* imm)
{
    // As an instance of this class is supposed to be owned by IMS,
    // and it has a reference to the IMS (the current IME),
    // we just need to call back its onViewClicked() here.
    // It should be good to avoid unnecessary IPCs by doing this as well.
    if (mIME != NULL) {
        mIME->OnViewClicked(FALSE);
    }
}

void CExtractEditText::DeleteText_internal(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    // Do not call the super method.
    // This will change the source TextView instead, which will update the ExtractTextView.
    mIME->OnExtractedDeleteText(start, end);
}

void CExtractEditText::ReplaceText_internal(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ICharSequence* text)
{
    // Do not call the super method.
    // This will change the source TextView instead, which will update the ExtractTextView.
    mIME->OnExtractedReplaceText(start, end, text);
}

void CExtractEditText::SetSpan_internal(
    /* [in] */ IInterface* span,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 flags)
{
    // Do not call the super method.
    // This will change the source TextView instead, which will update the ExtractTextView.
    mIME->OnExtractedSetSpan(span, start, end, flags);
}

void CExtractEditText::SetCursorPosition_internal(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    // Do not call the super method.
    // This will change the source TextView instead, which will update the ExtractTextView.
    mIME->OnExtractedSelectionChanged(start, end);
}

ECode CExtractEditText::constructor(
    /* [in] */ IContext* context)
{
    return EditText::constructor(context, NULL);
}

ECode CExtractEditText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return EditText::constructor(context, attrs, 0x0101006e /*com.android.internal.R.attr.editTextStyle*/);
}

ECode CExtractEditText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CExtractEditText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return EditText::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CExtractEditText::SetIME(
    /* [in] */ IInputMethodService* ime)
{
    mIME = ime;
    return NOERROR;
}

ECode CExtractEditText::StartInternalChanges()
{
    mSettingExtractedText += 1;
    return NOERROR;
}

ECode CExtractEditText::FinishInternalChanges()
{
    mSettingExtractedText -= 1;
    return NOERROR;
}

ECode CExtractEditText::HasVerticalScrollBar(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = EditText::ComputeVerticalScrollRange() > EditText::ComputeVerticalScrollExtent();
    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
