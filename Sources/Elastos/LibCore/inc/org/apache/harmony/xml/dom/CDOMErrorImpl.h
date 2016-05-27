
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDOMErrorImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDOMErrorImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetSeverity(
        /* [out] */ Int16 * pValue);

    CARAPI GetMessage(
        /* [out] */ String * pStr);

    CARAPI GetType(
        /* [out] */ String * pStr);

    CARAPI GetRelatedException(
        /* [out] */ IObject ** ppObj);

    CARAPI GetRelatedData(
        /* [out] */ IObject ** ppObj);

    CARAPI GetLocation(
        /* [out] */ Org::W3c::Dom::IDOMLocator ** ppDomloc);

    CARAPI constructor(
        /* [in] */ Int32 severity,
        /* [in] */ const String& type);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__
