#include "org/apache/harmony/xml/dom/CDocumentTypeImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImplHelper.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"
#include <elastos/utility/logging/Logger.h>

using Org::W3c::Dom::EIID_IDocumentType;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_OBJECT_IMPL(CDocumentTypeImpl);
CAR_INTERFACE_IMPL(CDocumentTypeImpl, LeafNodeImpl, IDocumentType);

CDocumentTypeImpl::CDocumentTypeImpl()
{
}

ECode CDocumentTypeImpl::constructor(
    /* [in] */ Org::W3c::Dom::IDocument * pDocument,
    /* [in] */ const String& qualifiedName,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    LeafNodeImpl::constructor(pDocument);

    if (qualifiedName.IsNull() || String("").Equals(qualifiedName)) {
        //throw new DOMException(DOMException.NAMESPACE_ERR, qualifiedName);
        Logger::E("CDocumentTypeImpl", "NAMESPACE_ERR, qualifiedName:%s", qualifiedName.string());
        assert(0);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 prefixSeparator = qualifiedName.LastIndexOf(":");
    if (prefixSeparator != -1) {
        String prefix = qualifiedName.Substring(0, prefixSeparator);
        String localName = qualifiedName.Substring(prefixSeparator + 1);

        Boolean isxmlid;
        AutoPtr<IDocumentImplHelper> diHelper;
        CDocumentImplHelper::AcquireSingleton((IDocumentImplHelper**)&diHelper);
        if (!(diHelper->IsXMLIdentifier(prefix, &isxmlid), isxmlid)) {
            //throw new DOMException(DOMException.NAMESPACE_ERR, qualifiedName);
            Logger::E("CDocumentTypeImpl", "2 NAMESPACE_ERR, qualifiedName:%s", qualifiedName.string());
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (!(diHelper->IsXMLIdentifier(localName, &isxmlid), isxmlid)) {
            //throw new DOMException(DOMException.INVALID_CHARACTER_ERR, qualifiedName);
            Logger::E("CDocumentTypeImpl", "INVALID_CHARACTER_ERR , qualifiedName:%s", qualifiedName.string());
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    } else {
        Boolean isxmlid;
        AutoPtr<IDocumentImplHelper> diHelper;
        CDocumentImplHelper::AcquireSingleton((IDocumentImplHelper**)&diHelper);
        if (!(diHelper->IsXMLIdentifier(qualifiedName, &isxmlid), isxmlid)) {
            //throw new DOMException(DOMException.INVALID_CHARACTER_ERR, qualifiedName);
            Logger::E("CDocumentTypeImpl", "2 INVALID_CHARACTER_ERR, qualifiedName:%s", qualifiedName.string());
            assert(0);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    this->qualifiedName = qualifiedName;
    this->publicId = publicId;
    this->systemId = systemId;
    return NOERROR;
}

ECode CDocumentTypeImpl::GetNodeName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = qualifiedName;
    return NOERROR;
}

ECode CDocumentTypeImpl::GetNodeType(
    /* [out] */ Int16 * pValue)
{
    VALIDATE_NOT_NULL(*pValue);
    *pValue = INode::DOCUMENT_TYPE_NODE;
    return NOERROR;
}

ECode CDocumentTypeImpl::GetEntities(
    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
{
    // TODO Dummy. Implement this later, if at all (we're DOM level 2 only).
    VALIDATE_NOT_NULL(ppNodemap);
    *ppNodemap = NULL;
    return NOERROR;
}

ECode CDocumentTypeImpl::GetInternalSubset(
    /* [out] */ String * pStr)
{
    // TODO Dummy. Implement this later, if at all (we're DOM level 2 only).
    VALIDATE_NOT_NULL(pStr);
    *pStr = String(NULL);
    return NOERROR;
}

ECode CDocumentTypeImpl::GetName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = qualifiedName;
    return NOERROR;
}

ECode CDocumentTypeImpl::GetNotations(
    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
{
    // TODO Dummy. Implement this later, if at all (we're DOM level 2 only).
    VALIDATE_NOT_NULL(ppNodemap);
    *ppNodemap = NULL;
    return NOERROR;
}

ECode CDocumentTypeImpl::GetPublicId(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = publicId;
    return NOERROR;
}

ECode CDocumentTypeImpl::GetSystemId(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = systemId;
    return NOERROR;
}

ECode CDocumentTypeImpl::GetTextContent(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String(NULL);
    return NOERROR;
}

//========================================
//ECode CDocumentTypeImpl::GetNodeValue(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::SetNodeValue(
//    /* [in] */ const String& nodeValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetParentNode(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetChildNodes(
//    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetFirstChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetLastChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetPreviousSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetNextSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetAttributes(
//    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetOwnerDocument(
//    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::InsertBefore(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pRefChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::ReplaceChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::RemoveChild(
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::AppendChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::HasChildNodes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::CloneNode(
//    /* [in] */ Boolean deep,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::Normalize()
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::IsSupported(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetNamespaceURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetPrefix(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::SetPrefix(
//    /* [in] */ const String& prefix)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetLocalName(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::HasAttributes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetBaseURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::CompareDocumentPosition(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Int16 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::SetTextContent(
//    /* [in] */ const String& textContent)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::IsSameNode(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::LookupPrefix(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::IsDefaultNamespace(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::LookupNamespaceURI(
//    /* [in] */ const String& prefix,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::IsEqualNode(
//    /* [in] */ Org::W3c::Dom::INode * pArg,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetFeature(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::SetUserData(
//    /* [in] */ const String& key,
//    /* [in] */ IObject * pData,
//    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::GetUserData(
//    /* [in] */ const String& key,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::IsParentOf(
//    /* [in] */ Org::W3c::Dom::INode * pNode,
//    /* [out] */ Boolean * pIsParentOf)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentTypeImpl::constructor(
//    /* [in] */ Org::W3c::Dom::IDocument * pDocument,
//    /* [in] */ const String& qualifiedName,
//    /* [in] */ const String& publicId,
//    /* [in] */ const String& systemId)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}

}
}
}
}
}

