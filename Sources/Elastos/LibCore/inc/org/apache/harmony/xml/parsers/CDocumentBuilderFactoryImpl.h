
#ifndef __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Xml_Parsers_CDocumentBuilderFactoryImpl.h"
#include "elastosx/xml/parsers/DocumentBuilderFactory.h"

using Elastosx::Xml::Parsers::DocumentBuilderFactory;
using Org::W3c::Dom::IDOMImplementation;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

CarClass(CDocumentBuilderFactoryImpl)
    , public DocumentBuilderFactory
{
public:

    CARAPI NewDocumentBuilder(
        /* [out] */ Elastosx::Xml::Parsers::IDocumentBuilder ** ppBuilder);

    CARAPI SetAttribute(
        /* [in] */ const String& name,
        /* [in] */ IObject * pValue);

    CARAPI GetAttribute(
        /* [in] */ const String& name,
        /* [out] */ IInterface ** ppAttri);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean * pFeature);

private:
    static const String NAMESPACES;// = "http://xml.org/sax/features/namespaces";
    static const String VALIDATION;// = "http://xml.org/sax/features/validation";
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__
