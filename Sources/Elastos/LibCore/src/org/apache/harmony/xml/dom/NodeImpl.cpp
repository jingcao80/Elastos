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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/harmony/xml/dom/NodeImpl.h"
#include "org/apache/harmony/xml/dom/CAttrImpl.h"
#include "org/apache/harmony/xml/dom/CNodeListImpl.h"
#include "org/apache/harmony/xml/dom/CDOMImplementationImplHelper.h"
#include "org/apache/harmony/xml/dom/CDocumentImplHelper.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/CElementImpl.h"
#include "elastos/net/CURI.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/utility/CArrayList.h"
#include <elastos/utility/logging/Logger.h>

//import java.net.URI;
//import java.net.URISyntaxException;
//import java.util.ArrayList;
//import java.util.List;
//import java.util.Map;
//import org.w3c.dom.Attr;
//import org.w3c.dom.CharacterData;
//import org.w3c.dom.DOMException;
//import org.w3c.dom.Document;
//import org.w3c.dom.Element;
//import org.w3c.dom.NamedNodeMap;
//import org.w3c.dom.Node;
//import org.w3c.dom.NodeList;
//import org.w3c.dom.ProcessingInstruction;
//import org.w3c.dom.TypeInfo;
//import org.w3c.dom.UserDataHandler;
using Elastos::Net::IURI;
using Org::W3c::Dom::INamedNodeMap;
using Org::W3c::Dom::IText;
using Org::W3c::Dom::IElement;
using Org::W3c::Dom::ICharacterData;
using Org::W3c::Dom::IProcessingInstruction;
using Org::W3c::Dom::IAttr;
using Org::W3c::Dom::EIID_ITypeInfo;
using Org::W3c::Dom::EIID_INode;
using Elastos::Core::CoreUtils;
using Elastos::Net::CURI;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

static AutoPtr<INodeList> InitEMPYT_LIST()
{
    AutoPtr<INodeList> result;
    CNodeListImpl::New((INodeList**)&result);
    return result;
}

AutoPtr<INodeList> NodeImpl::EMPTY_LIST = InitEMPYT_LIST();
AutoPtr<ITypeInfo> NodeImpl::NULL_TYPE_INFO = new NodeImpl::NullTypeInfo();

CAR_INTERFACE_IMPL(NodeImpl::NullTypeInfo, Object, ITypeInfo);

ECode NodeImpl::NullTypeInfo::GetTypeName(
    /* [out] */ String* str)
{
    *str = String(NULL);
    return NOERROR;
}

ECode NodeImpl::NullTypeInfo::GetTypeNamespace(
    /* [out] */ String* str)
{
    *str = String(NULL);
    return NOERROR;
}

ECode NodeImpl::NullTypeInfo::IsDerivedFrom(
    /* [in] */ const String& typeNamespaceArg,
    /* [in] */ const String& typeNameArg,
    /* [in] */ Int32 derivationMethod,
    /* [out] */ Boolean* value)
{
    *value = FALSE;
    return NOERROR;
}

NodeImpl::UserData::UserData(
    /* [in] */ IObject* value,
    /* [in] */ IUserDataHandler* handler)
{
    this->value = value;
    this->handler = handler;
}

CAR_INTERFACE_IMPL(NodeImpl, Object, INode);

NodeImpl::NodeImpl()
{
}

ECode NodeImpl::constructor(
    /* [in] */ IDocument* document)
{
    this->document = (CDocumentImpl*)document;
    return NOERROR;
}

ECode NodeImpl::AppendChild(
    /* [in] */ INode* newChild,
    /* [out] */ INode** node)
{
    //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
    Logger::E("NodeImpl", "AppendChild, DOMException");
    assert(0);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode NodeImpl::CloneNode(
    /* [in] */ Boolean deep,
    /* [out] */ INode** node)
{
    CDocumentImpl* cd = (CDocumentImpl*)document.Get();
    cd->CloneOrImportNode(IUserDataHandler::NODE_CLONED, this, deep);
    return NOERROR;
}

ECode NodeImpl::GetAttributes(
    /* [out] */ INamedNodeMap** nodemap)
{
    VALIDATE_NOT_NULL(nodemap);
    *nodemap = NULL;
    return NOERROR;
}

ECode NodeImpl::GetChildNodes(
    /* [out] */ INodeList** nodelist)
{
    VALIDATE_NOT_NULL(nodelist);
    *nodelist = EMPTY_LIST;
    REFCOUNT_ADD(*nodelist);
    return NOERROR;
}

ECode NodeImpl::GetFirstChild(
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    *node = NULL;
    return NOERROR;
}

ECode NodeImpl::GetLastChild(
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    *node = NULL;
    return NOERROR;
}

ECode NodeImpl::GetLocalName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    return NOERROR;
}

