
#ifndef __ELASTOSX_XML_XPATH_CXPATHCONSTANTS_H__
#define __ELASTOSX_XML_XPATH_CXPATHCONSTANTS_H__

#include "Object.h"
#include "_Elastosx_Xml_XPath_CXPathConstants.h"

using Elastos::Core::Object;
using Elastosx::Xml::Namespace::IQName;

namespace Elastosx {
namespace Xml {
namespace XPath {

CarClass(CXPathConstants)
    , public Object
    , public IXPathConstants
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetQNameNUMBER(
        /* [out] */ IQName** qName);

    CARAPI GetQNameSTRING(
        /* [out] */ IQName** qName);

    CARAPI GetQNameBOOLEAN(
        /* [out] */ IQName** qName);

    CARAPI GetQNameNODESET(
        /* [out] */ IQName** qName);

    CARAPI GetQNameNODE(
        /* [out] */ IQName** qName);

public:
    static const AutoPtr<IQName> NUMBER;
    static const AutoPtr<IQName> STRING;
    static const AutoPtr<IQName> BOOLEAN;
    static const AutoPtr<IQName> NODESET;
    static const AutoPtr<IQName> NODE;
};

}// namespace XPath
}// namespace Xml
}// namespace Elastosx

#endif // __ELASTOSX_XML_XPATH_CXPATHCONSTANTS_H__
