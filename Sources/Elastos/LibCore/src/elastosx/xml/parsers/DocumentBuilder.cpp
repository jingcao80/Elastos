
#include "Elastos.CoreLibrary.IO.h"
#include "DocumentBuilder.h"
#include "CInputSource.h"
#include "elastosx/xml/parsers/FilePathToURI.h"
#include "utility/logging/Slogger.h"

using Org::Xml::Sax::CInputSource;

namespace Elastosx {
namespace Xml {
namespace Parsers {

CAR_INTERFACE_IMPL(DocumentBuilder, Object, IDocumentBuilder)

ECode DocumentBuilder::Reset()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DocumentBuilder::Parse(
    /* [in]*/ IInputStream* is,
    /* [out]*/ IDocument** doc)
{
    if (is == NULL) {
        // throw new IllegalArgumentException("InputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> in;
    CInputSource::New(is, (IInputSource**)&in);
    return Parse(in, doc);
}

ECode DocumentBuilder::Parse(
    /* [in]*/ IInputStream* is,
    /* [in]*/ const String& systemId,
    /* [out]*/ IDocument** doc)
{
    if (is == NULL) {
        // throw new IllegalArgumentException("InputStream cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> in;
    CInputSource::New(is, (IInputSource**)&in);
    in->SetSystemId(systemId);
    return Parse(in, doc);
}

ECode DocumentBuilder::Parse(
    /* [in]*/ const String& uri,
    /* [out]*/ IDocument** doc)
{
    if (uri == NULL) {
        // throw new IllegalArgumentException("URI cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInputSource> in;
    CInputSource::New(uri, (IInputSource**)&in);
    return Parse(in, doc);
}

ECode DocumentBuilder::Parse(
    /* [in]*/ IFile* f,
    /* [out]*/ IDocument** doc)
{
    if (f == NULL) {
        // throw new IllegalArgumentException("File cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String aPath;
    f->GetAbsolutePath(&aPath);
    String escapedURI = FilePathToURI::Filepath2URI(aPath);

    // if (DEBUG) {
    //     System.out.println("Escaped URI = " + escapedURI);
    // }

    AutoPtr<IInputSource> in;
    CInputSource::New(escapedURI, (IInputSource**)&in);
    return Parse(in, doc);
}

ECode DocumentBuilder::GetSchema(
    /* [out]*/ ISchema** schema)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DocumentBuilder::IsXIncludeAware(
    /* [out]*/ Boolean* isAware)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx
