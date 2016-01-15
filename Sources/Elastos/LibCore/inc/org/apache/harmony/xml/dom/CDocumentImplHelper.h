
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPLHELPER_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPLHELPER_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDocumentImplHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDocumentImplHelper)
{
public:
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