ECode NodeImpl::GetNamespaceURI(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    return NOERROR;
}

ECode NodeImpl::GetNextSibling(
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    *node = NULL;
    return NOERROR;
}

ECode NodeImpl::GetNodeName(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    return NOERROR;
}

ECode NodeImpl::GetNodeValue(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    return NOERROR;
}

ECode NodeImpl::GetOwnerDocument(
    /* [out] */ IDocument** doc)
{
    VALIDATE_NOT_NULL(doc);
    if (INode::Probe(document.Get()) == INode::Probe(this)) {
        *doc = NULL;
    }
    else {
        *doc = document;
        REFCOUNT_ADD(*doc);
    }
    return NOERROR;
}

ECode NodeImpl::GetParentNode(
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    *node = NULL;
    return NOERROR;
}

ECode NodeImpl::GetPrefix(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    return NOERROR;
}

ECode NodeImpl::GetPreviousSibling(
    /* [out] */ INode** node)
{
    VALIDATE_NOT_NULL(node);
    *node = NULL;
    return NOERROR;
}

ECode NodeImpl::HasAttributes(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    return NOERROR;
}

ECode NodeImpl::HasChildNodes(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    return NOERROR;
}

ECode NodeImpl::InsertBefore(
    /* [in] */ INode* newChild,
    /* [in] */ INode* refChild,
    /* [out] */ INode** node)
{
    //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
    Logger::E("NodeImpl", "InsertBefore, DOMException");
    assert(0);
    return NOERROR;
}

ECode NodeImpl::IsSupported(
    /* [in] */ const String& feature,
    /* [in] */ const String& version,
    /* [out] */ Boolean* value)
{
    AutoPtr<IDOMImplementationImplHelper> helper;
    CDOMImplementationImplHelper::AcquireSingleton((IDOMImplementationImplHelper**)&helper);
    AutoPtr<IDOMImplementation> domImpl;
    helper->GetInstance((IDOMImplementation**)&domImpl);
    return domImpl->HasFeature(feature, version, value);
}

ECode NodeImpl::Normalize()
{
    return NOERROR;
}

ECode NodeImpl::RemoveChild(
    /* [in] */ INode* oldChild,
    /* [out] */ INode** node)
{
    Logger::E("NodeImpl", "RemoveChild, DOMException");
    assert(0);
    return NOERROR;
    //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
}

ECode NodeImpl::ReplaceChild(
    /* [in] */ INode* newChild,
    /* [in] */ INode* oldChild,
    /* [out] */ INode** node)
{
    //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
    Logger::E("NodeImpl", "ReplaceChild, DOMException");
    assert(0);
    return NOERROR;
}

