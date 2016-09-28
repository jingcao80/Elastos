
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CELEMENTIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CELEMENTIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CElementImpl.h"
#include <elastos/core/Object.h>
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"

using Elastos::Core::Object;
using Org::W3c::Dom::IElement;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CElementImpl)
    , public InnerNodeImpl
    , public IElement
{
public:
    class ElementAttrNamedNodeMapImpl
        : public Object
        , public INamedNodeMap
    {
    public:
        CAR_INTERFACE_DECL();

        ECode constructor(
            /* [in] */ IElement* host);

        CARAPI GetLength(
            /* [out] */ Int32* result);

        CARAPI GetNamedItem(
            /* [in] */ const String& name,
            /* [out] */ INode** result);

        CARAPI GetNamedItemNS(
            /* [in] */ const String& namespaceURI,
            /* [in] */ const String& localName,
            /* [out] */ INode** result);

        CARAPI Item(
            /* [in] */ Int32 index,
            /* [out] */ INode** result);

        CARAPI RemoveNamedItem(
            /* [in] */ const String& name,
            /* [out] */ INode** result);

        CARAPI RemoveNamedItemNS(
            /* [in] */ const String& namespaceURI,
            /* [in] */ const String& localName,
            /* [out] */ INode** result);

        CARAPI SetNamedItem(
            /* [in] */ INode* arg,
            /* [out] */ INode** result);

        CARAPI SetNamedItemNS(
            /* [in] */ INode* arg,
            /* [out] */ INode** result);

    private:
        CARAPI_(Int32) IndexOfItem(
            /* [in] */ const String& name);

        CARAPI_(Int32) IndexOfItemNS(
            /* [in] */ const String& namespaceURI,
            /* [in] */ const String& localName);

    private:
        CElementImpl* mHost;
    };

public:
    CAR_OBJECT_DECL();
    CAR_INTERFACE_DECL();

    CElementImpl();

    CARAPI constructor(
        /* [in] */ IDocument* document,
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName);

    CARAPI constructor(
        /* [in] */ IDocument* document,
        /* [in] */ const String& name);

    CARAPI GetAttribute(
        /* [in] */ const String& name,
        /* [out] */ String * pStr);

    CARAPI GetAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ String * pStr);

    CARAPI GetAttributeNode(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI GetAttributeNodeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI GetAttributes(
        /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap);

    CARAPI GetElementsByTagName(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI GetElementsByTagNameNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI GetNamespaceURI(
        /* [out] */ String * pStr);

    CARAPI GetLocalName(
        /* [out] */ String * pStr);

    CARAPI GetNodeName(
        /* [out] */ String * pStr);

    CARAPI GetNodeType(
        /* [out] */ Int16 * pValue);

    CARAPI GetPrefix(
        /* [out] */ String * pStr);

    CARAPI GetTagName(
        /* [out] */ String * pStr);

    CARAPI HasAttributes(
        /* [out] */ Boolean * pValue);

    CARAPI HasAttribute(
        /* [in] */ const String& name,
        /* [out] */ Boolean * pValue);

    CARAPI HasAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Boolean * pValue);

    CARAPI RemoveAttribute(
        /* [in] */ const String& name);

    CARAPI RemoveAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName);

    CARAPI RemoveAttributeNode(
        /* [in] */ Org::W3c::Dom::IAttr * pOldAttr,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI SetAttribute(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI SetAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [in] */ const String& value);

    CARAPI SetAttributeNode(
        /* [in] */ Org::W3c::Dom::IAttr * pNewAttr,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI SetAttributeNodeNS(
        /* [in] */ Org::W3c::Dom::IAttr * pNewAttr,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI SetPrefix(
        /* [in] */ const String& prefix);

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

    // ================
    //CARAPI GetNodeValue(
    //    /* [out] */ String * pStr);

    //CARAPI SetNodeValue(
    //    /* [in] */ const String& nodeValue);

    //CARAPI GetParentNode(
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI GetChildNodes(
    //    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    //CARAPI GetFirstChild(
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI GetLastChild(
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI GetPreviousSibling(
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI GetNextSibling(
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI GetOwnerDocument(
    //    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc);

    //CARAPI InsertBefore(
    //    /* [in] */ Org::W3c::Dom::INode * pNewChild,
    //    /* [in] */ Org::W3c::Dom::INode * pRefChild,
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI ReplaceChild(
    //    /* [in] */ Org::W3c::Dom::INode * pNewChild,
    //    /* [in] */ Org::W3c::Dom::INode * pOldChild,
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI RemoveChild(
    //    /* [in] */ Org::W3c::Dom::INode * pOldChild,
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI AppendChild(
    //    /* [in] */ Org::W3c::Dom::INode * pNewChild,
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI HasChildNodes(
    //    /* [out] */ Boolean * pValue);

    //CARAPI CloneNode(
    //    /* [in] */ Boolean deep,
    //    /* [out] */ Org::W3c::Dom::INode ** ppNode);

    //CARAPI Normalize();

    //CARAPI IsSupported(
    //    /* [in] */ const String& feature,
    //    /* [in] */ const String& ver,
    //    /* [out] */ Boolean * pValue);

    //CARAPI GetBaseURI(
    //    /* [out] */ String * pStr);

    //CARAPI CompareDocumentPosition(
    //    /* [in] */ Org::W3c::Dom::INode * pOther,
    //    /* [out] */ Int16 * pValue);

    //CARAPI GetTextContent(
    //    /* [out] */ String * pStr);

    //CARAPI SetTextContent(
    //    /* [in] */ const String& textContent);

    //CARAPI IsSameNode(
    //    /* [in] */ Org::W3c::Dom::INode * pOther,
    //    /* [out] */ Boolean * pValue);

    //CARAPI LookupPrefix(
    //    /* [in] */ const String& namespaceURI,
    //    /* [out] */ String * pStr);

    //CARAPI IsDefaultNamespace(
    //    /* [in] */ const String& namespaceURI,
    //    /* [out] */ Boolean * pValue);

    //CARAPI LookupNamespaceURI(
    //    /* [in] */ const String& prefix,
    //    /* [out] */ String * pStr);

    //CARAPI IsEqualNode(
    //    /* [in] */ Org::W3c::Dom::INode * pArg,
    //    /* [out] */ Boolean * pValue);

    //CARAPI GetFeature(
    //    /* [in] */ const String& feature,
    //    /* [in] */ const String& ver,
    //    /* [out] */ IObject ** ppObj);

    //CARAPI SetUserData(
    //    /* [in] */ const String& key,
    //    /* [in] */ IObject * pData,
    //    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
    //    /* [out] */ IObject ** ppObj);

    //CARAPI GetUserData(
    //    /* [in] */ const String& key,
    //    /* [out] */ IObject ** ppObj);

    //CARAPI IsParentOf(
    //    /* [in] */ Org::W3c::Dom::INode * pNode,
    //    /* [out] */ Boolean * pIsParentOf);

private:
    CARAPI GetElementById(
        /* [in] */ const String& name,
        /* [out] */ IElement** result);

    CARAPI_(Int32) IndexOfAttribute(
        /* [in] */ const String& name);

    CARAPI_(Int32) IndexOfAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName);

private:
    Boolean namespaceAware;
    String namespaceURI;
    String prefix;
    String localName;

    //List<AttrImpl> attributes = new ArrayList<AttrImpl>();
    AutoPtr<IList> attributes;

    friend class CDocumentImpl;
    friend class NodeImpl;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CELEMENTIMPL_H__
