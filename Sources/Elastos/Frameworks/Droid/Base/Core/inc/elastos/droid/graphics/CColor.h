
#ifndef __ELASTOS_DROID_GRAPHICS_CCOLOR_H__
#define __ELASTOS_DROID_GRAPHICS_CCOLOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Graphics_CColor.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * The Color class defines methods for creating and converting color ints.
 * Colors are represented as packed ints, made up of 4 bytes: alpha, red,
 * green, blue. The values are unpremultiplied, meaning any transparency is
 * stored solely in the alpha component, and not in the color components. The
 * components are stored as follows (alpha << 24) | (red << 16) |
 * (green << 8) | blue. Each component ranges between 0..255 with 0
 * meaning no contribution for that component, and 255 meaning 100%
 * contribution. Thus opaque-black would be 0xFF000000 (100% opaque but
 * no contributions from red, green, or blue), and opaque-white would be
 * 0xFFFFFFFF
 */
CarClass(CColor)
    , public Singleton
    , public IColor
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * Returns the brightness component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    CARAPI Brightness(
        /* [in] */ Int32 color,
        /* [out] */ Float* brightness);

    /**
     * Convert HSV components to an ARGB color. Alpha set to 0xFF.
     *     hsv[0] is Hue [0 .. 360)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * If hsv values are out of range, they are pinned.
     * @param hsv  3 element array which holds the input HSV components.
     * @return the resulting argb color
    */
    CARAPI HSVToColor(
        /* [in] */ ArrayOf<Float>* hsv,
        /* [out] */ Int32* color);

    /**
     * Convert HSV components to an ARGB color. The alpha component is passed
     * through unchanged.
     *     hsv[0] is Hue [0 .. 360)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * If hsv values are out of range, they are pinned.
     * @param alpha the alpha component of the returned argb color.
     * @param hsv  3 element array which holds the input HSV components.
     * @return the resulting argb color
    */
    CARAPI HSVToColor(
        /* [in] */ Int32 alpha,
        /* [in] */ ArrayOf<Float>* hsv,
        /* [out] */ Int32* color);

    /**
     * Convert RGB components to HSV.
     *     hsv[0] is Hue [0 .. 360)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * @param red  red component value [0..255]
     * @param green  green component value [0..255]
     * @param blue  blue component value [0..255]
     * @param hsv  3 element array which holds the resulting HSV components.
     */
    CARAPI RGBToHSV(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [out] */ ArrayOf<Float>* hsv);

    /**
     * Return the alpha component of a color int. This is the same as saying
     * color >>> 24
     */
    CARAPI Alpha(
        /* [in] */ Int32 color,
        /* [out] */ Int32* alpha);

    /**
     * Return a color-int from alpha, red, green, blue components.
     * These component values should be [0..255], but there is no
     * range check performed, so if they are out of range, the
     * returned color is undefined.
     * @param alpha Alpha component [0..255] of the color
     * @param red   Red component [0..255] of the color
     * @param green Green component [0..255] of the color
     * @param blue  Blue component [0..255] of the color
     */
    CARAPI Argb(
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [out] */ Int32* argb);

    /**
     * Return the blue component of a color int. This is the same as saying
     * color & 0xFF
     */
    CARAPI Blue(
        /* [in] */ Int32 color,
        /* [out] */ Int32* blue);

    /**
     * Convert the argb color to its HSV components.
     *     hsv[0] is Hue [0 .. 360)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * @param color the argb color to convert. The alpha component is ignored.
     * @param hsv  3 element array which holds the resulting HSV components.
     */
    CARAPI ColorToHSV(
        /* [in] */ Int32 color,
        /* [out] */ ArrayOf<Float>* hsv);

    /**
     * Return the green component of a color int. This is the same as saying
     * (color >> 8) & 0xFF
     */
    CARAPI Green(
        /* [in] */ Int32 color,
        /* [out] */ Int32* green);

    /**
     * Returns the hue component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    CARAPI Hue(
        /* [in] */ Int32 color,
        /* [out] */ Float* hue);

    /**
     * Parse the color string, and return the corresponding color-int.
     * If the string cannot be parsed, throws an IllegalArgumentException
     * exception. Supported formats are:
     * #RRGGBB
     * #AARRGGBB
     * 'red', 'blue', 'green', 'black', 'white', 'gray', 'cyan', 'magenta',
     * 'yellow', 'lightgray', 'darkgray'
     */
    CARAPI ParseColor(
        /* [in] */ const String& colorString,
        /* [out] */ Int32* color);

    /**
     * Return the red component of a color int. This is the same as saying
     * (color >> 16) & 0xFF
     */
    CARAPI Red(
        /* [in] */ Int32 color,
        /* [out] */ Int32* red);

    /**
     * Return a color-int from red, green, blue components.
     * The alpha component is implicity 255 (fully opaque).
     * These component values should be [0..255], but there is no
     * range check performed, so if they are out of range, the
     * returned color is undefined.
     * @param red  Red component [0..255] of the color
     * @param green Green component [0..255] of the color
     * @param blue  Blue component [0..255] of the color
     */
    CARAPI Rgb(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [out] */ Int32* rgb);

    /**
     * Returns the saturation component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    CARAPI Saturation(
        /* [in] */ Int32 color,
        /* [out] */ Float* saturation);

    /**
     * Convert HSB components to an ARGB color. Alpha set to 0xFF.
     *     hsv[0] is Hue [0 .. 1)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * If hsv values are out of range, they are pinned.
     * @param hsb  3 element array which holds the input HSB components.
     * @return the resulting argb color
     *
     * @hide Pending API council
     */
    CARAPI HSBtoColor(
        /* [in] */ ArrayOf<Float>* hsb,
        /* [out] */ Int32* color);

    /**
     * Convert HSB components to an ARGB color. Alpha set to 0xFF.
     *     hsv[0] is Hue [0 .. 1)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * If hsv values are out of range, they are pinned.
     * @param h Hue component
     * @param s Saturation component
     * @param b Brightness component
     * @return the resulting argb color
     *
     * @hide Pending API council
     */
    CARAPI HSBtoColor(
        /* [in] */ Float h,
        /* [in] */ Float s,
        /* [in] */ Float b,
        /* [out] */ Int32* color);

    /**
     * Converts an HTML color (named or numeric) to an integer RGB value.
     *
     * @param color Non-null color string.
     *
     * @return A color value, or {@code -1} if the color string could not be interpreted.
     *
     * @hide
     */
    static CARAPI_(Int32) GetHtmlColor(
        /* [in] */ const String& color);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CCOLOR_H__
