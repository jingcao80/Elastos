
#include "elastos/droid/systemui/statusbar/phone/KeyguardPreviewContainer.h"

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

KeyguardPreviewContainer::BlackBarDrawable::BlackBarDrawable(
    /* [in] */ KeyguardPreviewContainer* host)
    : mHost(host)
{}

ECode KeyguardPreviewContainer::BlackBarDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 s = 0;
    canvas->Save(&s);
    Int32 h = 0, b = 0, w = 0;
    mHost->GetHeight(&h);
    mHost->GetWidth(&w);
    mHost->GetPaddingBottom(&b);
    Boolean tmp = FALSE;
    canvas->ClipRect(0, h - b, w, h, &tmp);
    canvas->DrawColor(IColor::BLACK);
    canvas->Restore();
    return NOERROR;
}

ECode KeyguardPreviewContainer::BlackBarDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    // noop
    return NOERROR;
}

ECode KeyguardPreviewContainer::BlackBarDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // noop
    return NOERROR;
}

ECode KeyguardPreviewContainer::BlackBarDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::OPAQUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyguardPreviewContainer, FrameLayout, IKeyguardPreviewContainer);
KeyguardPreviewContainer::KeyguardPreviewContainer(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    mBlackBarDrawable = new BlackBarDrawable(this);
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
