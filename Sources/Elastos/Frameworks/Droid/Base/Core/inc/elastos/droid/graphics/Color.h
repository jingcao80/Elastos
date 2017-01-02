//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_GRAPHICS_COLOR_H__
#define __ELASTOS_DROID_GRAPHICS_COLOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

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
class Color
{
public:
    /**
     * Returns the brightness component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    static CARAPI_(Float) Brightness(
        /* [in] */ Int32 color);

    /**
     * Convert HSV components to an ARGB color. Alpha set to 0xFF.
     *     hsv[0] is Hue [0 .. 360)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * If hsv values are out of range, they are pinned.
     * @param hsv  3 element array which holds the input HSV components.
     * @return the resulting argb color
    */
    static CARAPI HSVToColor(
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
    static CARAPI HSVToColor(
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
    static CARAPI RGBToHSV(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [out] */ ArrayOf<Float>* hsv);

    /**
     * Return the alpha component of a color int. This is the same as saying
     * color >>> 24
     */
    static CARAPI_(Int32) Alpha(
        /* [in] */ Int32 color);

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
    static CARAPI_(Int32) Argb(
        /* [in] */ Int32 alpha,
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue);

    /**
     * Return the blue component of a color int. This is the same as saying
     * color & 0xFF
     */
    static CARAPI_(Int32) Blue(
        /* [in] */ Int32 color);

    /**
     * Convert the argb color to its HSV components.
     *     hsv[0] is Hue [0 .. 360)
     *     hsv[1] is Saturation [0...1]
     *     hsv[2] is Value [0...1]
     * @param color the argb color to convert. The alpha component is ignored.
     * @param hsv  3 element array which holds the resulting HSV components.
     */
    static CARAPI ColorToHSV(
        /* [in] */ Int32 color,
        /* [out] */ ArrayOf<Float>* hsv);

    /**
     * Return the green component of a color int. This is the same as saying
     * (color >> 8) & 0xFF
     */
    static CARAPI_(Int32) Green(
        /* [in] */ Int32 color);

    /**
     * Returns the hue component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    static CARAPI_(Float) Hue(
        /* [in] */ Int32 color);

    /**
     * Parse the color string, and return the corresponding color-int.
     * If the string cannot be parsed, throws an IllegalArgumentException
     * exception. Supported formats are:
     * #RRGGBB
     * #AARRGGBB
     * 'red', 'blue', 'green', 'black', 'white', 'gray', 'cyan', 'magenta',
     * 'yellow', 'lightgray', 'darkgray'
     */
    static CARAPI ParseColor(
        /* [in] */ const String& colorString,
        /* [out] */ Int32* color);

    /**
     * Return the red component of a color int. This is the same as saying
     * (color >> 16) & 0xFF
     */
    static CARAPI_(Int32) Red(
        /* [in] */ Int32 color);

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
    static CARAPI_(Int32) Rgb(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue);

    /**
     * Returns the saturation component of a color int.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @hide Pending API council
     */
    static CARAPI_(Float) Saturation(
        /* [in] */ Int32 color);

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
    static CARAPI_(Int32) HSBtoColor(
        /* [in] */ ArrayOf<Float>* hsb);

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
    static CARAPI_(Int32) HSBtoColor(
        /* [in] */ Float h,
        /* [in] */ Float s,
        /* [in] */ Float b);

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

private:
    static CARAPI_(void) NativeRGBToHSV(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [out] */ ArrayOf<Float>* hsv);

    static CARAPI_(Int32) NativeHSVToColor(
        /* [in] */ Int32 alpha,
        /* [in] */ ArrayOf<Float>* hsv);

private:
    static AutoPtr<HashMap<String, Int32> > sColorNameMap;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_COLOR_H__
