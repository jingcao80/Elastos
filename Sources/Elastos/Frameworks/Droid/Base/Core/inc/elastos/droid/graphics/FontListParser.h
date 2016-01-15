
#ifndef __ELASTOS_DROID_GRAPHICS_FONTLISTPARSER_H__
#define __ELASTOS_DROID_GRAPHICS_FONTLISTPARSER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::IO::IInputStream;
using Elastos::Utility::Etl::List;
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
    : public Object
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
        }
    public:
        List<AutoPtr<Family> > mFamilies;
        List<AutoPtr<Alias> > mAliases;
    };

    class Font
        : public Object
    {
    public:
        Font(
            /* [in] */ const String& fontName,
            /* [in] */ Int32 weight,
            /* [in] */ Boolean isItalic)
            : mFontName(fontName)
            , mWeight(weight)
            , mIsItalic(isItalic)
        {
        }

    public:
        String mFontName;
        Int32 mWeight;
        Boolean mIsItalic;
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
            /* [in] */ List<AutoPtr<Font> >* fonts,
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
        AutoPtr<List<AutoPtr<Font> > > mFonts;
        String mLang;
        String mVariant;
    };

public:
    /* Parse fallback list (no names) */
    static CARAPI Parse(
        /* [in] */ IInputStream* in,
        /* [out] */ Config** config) /*throws XmlPullParserException, IOException*/;

private:
    static CARAPI ReadFamilies(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Config** result) /*throws XmlPullParserException, IOException*/;

    static CARAPI ReadFamily(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Family** result) /*throws XmlPullParserException, IOException*/;

    static CARAPI ReadAlias(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Alias** result) /*throws XmlPullParserException, IOException*/;

    static CARAPI Skip(
        /* [in] */ IXmlPullParser* parser) /*throws XmlPullParserException, IOException*/;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_FONTLISTPARSER_H__
