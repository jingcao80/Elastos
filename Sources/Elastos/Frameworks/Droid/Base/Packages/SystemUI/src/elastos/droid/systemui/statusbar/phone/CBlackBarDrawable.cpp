
#include "elastos/droid/systemui/statusbar/phone/CBlackBarDrawable.h"
#include "elastos/droid/systemui/statusbar/phone/KeyguardPreviewContainer.h"

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CBlackBarDrawable);
ECode CBlackBarDrawable::constructor(
    /* [in] */ IKeyguardPreviewContainer* host)
{
    mHost = (KeyguardPreviewContainer*)host;
    return Drawable::constructor();
}

ECode CBlackBarDrawable::Draw(
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

ECode CBlackBarDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    // noop
    return NOERROR;
}

ECode CBlackBarDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // noop
    return NOERROR;
}

ECode CBlackBarDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::OPAQUE;
    return NOERROR;
}


} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
