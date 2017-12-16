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

#ifndef __ELASTOS_DROID_GRAPHICS_FONTS_FONTVARIATIONAXIS_H__
#define __ELASTOS_DROID_GRAPHICS_FONTS_FONTVARIATIONAXIS_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Fonts {

/**
 * Class that holds information about single font variation axis.
 */
class FontVariationAxis
    : public Object
    , public IFontVariationAxis
{
public:
    CARAPI constructor(
        /* [in] */ const String& tagString,
        /* [in] */ Float styleValue);

    CAR_INTERFACE_DECL()

    /**
     * Returns the OpenType style tag value.
     * @hide
     */
    CARAPI GetOpenTypeTagValue(
        /* [out] */ Int32* tagValue);

    /**
     * Returns the variable font axis tag associated to this axis.
     */
    CARAPI GetTag(
        /* [out] */ String* tag);

    /**
     * Returns the style value associated to the given axis for this font.
     */
    CARAPI GetStyleValue(
        /* [out] */ Float* styleValue);

    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI_(Int32) MakeTag(
        /* [in] */ const String& tagString);

private:
    static CARAPI_(Boolean) IsValidTag(
        /* [in] */ const String& tagString);

    static CARAPI_(AutoPtr<IPattern>) Init_TAG_PATTERN();

private:
    Int32 mTag;
    String mTagString;
    Float mStyleValue;

    static const AutoPtr<IPattern> TAG_PATTERN;
};

} // namespace Fonts
} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_FONTS_FONTVARIATIONAXIS_H__