ECode NodeImpl::SetNodeValue(
    /* [in] */ const String& nodeValue)
{
    Int16 nodeType;
    GetNodeType(&nodeType);
    switch (nodeType) {
        case CDATA_SECTION_NODE:
        case COMMENT_NODE:
        case TEXT_NODE:
            ICharacterData::Probe(this)->SetData(nodeValue);
            return NOERROR;

        case PROCESSING_INSTRUCTION_NODE:
            IProcessingInstruction::Probe(this)->SetData(nodeValue);
            return NOERROR;

        case ATTRIBUTE_NODE:
            IAttr::Probe(this)->SetValue(nodeValue);
            return NOERROR;

        case ELEMENT_NODE:
        case ENTITY_REFERENCE_NODE:
        case ENTITY_NODE:
        case DOCUMENT_NODE:
        case DOCUMENT_TYPE_NODE:
        case DOCUMENT_FRAGMENT_NODE:
        case NOTATION_NODE:
            return NOERROR; // do nothing!

        default:
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
            //        "Unsupported node type " + getNodeType());
            Logger::E("NodeImpl", "SetNodeValue, Unsupported node type: %d", nodeType);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

ECode NodeImpl::SetPrefix(
    /* [in] */ const String& prefix)
{
    return NOERROR;
}

/**
 * Validates the element or attribute namespace prefix on this node.
 *
 * @param namespaceAware whether this node is namespace aware
 * @param namespaceURI this node's namespace URI
 */
String NodeImpl::ValidatePrefix(
    /* [in] */ const String& prefix,
    /* [in] */ Boolean namespaceAware,
    /* [in] */ const String& namespaceURI)
{
    if (!namespaceAware) {
        //throw new DOMException(DOMException.NAMESPACE_ERR, prefix);
        Logger::E("NodeImpl", "ValidatePrefix, 1 DOMException");
        assert(0);
        return String(NULL);
    }

    if (!prefix.IsNull()) {
        AutoPtr<IDocumentImplHelper> helper;
        CDocumentImplHelper::AcquireSingleton((IDocumentImplHelper**)&helper);
        Boolean xmlId;
        if (namespaceURI.IsNull()
                || !(helper->IsXMLIdentifier(prefix, &xmlId), xmlId)
                || (String("xml").Equals(prefix) && !String("http://www.w3.org/XML/1998/namespace").Equals(namespaceURI))
                || (String("xmlns").Equals(prefix) && !String("http://www.w3.org/2000/xmlns/").Equals(namespaceURI))) {
            //throw new DOMException(DOMException.NAMESPACE_ERR, prefix);
            Logger::E("NodeImpl", "ValidatePrefix, 2 DOMException");
            assert(0);
            return String(NULL);
        }
    }

    return prefix;
}

void NodeImpl::SetNameNS(
    /* [in] */ NodeImpl* node,
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& _qualifiedName)
{
    String qualifiedName = _qualifiedName;
    if (qualifiedName.IsNull()) {
        //throw new DOMException(DOMException.NAMESPACE_ERR, qualifiedName);
        Logger::E("NodeImpl", "SetNameNS, 1 DOMException");
        assert(0);
        return;
    }

    String prefix;
    Int32 p = qualifiedName.LastIndexOf(":");
    if (p != -1) {
        prefix = ValidatePrefix(qualifiedName.Substring(0, p), TRUE, namespaceURI);
        qualifiedName = qualifiedName.Substring(p + 1);
    }

    AutoPtr<IDocumentImplHelper> helper;
    CDocumentImplHelper::AcquireSingleton((IDocumentImplHelper**)&helper);
    Boolean xmlId;
    if (!(helper->IsXMLIdentifier(qualifiedName, &xmlId), xmlId)) {
        //throw new DOMException(DOMException.INVALID_CHARACTER_ERR, qualifiedName);
        Logger::E("NodeImpl", "SetNameNS, 2 DOMException");
        assert(0);
        return;
    }

    Int16 nodeType;
    node->GetNodeType(&nodeType);
    switch (nodeType) {
    case ATTRIBUTE_NODE:
    {
        if (String("xmlns").Equals(qualifiedName)
                && !String("http://www.w3.org/2000/xmlns/").Equals(namespaceURI)) {
            //throw new DOMException(DOMException.NAMESPACE_ERR, qualifiedName);
            Logger::E("NodeImpl", "SetNameNS, 3 DOMException");
            assert(0);
            return;
        }

        CAttrImpl* attr = (CAttrImpl*)(IAttr::Probe(node));
        attr->namespaceAware = TRUE;
        attr->namespaceURI = namespaceURI;
        attr->prefix = prefix;
        attr->localName = qualifiedName;
        break;
    }
    case ELEMENT_NODE:
    {
        CElementImpl* element = (CElementImpl*)(IElement::Probe(node));
        element->namespaceAware = TRUE;
        element->namespaceURI = namespaceURI;
        element->prefix = prefix;
        element->localName = qualifiedName;
        break;
    }
    default:
        //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
        //        "Cannot rename nodes of type " + node.getNodeType());
        Logger::E("NodeImpl", "SetNameNS, 4 DOMException");
        assert(0);
        return;
    }
}

void NodeImpl::SetName(
    /* [in] */ NodeImpl* node,
    /* [in] */ const String& name)
{
    Int32 prefixSeparator = name.LastIndexOf(":");
    AutoPtr<IDocumentImplHelper> helper;
    CDocumentImplHelper::AcquireSingleton((IDocumentImplHelper**)&helper);
    Boolean xmlId;
    if (prefixSeparator != -1) {
        String prefix = name.Substring(0, prefixSeparator);
        String localName = name.Substring(prefixSeparator + 1);
        if (!(helper->IsXMLIdentifier(prefix, &xmlId), xmlId) || !(helper->IsXMLIdentifier(localName, &xmlId), xmlId)) {
            //throw new DOMException(DOMException.INVALID_CHARACTER_ERR, name);
            Logger::E("NodeImpl", "SetName, 1 DOMException");
            assert(0);
            return;
        }
    } else if (!(helper->IsXMLIdentifier(name, &xmlId), xmlId)) {
        //throw new DOMException(DOMException.INVALID_CHARACTER_ERR, name);
        Logger::E("NodeImpl", "SetName, 2 DOMException");
        assert(0);
        return;
    }

    Int16 nodeType;
    node->GetNodeType(&nodeType);
    switch (nodeType) {
    case ATTRIBUTE_NODE:
    {
        CAttrImpl* attr = (CAttrImpl*)(IAttr::Probe(node));
        attr->namespaceAware = FALSE;
        attr->localName = name;
        break;
    }
    case ELEMENT_NODE:
    {
        CElementImpl* element = (CElementImpl*)(IAttr::Probe(node));
        element->namespaceAware = FALSE;
        element->localName = name;
        break;
    }
    default:
        //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
        //        "Cannot rename nodes of type " + node.getNodeType());
        Logger::E("NodeImpl", "SetName, 3 DOMException");
        assert(0);
        return;
    }
}

ECode NodeImpl::GetBaseURI(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    Int16 nodeType;
    GetNodeType(&nodeType);
    switch (nodeType) {
        case DOCUMENT_NODE: {
            String dURI;
            IDocument::Probe(this)->GetDocumentURI(&dURI);
            *str = SanitizeUri(dURI);
            return NOERROR;
        }
        case ELEMENT_NODE: {
            CElementImpl* element = (CElementImpl*)(IElement::Probe(this));
            String uri;
            element->GetAttributeNS(String("http://www.w3.org/XML/1998/namespace"), String("base"), &uri); // or "xml:base"

            //try {
                // if this node has no base URI, return the parent's.
                if (uri.IsNull() || uri.IsEmpty()) {
                    *str = GetParentBaseUri();
                    return NOERROR;
                }

                // if this node's URI is absolute, return it
                AutoPtr<IURI> iURI;
                CURI::New(uri, (IURI**)&iURI);
                Boolean isAbsolute;
                iURI->IsAbsolute(&isAbsolute);
                if (isAbsolute) {
                    *str = uri;
                    return NOERROR;
                }

                // this node has a relative URI. Try to resolve it against the
                // parent, but if that doesn't work just give up and return null.
                String parentUri = GetParentBaseUri();
                if (parentUri.IsNull()) {
                    *str = String(NULL);
                    return NOERROR;
                }

                iURI = NULL;
                CURI::New(parentUri, (IURI**)&iURI);
                AutoPtr<IURI> iURI2;
                iURI->Resolve(uri, (IURI**)&iURI2);
                String rVal;
                IObject::Probe(iURI2)->ToString(&rVal);
                *str = rVal;
                return NOERROR;
            //} catch (URISyntaxException e) {
            //    return null;
            //}
        }
        case PROCESSING_INSTRUCTION_NODE:
            *str = GetParentBaseUri();
            return NOERROR;

        case NOTATION_NODE:
        case ENTITY_NODE:
            // When we support these node types, the parser should
            // initialize a base URI field on these nodes.
            *str = String(NULL);
            return NOERROR;

        case ENTITY_REFERENCE_NODE:
            // TODO: get this value from the parser, falling back to the
            // referenced entity's baseURI if that doesn't exist
            *str = String(NULL);
            return NOERROR;

        case DOCUMENT_TYPE_NODE:
        case DOCUMENT_FRAGMENT_NODE:
        case ATTRIBUTE_NODE:
        case TEXT_NODE:
        case CDATA_SECTION_NODE:
        case COMMENT_NODE:
            *str = String(NULL);
            return NOERROR;

        default:
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
            //        "Unsupported node type " + getNodeType());
            Logger::E("NodeImpl", "GetBaseURI, DOMException");
            assert(0);
            *str = String(NULL);
            return NOERROR;
    }
}

String NodeImpl::GetParentBaseUri()
{
    AutoPtr<INode> parentNode;
    GetParentNode((INode**)&parentNode);
    if (parentNode != NULL) {
        String str;
        parentNode->GetBaseURI(&str);
        return str;
    }
    else {
        return String(NULL);
    }
}

/**
 * Returns the sanitized input if it is a URI, or {@code null} otherwise.
 */
String NodeImpl::SanitizeUri(
    /* [in] */ const String& uri)
{
    if (uri.IsNull() || uri.GetLength() == 0) {
        return String(NULL);
    }
    //try {
    AutoPtr<IURI> iURI;
    CURI::New(uri, (IURI**)&iURI);
    String rVal;
    IObject::Probe(iURI)->ToString(&rVal);
    return rVal;
    //} catch (URISyntaxException e) {
    //    return null;
    //}
}

ECode NodeImpl::CompareDocumentPosition(
    /* [in] */ INode* other,
    /* [out] */ Int16* value)
{
    //throw new UnsupportedOperationException(); // TODO
    Logger::E("NodeImpl", "GetBaseURI, DOMException");
    assert(0);
    return NOERROR;
}

ECode NodeImpl::GetTextContent(
    /* [out] */ String* str)
{
    return GetNodeValue(str);
}

void NodeImpl::GetTextContent(
    /* [in] */ IStringBuilder* buf)
{
    String content;
    GetNodeValue(&content);
    if (!content.IsNull()) {
        buf->Append(content);
    }
}

ECode NodeImpl::SetTextContent(
    /* [in] */ const String& textContent)
{
    Int16 nodeType;
    GetNodeType(&nodeType);
    switch (nodeType) {
        case DOCUMENT_TYPE_NODE:
        case DOCUMENT_NODE:
            return NOERROR; // do nothing!

        case ELEMENT_NODE:
        case ENTITY_NODE:
        case ENTITY_REFERENCE_NODE:
        case DOCUMENT_FRAGMENT_NODE: {
            // remove all existing children
            AutoPtr<INode> child;
            GetFirstChild((INode**)&child);
            while (child != NULL) {
                AutoPtr<INode> res;
                RemoveChild(child, (INode**)&res);
            }
            // create a text node to hold the given content
            if (!textContent.IsNull()&& textContent.GetLength() != 0) {
                AutoPtr<IText> text;
                document->CreateTextNode(textContent, (IText**)&text);
                AutoPtr<INode> res;
                AppendChild(INode::Probe(text), (INode**)&res);
            }
            return NOERROR;
        }
        case ATTRIBUTE_NODE:
        case TEXT_NODE:
        case CDATA_SECTION_NODE:
        case PROCESSING_INSTRUCTION_NODE:
        case COMMENT_NODE:
        case NOTATION_NODE:
            SetNodeValue(textContent);
            return NOERROR;

        default:
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
            //        "Unsupported node type " + getNodeType());
            Logger::E("NodeImpl", "SetTextContent, DOMException");
            assert(0);
            return NOERROR;
    }
}

ECode NodeImpl::IsSameNode(
    /* [in] */ INode* other,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = INode::Probe(this) == other;
    return NOERROR;
}

AutoPtr<NodeImpl> NodeImpl::GetNamespacingElement()
{
    Int16 nodeType;
    GetNodeType(&nodeType);
    switch (nodeType) {
        case ELEMENT_NODE:
            return this;

        case DOCUMENT_NODE:
        {
            AutoPtr<IElement> ele;
            IDocument::Probe(this)->GetDocumentElement((IElement**)&ele);
            return (NodeImpl*)(INode::Probe(ele));
        }

        case ENTITY_NODE:
        case NOTATION_NODE:
        case DOCUMENT_FRAGMENT_NODE:
        case DOCUMENT_TYPE_NODE:
            return NULL;

        case ATTRIBUTE_NODE:
        {
            IAttr* attr = IAttr::Probe(this);
            AutoPtr<IElement> ele;
            attr->GetOwnerElement((IElement**)&ele);
            return (NodeImpl*)(INode::Probe(ele));
        }
        case TEXT_NODE:
        case CDATA_SECTION_NODE:
        case ENTITY_REFERENCE_NODE:
        case PROCESSING_INSTRUCTION_NODE:
        case COMMENT_NODE:
            return GetContainingElement();

        default:
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
            //        "Unsupported node type " + getNodeType());
            Logger::E("NodeImpl", "GetNamespacingElement, DOMException");
            assert(0);
            return NULL;
    }
}

/**
 * Returns the nearest ancestor element that contains this node.
 */
AutoPtr<NodeImpl> NodeImpl::GetContainingElement()
{
    AutoPtr<INode> p, tmp;
    GetParentNode((INode**)&p);
    for (; p != NULL; tmp->GetParentNode((INode**)&p)) {
        tmp = p;
        Int16 nodeType;
        p->GetNodeType(&nodeType);
        if (nodeType == ELEMENT_NODE) {
            return (NodeImpl*)INode::Probe(p);
        }
    }
    return NULL;
}

ECode NodeImpl::LookupPrefix(
    /* [in] */ const String& namespaceURI,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    if (namespaceURI.IsNull()) {
        return NOERROR;
    }

    // the XML specs define some prefixes (like "xml" and "xmlns") but this
    // API is explicitly defined to ignore those.

    AutoPtr<NodeImpl> target = GetNamespacingElement();
    for (NodeImpl* node = target.Get(); node != NULL; node = node->GetContainingElement()) {
        // check this element's namespace first
        String nsURI;
        node->GetNamespaceURI(&nsURI);
        String prefix;
        node->GetPrefix(&prefix);
        if (namespaceURI.Equals(nsURI)
                && target->IsPrefixMappedToUri(prefix, namespaceURI)) {
            node->GetPrefix(str);
            return NOERROR;
        }

        // search this element for an attribute of this form:
        //   xmlns:foo="http://namespaceURI"
        Boolean hasAttributes;
        if (!(node->HasAttributes(&hasAttributes), hasAttributes)) {
            continue;
        }
        AutoPtr<INamedNodeMap> attributes;
        node->GetAttributes((INamedNodeMap**)&attributes);
        Int32 length;
        attributes->GetLength(&length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<INode> attr;
            attributes->Item(i, (INode**)&attr);

            String nsURI;
            attr->GetNamespaceURI(&nsURI);
            String prefix;
            attr->GetPrefix(&prefix);
            String nodeValue;
            attr->GetNodeValue(&nodeValue);
            if (!String("http://www.w3.org/2000/xmlns/").Equals(nsURI)
                    || !String("xmlns").Equals(prefix)
                    || !namespaceURI.Equals(nodeValue)) {
                continue;
            }
            String localName;
            attr->GetLocalName(&localName);
            if (target->IsPrefixMappedToUri(localName, namespaceURI)) {
                *str = localName;
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

Boolean NodeImpl::IsPrefixMappedToUri(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri)
{
    if (prefix.IsNull()) {
        return FALSE;
    }

    String actual;
    LookupNamespaceURI(prefix, &actual);
    return uri.Equals(actual);
}

ECode NodeImpl::IsDefaultNamespace(
    /* [in] */ const String& namespaceURI,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    String actual;
    LookupNamespaceURI(String(NULL), &actual); // null yields the default namespace
    if (namespaceURI.IsNull()) {
        *value = actual.IsNull();
    }
    else {
        *value = namespaceURI.Equals(actual);
    }
    return NOERROR;
}

ECode NodeImpl::LookupNamespaceURI(
    /* [in] */ const String& prefix,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<NodeImpl> target = GetNamespacingElement();
    for (NodeImpl* node = target; node != NULL; node = node->GetContainingElement()) {
        // check this element's namespace first
        String nodePrefix;
        node->GetPrefix(&nodePrefix);
        String nsURI;
        node->GetNamespaceURI(&nsURI);
        if (!nsURI.IsNull()) {
            if (prefix.IsNull() // null => default prefix
                    ? nodePrefix.IsNull()
                    : prefix.Equals(nodePrefix)) {
                *str = nsURI;
                return NOERROR;
            }
        }

        // search this element for an attribute of the appropriate form.
        //    default namespace: xmlns="http://resultUri"
        //          non default: xmlns:specifiedPrefix="http://resultUri"
        Boolean hasAttributes;
        if (!(node->HasAttributes(&hasAttributes), hasAttributes)) {
            continue;
        }
        AutoPtr<INamedNodeMap> attributes;
        node->GetAttributes((INamedNodeMap**)&attributes);
        Int32 length;
        attributes->GetLength(&length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<INode> attr;
            attributes->Item(i, (INode**)&attr);
            String nsURI;
            attr->GetNamespaceURI(&nsURI);
            if (!String("http://www.w3.org/2000/xmlns/").Equals(nsURI)) {
                continue;
            }
            String nodeName, aPrefix, localName;
            attr->GetNodeName(&nodeName);
            attr->GetPrefix(&aPrefix);
            attr->GetLocalName(&localName);

            if (prefix.IsNull()// null => default prefix
                    ? String("xmlns").Equals(nodeName)
                    : String("xmlns").Equals(aPrefix) && prefix.Equals(localName)) {
                String value;
                attr->GetNodeValue(&value);
                *str = value.GetLength() > 0 ? value : String(NULL);
                return NOERROR;
            }
        }
    }

    *str = String(NULL);
    return NOERROR;;
}

AutoPtr<IList> NodeImpl::CreateEqualityKey(
    /* [in] */ INode* node)
{
    //List<Object> values = new ArrayList<Object>();
    AutoPtr<IList> values;
    CArrayList::New((IList**)&values);
    Int16 nodeType;
    node->GetNodeType(&nodeType);
    values->Add(CoreUtils::Convert(nodeType));
    String nodeName;
    node->GetNodeName(&nodeName);
    values->Add(CoreUtils::Convert(nodeName));
    String localName;
    node->GetLocalName(&localName);
    values->Add(CoreUtils::Convert(localName));
    String nsURI;
    node->GetNamespaceURI(&nsURI);
    values->Add(CoreUtils::Convert(nsURI));
    String prefix;
    node->GetPrefix(&prefix);
    values->Add(CoreUtils::Convert(prefix));
    String nodeValue;
    node->GetNodeValue(&nodeValue);
    values->Add(CoreUtils::Convert(nodeValue));
    AutoPtr<INode> child, tmp;
    for (node->GetFirstChild((INode**)&child); child != NULL; tmp->GetNextSibling((INode**)&child)) {
        tmp = child;
        values->Add(child);
        child = NULL;
    }

    switch (nodeType) {
        case DOCUMENT_TYPE_NODE:
        {
            //DocumentTypeImpl doctype = (DocumentTypeImpl) node;
            IDocumentType* doctype = IDocumentType::Probe(node);
            String publicId;
            doctype->GetPublicId(&publicId);
            values->Add(CoreUtils::Convert(publicId));
            String systemId;
            doctype->GetSystemId(&systemId);
            values->Add(CoreUtils::Convert(systemId));
            String iSubset;
            doctype->GetInternalSubset(&iSubset);
            values->Add(CoreUtils::Convert(iSubset));
            AutoPtr<INamedNodeMap> nodeMap;
            doctype->GetEntities((INamedNodeMap**)&nodeMap);
            values->Add(nodeMap);
            AutoPtr<INamedNodeMap> notations;
            doctype->GetNotations((INamedNodeMap**)&notations);
            values->Add(notations);
            break;
        }
        case ELEMENT_NODE:
        {
            //Element element = (Element) node;
            IElement* element = IElement::Probe(node);
            AutoPtr<INamedNodeMap> attributes;
            INode::Probe(element)->GetAttributes((INamedNodeMap**)&attributes);
            values->Add(attributes);
            break;
        }
    }

    return values;
}

ECode NodeImpl::IsEqualNode(
    /* [in] */ INode* arg,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    if (arg == INode::Probe(this)) {
        *value = TRUE;
        return NOERROR;
    }

    //List<Object> listA = createEqualityKey(this);
    //List<Object> listB = createEqualityKey(arg);
    AutoPtr<IList> listA = CreateEqualityKey(this);
    AutoPtr<IList> listB = CreateEqualityKey(arg);

    Int32 sizeA, sizeB;
    listA->GetSize(&sizeA);
    listB->GetSize(&sizeB);
    if (sizeA != sizeB) {
        *value = FALSE;
        return NOERROR;
    }

    for (Int32 i = 0; i < sizeA; i++) {
        AutoPtr<IInterface> oa, ob;
        listA->Get(i, (IInterface**)&oa);
        listB->Get(i, (IInterface**)&ob);

        Object* a = (Object*)IObject::Probe(oa);;
        Object* b = (Object*)IObject::Probe(ob);

        if (a == b) {
            continue;

        } else if (a == NULL || b == NULL) {
            *value = FALSE;
            return NOERROR;

        } else if (ICharSequence::Probe(a) != NULL || IInteger16::Probe(a) != NULL) {
            if (!(Object::Equals(a, b))) {
                *value = FALSE;
                return NOERROR;
            }

        } else if (INamedNodeMap::Probe(a) != NULL) {
            if (!(INamedNodeMap::Probe(b) == NULL)
                    || !NamedNodeMapsEqual(INamedNodeMap::Probe(a), INamedNodeMap::Probe(b))) {
                *value = FALSE;
                return NOERROR;
            }

        } else if (INode::Probe(a) != NULL) {
            Boolean isen;
            if (!(INode::Probe(b) != NULL)
                    || !(INode::Probe(a)->IsEqualNode(INode::Probe(b), &isen), isen)) {
                *value = FALSE;
                return NOERROR;
            }

        } else {
            //throw new AssertionError(); // unexpected type
            Logger::E("NodeImpl", "IsEqualNode");
            assert(0);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    *value = TRUE;
    return NOERROR;;
}

Boolean NodeImpl::NamedNodeMapsEqual(
    /* [in] */ INamedNodeMap* a,
    /* [in] */ INamedNodeMap* b)
{
    Int32 lengthA, lengthB;
    a->GetLength(&lengthA);
    b->GetLength(&lengthB);
    if (lengthA != lengthB) {
        return FALSE;
    }
    for (Int32 i = 0; i < lengthA; i++) {
        AutoPtr<INode> aNode;
        a->Item(i, (INode**)&aNode);
        String localName;
        aNode->GetLocalName(&localName);
        String nodeName;
        aNode->GetNodeName(&nodeName);
        String nsURI;
        aNode->GetNamespaceURI(&nsURI);

        AutoPtr<INode> bNode;
        if (localName.IsNull()) {
            b->GetNamedItem(nodeName, (INode**)&bNode);
        }
        else {
            b->GetNamedItemNS(nsURI, localName, (INode**)&bNode);
        }
        Boolean isen;
        if (bNode == NULL || !(aNode->IsEqualNode(bNode, &isen), isen)) {
            return FALSE;
        }
    }
    return TRUE;
}

ECode NodeImpl::GetFeature(
    /* [in] */ const String& feature,
    /* [in] */ const String& version,
    /* [out] */ IObject** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;
    Boolean res;
    if (IsSupported(feature, version, &res), res) {
        *obj = this;
        REFCOUNT_ADD(*obj);
    }
    return NOERROR;
}

ECode NodeImpl::SetUserData(
    /* [in] */ const String& key,
    /* [in] */ IObject* data,
    /* [in] */ IUserDataHandler* handler,
    /* [out] */ IObject** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;
    if (key.IsNull()) {
        //throw new NullPointerException("key == null");
        Logger::E("NodeImpl", "SetUserData, key == null");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }
    //TODO
    assert(0);
    //Map<String, UserData> map = document.getUserDataMap(this);
    //UserData previous = data == null
    //        ? map.remove(key)
    //        : map.put(key, new UserData(data, handler));
    //return previous != null ? previous.value : null;
    return NOERROR;
}

ECode NodeImpl::GetUserData(
    /* [in] */ const String& key,
    /* [out] */ IObject** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;
    if (key.IsNull()) {
        //throw new NullPointerException("key == null");
        Logger::E("NodeImpl", "GetUserData, key == null");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }
    //TODO
    assert(0);
    //Map<String, UserData> map = document.getUserDataMapForRead(this);
    //UserData userData = map.get(key);
    //return userData != null ? userData.value : null;
    return NOERROR;
}

} // Dom
} // Xml
} // Harmony
} // Apache
} // Org
