
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CELEMENTIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CELEMENTIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CElementImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CElementImpl)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetNodeName(
        /* [out] */ String * pStr);

    CARAPI GetNodeValue(
        /* [out] */ String * pStr);

    CARAPI SetNodeValue(
        /* [in] */ const String& nodeValue);

    CARAPI GetNodeType(
        /* [out] */ Int16 * pValue);

    CARAPI GetParentNode(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetChildNodes(
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI GetFirstChild(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetLastChild(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetPreviousSibling(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetNextSibling(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetAttributes(
        /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap);

    CARAPI GetOwnerDocument(
        /* [out] */ Org::W3c::Dom::IDocument ** ppDoc);

    CARAPI InsertBefore(
        /* [in] */ Org::W3c::Dom::INode * pNewChild,
        /* [in] */ Org::W3c::Dom::INode * pRefChild,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI ReplaceChild(
        /* [in] */ Org::W3c::Dom::INode * pNewChild,
        /* [in] */ Org::W3c::Dom::INode * pOldChild,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI RemoveChild(
        /* [in] */ Org::W3c::Dom::INode * pOldChild,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI AppendChild(
        /* [in] */ Org::W3c::Dom::INode * pNewChild,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI HasChildNodes(
        /* [out] */ Boolean * pValue);

    CARAPI CloneNode(
        /* [in] */ Boolean deep,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI Normalize();

    CARAPI IsSupported(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ Boolean * pValue);

    CARAPI GetNamespaceURI(
        /* [out] */ String * pStr);

    CARAPI GetPrefix(
        /* [out] */ String * pStr);

    CARAPI SetPrefix(
        /* [in] */ const String& prefix);

    CARAPI GetLocalName(
        /* [out] */ String * pStr);

    CARAPI HasAttributes(
        /* [out] */ Boolean * pValue);

    CARAPI GetBaseURI(
        /* [out] */ String * pStr);

    CARAPI CompareDocumentPosition(
        /* [in] */ Org::W3c::Dom::INode * pOther,
        /* [out] */ Int16 * pValue);

    CARAPI GetTextContent(
        /* [out] */ String * pStr);

    CARAPI SetTextContent(
        /* [in] */ const String& textContent);

    CARAPI IsSameNode(
        /* [in] */ Org::W3c::Dom::INode * pOther,
        /* [out] */ Boolean * pValue);

    CARAPI LookupPrefix(
        /* [in] */ const String& namespaceURI,
        /* [out] */ String * pStr);

    CARAPI IsDefaultNamespace(
        /* [in] */ const String& namespaceURI,
        /* [out] */ Boolean * pValue);

    CARAPI LookupNamespaceURI(
        /* [in] */ const String& prefix,
        /* [out] */ String * pStr);

    CARAPI IsEqualNode(
        /* [in] */ Org::W3c::Dom::INode * pArg,
        /* [out] */ Boolean * pValue);

    CARAPI GetFeature(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ IObject ** ppObj);

    CARAPI SetUserData(
        /* [in] */ const String& key,
        /* [in] */ IObject * pData,
        /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
        /* [out] */ IObject ** ppObj);

    CARAPI GetUserData(
        /* [in] */ const String& key,
        /* [out] */ IObject ** ppObj);

    CARAPI IsParentOf(
        /* [in] */ Org::W3c::Dom::INode * pNode,
        /* [out] */ Boolean * pIsParentOf);

    CARAPI GetTagName(
        /* [out] */ String * pStr);

    CARAPI GetAttribute(
        /* [in] */ const String& name,
        /* [out] */ String * pStr);

    CARAPI SetAttribute(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI RemoveAttribute(
        /* [in] */ const String& name);

    CARAPI GetAttributeNode(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI SetAttributeNode(
        /* [in] */ Org::W3c::Dom::IAttr * pNewAttr,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI RemoveAttributeNode(
        /* [in] */ Org::W3c::Dom::IAttr * pOldAttr,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI GetElementsByTagName(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI GetAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ String * pStr);

    CARAPI SetAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [in] */ const String& value);

    CARAPI RemoveAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName);

    CARAPI GetAttributeNodeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI SetAttributeNodeNS(
        /* [in] */ Org::W3c::Dom::IAttr * pNewAttr,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI GetElementsByTagNameNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI HasAttribute(
        /* [in] */ const String& name,
        /* [out] */ Boolean * pValue);

    CARAPI HasAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Boolean * pValue);

    CARAPI GetSchemaTypeInfo(
        /* [out] */ Org::W3c::Dom::ITypeInfo ** ppInfo);

    CARAPI SetIdAttribute(
        /* [in] */ const String& name,
        /* [in] */ Boolean isId);

    CARAPI SetIdAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [in] */ Boolean isId);

    CARAPI SetIdAttributeNode(
        /* [in] */ Org::W3c::Dom::IAttr * pIdAttr,
        /* [in] */ Boolean isId);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CELEMENTIMPL_H__
