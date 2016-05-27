
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPLHELPER_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPLHELPER_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDocumentImplHelper.h"

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDocumentImplHelper)
    , public Singleton
    , public IDocumentImplHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI IsXMLIdentifier(
        /* [in] */ const String& s,
        /* [out] */ Boolean * pResult);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPLHELPER_H__
