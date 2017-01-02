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
#include "org/apache/harmony/xml/dom/CAttrImpl.h"
#include "org/apache/harmony/xml/dom/CCDATASectionImpl.h"
#include "org/apache/harmony/xml/dom/CProcessingInstructionImpl.h"
#include "org/apache/harmony/xml/dom/CEntityReferenceImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/CDOMConfigurationImpl.h"
#include "org/apache/harmony/xml/dom/CCommentImpl.h"
#include "org/apache/harmony/xml/dom/CElementImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentFragmentImpl.h"
#include "org/apache/harmony/xml/dom/TextImpl.h"
#include "org/apache/harmony/xml/dom/CElementImpl.h"
#include <elastos/utility/logging/Logger.h>

using Org::W3c::Dom::IText;
using Org::W3c::Dom::IEntityReference;
using Org::W3c::Dom::IProcessingInstruction;
using Org::W3c::Dom::IDOMConfiguration;
using Org::W3c::Dom::EIID_IDocument;
using Org::W3c::Dom::IElement;
using Org::W3c::Dom::IAttr;
using Org::W3c::Dom::ICDATASection;
using Org::W3c::Dom::ICharacterData;
using Org::W3c::Dom::IComment;
using Org::W3c::Dom::IDocumentFragment;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_OBJECT_IMPL(CDocumentImpl);
CAR_INTERFACE_IMPL(CDocumentImpl, InnerNodeImpl, IDocument);

CDocumentImpl::CDocumentImpl()
    : xmlVersion("1.0")
    , xmlStandalone(FALSE)
    , strictErrorChecking(TRUE)
{
}

ECode CDocumentImpl::constructor(
    /* [in] */ IDOMImplementation* impl,
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& qualifiedName,
    /* [in] */ IDocumentType* doctype,
    /* [in] */ const String& inputEncoding)
{
    InnerNodeImpl::constructor(NULL);
    this->document = this;
    this->domImplementation = impl;
    this->inputEncoding = inputEncoding;

    if (doctype != NULL) {
        AutoPtr<INode> node;
        AppendChild(INode::Probe(doctype), (INode**)&node);
    }

    if (!qualifiedName.IsNull()) {
        AutoPtr<IElement> element;
        CreateElementNS(namespaceURI, qualifiedName, (IElement**)&element);
        AutoPtr<INode> tmp;
        AppendChild(INode::Probe(element), (INode**)&tmp);
    }
    return NOERROR;
}

ECode CDocumentImpl::ImportNode(
    /* [in] */ Org::W3c::Dom::INode * pImportedNode,
    /* [in] */ Boolean deep,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    VALIDATE_NOT_NULL(ppNode);
    *ppNode = CloneOrImportNode(IUserDataHandler::NODE_IMPORTED, pImportedNode, deep);
    return NOERROR;
}

