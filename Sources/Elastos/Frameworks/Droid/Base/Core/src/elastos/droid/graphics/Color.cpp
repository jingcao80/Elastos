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

#include <Elastos.CoreLibrary.Core.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/droid/utility/MathUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <skia/core/SkScalar.h>
#include <skia/core/SkColor.h>

using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Graphics {

static AutoPtr<HashMap<String, Int32> > InitColorNameMap()
{
    AutoPtr<HashMap<String, Int32> > map = new HashMap<String, Int32>(23);
    assert(map != NULL);
    (*map)[String("black")] = IColor::BLACK;
    (*map)[String("darkgray")] = IColor::DKGRAY;
    (*map)[String("gray")] = IColor::GRAY;
    (*map)[String("lightgray")] = IColor::LTGRAY;
    (*map)[String("white")] = IColor::WHITE;
    (*map)[String("red")] = IColor::RED;
    (*map)[String("green")] = IColor::GREEN;
    (*map)[String("blue")] = IColor::BLUE;
    (*map)[String("yellow")] = IColor::YELLOW;
    (*map)[String("cyan")] = IColor::CYAN;
    (*map)[String("magenta")] = IColor::MAGENTA;
    (*map)[String("aqua")] =  0xFF00FFFF;
    (*map)[String("fuchsia")] =  0xFFFF00FF;
    (*map)[String("darkgrey")] =  IColor::DKGRAY;
    (*map)[String("grey")] =  IColor::GRAY;
    (*map)[String("lightgrey")] =  IColor::LTGRAY;
    (*map)[String("lime")] =  0xFF00FF00;
    (*map)[String("maroon")] =  0xFF800000;
    (*map)[String("navy")] =  0xFF000080;
    (*map)[String("olive")] =  0xFF808000;
    (*map)[String("purple")] =  0xFF800080;
    (*map)[String("silver")] =  0xFFC0C0C0;
    (*map)[String("teal")] =  0xFF008080;
    return map;
}

AutoPtr<HashMap<String, Int32> > Color::sColorNameMap = InitColorNameMap();
Int32 Color::Alpha(
    /* [in] */ Int32 color)
{
    return (UInt32)color >> 24;
}

Int32 Color::Red(
    /* [in] */ Int32 color)
{
    return (color >> 16) & 0xFF;
}

Int32 Color::Green(
    /* [in] */ Int32 color)
{
    return (color >> 8) & 0xFF;
}

Int32 Color::Blue(
    /* [in] */ Int32 color)
{
    return color & 0xFF;
}

Int32 Color::Rgb(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue)
{
    return (0xFF << 24) | (red << 16) | (green << 8) | blue;
}

Int32 Color::Argb(
    /* [in] */ Int32 alpha,
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue)
{
    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

Float Color::Hue(
    /* [in] */ Int32 color)
{
    Int32 r = (color >> 16) & 0xFF;
    Int32 g = (color >> 8) & 0xFF;
    Int32 b = color & 0xFF;

    Int32 V = Elastos::Core::Math::Max(b, Elastos::Core::Math::Max(r, g));
    Int32 temp = Elastos::Core::Math::Min(b, Elastos::Core::Math::Min(r, g));

    Float H;

    if (V == temp) {
        H = 0;
    }
    else {
        const Float vtemp = (Float) (V - temp);
        const Float cr = (V - r) / vtemp;
        const Float cg = (V - g) / vtemp;
        const Float cb = (V - b) / vtemp;

        if (r == V) {
            H = cb - cg;
        }
        else if (g == V) {
            H = 2 + cr - cb;
        }
        else {
            H = 4 + cg - cr;
        }

        H /= 6.f;
        if (H < 0) {
            H++;
        }
    }

    return H;
}

Float Color::Saturation(
    /* [in] */ Int32 color)
{
    Int32 r = (color >> 16) & 0xFF;
    Int32 g = (color >> 8) & 0xFF;
    Int32 b = color & 0xFF;


    Int32 V = Elastos::Core::Math::Max(b, Elastos::Core::Math::Max(r, g));
    Int32 temp = Elastos::Core::Math::Min(b, Elastos::Core::Math::Min(r, g));

    Float S;

    if (V == temp) {
        S = 0;
    }
    else {
        S = (V - temp) / (Float) V;
    }

    return S;
}

Float Color::Brightness(
    /* [in] */ Int32 color)
{

    Int32 r = (color >> 16) & 0xFF;
    Int32 g = (color >> 8) & 0xFF;
    Int32 b = color & 0xFF;

    Int32 V = Elastos::Core::Math::Max(b, Elastos::Core::Math::Max(r, g));

    return (V / 255.f);
}

ECode Color::ParseColor(
    /* [in] */ const String& colorString,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);

    if (colorString.GetChar(0) == '#') {
       // Use a long to avoid rollovers on #ffXXXXXX
        Int64 c = StringUtils::ParseInt64(colorString.Substring(1), 16);
        if (colorString.GetLength() == 7) {
            // Set the alpha value
            c |= 0x00000000ff000000;
        }
        else if (colorString.GetLength() != 9) {
            //throw new IllegalArgumentException("Unknown color");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *color = (Int32)c;
        return NOERROR;
    }
    else {
        String cstr = colorString.ToLowerCase(/*Locale.ROOT*/);
        HashMap<String, Int32>::Iterator it = sColorNameMap->Find(cstr);
        if (it != sColorNameMap->End()) {
            *color = it->mSecond;
            return NOERROR;
        }
    }
    // throw new IllegalArgumentException("Unknown color");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

Int32 Color::HSBtoColor(
    /* [in] */ ArrayOf<Float>* hsb)
{
    return HSBtoColor((*hsb)[0], (*hsb)[1], (*hsb)[2]);
}

Int32 Color::GetHtmlColor(
    /* [in] */ const String& color)
{
    HashMap<String, Int32>::Iterator it = sColorNameMap->Find(color.ToLowerCase(/*Locale.ROOT*/));
    if (it != sColorNameMap->End()) {
        return it->mSecond;
    }
    else {
        // try {
        AutoPtr<ICharSequence> cs;
        CString::New(color, (ICharSequence**)&cs);
        return XmlUtils::ConvertValueToInt32(cs, -1);
        // } catch (NumberFormatException nfe) {
        //     return -1;
        // }
    }
    return -1;
}

Int32 Color::HSBtoColor(
    /* [in] */ Float h,
    /* [in] */ Float s,
    /* [in] */ Float b)
{
    h = MathUtils::Constrain(h, 0.0f, 1.0f);
    s = MathUtils::Constrain(s, 0.0f, 1.0f);
    b = MathUtils::Constrain(b, 0.0f, 1.0f);

    Float red = 0.0f;
    Float green = 0.0f;
    Float blue = 0.0f;

    Float hf = (h - (Int32) h) * 6.0f;
    Int32 ihf = (Int32) hf;
    Float f = hf - ihf;
    Float pv = b * (1.0f - s);
    Float qv = b * (1.0f - s * f);
    Float tv = b * (1.0f - s * (1.0f - f));

    switch (ihf) {
        case 0:         // Red is the dominant color
            red = b;
            green = tv;
            blue = pv;
            break;
        case 1:         // Green is the dominant color
            red = qv;
            green = b;
            blue = pv;
            break;
        case 2:
            red = pv;
            green = b;
            blue = tv;
            break;
        case 3:         // Blue is the dominant color
            red = pv;
            green = qv;
            blue = b;
            break;
        case 4:
            red = tv;
            green = pv;
            blue = b;
            break;
        case 5:         // Red is the dominant color
            red = b;
            green = pv;
            blue = qv;
            break;
    }

    return 0xFF000000 | (((Int32) (red * 255.0f)) << 16) |
            (((Int32) (green * 255.0f)) << 8) | ((Int32) (blue * 255.0f));
}

ECode Color::RGBToHSV(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [out] */ ArrayOf<Float>* hsv)
{
    VALIDATE_NOT_NULL(hsv);

    if (hsv->GetLength() < 3) {
        // throw new RuntimeException("3 components required for hsv");
        return E_RUNTIME_EXCEPTION;
    }

    NativeRGBToHSV(red, green, blue, hsv);
    return NOERROR;
}

ECode Color::ColorToHSV(
    /* [in] */ Int32 color,
    /* [out] */ ArrayOf<Float>* hsv)
{
    VALIDATE_NOT_NULL(hsv);

    return RGBToHSV((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, hsv);
}

ECode Color::HSVToColor(
    /* [in] */ ArrayOf<Float>* hsv,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);

    return HSVToColor(0xFF, hsv, color);
}

ECode Color::HSVToColor(
    /* [in] */ Int32 alpha,
    /* [in] */ ArrayOf<Float>* hsv,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);

    if (hsv->GetLength() < 3) {
        // throw new RuntimeException("3 components required for hsv");
        return E_RUNTIME_EXCEPTION;
    }
    *color = NativeHSVToColor(alpha, hsv);
    return NOERROR;
}

void Color::NativeRGBToHSV(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in, out] */ ArrayOf<Float>* hsvArray)
{
    SkScalar hsv[3];
    SkRGBToHSV(red, green, blue, hsv);

    SkASSERT(hsvArray);
    if (hsvArray->GetLength() < 3) {
        sk_throw();
    }

    Float* values = hsvArray->GetPayload();
    for (Int32 i = 0; i < 3; i++) {
        values[i] = SkScalarToFloat(hsv[i]);
    }
}

Int32 Color::NativeHSVToColor(
    /* [in] */ Int32 alpha,
    /* [in] */ ArrayOf<Float>* hsvArray)
{
    if (hsvArray->GetLength() < 3) {
        sk_throw();
        return 0;
    }

#ifdef SK_SCALAR_IS_FLOAT
    SkScalar*   hsv = hsvArray->GetPayload();
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif

    return static_cast<Int32>(SkHSVToColor(alpha, hsv));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
