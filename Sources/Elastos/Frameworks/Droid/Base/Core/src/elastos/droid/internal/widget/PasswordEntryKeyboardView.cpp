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

#include "elastos/droid/internal/widget/PasswordEntryKeyboardView.h"

using Elastos::Droid::InputMethodService::IKeyboard;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const Int32 PasswordEntryKeyboardView::KEYCODE_OPTIONS = -100;
const Int32 PasswordEntryKeyboardView::KEYCODE_SHIFT_LONGPRESS = -101;
const Int32 PasswordEntryKeyboardView::KEYCODE_VOICE = -102;
const Int32 PasswordEntryKeyboardView::KEYCODE_F1 = -103;
const Int32 PasswordEntryKeyboardView::KEYCODE_NEXT_LANGUAGE = -104;

CAR_INTERFACE_IMPL(PasswordEntryKeyboardView, KeyboardView, IPasswordEntryKeyboardView)

PasswordEntryKeyboardView::PasswordEntryKeyboardView()
{}

ECode PasswordEntryKeyboardView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode PasswordEntryKeyboardView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode PasswordEntryKeyboardView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return KeyboardView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode PasswordEntryKeyboardView::SetShifted(
    /* [in] */ Boolean shifted,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    ECode ec = KeyboardView::SetShifted(shifted, res);
    // invalidate both shift keys
    AutoPtr<IKeyboard> kb;
    GetKeyboard((IKeyboard**)&kb);
    AutoPtr<ArrayOf<Int32> > indices;
    kb->GetShiftKeyIndices((ArrayOf<Int32>**)&indices);
    for (Int32 i = 0; i < indices->GetLength(); i++) {
        InvalidateKey((*indices)[i]);
    }
    return ec;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos