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

#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CEXTRACTEDITTEXT_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CEXTRACTEDITTEXT_H__

#include "_Elastos_Droid_InputMethodService_CExtractEditText.h"
#include "elastos/droid/widget/EditText.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::EditText;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

/***
 * Specialization of {@link EditText} for showing and interacting with the
 * extracted text in a full-screen input method.
 */
CarClass(CExtractEditText)
    , public EditText
    , public IExtractEditText
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CExtractEditText();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetIME(
        /* [in] */ IInputMethodService* ime);

    /**
     * Start making changes that will not be reported to the client.  That
     * is, {@link #onSelectionChanged(Int32, Int32)} will not result in sending
     * the new selection to the client
     */
    CARAPI StartInternalChanges();

    /**
     * Finish making changes that will not be reported to the client.  That
     * is, {@link #onSelectionChanged(Int32, Int32)} will not result in sending
     * the new selection to the client
     */
    CARAPI FinishInternalChanges();

    /**
     * Return true if the edit text is currently showing a scroll bar.
     */
    CARAPI HasVerticalScrollBar(
        /* [out] */ Boolean* has);

    /**
     * Implement just to keep track of when we are setting text from the
     * client (vs. seeing changes in ourself from the user).
     */
    //@Override
    CARAPI SetExtractedText(
        /* [in] */ IExtractedText* text);

    /**
     * Redirect clicks to the IME for handling there.  First allows any
     * on click handler to run, though.
     */
    //@Override
    CARAPI PerformClick(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTextContextMenuItem(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* result);

    /**
     * We are always considered to be an input method target.
     */
    //@Override
    CARAPI IsInputMethodTarget(
        /* [out] */ Boolean* result);

    /**
     * Pretend like the window this view is in always has focus, so its
     * highlight and cursor will be displayed.
     */
    //@Override
    CARAPI HasWindowFocus(
        /* [out] */ Boolean* result);

    /**
     * Pretend like this view always has focus, so its
     * highlight and cursor will be displayed.
     */
    //@Override
    CARAPI IsFocused(
        /* [out] */ Boolean* result);

    /**
     * Pretend like this view always has focus, so its
     * highlight and cursor will be displayed.
     */
    //@Override
    CARAPI HasFocus(
        /* [out] */ Boolean* result);

protected:
    /**
     * Report to the underlying text editor about selection changes.
     */
    //@Override
    CARAPI_(void) OnSelectionChanged(
        /* [in] */ Int32 selStart,
        /* [in] */ Int32 selEnd);

    /**
     * @hide
     */
    //@Override
    virtual CARAPI_(void) ViewClicked(
        /* [in] */ IInputMethodManager* imm);

    /**
     * {@inheritDoc}
     * @hide
     */
    //@Override
    virtual CARAPI_(void) DeleteText_internal(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * {@inheritDoc}
     * @hide
     */
    //@Override
    virtual CARAPI_(void) ReplaceText_internal(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ICharSequence* text);

    /**
     * {@inheritDoc}
     * @hide
     */
    //@Override
    virtual CARAPI_(void) SetSpan_internal(
        /* [in] */ IInterface* span,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 flags);

    /**
     * {@inheritDoc}
     * @hide
     */
    //@Override
    virtual CARAPI_(void) SetCursorPosition_internal(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

protected:
    AutoPtr<IInputMethodService> mIME;
    Int32 mSettingExtractedText;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CEXTRACTEDITTEXT_H__
