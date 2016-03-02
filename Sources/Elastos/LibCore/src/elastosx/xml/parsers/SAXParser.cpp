
#include "Elastos.CoreLibrary.IO.h"
#include "SAXParser.h"
#include "CInputSource.h"
#include "FilePathToURI.h"
#include "utility/logging/Slogger.h"

using Org::Xml::Sax::CInputSource;
using Org::Xml::Sax::IInputSource;
using Org::Xml::Sax::IEntityResolver;
using Org::Xml::Sax::IErrorHandler;
using Org::Xml::Sax::IParser;
using Org::Xml::Sax::IContentHandler;
using Org::Xml::Sax::IXMLReader;

namespace Elastosx {
namespace Xml {
namespace Parsers {

CAR_INTERFACE_IMPL(SAXParser, Object, ISAXParser)

ECode SAXParser::Reset()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode SAXParser::Parse(
    /* [in] */ IInputStream* is,
    /* [in] */ IDocumentHandler* hb)
{
    if (is == NULL) {
        SLOGGERD("SAXParser", "InputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    }

    AutoPtr<IInputSource> input;
    CInputSource::New(is, (IInputSource**)&input);
    return Parse(input, hb);
}

ECode SAXParser::Parse(
    /* [in] */ IInputStream* is,
    /* [in] */ IDocumentHandler* hb,
    /* [in] */ const String& systemId)
{
    if (is == NULL) {
        SLOGGERD("SAXParser", "InputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> input;
    CInputSource::New(is, (IInputSource**)&input);
    input->SetSystemId(systemId);
    return Parse(input, hb);
}

ECode SAXParser::Parse(
    /* [in] */ IInputStream* is,
    /* [in] */ IDTDHandler* dh)
{
    if (is == NULL) {
        SLOGGERD("SAXParser", "InputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> input;
    CInputSource::New(is, (IInputSource**)&input);
    return Parse(input, dh);
}

ECode SAXParser::Parse(
    /* [in] */ IInputStream* is,
    /* [in] */ IDTDHandler* dh,
    /* [in] */ const String& systemId)
{
    if (is == NULL) {
        SLOGGERD("SAXParser", "InputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> input;
    CInputSource::New(is, (IInputSource**)&input);
    input->SetSystemId(systemId);
    return Parse(input, dh);
}

ECode SAXParser::Parse(
    /* [in] */ const String& uri,
    /* [in] */ IDocumentHandler* hb)
{
    if (uri == NULL) {
        SLOGGERD("SAXParser", "uri cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> input;
    CInputSource::New(uri, (IInputSource**)&input);
    return Parse(input, hb);
}

ECode SAXParser::Parse(
    /* [in] */ const String& uri,
    /* [in] */ IDTDHandler* dh)
{
    if (uri == NULL) {
        SLOGGERD("SAXParser", "uri cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> input;
    CInputSource::New(uri, (IInputSource**)&input);
    return Parse(input, dh);
}

ECode SAXParser::Parse(
    /* [in] */ IFile* f,
    /* [in] */ IDocumentHandler* hb)
{
    if (f == NULL) {
        SLOGGERD("SAXParser", "File cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String absPath;
    f->GetAbsolutePath(&absPath);
    String escapedURI = FilePathToURI::Filepath2URI(absPath);

    // if (DEBUG) {
    //     System.out.println("Escaped URI = " + escapedURI);
    // }

    AutoPtr<IInputSource> input;
    CInputSource::New(escapedURI, (IInputSource**)&input);
    return Parse(input, hb);
}

ECode SAXParser::Parse(
    /* [in] */ IFile* f,
    /* [in] */ IDTDHandler* dh)
{
    if (f == NULL) {
        SLOGGERD("SAXParser", "File cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String absPath;
    f->GetAbsolutePath(&absPath);
    String escapedURI = FilePathToURI::Filepath2URI(absPath);

    // if (DEBUG) {
    //     System.out.println("Escaped URI = " + escapedURI);
    // }

    AutoPtr<IInputSource> input;
    CInputSource::New(escapedURI, (IInputSource**)&input);
    return Parse(input, dh);
}

ECode SAXParser::Parse(
    /* [in] */ IInputSource* is,
    /* [in] */ IDocumentHandler* hb)
{
    if (is == NULL) {
        SLOGGERD("SAXParser", "InputSource cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IParser> parser;
    GetParser((IParser**)&parser);
    if (hb != NULL) {
        parser->SetDocumentHandler(hb);
        parser->SetEntityResolver(IEntityResolver::Probe(hb));
        parser->SetErrorHandler(IErrorHandler::Probe(hb));
        parser->SetDTDHandler(IDTDHandler::Probe(hb));
    }
    return parser->Parse(is);
}

ECode SAXParser::Parse(
    /* [in] */ IInputSource* is,
    /* [in] */ IDTDHandler* dh)
{
    if (is == NULL) {
        SLOGGERD("SAXParser", "InputSource cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IXMLReader> reader;
    GetXMLReader((IXMLReader**)&reader);
    if (dh != NULL) {
        reader->SetContentHandler(IContentHandler::Probe(dh));
        reader->SetEntityResolver(IEntityResolver::Probe(dh));
        reader->SetErrorHandler(IErrorHandler::Probe(dh));
        reader->SetDTDHandler(IDTDHandler::Probe(dh));
    }
    return reader->Parse(is);
}

ECode SAXParser::GetSchema(
    /* [out] */ ISchema** schema)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode SAXParser::IsXIncludeAware(
    /* [out] */ Boolean* isAware)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx