
#ifndef __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__
#define __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.External.h"
#include "_Org_Apache_Harmony_Xml_Parsers_CSAXParserImpl.h"
#include "elastosx/xml/parsers/SAXParser.h"
#include <elastos/core/Object.h>

using Org::Xml::Sax::IParser;
using Org::Xml::Sax::IXMLReader;
using Elastos::Core::Object;
using Elastos::Utility::IMap;
using Elastosx::Xml::Parsers::SAXParser;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

CarClass(CSAXParserImpl)
    , public SAXParser
{
public:

    CARAPI constructor(
        /* [in] */ IMap* features);

    CARAPI Reset();

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

    CARAPI GetParser(
        /* [out] */ IParser** parser);

private:
    CARAPI ResetInternal();

private:
    //Map<String, Boolean> initialFeatures;
    AutoPtr<IMap> initialFeatures;
    AutoPtr<IXMLReader> reader;
    AutoPtr<IParser> parser;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__
