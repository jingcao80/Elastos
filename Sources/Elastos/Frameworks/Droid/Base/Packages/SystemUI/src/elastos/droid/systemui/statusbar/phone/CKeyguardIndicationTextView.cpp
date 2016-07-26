
#include "elastos/droid/systemui/statusbar/phone/CKeyguardIndicationTextView.h"
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CKeyguardIndicationTextView)

CAR_INTERFACE_IMPL(CKeyguardIndicationTextView, TextView, IKeyguardIndicationTextView)

ECode CKeyguardIndicationTextView::SwitchIndication(
    /* [in] */ ICharSequence* text)
{
    // TODO: Animation, make sure that we will show one indication long enough.
    if (TextUtils::IsEmpty(text)) {
        SetVisibility(IView::INVISIBLE);
    }
    else {
        SetVisibility(IView::VISIBLE);
        SetText(text);
    }
    return NOERROR;
}

ECode CKeyguardIndicationTextView::SwitchIndication(
    /* [in] */ Int32 textResId)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<ICharSequence> text;
    res->GetText(textResId, (ICharSequence**)&text);
    SwitchIndication(text);
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
