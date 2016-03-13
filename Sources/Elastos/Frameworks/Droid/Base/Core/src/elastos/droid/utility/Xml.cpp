
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/utility/CXmlPullAttributes.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::IO::IReader;
using Elastos::IO::IStringReader;
using Elastos::IO::CStringReader;
using Org::Kxml2::IO::IKXmlParser;
using Org::Kxml2::IO::CKXmlParser;
using Org::Xml::Sax::IXMLReader;
using Org::Xml::Sax::IInputSource;
using Org::Xml::Sax::CInputSource;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserFactoryHelper;
using Org::Xmlpull::V1::CXmlPullParserFactoryHelper;
// using Org::Apache::Harmony::Xml::CExpatReader;

namespace Elastos {
namespace Droid {
namespace Utility {

String Xml::FEATURE_RELAXED("http://xmlpull.org/v1/doc/features.html#relaxed");

const String Xml::XmlSerializerFactory::TYPE("org.kxml2.io.KXmlParser,org.kxml2.io.KXmlSerializer");
AutoPtr<IXmlPullParserFactory> Xml::XmlSerializerFactory::mInstance;

AutoPtr<IXmlPullParserFactory> Xml::XmlSerializerFactory::GetInstance()
{
    if (mInstance == NULL) {
        AutoPtr<IXmlPullParserFactoryHelper> helper;
        CXmlPullParserFactoryHelper::AcquireSingleton((IXmlPullParserFactoryHelper**)&helper);
        helper->NewInstance((IXmlPullParserFactory**)&mInstance);
    }
    return mInstance;
}

const Xml::Encoding Xml::Encoding::US_ASCII(String("US-ASCII"));
const Xml::Encoding Xml::Encoding::UTF_8(String("UTF-8"));
const Xml::Encoding Xml::Encoding::UTF_16(String("UTF-16"));
const Xml::Encoding Xml::Encoding::ISO_8859_1(String("ISO-8859-1"));

ECode Xml::Parse(
    /* [in] */ const String& xml,
    /* [in] */ IContentHandler* contentHandler)
{
    // try {
    AutoPtr<IXMLReader> reader;
    assert(0 && "TODO");
    //CExpatReader::New((IXMLReader**)&reader);
    reader->SetContentHandler(contentHandler);

    ECode ec = NOERROR;
    AutoPtr<IStringReader> sr;
    ec = CStringReader::New(xml, (IStringReader**)&sr);
    if (ec == (ECode)E_IO_EXCEPTION) return E_ASSERTION_ERROR;

    AutoPtr<IInputSource> source;
    ec = CInputSource::New(IReader::Probe(sr), (IInputSource**)&source);
    if (ec == (ECode)E_IO_EXCEPTION) return E_ASSERTION_ERROR;

    ec = reader->Parse(source);
    if (ec == (ECode)E_IO_EXCEPTION) return E_ASSERTION_ERROR;
    // } catch (IOException e) {
    //     throw new AssertionError(e);
    // }
    return NOERROR;
}

ECode Xml::Parse(
    /* [in] */ IReader* in,
    /* [in] */ IContentHandler* contentHandler)
{
    AutoPtr<IXMLReader> reader;
    assert(0 && "TODO");
    //CExpatReader::New((IXMLReader**)&reader);
    reader->SetContentHandler(contentHandler);

    AutoPtr<IInputSource> source;
    CInputSource::New(in, (IInputSource**)&source);

    return reader->Parse(source);
}

ECode Xml::Parse(
    /* [in] */ IInputStream* in,
    /* [in] */ const Xml::Encoding& encoding,
    /* [in] */ IContentHandler* contentHandler)
{
    AutoPtr<IXMLReader> reader;
    assert(0 && "TODO");
    //CExpatReader::New((IXMLReader**)&reader);
    reader->SetContentHandler(contentHandler);

    AutoPtr<IInputSource> source;
    CInputSource::New(in, (IInputSource**)&source);
    source->SetEncoding(encoding.mExpatName);

    return reader->Parse(source);
}

ECode Xml::NewPullParser(
    /* [out] */ IXmlPullParser** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    // try {
    AutoPtr<IXmlPullParser> parser;
    ECode ec = CKXmlParser::New((IXmlPullParser**)&parser);
    FAIL_GOTO(ec, _EXIT)

    ec = parser->SetFeature(IXmlPullParser::FEATURE_PROCESS_DOCDECL, TRUE);
    FAIL_GOTO(ec, _EXIT)

    ec = parser->SetFeature(IXmlPullParser::FEATURE_PROCESS_NAMESPACES, TRUE);
    FAIL_GOTO(ec, _EXIT)

    *result = parser;
    REFCOUNT_ADD(*result)
    return NOERROR;

_EXIT:
    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        return E_ASSERTION_ERROR;
    }

    return ec;
}

ECode Xml::NewSerializer(
    /* [out] */ IXmlSerializer** xs)
{
    // try {
    AutoPtr<IXmlPullParserFactory> fact = XmlSerializerFactory::GetInstance();
    return fact->NewSerializer(xs);
    // } catch (XmlPullParserException e) {
    //     throw new AssertionError(e);
    // }
}

Xml::Encoding Xml::FindEncodingByName(
    /* [in] */ const String& encodingName)
{
    if (encodingName.IsNull()) {
        return Encoding::UTF_8;
    }

    if (encodingName.EqualsIgnoreCase(Encoding::US_ASCII.mExpatName)) {
        return Encoding::US_ASCII;
    }
    else if (encodingName.EqualsIgnoreCase(Encoding::UTF_8.mExpatName)) {
        return Encoding::UTF_8;
    }
    else if (encodingName.EqualsIgnoreCase(Encoding::UTF_16.mExpatName)) {
        return Encoding::UTF_16;
    }
    else if (encodingName.EqualsIgnoreCase(Encoding::ISO_8859_1.mExpatName)) {
        return Encoding::ISO_8859_1;
    }

    assert(0 && "UnsupportedEncodingException");
    return Encoding::UTF_8;
}

AutoPtr<IAttributeSet> Xml::AsAttributeSet(
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr<IAttributeSet> set = IAttributeSet::Probe(parser);
    if (set == NULL) {
        CXmlPullAttributes::New(parser, (IAttributeSet**)&set);
    }

    return set;
}


} // namespace Utility
} // namespace Droid
} // namespace Elastos
