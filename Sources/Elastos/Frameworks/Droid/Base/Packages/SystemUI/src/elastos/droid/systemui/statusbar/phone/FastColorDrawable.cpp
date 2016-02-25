#include "elastos/droid/systemui/statusbar/phone/FastColorDrawable.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


FastColorDrawable::FastColorDrawable()
    : mColor(0)
{
}

FastColorDrawable::FastColorDrawable(
    /* [in] */ Int32 color)
{
    Init(color);
}

ECode FastColorDrawable::Init(
        /* [in] */ Int32 color)
{
    mColor = 0xff000000 | color;
    return NOERROR;
}

//@Override
ECode FastColorDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    return canvas->DrawColor(mColor, Elastos::Droid::Graphics::PorterDuffMode_SRC);
}

ECode FastColorDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return NOERROR;
}

ECode FastColorDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return NOERROR;
}

Int32 FastColorDrawable::GetOpacity()
{
    return Elastos::Droid::Graphics::IPixelFormat::OPAQUE;
}

ECode FastColorDrawable::SetBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return NOERROR;
}

ECode FastColorDrawable::SetBounds(
    /* [in] */ IRect* bounds)
{
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
