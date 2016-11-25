
#include "org/apache/harmony/xml/dom/CDOMImplementationImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentTypeImpl.h"

using Org::W3c::Dom::EIID_IDOMImplementation;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_OBJECT_IMPL(CDOMImplementationImpl);
CAR_INTERFACE_IMPL(CDOMImplementationImpl, Object, IDOMImplementation);

ECode CDOMImplementationImpl::CreateDocument(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& qualifiedName,
    /* [in] */ IDocumentType* doctype,
    /* [out] */ IDocument** doc)
{
    VALIDATE_NOT_NULL(doc);
    AutoPtr<CDocumentImpl> result = new CDocumentImpl();
    result->constructor(this, namespaceURI, qualifiedName, doctype, String(NULL));
    *doc = result;
    REFCOUNT_ADD(*doc);
    return NOERROR;
}

ECode CDOMImplementationImpl::CreateDocumentType(
    /* [in] */ const String& qualifiedName,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [out] */ IDocumentType** doc)
{
    VALIDATE_NOT_NULL(doc);
    AutoPtr<CDocumentTypeImpl> result = new CDocumentTypeImpl();
    result->constructor(NULL, qualifiedName, publicId, systemId);
    *doc = result;
    REFCOUNT_ADD(*doc);
    return NOERROR;
}

ECode CDOMImplementationImpl::HasFeature(
    /* [in] */ const String& _feature,
    /* [in] */ const String& version,
    /* [out] */ Boolean* value)
{
    String feature = _feature;
    VALIDATE_NOT_NULL(value);
    Boolean anyVersion  = FALSE;
    if (version.IsNull() || version.GetLength() == 0) {
        anyVersion = TRUE;
    }
    if (feature.StartWith("+")) {
        feature = feature.Substring(1);
    }

    // TODO: fully implement these APIs:
    // "LS" (org.w3c.dom.ls) versions "3.0"
    // "ElementTraversal" (org.w3c.dom.traversal) versions "1.0"

    if (feature.EqualsIgnoreCase("Core")) {
        *value = anyVersion || version.Equals("1.0") || version.Equals("2.0") || version.Equals("3.0");
    } else if (feature.EqualsIgnoreCase("XML")) {
        *value = anyVersion || version.Equals("1.0") || version.Equals("2.0") || version.Equals("3.0");
    } else if (feature.EqualsIgnoreCase("XMLVersion")) {
        *value = anyVersion || version.Equals("1.0") || version.Equals("1.1");
    } else {
        *value = FALSE;
    }
    return NOERROR;
}

ECode CDOMImplementationImpl::GetFeature(
    /* [in] */ const String& feature,
    /* [in] */ const String& version,
    /* [out] */ IObject** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;
    Boolean hasFeature;
    HasFeature(feature, version, &hasFeature);
    if (hasFeature) {
        *obj = this;
        REFCOUNT_ADD(*obj);
    }

    return NOERROR;
}

}
}
}
}
}

