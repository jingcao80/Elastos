
#ifndef __ORG_APACHE_HARMONY_XML_CEXPATREADER_H__
#define __ORG_APACHE_HARMONY_XML_CEXPATREADER_H__

#include "_Org_Apache_Harmony_Xml_CExpatReader.h"
#include <elastos/core/Object.h>

using Org::Xml::Sax::IXMLReader;
using Org::Xml::Sax::IContentHandler;
using Org::Xml::Sax::IDTDHandler;
using Org::Xml::Sax::IEntityResolver;
using Org::Xml::Sax::IErrorHandler;
using Org::Xml::Sax::Ext::ILexicalHandler;
using Elastos::IO::IReader;
using Elastos::IO::IInputStream;
using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {

CarClass(CExpatReader)
    , public Object
    , public IExpatReader
    , public IXMLReader
{
private:
    class Feature
    {
    public:
        static const String BASE_URI;// = "http://xml.org/sax/features/";
        static const String VALIDATION;// = BASE_URI + "validation";
        static const String NAMESPACES;// = BASE_URI + "namespaces";
        static const String NAMESPACE_PREFIXES;// = BASE_URI + "namespace-prefixes";
        static const String STRING_INTERNING;// = BASE_URI + "string-interning";
        static const String EXTERNAL_GENERAL_ENTITIES;// = BASE_URI + "external-general-entities";
        static const String EXTERNAL_PARAMETER_ENTITIES;// = BASE_URI + "external-parameter-entities";
    };
public:

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean * pFeature);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface ** ppValue);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface * pValue);

    CARAPI SetEntityResolver(
        /* [in] */ IEntityResolver * pResolver);

    CARAPI GetEntityResolver(
        /* [out] */ IEntityResolver ** ppResolver);

    CARAPI SetDTDHandler(
        /* [in] */ IDTDHandler * pHandler);

    CARAPI GetDTDHandler(
        /* [out] */ IDTDHandler ** ppHandler);

    CARAPI SetContentHandler(
        /* [in] */ IContentHandler * pHandler);

    CARAPI GetContentHandler(
        /* [out] */ IContentHandler ** ppHandler);

    CARAPI SetErrorHandler(
        /* [in] */ IErrorHandler * pHandler);

    CARAPI GetErrorHandler(
        /* [out] */ IErrorHandler ** ppHandler);

    CARAPI Parse(
        /* [in] */ Org::Xml::Sax::IInputSource * pInput);

    CARAPI Parse(
        /* [in] */ IReader* in,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    CARAPI Parse(
        /* [in] */ IInputStream* in,
        /* [in] */ const String& charsetName,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    CARAPI Parse(
        /* [in] */ const String& systemId);

    CARAPI GetLexicalHandler(
        /* [out] */ ILexicalHandler ** ppHandler);

    CARAPI SetLexicalHandler(
        /* [in] */ ILexicalHandler * pLexicalHandler);

    CARAPI IsNamespaceProcessingEnabled(
        /* [out] */ Boolean * pEnabled);

    CARAPI SetNamespaceProcessingEnabled(
        /* [in] */ Boolean processNamespaces);

private:
    // TODO: Add your private member variables here.
    /*
     * ExpatParser accesses these fields directly during parsing. The user
     * should be able to safely change them during parsing.
     */
    /*package*/ AutoPtr<IContentHandler> contentHandler;
    /*package*/ AutoPtr<IDTDHandler> dtdHandler;
    /*package*/ AutoPtr<IEntityResolver> entityResolver;
    /*package*/ AutoPtr<IErrorHandler> errorHandler;
    /*package*/ AutoPtr<ILexicalHandler> lexicalHandler;

    Boolean processNamespaces;// = true;
    Boolean processNamespacePrefixes;// = false;

    static const String LEXICAL_HANDLER_PROPERTY;// = "http://xml.org/sax/properties/lexical-handler";
};

}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_CEXPATREADER_H__
