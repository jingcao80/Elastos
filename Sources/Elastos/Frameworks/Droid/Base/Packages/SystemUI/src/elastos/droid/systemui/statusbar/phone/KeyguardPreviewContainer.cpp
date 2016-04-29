
#include "elastos/droid/systemui/statusbar/phone/KeyguardPreviewContainer.h"
#include "elastos/droid/systemui/statusbar/phone/CBlackBarDrawable.h"

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_INTERFACE_IMPL(KeyguardPreviewContainer, FrameLayout, IKeyguardPreviewContainer);
KeyguardPreviewContainer::KeyguardPreviewContainer(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    CBlackBarDrawable::New(this, (IDrawable**)&mBlackBarDrawable);
    FrameLayout::constructor(context, attrs);
    SetBackground(mBlackBarDrawable);
}

ECode KeyguardPreviewContainer::OnApplyWindowInsets(
    /* [in] */ IWindowInsets* insets,
    /* [out] */ IWindowInsets** sets)
{
    VALIDATE_NOT_NULL(sets);
    Int32 value = 0;
    insets->GetStableInsetBottom(&value);
    SetPadding(0, 0, 0, value);
    return FrameLayout::OnApplyWindowInsets(insets, sets);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
