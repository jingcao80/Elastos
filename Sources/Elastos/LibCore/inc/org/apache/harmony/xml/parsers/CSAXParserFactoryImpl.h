
#ifndef __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERFACTORYIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Harmony_Xml_Parsers_CSAXParserFactoryImpl.h"
#include "elastosx/xml/parsers/SAXParserFactory.h"

using Elastosx::Xml::Parsers::SAXParserFactory;
using Elastos::Utility::IMap;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

CarClass(CSAXParserFactoryImpl)
    , public SAXParserFactory
{
public:

    CSAXParserFactoryImpl();

    CARAPI NewSAXParser(
        /* [out] */ Elastosx::Xml::Parsers::ISAXParser ** ppParser);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean * pIsAware);

    CARAPI IsValidating(
        /* [out] */ Boolean * pIsValidating);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean * pFeature);

    CARAPI SetNamespaceAware(
        /* [in] */ Boolean awareness);

    CARAPI SetValidating(
        /* [in] */ Boolean validating);

private:
    static const String NAMESPACES;// = "http://xml.org/sax/features/namespaces";
    static const String VALIDATION;// = "http://xml.org/sax/features/validation";
    //Map<String, Boolean> features = new HashMap<String, Boolean>();
    AutoPtr<IMap> features;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERFACTORYIMPL_H__
