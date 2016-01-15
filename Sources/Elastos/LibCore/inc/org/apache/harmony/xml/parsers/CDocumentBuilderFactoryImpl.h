
#ifndef __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Xml_Parsers_CDocumentBuilderFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

CarClass(CDocumentBuilderFactoryImpl)
{
public:
    CARAPI NewDocumentBuilder(
        /* [out] */ Elastosx::Xml::Parsers::IDocumentBuilder ** ppBuilder);

    CARAPI SetNamespaceAware(
        /* [in] */ Boolean awareness);

    CARAPI SetValidating(
        /* [in] */ Boolean validating);

    CARAPI SetIgnoringElementContentWhitespace(
        /* [in] */ Boolean whitespace);

    CARAPI SetExpandEntityReferences(
        /* [in] */ Boolean expandEntityRef);

    CARAPI SetIgnoringComments(
        /* [in] */ Boolean ignoreComments);

    CARAPI SetCoalescing(
        /* [in] */ Boolean coalescing);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean * pIsAware);

    CARAPI IsValidating(
        /* [out] */ Boolean * pIsValidating);

    CARAPI IsIgnoringElementContentWhitespace(
        /* [out] */ Boolean * pIsIgnoringElementContentWhiteSpace);

    CARAPI IsExpandEntityReferences(
        /* [out] */ Boolean * pIsExpandEntityReferences);

    CARAPI IsIgnoringComments(
        /* [out] */ Boolean * pComments);

    CARAPI IsCoalescing(
        /* [out] */ Boolean * pIsCoalescing);

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

    CARAPI GetSchema(
        /* [out] */ Elastosx::Xml::Validation::ISchema ** ppSchema);

    CARAPI SetSchema(
        /* [in] */ Elastosx::Xml::Validation::ISchema * pSchema);

    CARAPI SetXIncludeAware(
        /* [in] */ Boolean state);

    CARAPI IsXIncludeAware(
        /* [out] */ Boolean * pIsXIncludeAware);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__
