
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CColor.h"
#include "elastos/droid/graphics/Color.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CColor);
CAR_INTERFACE_IMPL(CColor, Object, IColor);
ECode CColor::Alpha(
    /* [in] */ Int32 color,
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);

    *alpha = Color::Alpha(color);
    return NOERROR;
}

ECode CColor::Red(
    /* [in] */ Int32 color,
    /* [out] */ Int32* red)
{
    VALIDATE_NOT_NULL(red);

    *red = Color::Red(color);
    return NOERROR;
}

ECode CColor::Green(
    /* [in] */ Int32 color,
    /* [out] */ Int32* green)
{
    VALIDATE_NOT_NULL(green);

    *green = Color::Green(color);
    return NOERROR;
}

ECode CColor::Blue(
    /* [in] */ Int32 color,
    /* [out] */ Int32* blue)
{
    VALIDATE_NOT_NULL(blue);

    *blue = Color::Blue(color);
    return NOERROR;
}

ECode CColor::Rgb(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [out] */ Int32* rgb)
{
    VALIDATE_NOT_NULL(rgb);

    *rgb = Color::Rgb(red, green, blue);
    return NOERROR;
}

ECode CColor::Argb(
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [out] */ Int32* argb)
{
    VALIDATE_NOT_NULL(argb);

    *argb = Color::Argb(alpha, red, green, blue);
    return NOERROR;
}

ECode CColor::Hue(
    /* [in] */ Int32 color,
    /* [out] */ Float* hue)
{
    VALIDATE_NOT_NULL(hue);

    *hue = Color::Hue(color);
    return NOERROR;
}

ECode CColor::Saturation(
    /* [in] */ Int32 color,
    /* [out] */ Float* saturation)
{
    VALIDATE_NOT_NULL(saturation);

    *saturation = Color::Saturation(color);
    return NOERROR;
}

ECode CColor::Brightness(
    /* [in] */ Int32 color,
    /* [out] */ Float* brightness)
{
    VALIDATE_NOT_NULL(brightness);

    *brightness = Color::Brightness(color);
    return NOERROR;
}

ECode CColor::ParseColor(
    /* [in] */ const String& colorString,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    return Color::ParseColor(colorString, color);
}

ECode CColor::HSBtoColor(
    /* [in] */ ArrayOf<Float>* hsb,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);

    *color = Color::HSBtoColor(hsb);
    return NOERROR;
}

Int32 CColor::GetHtmlColor(
    /* [in] */ const String& color)
{
    return Color::GetHtmlColor(color);
}

ECode CColor::HSBtoColor(
    /* [in] */ Float h,
    /* [in] */ Float s,
    /* [in] */ Float b,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);

    *color = Color::HSBtoColor(h, s, b);
    return NOERROR;
}

ECode CColor::RGBToHSV(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [out] */ ArrayOf<Float>* hsv)
{
    VALIDATE_NOT_NULL(hsv);
    return Color::RGBToHSV(red, green, blue, hsv);
}

ECode CColor::ColorToHSV(
    /* [in] */ Int32 color,
    /* [out] */ ArrayOf<Float>* hsv)
{
    VALIDATE_NOT_NULL(hsv);
    return Color::ColorToHSV(color, hsv);
}

ECode CColor::HSVToColor(
    /* [in] */ ArrayOf<Float>* hsv,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    return Color::HSVToColor(hsv, color);
}

ECode CColor::HSVToColor(
    /* [in] */ Int32 alpha,
    /* [in] */ ArrayOf<Float>* hsv,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    return Color::HSVToColor(alpha, hsv, color);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
