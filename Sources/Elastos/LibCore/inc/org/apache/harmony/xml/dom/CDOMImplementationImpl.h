
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOMIMPLEMENTATIONIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOMIMPLEMENTATIONIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDOMImplementationImpl.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Org::W3c::Dom::IDOMImplementation;
using Org::W3c::Dom::IDocumentType;
using Org::W3c::Dom::IDocument;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDOMImplementationImpl)
    , public Object
    , public IDOMImplementation
{
public:
    CAR_OBJECT_DECL()
    CAR_INTERFACE_DECL()

    CARAPI CreateDocument(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [in] */ IDocumentType* doctype,
        /* [out] */ IDocument** doc);

    CARAPI CreateDocumentType(
        /* [in] */ const String& qualifiedName,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId,
        /* [out] */ IDocumentType** doc);

    CARAPI HasFeature(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ Boolean* value);

    CARAPI GetFeature(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ IObject** obj);
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOMIMPLEMENTATIONIMPL_H__
