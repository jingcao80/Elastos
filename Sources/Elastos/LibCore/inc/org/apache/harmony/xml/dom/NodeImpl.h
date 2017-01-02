//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef _ORG_APACHE_HARMONY_XML_DOM_NODEIMPL_H__
#define _ORG_APACHE_HARMONY_XML_DOM_NODEIMPL_H__

#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.External.h"
//#include "org/apache/harmony/xml/dom/CDocumentImpl.h"

using Org::W3c::Dom::IUserDataHandler;
using Org::W3c::Dom::INode;
using Org::W3c::Dom::IDocument;
using Org::W3c::Dom::INodeList;
using Org::W3c::Dom::INamedNodeMap;
using Org::W3c::Dom::ITypeInfo;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;

//import java.net.URI;
//import java.net.URISyntaxException;
//import java.util.ArrayList;
//import java.util.List;
//import java.util.Map;
//import org.w3c.dom.Attr;
//import org.w3c.dom.CharacterData;
//import org.w3c.dom.DOMException;
//import org.w3c.dom.Element;
//import org.w3c.dom.ProcessingInstruction;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

//class CDocumentImpl;
/**
 * A straightforward implementation of the corresponding W3C DOM node.
 *
 * <p>Some fields have package visibility so other classes can access them while
 * maintaining the DOM structure.
 *
 * <p>This class represents a Node that has neither a parent nor children.
 * Subclasses may have either.
 *
 * <p>Some code was adapted from Apache Xerces.
 */
class NodeImpl
    : public Object
    , public INode
{
public:
    class NullTypeInfo
        : public Object
        , public ITypeInfo
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI GetTypeName(
            /* [out] */ String* str);

        CARAPI GetTypeNamespace(
            /* [out] */ String* str);

        CARAPI IsDerivedFrom(
            /* [in] */ const String& typeNamespaceArg,
            /* [in] */ const String& typeNameArg,
            /* [in] */ Int32 derivationMethod,
            /* [out] */ Boolean* value);
    };

    class UserData
        : public Object
    {
    public:
        AutoPtr<IObject> value;
        AutoPtr<IUserDataHandler> handler;
        UserData(
            /* [in] */ IObject* value,
            /* [in] */ IUserDataHandler* handler);
    };

