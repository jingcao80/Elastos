
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CELEMENTATTRNAMEDNODEMAPIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CELEMENTATTRNAMEDNODEMAPIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CElementAttrNamedNodeMapImpl.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CElementAttrNamedNodeMapImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetNamedItem(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI SetNamedItem(
        /* [in] */ Org::W3c::Dom::INode * pArg,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI RemoveNamedItem(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI Item(
        /* [in] */ Int32 index,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetLength(
        /* [out] */ Int32 * pValue);

    CARAPI GetNamedItemNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI SetNamedItemNS(
        /* [in] */ Org::W3c::Dom::INode * pArg,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI RemoveNamedItemNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CELEMENTATTRNAMEDNODEMAPIMPL_H__
