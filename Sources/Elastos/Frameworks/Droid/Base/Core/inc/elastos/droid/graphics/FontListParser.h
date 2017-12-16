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

#ifndef __ELASTOS_DROID_GRAPHICS_FONTLISTPARSER_H__
#define __ELASTOS_DROID_GRAPHICS_FONTLISTPARSER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Fonts::IFontVariationAxis;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Regex::IPattern;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * Parser for font config files.
 *
 * @hide
 */
class FontListParser
{
public:
    class Family;
    class Alias;
    class Config
        : public Object
    {
    public:
        Config()
        {
            CArrayList::New((IList**)&mFamilies);
            CArrayList::New((IList**)&mAliases);
        }

    public:
        AutoPtr<IList> mFamilies;
        AutoPtr<IList> mAliases;
    };

    class Font
        : public Object
    {
    public:
        Font(
            /* [in] */ const String& fontName,
            /* [in] */ Int32 ttcIndex,
            /* [in] */ ArrayOf<IFontVariationAxis*>* axes,
            /* [in] */ Int32 weight,
            /* [in] */ Boolean isItalic)
            : mFontName(fontName)
            , mTtcIndex(ttcIndex)
            , mAxes(axes)
            , mWeight(weight)
            , mIsItalic(isItalic)
        {}

    public:
        String mFontName;
        Int32 mTtcIndex;
        AutoPtr< ArrayOf<IFontVariationAxis*> > mAxes;
        Int32 mWeight;
        Boolean mIsItalic;
        AutoPtr<IUri> mUri;
    };

    class Alias
        : public Object
    {
    public:
        Alias()
            : mWeight(0)
        {}

    public:
        String mName;
        String mToName;
        Int32 mWeight;
    };

    class Family
        : public Object
    {
    public:
        Family(
            /* [in] */ const String& name,
            /* [in] */ IList* fonts,
            /* [in] */ const String& lang,
            /* [in] */ const String& variant)
            : mName(name)
            , mFonts(fonts)
            , mLang(lang)
            , mVariant(variant)
        {
        }

    public:
        String mName;
        AutoPtr<IList> mFonts;
        String mLang;
        String mVariant;
    };

public:
    /* Parse fallback list (no names) */
    static CARAPI Parse(
        /* [in] */ IFile* configFilename,
        /* [in] */ IFile* fontDir,
        /* [out] */ Config** config);

    static CARAPI ParseLegacyFormat(
        /* [in] */ IInputStream* in,
        /* [in] */ const String& dirName,
        /* [out] */ Config** config);

    static CARAPI ParseNormalFormat(
        /* [in] */ IInputStream* in,
        /* [in] */ const String& dirName,
        /* [out] */ Config** config);

private:
    static CARAPI IsLegacyFormat(
        /* [in] */ IFile* configFilename,
        /* [out] */ Boolean* result);

    static CARAPI ReadFamilies(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& dirPath,
        /* [out] */ Config** result);

    static CARAPI ReadFamily(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& dirPath,
        /* [out] */ Family** result);

    static CARAPI ReadFont(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Font** result);

    static CARAPI ReadAxis(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IFontVariationAxis** result);

    static CARAPI ReadAlias(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Alias** result);

    static CARAPI Skip(
        /* [in] */ IXmlPullParser* parser);

    static CARAPI_(AutoPtr<IPattern>) Init_FILENAME_WHITESPACE_PATTERN();

private:
    static AutoPtr<IPattern> FILENAME_WHITESPACE_PATTERN;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_FONTLISTPARSER_H__
