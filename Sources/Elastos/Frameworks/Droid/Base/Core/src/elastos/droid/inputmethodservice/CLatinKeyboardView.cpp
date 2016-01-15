
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/inputmethodservice/CLatinKeyboardView.h"

using Elastos::Droid::InputMethodService::ISoftKeyboard;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CAR_OBJECT_IMPL(CLatinKeyboardView);
ECode CLatinKeyboardView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return KeyboardView::constructor(ctx, attrs);
}

ECode CLatinKeyboardView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return KeyboardView::constructor(ctx, attrs, defStyle);
}

ECode CLatinKeyboardView::OnLongPress(
    /* [in] */ IKeyboardKey* popupKey,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr< ArrayOf<Int32> > codes;
    popupKey->GetCodes((ArrayOf<Int32>**)&codes);
    assert(codes != NULL);
    if ((*codes)[0] == IKeyboard::KEYCODE_CANCEL) {
        AutoPtr<IOnKeyboardActionListener> listener;
        KeyboardView::GetOnKeyboardActionListener((IOnKeyboardActionListener**)&listener);
        listener->OnKey(ISoftKeyboard::KEYCODE_OPTIONS, NULL);
        *res = TRUE;
        return NOERROR;
    }
    else {
        return KeyboardView::OnLongPress(popupKey, res);
    }
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
