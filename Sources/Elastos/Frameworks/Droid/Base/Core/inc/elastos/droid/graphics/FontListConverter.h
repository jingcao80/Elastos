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

#ifndef __ELASTOS_DROID_GRAPHICS_FONTLISTCONVERTER_H__
#define __ELASTOS_DROID_GRAPHICS_FONTLISTCONVERTER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/graphics/FontListParser.h"
#include "elastos/droid/graphics/LegacyFontListParser.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::HashMap;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Graphics::LegacyFontListParser::Family)

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * Converts a list of Family to FontListParser.Config
 * {@hide}
 */
class FontListConverter
    : public Object
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    FontListConverter(
        /* [in] */ IList* families,
        /* [in] */ const String& fontDir);

    FontListConverter(
        /* [in] */ LegacyFontListParser::Family* family,
        /* [in] */ const String& fontDir);

    CARAPI_(AutoPtr<FontListParser::Config>) Convert();

protected:
    /**
     *  A "normal" style is just standard font,
     *  eg Roboto is normal. Roboto-Thin is styled.
     */
    CARAPI_(Boolean) IsNormalStyle(
        /* [in] */ LegacyFontListParser::Family* family);

    CARAPI_(AutoPtr<IList>) ConvertFamilies();

    CARAPI_(AutoPtr<FontListParser::Family>) ConvertFamily(
        /* [in] */ LegacyFontListParser::Family* legacyFamily);

    CARAPI_(AutoPtr<IList>) ConvertFonts(
        /* [in] */ IList* fileset);

    CARAPI_(AutoPtr<IList>) CreateAliases();

private:
    CARAPI_(void) FindFamilyVariants();

    CARAPI_(AutoPtr<IList>) FindVariants(
        /* [in] */ LegacyFontListParser::Family* normalFamily,
        /* [in] */ IList* legacyFamilies);

    CARAPI_(AutoPtr<IList>) GetAliasesForRelatedFamilies();

    CARAPI_(AutoPtr<IList>) AdaptNamesetAliases(
        /* [in] */ IList* nameset,
        /* [in] */ const String& toName);

    CARAPI_(AutoPtr<IList>) AdaptNamesetAliases(
        /* [in] */ IList* nameset);

public:
    // Arbitrarily chosen list based on L's fonts.xml.
    // There could be more out there, but we can't use a generic pattern of "fontName-styleName"
    // as "sans-serif" would be translated as a font called "sans" with the style "serif".
    static AutoPtr< ArrayOf<String> > STYLES;

protected:
    // These array values were determined by the order
    // of fonts in a fileset. The order is:
    // "Normal, Bold, Italic, BoldItalic"
    // Additionally the weight values in L's fonts.xml
    // are used to determine a generic weight value for each type
    // e.g The 2nd entry in a fileset is the bold font.
    static AutoPtr< ArrayOf<Int32> > WEIGHTS;
    static AutoPtr< ArrayOf<Boolean> > ITALICS;
    static Int32 DEFAULT_WEIGHT;

private:
    // Maps a "normal" family to a list of similar families differing by style
    // Example: "sans-serif" -> { sans-serif-thin, sans-serif-light, sans-serif-medium }
    HashMap< AutoPtr<LegacyFontListParser::Family>, AutoPtr<IList> > mFamilyVariants;
    AutoPtr<IList> mFamilies;
    String mFontDir;
    static const StaticInitializer sInitializer;
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_FONTLISTCONVERTER_H__