ECode CDocumentImpl::AdoptNode(
    /* [in] */ Org::W3c::Dom::INode * node,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    VALIDATE_NOT_NULL(ppNode);
    *ppNode = NULL;
    //TODO
    //if (!(node instanceof NodeImpl)) {
        //return null; // the API specifies this quiet failure
    //}
    AutoPtr<NodeImpl> nodeImpl = (NodeImpl*)node;
    Int16 nodeType;
    nodeImpl->GetNodeType(&nodeType);
    switch (nodeType) {
        case INode::ATTRIBUTE_NODE:
        {
            AutoPtr<CAttrImpl> attr = (CAttrImpl*)node;
            if (attr->ownerElement != NULL) {
                AutoPtr<IAttr> tmp;
                attr->ownerElement->RemoveAttributeNode(attr, (IAttr**)&tmp);
            }
            break;
        }
        case INode::DOCUMENT_FRAGMENT_NODE:
        case INode::ENTITY_REFERENCE_NODE:
        case INode::PROCESSING_INSTRUCTION_NODE:
        case INode::TEXT_NODE:
        case INode::CDATA_SECTION_NODE:
        case INode::COMMENT_NODE:
        case INode::ELEMENT_NODE:
            break;

        case INode::DOCUMENT_NODE:
        case INode::DOCUMENT_TYPE_NODE:
        case INode::ENTITY_NODE:
        case INode::NOTATION_NODE:
        {
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
            //        "Cannot adopt nodes of type " + nodeImpl.getNodeType());
            Logger::E("CDocumentImpl", "AdoptNode, Cannot adopt nodes of type:%d", nodeType);
            assert(0);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        default:
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
            //        "Unsupported node type " + node.getNodeType());
            Logger::E("CDocumentImpl", "AdoptNode, unsupported node type:%d", nodeType);
            assert(0);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    AutoPtr<INode> parent;
    nodeImpl->GetParentNode((INode**)&parent);
    if (parent != NULL) {
        AutoPtr<INode> tmp;
        parent->RemoveChild(nodeImpl, (INode**)&tmp);
    }

    ChangeDocumentToThis(nodeImpl);
    NotifyUserDataHandlers(IUserDataHandler::NODE_ADOPTED, node, NULL);
    *ppNode = nodeImpl;
    REFCOUNT_ADD(*ppNode);
    return NOERROR;
}

//===============================================
ECode CDocumentImpl::GetNodeName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String("#document");
    return NOERROR;
}

ECode CDocumentImpl::GetNodeType(
    /* [out] */ Int16 * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = INode::DOCUMENT_NODE;
    return NOERROR;
}

ECode CDocumentImpl::InsertChildAt(
    /* [in] */ INode* toInsert,
    /* [in] */ Int32 index,
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<IElement> element;
    if (IElement::Probe(toInsert) != NULL && (GetDocumentElement((IElement**)&element), element) != NULL) {
        //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR,
        //        "Only one root element allowed");
        Logger::E("CDocumentImpl", "HIERARCHY_REQUEST_ERR, Only one root element allowed");
        assert(0);
        return NOERROR;
    }
    AutoPtr<IDocumentType> docType;
    if (IDocumentType::Probe(toInsert) != NULL && (GetDoctype((IDocumentType**)&docType), docType) != NULL) {
        //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR,
        //        "Only one DOCTYPE element allowed");
        Logger::E("CDocumentImpl", "HIERARCHY_REQUEST_ERR, Only one DOCTYPE element allowed");
        assert(0);
        return NOERROR;
    }
    return InnerNodeImpl::InsertChildAt(toInsert, index, result);
}

ECode CDocumentImpl::GetTextContent(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String(NULL);
    return NOERROR;
}

ECode CDocumentImpl::GetDoctype(
    /* [out] */ Org::W3c::Dom::IDocumentType ** ppDoctype)
{
    VALIDATE_NOT_NULL(ppDoctype);
    *ppDoctype = NULL;

    Int32 size;
    children->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> value;
        children->Get(i, (IInterface**)&value);
        LeafNodeImpl* child = (LeafNodeImpl*)INode::Probe(value);
        if (IDocumentType::Probe(child) != NULL) {
            *ppDoctype = IDocumentType::Probe(child);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CDocumentImpl::GetImplementation(
    /* [out] */ Org::W3c::Dom::IDOMImplementation ** ppDom)
{
    VALIDATE_NOT_NULL(ppDom);
    *ppDom = domImplementation;
    REFCOUNT_ADD(*ppDom);
    return NOERROR;
}

ECode CDocumentImpl::GetDocumentElement(
    /* [out] */ Org::W3c::Dom::IElement ** ppElement)
{
    VALIDATE_NOT_NULL(ppElement);
    *ppElement = NULL;

    Int32 size;
    children->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> value;
        children->Get(i, (IInterface**)&value);
        LeafNodeImpl* child = (LeafNodeImpl*)INode::Probe(value);
        if (IElement::Probe(child) != NULL) {
            *ppElement= IElement::Probe(child);
            REFCOUNT_ADD(*ppElement);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CDocumentImpl::CreateElement(
    /* [in] */ const String& tagName,
    /* [out] */ Org::W3c::Dom::IElement ** ppElement)
{
    VALIDATE_NOT_NULL(ppElement);
    AutoPtr<CElementImpl> ele = new CElementImpl();
    ele->constructor(this, tagName);
    *ppElement = ele;
    REFCOUNT_ADD(*ppElement);
    return NOERROR;
}

ECode CDocumentImpl::CreateDocumentFragment(
    /* [out] */ Org::W3c::Dom::IDocumentFragment ** ppDocfra)
{
    VALIDATE_NOT_NULL(ppDocfra);
    AutoPtr<CDocumentFragmentImpl> dfi = new CDocumentFragmentImpl();
    dfi->constructor(this);
    *ppDocfra = dfi;
    REFCOUNT_ADD(*ppDocfra);
    return NOERROR;
}

ECode CDocumentImpl::CreateTextNode(
    /* [in] */ const String& data,
    /* [out] */ Org::W3c::Dom::IText ** ppText)
{
    VALIDATE_NOT_NULL(ppText);
    AutoPtr<TextImpl> result = new TextImpl();
    result->constructor(this, data);
    *ppText = result;
    REFCOUNT_ADD(*ppText);
    return NOERROR;
}

ECode CDocumentImpl::CreateComment(
    /* [in] */ const String& data,
    /* [out] */ Org::W3c::Dom::IComment ** ppComment)
{
    VALIDATE_NOT_NULL(ppComment);
    AutoPtr<CCommentImpl> comment = new CCommentImpl();
    comment->constructor(this, data);
    *ppComment = comment;
    REFCOUNT_ADD(*ppComment);
    return NOERROR;
}

ECode CDocumentImpl::CreateCDATASection(
    /* [in] */ const String& data,
    /* [out] */ Org::W3c::Dom::ICDATASection ** ppDatasection)
{
    VALIDATE_NOT_NULL(ppDatasection);
    AutoPtr<CCDATASectionImpl> result = new CCDATASectionImpl();
    result->constructor(this, data);
    *ppDatasection = result;
    REFCOUNT_ADD(*ppDatasection);
    return NOERROR;
}

ECode CDocumentImpl::CreateProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data,
    /* [out] */ Org::W3c::Dom::IProcessingInstruction ** ppProins)
{
    VALIDATE_NOT_NULL(ppProins);
    AutoPtr<CProcessingInstructionImpl> result = new CProcessingInstructionImpl();
    result->constructor(this, target, data);
    *ppProins = result;
    REFCOUNT_ADD(*ppProins);
    return NOERROR;
}

ECode CDocumentImpl::CreateAttribute(
    /* [in] */ const String& name,
    /* [out] */ Org::W3c::Dom::IAttr ** ppAttr)
{
    VALIDATE_NOT_NULL(ppAttr);
    AutoPtr<CAttrImpl> result = new CAttrImpl();
    result->constructor(this, name);
    *ppAttr = result;
    REFCOUNT_ADD(*ppAttr);
    return NOERROR;
}

ECode CDocumentImpl::CreateEntityReference(
    /* [in] */ const String& name,
    /* [out] */ Org::W3c::Dom::IEntityReference ** ppEntity)
{
    VALIDATE_NOT_NULL(ppEntity);
    AutoPtr<CEntityReferenceImpl> eri = new CEntityReferenceImpl();
    eri->constructor(this, name);
    *ppEntity = eri;
    REFCOUNT_ADD(*ppEntity);
    return NOERROR;
}

ECode CDocumentImpl::GetElementsByTagName(
    /* [in] */ const String& tagname,
    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
{
    VALIDATE_NOT_NULL(ppNodelist);
    AutoPtr<CNodeListImpl> result = new CNodeListImpl();
    InnerNodeImpl::GetElementsByTagName(result, tagname);
    *ppNodelist = result;
    REFCOUNT_ADD(*ppNodelist);
    return NOERROR;
}

ECode CDocumentImpl::CreateElementNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& qualifiedName,
    /* [out] */ Org::W3c::Dom::IElement ** ppElement)
{
    VALIDATE_NOT_NULL(ppElement);
    AutoPtr<CElementImpl> ele = new CElementImpl();
    ele->constructor(this, namespaceURI, qualifiedName);
    *ppElement = ele;
    REFCOUNT_ADD(*ppElement);
    return NOERROR;
}

ECode CDocumentImpl::CreateAttributeNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& qualifiedName,
    /* [out] */ Org::W3c::Dom::IAttr ** ppAttr)
{
    VALIDATE_NOT_NULL(ppAttr);
    AutoPtr<CAttrImpl> result = new CAttrImpl();
    result->constructor(this, namespaceURI, qualifiedName);
    *ppAttr = result;
    REFCOUNT_ADD(*ppAttr);
    return NOERROR;
}

ECode CDocumentImpl::GetElementsByTagNameNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName,
    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
{
    VALIDATE_NOT_NULL(ppNodelist);
    AutoPtr<CNodeListImpl> result = new CNodeListImpl();
    InnerNodeImpl::GetElementsByTagNameNS(result, namespaceURI, localName);
    *ppNodelist = result;
    REFCOUNT_ADD(*ppNodelist);
    return NOERROR;
}

ECode CDocumentImpl::GetElementById(
    /* [in] */ const String& elementId,
    /* [out] */ Org::W3c::Dom::IElement ** ppElment)
{
    VALIDATE_NOT_NULL(ppElment);
    *ppElment = NULL;

    AutoPtr<IElement> element;
    GetDocumentElement((IElement**)&element);
    AutoPtr<CElementImpl> root = (CElementImpl*)(element.Get());

    if (root != NULL) {
        root->GetElementById(elementId, ppElment);
    }
    return NOERROR;
}

ECode CDocumentImpl::GetInputEncoding(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = inputEncoding;
    return NOERROR;
}

ECode CDocumentImpl::GetXmlEncoding(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = xmlEncoding;
    return NOERROR;
}

ECode CDocumentImpl::GetXmlStandalone(
    /* [out] */ Boolean * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = xmlStandalone;
    return NOERROR;
}

ECode CDocumentImpl::SetXmlStandalone(
    /* [in] */ Boolean xmlStandalone)
{
    this->xmlStandalone = xmlStandalone;
    return NOERROR;
}

ECode CDocumentImpl::GetXmlVersion(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = xmlVersion;
    return NOERROR;
}

ECode CDocumentImpl::SetXmlVersion(
    /* [in] */ const String& xmlVersion)
{
    this->xmlVersion = xmlVersion;
    return NOERROR;
}

ECode CDocumentImpl::GetStrictErrorChecking(
    /* [out] */ Boolean * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = strictErrorChecking;
    return NOERROR;
}

ECode CDocumentImpl::SetStrictErrorChecking(
    /* [in] */ Boolean strictErrorChecking)
{
    this->strictErrorChecking = strictErrorChecking;
    return NOERROR;
}

ECode CDocumentImpl::GetDocumentURI(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = documentUri;
    return NOERROR;
}

ECode CDocumentImpl::SetDocumentURI(
    /* [in] */ const String& documentURI)
{
    this->documentUri = documentUri;
    return NOERROR;
}

ECode CDocumentImpl::GetDomConfig(
    /* [out] */ Org::W3c::Dom::IDOMConfiguration ** ppDomconfig)
{
    VALIDATE_NOT_NULL(ppDomconfig);
    if (domConfiguration == NULL) {
        domConfiguration = new CDOMConfigurationImpl();
    }
    *ppDomconfig = IDOMConfiguration::Probe(domConfiguration);
    REFCOUNT_ADD(*ppDomconfig);
    return NOERROR;
}

ECode CDocumentImpl::NormalizeDocument()
{
    AutoPtr<IElement> root;
    GetDocumentElement((IElement**)&root);
    if (root == NULL) {
        return NOERROR;
    }

    AutoPtr<IDOMConfiguration> domConfig;
    GetDomConfig((IDOMConfiguration**)&domConfig);
    ((CDOMConfigurationImpl*)domConfig.Get())->Normalize(INode::Probe(root));
    return NOERROR;
}

ECode CDocumentImpl::RenameNode(
    /* [in] */ Org::W3c::Dom::INode * node,
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& qualifiedName,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    VALIDATE_NOT_NULL(ppNode);
    *ppNode = NULL;
    AutoPtr<IDocument> doc;
    node->GetOwnerDocument((IDocument**)&doc);
    if (doc.Get() != IDocument::Probe(this)) {
        //throw new DOMException(DOMException.WRONG_DOCUMENT_ERR, null);
        Logger::E("CDocumentImpl", "WRONG_DOCUMENT_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    SetNameNS((NodeImpl*) node, namespaceURI, qualifiedName);
    NotifyUserDataHandlers(IUserDataHandler::NODE_RENAMED, node, NULL);
    *ppNode = node;
    REFCOUNT_ADD(*ppNode);
    return NOERROR;
}

//ECode CDocumentImpl::constructor(
//    /* [in] */ Org::W3c::Dom::IDOMImplementation * pImpl,
//    /* [in] */ const String& namespaceURI,
//    /* [in] */ const String& qualifiedName,
//    /* [in] */ Org::W3c::Dom::IDocumentType * pDoctype,
//    /* [in] */ const String& inputEncoding)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}

AutoPtr<NodeImpl> CDocumentImpl::ShallowCopy(
    /* [in] */ Int16 operation,
    /* [in] */ INode* node)
{
    Int16 nodeType;
    node->GetNodeType(&nodeType);
    switch (nodeType) {
        case INode::ATTRIBUTE_NODE:
        {
            AutoPtr<CAttrImpl> attr = (CAttrImpl*) node;
            AutoPtr<CAttrImpl> attrCopy;
            if (attr->namespaceAware) {
                String nsURI, localName;
                attr->GetNamespaceURI(&nsURI);
                attr->GetLocalName(&localName);
                AutoPtr<IAttr> iattr;
                CreateAttributeNS(nsURI, localName, (IAttr**)&iattr);
                attrCopy = (CAttrImpl*)(iattr.Get());
                String prefix;
                attr->GetPrefix(&prefix);
                attrCopy->SetPrefix(prefix);
            } else {
                String name;
                attr->GetName(&name);
                AutoPtr<IAttr> iattr;
                CreateAttribute(name, (IAttr**)&iattr);
                attrCopy = (CAttrImpl*)(iattr.Get());
            }
            String value;
            attr->GetValue(&value);
            attrCopy->SetNodeValue(value);
            return attrCopy;
        }
        case INode::CDATA_SECTION_NODE:
        {
            AutoPtr<ICDATASection> dataSection;
            String data;
            ICharacterData::Probe(node)->GetData(&data);
            CreateCDATASection(data, (ICDATASection**)&dataSection);
            return (NodeImpl*)(INode::Probe(dataSection));
        }
        case INode::COMMENT_NODE:
        {
            AutoPtr<IComment> comment;
            String data;
            ICharacterData::Probe(node)->GetData(&data);
            CreateComment(data, (IComment**)&comment);
            return (NodeImpl*)(INode::Probe(comment));
        }
        case INode::DOCUMENT_FRAGMENT_NODE:
        {
            AutoPtr<IDocumentFragment> df;
            CreateDocumentFragment((IDocumentFragment**)&df);
            return (NodeImpl*)(INode::Probe(df));
        }
        case INode::DOCUMENT_NODE:
        case INode::DOCUMENT_TYPE_NODE:
        {
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
            //        "Cannot copy node of type " + node.getNodeType());
            Logger::E("CDocumentImpl", "ShallowCopy DOMException.NOT_SUPPORTED_ERR Cannot copy node of type:%d", nodeType);
            assert(0);
            return NULL;
        }
        case INode::ELEMENT_NODE:
        {
            AutoPtr<CElementImpl> element = (CElementImpl*)IElement::Probe(node);
            AutoPtr<CElementImpl> elementCopy;
            if (element->namespaceAware) {
                String nsURI, localName;
                element->GetNamespaceURI(&nsURI);
                element->GetLocalName(&localName);
                AutoPtr<IElement> iele;
                CreateElementNS(nsURI, localName, (IElement**)&iele);
                elementCopy = (CElementImpl*)(iele.Get());
                String prefix;
                element->GetPrefix(&prefix);
                elementCopy->SetPrefix(prefix);
            } else {
                String tagName;
                element->GetTagName(&tagName);
                AutoPtr<IElement> iele;
                CreateElement(tagName, (IElement**)&iele);
                elementCopy = (CElementImpl*)(iele.Get());
            }

            AutoPtr<INamedNodeMap> attributes;
            element->GetAttributes((INamedNodeMap**)&attributes);
            Int32 length;
            attributes->GetLength(&length);
            for (Int32 i = 0; i < length; i++) {
                AutoPtr<INode> inode;
                attributes->Item(i, (INode**)&inode);
                AutoPtr<CAttrImpl> elementAttr = (CAttrImpl*)(IAttr::Probe(inode));
                AutoPtr<CAttrImpl> elementAttrCopy = (CAttrImpl*) (ShallowCopy(operation, elementAttr).Get());
                NotifyUserDataHandlers(operation, elementAttr, elementAttrCopy);
                if (elementAttr->namespaceAware) {
                    AutoPtr<IAttr> tmp;
                    elementCopy->SetAttributeNodeNS(elementAttrCopy, (IAttr**)&tmp);
                } else {
                    AutoPtr<IAttr> tmp;
                    elementCopy->SetAttributeNode(elementAttrCopy, (IAttr**)&tmp);
                }
            }
            return elementCopy;
        }
        case INode::ENTITY_NODE:
        case INode::NOTATION_NODE:
        {
            // TODO: implement this when we support these node types
            //throw new UnsupportedOperationException();
            Logger::E("CDocumentImpl", "ShallowCopy UnsupportedOperationException");
            assert(0);
            return NULL;
        }

        case INode::ENTITY_REFERENCE_NODE:
        {
            /*
             * When we support entities in the doctype, this will need to
             * behave differently for clones vs. imports. Clones copy
             * entities by value, copying the referenced subtree from the
             * original document. Imports copy entities by reference,
             * possibly referring to a different subtree in the new
             * document.
             */
            String nodeName;
            node->GetNodeName(&nodeName);
            AutoPtr<IEntityReference> er;
            CreateEntityReference(nodeName, (IEntityReference**)&er);
            return (NodeImpl*)INode::Probe(er);
        }
        case INode::PROCESSING_INSTRUCTION_NODE:
        {
            AutoPtr<IProcessingInstruction> pi = IProcessingInstruction::Probe(node);
            String target;
            pi->GetTarget(&target);
            String data;
            pi->GetData(&data);
            AutoPtr<IProcessingInstruction> ipi;
            CreateProcessingInstruction(target, data, (IProcessingInstruction**)&ipi);
            return (NodeImpl*)(INode::Probe(ipi));
        }
        case INode::TEXT_NODE:
        {
            String data;
            ICharacterData::Probe(node)->GetData(&data);
            AutoPtr<IText> text;
            CreateTextNode(data, (IText**)&text);
            return (NodeImpl*)(INode::Probe(text));
        }
        default:
            Logger::E("CDocumentImpl", "ShallowCopy Unsupported node type:%d", nodeType);
            assert(0);
            return NULL;
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR,
            //        "Unsupported node type " + node.getNodeType());
    }
}

AutoPtr<INode> CDocumentImpl::CloneOrImportNode(
    /* [in] */ Int16 operation,
    /* [in] */ INode* node,
    /* [in] */ Boolean deep)
{
    AutoPtr<NodeImpl> copy = ShallowCopy(operation, node);

    if (deep) {
        AutoPtr<INodeList> list;
        node->GetChildNodes((INodeList**)&list);
        Int32 length;
        list->GetLength(&length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<INode> node;
            list->Item(i, (INode**)&node);
            AutoPtr<INode> tmp;
            copy->AppendChild(CloneOrImportNode(operation, node, deep), (INode**)&tmp);
        }
    }

    NotifyUserDataHandlers(operation, node, copy);
    return copy;
}

void CDocumentImpl::ChangeDocumentToThis(
    /* [in] */ NodeImpl* node)
{
    //TODO
    assert(0);
    //Map<String, UserData> userData = node.document.GetUserDataMapForRead(node);
    //if (!userData.isEmpty()) {
    //    getUserDataMap(node).putAll(userData);
    //}
    //node.document = this;

    //// change the document on all child nodes
    //NodeList list = node.getChildNodes();
    //for (int i = 0; i < list.getLength(); i++) {
    //    changeDocumentToThis((NodeImpl) list.item(i));
    //}

    //// change the document on all attribute nodes
    //if (node.getNodeType() == Node.ELEMENT_NODE) {
    //    NamedNodeMap attributes = node.getAttributes();
    //    for (int i = 0; i < attributes.getLength(); i++) {
    //        changeDocumentToThis((AttrImpl) attributes.item(i));
    //    }
    //}
}

AutoPtr<IMap> CDocumentImpl::GetUserDataMap(
    /* [in] */ NodeImpl* node)
{
    //TODO
    assert(0);
    return NULL;
    //if (nodeToUserData == null) {
    //    nodeToUserData = new WeakHashMap<NodeImpl, Map<String, UserData>>();
    //}
    //Map<String, UserData> userDataMap = nodeToUserData.get(node);
    //if (userDataMap == null) {
    //    userDataMap = new HashMap<String, UserData>();
    //    nodeToUserData.put(node, userDataMap);
    //}
    //return userDataMap;
}

AutoPtr<IMap> CDocumentImpl::GetUserDataMapForRead(
    /* [in] */ NodeImpl* node)
{
    //TODO
    assert(0);
    return NULL;
    //if (nodeToUserData == null) {
    //    return Collections.emptyMap();
    //}
    //Map<String, UserData> userDataMap = nodeToUserData.get(node);
    //return userDataMap == null
    //    ? Collections.<String, UserData>emptyMap()
    //    : userDataMap;
}

void CDocumentImpl::NotifyUserDataHandlers(
    /* [in] */ Int16 operation,
    /* [in] */ INode* source,
    /* [in] */ NodeImpl* destination)
{
    //TODO
    assert(0);
    //if (!(source instanceof NodeImpl)) {
    //    return;
    //}

    //NodeImpl srcImpl = (NodeImpl) source;
    //if (srcImpl.document == null) {
    //    return;
    //}

    //for (Map.Entry<String, UserData> entry
    //        : srcImpl.document.getUserDataMapForRead(srcImpl).entrySet()) {
    //    UserData userData = entry.getValue();
    //    if (userData.handler != null) {
    //        userData.handler.handle(
    //                operation, entry.getKey(), userData.value, source, destination);
    //    }
    //}
}

//ECode CDocumentImpl::GetNodeValue(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::SetNodeValue(
//    /* [in] */ const String& nodeValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetParentNode(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetChildNodes(
//    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetFirstChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetLastChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetPreviousSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetNextSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetAttributes(
//    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetOwnerDocument(
//    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::InsertBefore(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pRefChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::ReplaceChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::RemoveChild(
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::AppendChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::HasChildNodes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::CloneNode(
//    /* [in] */ Boolean deep,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::Normalize()
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::IsSupported(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetNamespaceURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetPrefix(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::SetPrefix(
//    /* [in] */ const String& prefix)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetLocalName(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::HasAttributes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetBaseURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::CompareDocumentPosition(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Int16 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::SetTextContent(
//    /* [in] */ const String& textContent)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::IsSameNode(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::LookupPrefix(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::IsDefaultNamespace(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::LookupNamespaceURI(
//    /* [in] */ const String& prefix,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::IsEqualNode(
//    /* [in] */ Org::W3c::Dom::INode * pArg,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetFeature(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::SetUserData(
//    /* [in] */ const String& key,
//    /* [in] */ IObject * pData,
//    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::GetUserData(
//    /* [in] */ const String& key,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CDocumentImpl::IsParentOf(
//    /* [in] */ Org::W3c::Dom::INode * pNode,
//    /* [out] */ Boolean * pIsParentOf)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}

}
}
}
}
}

