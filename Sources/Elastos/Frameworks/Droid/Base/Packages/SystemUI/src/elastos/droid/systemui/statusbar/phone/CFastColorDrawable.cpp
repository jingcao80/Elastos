
#include "elastos/droid/systemui/statusbar/phone/CFastColorDrawable.h"

using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CFastColorDrawable)
CFastColorDrawable::CFastColorDrawable()
    : mColor(0)
{}

ECode CFastColorDrawable::constructor(
    /* [in] */ Int32 color)
{
    mColor = 0xff000000 | color;
    return Drawable::constructor();
}

ECode CFastColorDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    canvas->DrawColor(mColor, PorterDuffMode_SRC);
    return NOERROR;
}

ECode CFastColorDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return NOERROR;
}

ECode CFastColorDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return NOERROR;
}

ECode CFastColorDrawable::GetOpacity(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode CFastColorDrawable::SetBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return NOERROR;
}

ECode CFastColorDrawable::SetBounds(
    /* [in] */ IRect* bounds)
{
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
