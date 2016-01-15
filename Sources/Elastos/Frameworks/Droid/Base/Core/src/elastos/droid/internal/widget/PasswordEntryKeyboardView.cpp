
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