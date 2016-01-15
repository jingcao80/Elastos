
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/FontListParser.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Utility::Xml;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Graphics {

ECode FontListParser::Parse(
    /* [in] */ IInputStream* in,
    /* [out] */ Config** config) /*throws XmlPullParserException, IOException*/
{
    AutoPtr<IXmlPullParser> parser;
    Int32 nextTag = 0;
    ECode ec = Xml::NewPullParser((IXmlPullParser**)&parser);
    FAIL_GOTO(ec, error);
    ec = parser->SetInput(in, String(NULL));
    FAIL_GOTO(ec, error);
    ec = parser->NextTag(&nextTag);
    FAIL_GOTO(ec, error);
    ec = ReadFamilies(parser, config);
error:
    in->Close();
    return ec;
}

ECode FontListParser::ReadFamilies(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ Config** result) /*throws XmlPullParserException, IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<Config> config = new Config();
    parser->Require(IXmlPullParser::START_TAG, String(NULL), String("familyset"));
    Int32 next = 0;
    while ((parser->Next(&next), next) != IXmlPullParser::END_TAG) {
        Int32 type = 0;
        if ((parser->GetEventType(&type), type) != IXmlPullParser::START_TAG) continue;
        String name;
        parser->GetName(&name);
        if (name.Equals(String("family"))) {
            AutoPtr<Family> f;
            FAIL_RETURN(ReadFamily(parser, (Family**)&f));
            config->mFamilies.PushBack(f);
        }
        else if (name.Equals(String("alias"))) {
            AutoPtr<Alias> a;
            FAIL_RETURN(ReadAlias(parser, (Alias**)&a));
            config->mAliases.PushBack(a);
        }
        else {
            FAIL_RETURN(Skip(parser));
        }
    }
    *result = config;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode FontListParser::ReadFamily(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ Family** result) /*throws XmlPullParserException, IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    String name;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("name"), &name));
    String lang;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("lang"), &lang));
    String variant;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("variant"), &variant));
    AutoPtr<List<AutoPtr<Font> > > fonts = new List<AutoPtr<Font> >();
    Int32 next = 0;
    while ((parser->Next(&next), next) != IXmlPullParser::END_TAG) {
        Int32 type = 0;
        if ((parser->GetEventType(&type), type) != IXmlPullParser::START_TAG) continue;
        String tag;
        FAIL_RETURN(parser->GetName(&tag));
        if (tag.Equals(String("font"))) {
            String weightStr;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("weight"), &weightStr));
            Int32 weight = weightStr == NULL ? 400 : StringUtils::ParseInt32(weightStr);

            String value;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("style"), &value));
            Boolean isItalic = String("italic").Equals(value);
            String filename;
            FAIL_RETURN(parser->NextText(&filename));
            String fullFilename = String("/system/fonts/") + filename;
            fonts->PushBack(new Font(fullFilename, weight, isItalic));
        } else {
            FAIL_RETURN(Skip(parser));
        }
    }
    *result = new Family(name, fonts, lang, variant);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode FontListParser::ReadAlias(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ Alias** result) /*throws XmlPullParserException, IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<Alias> alias = new Alias();
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("name"), &alias->mName));
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("to"), &alias->mToName));
    String weightStr;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("weight"), &weightStr));
    if (weightStr == NULL) {
        alias->mWeight = 400;
    } else {
        alias->mWeight = StringUtils::ParseInt32(weightStr);
    }
    FAIL_RETURN(Skip(parser));  // alias tag is empty, ignore any contents and consume end tag
    *result = alias;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode FontListParser::Skip(
    /* [in] */ IXmlPullParser* parser) /*throws XmlPullParserException, IOException*/
{
    Int32 depth = 1, next = 0;
    while (depth > 0) {
        FAIL_RETURN(parser->Next(&next));
        switch (next) {
            case IXmlPullParser::START_TAG:
                depth++;
                break;
            case IXmlPullParser::END_TAG:
                depth--;
                break;
        }
    }
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