public:
    CAR_INTERFACE_DECL();
    NodeImpl();

    CARAPI constructor(
        /* [in] */ IDocument* document);

    CARAPI AppendChild(
        /* [in] */ INode* newChild,
        /* [out] */ INode** node);

    CARAPI CloneNode(
        /* [in] */ Boolean deep,
        /* [out] */ INode** node);

    CARAPI GetAttributes(
        /* [out] */ INamedNodeMap** nodemap);

    CARAPI GetChildNodes(
        /* [out] */ INodeList** nodelist);

    CARAPI GetFirstChild(
        /* [out] */ INode** node);

    CARAPI GetLastChild(
        /* [out] */ INode** node);

    CARAPI GetLocalName(
        /* [out] */ String* str);

    CARAPI GetNamespaceURI(
        /* [out] */ String* str);

    CARAPI GetNextSibling(
        /* [out] */ INode** node);

    CARAPI GetNodeName(
        /* [out] */ String* str);

    virtual CARAPI GetNodeType(
        /* [out] */ Int16* value) = 0;

    CARAPI GetNodeValue(
        /* [out] */ String* str);

    CARAPI GetOwnerDocument(
        /* [out] */ IDocument** doc);

    CARAPI GetParentNode(
        /* [out] */ INode** node);

    CARAPI GetPrefix(
        /* [out] */ String* str);

    CARAPI GetPreviousSibling(
        /* [out] */ INode** node);

    CARAPI HasAttributes(
        /* [out] */ Boolean* value);

    CARAPI HasChildNodes(
        /* [out] */ Boolean* value);

    CARAPI InsertBefore(
        /* [in] */ INode* newChild,
        /* [in] */ INode* refChild,
        /* [out] */ INode** node);

    CARAPI IsSupported(
        /* [in] */ const String& feature,
        /* [in] */ const String& version,
        /* [out] */ Boolean* value);

    CARAPI Normalize();

    CARAPI RemoveChild(
        /* [in] */ INode* oldChild,
        /* [out] */ INode** node);

    CARAPI ReplaceChild(
        /* [in] */ INode* newChild,
        /* [in] */ INode* oldChild,
        /* [out] */ INode** node);

    CARAPI SetNodeValue(
        /* [in] */ const String& nodeValue);

    CARAPI SetPrefix(
        /* [in] */ const String& prefix);

    CARAPI GetBaseURI(
        /* [out] */ String* str);

    /**
     * Validates the element or attribute namespace prefix on this node.
     *
     * @param namespaceAware whether this node is namespace aware
     * @param namespaceURI this node's namespace URI
     */
    static CARAPI_(String) ValidatePrefix(
        /* [in] */ const String& prefix,
        /* [in] */ Boolean namespaceAware,
        /* [in] */ const String& namespaceURI);

    /**
     * Sets {@code node} to be namespace-aware and assigns its namespace URI
     * and qualified name.
     *
     * @param node an element or attribute node.
     * @param namespaceURI this node's namespace URI. May be null.
     * @param qualifiedName a possibly-prefixed name like "img" or "html:img".
     */
    static CARAPI_(void) SetNameNS(
        /* [in] */ NodeImpl* node,
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName);

    /**
     * Sets {@code node} to be not namespace-aware and assigns its name.
     *
     * @param node an element or attribute node.
     */
    static CARAPI_(void) SetName(
        /* [in] */ NodeImpl* node,
        /* [in] */ const String& name);

    CARAPI CompareDocumentPosition(
        /* [in] */ INode* other,
        /* [out] */ Int16* value);

    CARAPI GetTextContent(
        /* [out] */ String* str);

    CARAPI_(void) GetTextContent(
        /* [in] */ IStringBuilder* buf);

    CARAPI SetTextContent(
        /* [in] */ const String& textContent);

    CARAPI IsSameNode(
        /* [in] */ INode* other,
        /* [out] */ Boolean* value);

    CARAPI LookupPrefix(
        /* [in] */ const String& namespaceURI,
        /* [out] */ String* str);

    /**
     * Returns true if the given prefix is mapped to the given URI on this
     * element. Since child elements can redefine prefixes, this check is
     * necessary: {@code
     * <foo xmlns:a="http://good">
     *   <bar xmlns:a="http://evil">
     *     <a:baz />
     *   </bar>
     * </foo>}
     *
     * @param prefix the prefix to find. Nullable.
     * @param uri the URI to match. Non-null.
     */
    CARAPI_(Boolean) IsPrefixMappedToUri(
        /* [in] */ const String& prefix,
        /* [in] */ const String& uri);

    CARAPI IsDefaultNamespace(
        /* [in] */ const String& namespaceURI,
        /* [out] */ Boolean* value);

    CARAPI LookupNamespaceURI(
        /* [in] */ const String& prefix,
        /* [out] */ String* str);

    CARAPI IsEqualNode(
        /* [in] */ INode* arg,
        /* [out] */ Boolean* value);

    CARAPI GetFeature(
        /* [in] */ const String& feature,
        /* [in] */ const String& version,
        /* [out] */ IObject** obj);

    CARAPI SetUserData(
        /* [in] */ const String& key,
        /* [in] */ IObject* data,
        /* [in] */ IUserDataHandler* handler,
        /* [out] */ IObject** obj);

    CARAPI GetUserData(
        /* [in] */ const String& key,
        /* [out] */ IObject** obj);

private:
    CARAPI_(String) GetParentBaseUri();

    /**
     * Returns the sanitized input if it is a URI, or {@code null} otherwise.
     */
    CARAPI_(String) SanitizeUri(
        /* [in] */ const String& uri);

    /**
     * Returns the element whose namespace definitions apply to this node. Use
     * this element when mapping prefixes to URIs and vice versa.
     */
    CARAPI_(AutoPtr<NodeImpl>) GetNamespacingElement();

    /**
     * Returns the nearest ancestor element that contains this node.
     */
    CARAPI_(AutoPtr<NodeImpl>) GetContainingElement();

    /**
     * Returns a list of objects such that two nodes are equal if their lists
     * are equal. Be careful: the lists may contain NamedNodeMaps and Nodes,
     * neither of which override Object.equals(). Such values must be compared
     * manually.
     */
    static CARAPI_(AutoPtr<IList>) CreateEqualityKey(
        /* [in] */ INode* node);

    CARAPI_(Boolean) NamedNodeMapsEqual(
        /* [in] */ INamedNodeMap* a,
        /* [in] */ INamedNodeMap* b);

protected:
    static AutoPtr<INodeList> EMPTY_LIST;// = new NodeListImpl();
    static AutoPtr<ITypeInfo> NULL_TYPE_INFO;// = new NullTypeInfo()

    /**
     * The containing document. This is non-null except for DocumentTypeImpl
     * nodes created by the DOMImplementation.
     */
    //AutoPtr<CDocumentImpl> document;
    AutoPtr<IDocument> document;

    friend class CElementImpl;
    friend class InnerNodeImpl;
};

} // Dom
} // Xml
} // Harmony
} // Apache
} // Org

#endif // _ORG_APACHE_HARMONY_XML_DOM_NODEIMPL_H__
