#ifndef __ELASTOSX_XML_PARSERS_DOCUMENTBUILDER_H__
#define __ELASTOSX_XML_PARSERS_DOCUMENTBUILDER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::IO::IFile;
using Org::Xml::Sax::IInputSource;
using Elastosx::Xml::Validation::ISchema;
using Org::W3c::Dom::IDocument;

namespace Elastosx {
namespace Xml {
namespace Parsers {

class DocumentBuilder
    : public Object
    , public IDocumentBuilder
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Reset();

    CARAPI Parse(
        /* [in]*/ IInputStream* is,
        /* [out]*/ IDocument** doc);

    CARAPI Parse(
        /* [in]*/ IInputStream* is,
        /* [in]*/ String systemId,
        /* [out]*/ IDocument** doc);

    CARAPI Parse(
        /* [in]*/ String uri,
        /* [out]*/ IDocument** doc);

    CARAPI Parse(
        /* [in]*/ IFile* f,
        /* [out]*/ IDocument** doc);

    using IDocumentBuilder::Parse;

    CARAPI GetSchema(
        /* [out]*/ ISchema** schema);

    CARAPI IsXIncludeAware(
        /* [out]*/ Boolean* isAware);
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx

#endif
