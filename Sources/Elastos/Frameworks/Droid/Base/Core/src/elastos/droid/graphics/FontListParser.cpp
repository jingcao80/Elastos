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

#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/FontListConverter.h"
#include "elastos/droid/graphics/FontListParser.h"
#include "elastos/droid/graphics/LegacyFontListParser.h"
#include "elastos/droid/graphics/fonts/FontVariationAxis.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::Fonts::FontVariationAxis;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;

namespace Elastos {
namespace Droid {
namespace Graphics {

AutoPtr<IPattern> FontListParser::FILENAME_WHITESPACE_PATTERN = Init_FILENAME_WHITESPACE_PATTERN();

ECode FontListParser::Parse(
    /* [in] */ IFile* configFilename,
    /* [in] */ IFile* fontDir,
    /* [out] */ Config** config)
{
    *config = NULL;
    AutoPtr<IFileInputStream> in;
    CFileInputStream::New(configFilename, (IFileInputStream**)&in);
    Boolean result;
    FAIL_RETURN(IsLegacyFormat(configFilename, &result));
    if (result) {
        String path;
        fontDir->GetAbsolutePath(&path);
        return ParseLegacyFormat(IInputStream::Probe(in), path, config);
    }
    else {
        String path;
        fontDir->GetAbsolutePath(&path);
        return ParseNormalFormat(IInputStream::Probe(in), path, config);
    }
}

ECode FontListParser::IsLegacyFormat(
    /* [in] */ IFile* configFilename,
    /* [out] */ Boolean* result)
{
    AutoPtr<IFileInputStream> in;
    CFileInputStream::New(configFilename, (IFileInputStream**)&in);
    Boolean isLegacy = FALSE;
    // try {
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(IInputStream::Probe(in), String(NULL));
    String version;
    Int32 tag;
    ECode ec = parser->NextTag(&tag);
    FAIL_GOTO(ec, EXIT);
    ec = parser->Require(IXmlPullParser::START_TAG, String(NULL), String("familyset"));
    FAIL_GOTO(ec, EXIT);
    ec = parser->GetAttributeValue(String(NULL), String("version"), &version);
    FAIL_GOTO(ec, EXIT);
    isLegacy = version.IsNull();
    // } finally {
EXIT:
    IInputStream::Probe(in)->Close();
    // }
    *result = isLegacy;
    return ec;
}

ECode FontListParser::ParseLegacyFormat(
    /* [in] */ IInputStream* in,
    /* [in] */ const String& dirName,
    /* [out] */ Config** config)
{
    VALIDATE_NOT_NULL(config);
    *config = NULL;
    // try {
    AutoPtr<IList> legacyFamilies;
    ECode ec = LegacyFontListParser::Parse(in, (IList**)&legacyFamilies);
    if (FAILED(ec)) {
        in->Close();
        return ec;
    }
    AutoPtr<FontListConverter> converter = new FontListConverter(legacyFamilies, dirName);
    AutoPtr<Config> cfg = converter->Convert();
    *config = cfg;
    REFCOUNT_ADD(*config);
    in->Close();
    return NOERROR;
    // } finally {
    //     in.close();
    // }
}

ECode FontListParser::ParseNormalFormat(
    /* [in] */ IInputStream* in,
    /* [in] */ const String& dirName,
    /* [out] */ Config** config)
{
    VALIDATE_NOT_NULL(config);
    *config = NULL;
    // try {
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(in, String(NULL));
    Int32 tag;
    ECode ec = parser->NextTag(&tag);
    if (FAILED(ec)) {
        in->Close();
        return ec;
    }
    ec = ReadFamilies(parser, dirName, config);
    in->Close();
    return ec;
    // } finally {
    //     in.close();
    // }
}

ECode FontListParser::ReadFamilies(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& dirPath,
    /* [out] */ Config** result)
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
            FAIL_RETURN(ReadFamily(parser, dirPath, (Family**)&f));
            config->mFamilies->Add((IObject*)f);
        }
        else if (name.Equals(String("alias"))) {
            AutoPtr<Alias> a;
            FAIL_RETURN(ReadAlias(parser, (Alias**)&a));
            config->mAliases->Add((IObject*)a);
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
    /* [in] */ const String& dirPath,
    /* [out] */ Family** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    String name;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("name"), &name));
    String lang;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("lang"), &lang));
    String variant;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("variant"), &variant));
    AutoPtr<IList> fonts;
    CArrayList::New((IList**)&fonts);
    AutoPtr<IFileHelper> helper;
    CFileHelper::AcquireSingleton((IFileHelper**)&helper);
    Char32 separatorChar;
    helper->GetSeparatorChar(&separatorChar);
    String prefix(dirPath);
    prefix.Append(separatorChar);
    Int32 next = 0;
    while ((parser->Next(&next), next) != IXmlPullParser::END_TAG) {
        Int32 type = 0;
        if ((parser->GetEventType(&type), type) != IXmlPullParser::START_TAG) continue;
        String tag;
        FAIL_RETURN(parser->GetName(&tag));
        if (tag.Equals(String("font"))) {
            AutoPtr<Font> font;
            FAIL_RETURN(ReadFont(parser, (Font**)&font));
            fonts->Add((IObject*)font);
        }
        else {
            FAIL_RETURN(Skip(parser));
        }
    }
    *result = new Family(name, fonts, lang, variant);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode FontListParser::ReadFont(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ Font** result)
{
    String indexStr;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("index"), &indexStr));
    Int32 index = indexStr.IsNull() ? 0 : StringUtils::ParseInt32(indexStr);

    AutoPtr<IList> axes;
    CArrayList::New((IList**)&axes);

    String weightStr;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("weight"), &weightStr));
    Int32 weight = weightStr.IsNull() ? 400 : StringUtils::ParseInt32(weightStr);

    String value;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("style"), &value));
    Boolean isItalic = value.Equals("italic");
    StringBuilder sb;
    Int32 next;
    while (parser->Next(&next), next != IXmlPullParser::END_TAG) {
        Int32 type = 0;
        if (parser->GetEventType(&type), type == IXmlPullParser::TEXT) {
            String text;
            parser->GetText(&text);
            sb += text;
        }
        if (parser->GetEventType(&type), type != IXmlPullParser::START_TAG) continue;
        String tag;
        parser->GetName(&tag);
        if (tag.Equals("axis")) {
            AutoPtr<IFontVariationAxis> axis;
            FAIL_RETURN(ReadAxis(parser, (IFontVariationAxis**)&axis));
            axes->Add(axis);
        }
        else {
            Skip(parser);
        }
    }
    AutoPtr<IMatcher> matcher;
    FILENAME_WHITESPACE_PATTERN->Matcher(sb.ToString(), (IMatcher**)&matcher);
    String sanitizedName;
    matcher->ReplaceAll(String(""), &sanitizedName);
    AutoPtr< ArrayOf<IFontVariationAxis*> > axesArray;
    axes->ToArray((ArrayOf<IInterface*>**)&axesArray);
    AutoPtr<Font> font = new Font(sanitizedName, index, axesArray, weight, isItalic);
    *result = font;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode FontListParser::ReadAxis(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFontVariationAxis** result)
{
    String tagStr;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("tag"), &tagStr));
    String styleValueStr;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("stylevalue"), &styleValueStr));
    Skip(parser); // axis tag is empty, ignore any contents and consume end tag
    AutoPtr<FontVariationAxis> axis = new FontVariationAxis();
    FAIL_RETURN(axis->constructor(tagStr, StringUtils::ParseFloat(styleValueStr)));
    *result = axis;
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

AutoPtr<IPattern> FontListParser::Init_FILENAME_WHITESPACE_PATTERN()
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(String("^[ \\n\\r\\t]+|[ \\n\\r\\t]+$"), (IPattern**)&pattern);
    return pattern;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
