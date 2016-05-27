
#ifndef __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Xml_Parsers_CSAXParserFactoryImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

CarClass(CSAXParserFactoryImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI NewSAXParser(
        /* [out] */ Elastosx::Xml::Parsers::ISAXParser ** ppParser);

    CARAPI SetNamespaceAware(
        /* [in] */ Boolean awareness);

    CARAPI SetValidating(
        /* [in] */ Boolean validating);

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

    CARAPI GetSchema(
        /* [out] */ Elastosx::Xml::Validation::ISchema ** ppSchema);

    CARAPI SetSchema(
        /* [in] */ Elastosx::Xml::Validation::ISchema * pSchema);

    CARAPI SetXIncludeAware(
        /* [in] */ Boolean state);

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

#endif // __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERFACTORYIMPL_H__
