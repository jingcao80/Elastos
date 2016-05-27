
#ifndef __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__
#define __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__

#include "_Org_Apache_Harmony_Xml_Parsers_CSAXParserImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

CarClass(CSAXParserImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI Reset();

    CARAPI Parse(
        /* [in] */ Elastos::IO::IInputStream * pIs,
        /* [in] */ Org::Xml::Sax::IDocumentHandler * pHb);

    CARAPI ParseEx(
        /* [in] */ Elastos::IO::IInputStream * pIs,
        /* [in] */ Org::Xml::Sax::IDocumentHandler * pHb,
        /* [in] */ const String& systemId);

    CARAPI ParseEx2(
        /* [in] */ Elastos::IO::IInputStream * pIs,
        /* [in] */ Org::Xml::Sax::IDTDHandler * pDh);

    CARAPI ParseEx3(
        /* [in] */ Elastos::IO::IInputStream * pIs,
        /* [in] */ Org::Xml::Sax::IDTDHandler * pDh,
        /* [in] */ const String& systemId);

    CARAPI ParseEx4(
        /* [in] */ const String& uri,
        /* [in] */ Org::Xml::Sax::IDocumentHandler * pHb);

    CARAPI ParseEx5(
        /* [in] */ const String& uri,
        /* [in] */ Org::Xml::Sax::IDTDHandler * pDh);

    CARAPI ParseEx6(
        /* [in] */ Elastos::IO::IFile * pF,
        /* [in] */ Org::Xml::Sax::IDocumentHandler * pHb);

    CARAPI ParseEx7(
        /* [in] */ Elastos::IO::IFile * pF,
        /* [in] */ Org::Xml::Sax::IDTDHandler * pDh);

    CARAPI ParseEx8(
        /* [in] */ Org::Xml::Sax::IInputSource * pIs,
        /* [in] */ Org::Xml::Sax::IDocumentHandler * pHb);

    CARAPI ParseEx9(
        /* [in] */ Org::Xml::Sax::IInputSource * pIs,
        /* [in] */ Org::Xml::Sax::IDTDHandler * pDh);

    CARAPI GetParser(
        /* [out] */ Org::Xml::Sax::IParser ** ppParser);

    CARAPI GetXMLReader(
        /* [out] */ Org::Xml::Sax::IXMLReader ** ppReader);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean * pIsAware);

    CARAPI IsValidating(
        /* [out] */ Boolean * pIsValidating);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface * pValue);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface ** ppProperty);

    CARAPI GetSchema(
        /* [out] */ Elastosx::Xml::Validation::ISchema ** ppSchema);

    CARAPI IsXIncludeAware(
        /* [out] */ Boolean * pIsAware);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__
