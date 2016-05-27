
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOMCONFIGURATIONIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOMCONFIGURATIONIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDOMConfigurationImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDOMConfigurationImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI SetParameter(
        /* [in] */ const String& name,
        /* [in] */ IObject * pValue);

    CARAPI GetParameter(
        /* [in] */ const String& name,
        /* [out] */ IObject ** ppObj);

    CARAPI CanSetParameter(
        /* [in] */ const String& name,
        /* [in] */ IObject * pValue,
        /* [out] */ Boolean * pReslut);

    CARAPI GetParameterNames(
        /* [out] */ Org::W3c::Dom::IDOMStringList ** ppDomlist);

    CARAPI Normalize(
        /* [in] */ Org::W3c::Dom::INode * pNode);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOMCONFIGURATIONIMPL_H__
