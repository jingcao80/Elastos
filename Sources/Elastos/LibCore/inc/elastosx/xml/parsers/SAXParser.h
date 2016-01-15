#ifndef __ELASTOSX_XML_PARSERS_SAXPARSER_H__
#define __ELASTOSX_XML_PARSERS_SAXPARSER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Org::Xml::Sax::IDocumentHandler;
using Org::Xml::Sax::IDTDHandler;
using Org::Xml::Sax::IInputSource;
using Elastosx::Xml::Validation::ISchema;

namespace Elastosx {
namespace Xml {
namespace Parsers {

class SAXParser
    : public ISAXParser
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Reset();

    CARAPI Parse(
        /* [in] */ IInputStream* is,
        /* [in] */ IDocumentHandler* hb);

    CARAPI Parse(
        /* [in] */ IInputStream* is,
        /* [in] */ IDocumentHandler* hb,
        /* [in] */ String systemId);

    CARAPI Parse(
        /* [in] */ IInputStream* is,
        /* [in] */ IDTDHandler* dh);

    CARAPI Parse(
        /* [in] */ IInputStream* is,
        /* [in] */ IDTDHandler* dh,
        /* [in] */ String systemId);

    CARAPI Parse(
        /* [in] */ String uri,
        /* [in] */ IDocumentHandler* hb);

    CARAPI Parse(
        /* [in] */ String uri,
        /* [in] */ IDTDHandler* dh);

    CARAPI Parse(
        /* [in] */ IFile* f,
        /* [in] */ IDocumentHandler* hb);

    CARAPI Parse(
        /* [in] */ IFile* f,
        /* [in] */ IDTDHandler* dh);

    CARAPI Parse(
        /* [in] */ IInputSource* is,
        /* [in] */ IDocumentHandler* hb);

    CARAPI Parse(
        /* [in] */ IInputSource* is,
        /* [in] */ IDTDHandler* dh);

    CARAPI GetSchema(
        /* [out] */ ISchema** schema);

    CARAPI IsXIncludeAware(
        /* [out] */ Boolean* isAware);
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx
#endif