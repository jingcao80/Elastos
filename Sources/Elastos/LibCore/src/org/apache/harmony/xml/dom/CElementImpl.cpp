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
#include "org/apache/harmony/xml/dom/CElementImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/CAttrImpl.h"
#include "elastos/utility/CArrayList.h"
#include <elastos/utility/logging/Logger.h>

using Org::W3c::Dom::EIID_INamedNodeMap;
using Org::W3c::Dom::IAttr;
using Org::W3c::Dom::EIID_IElement;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_INTERFACE_IMPL(CElementImpl::ElementAttrNamedNodeMapImpl, Object, INamedNodeMap);

ECode CElementImpl::ElementAttrNamedNodeMapImpl::constructor(
    /* [in] */ IElement* host)
{
    mHost = (CElementImpl*)host;
    return NOERROR;
}

ECode CElementImpl::ElementAttrNamedNodeMapImpl::GetLength(
    /* [out] */ Int32* result)
{
    return mHost->attributes->GetSize(result);
}

ECode CElementImpl::ElementAttrNamedNodeMapImpl::GetNamedItem(
    /* [in] */ const String& name,
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IAttr> attr;
    mHost->GetAttributeNode(name, (IAttr**)&attr);
    *result = INode::Probe(attr);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CElementImpl::ElementAttrNamedNodeMapImpl::GetNamedItemNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName,
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IAttr> attr;
    mHost->GetAttributeNodeNS(namespaceURI, localName, (IAttr**)&attr);
    *result = INode::Probe(attr);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CElementImpl::ElementAttrNamedNodeMapImpl::Item(
    /* [in] */ Int32 index,
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> iatt;
    mHost->attributes->Get(index, (IInterface**)&iatt);
    *result = INode::Probe(iatt);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CElementImpl::ElementAttrNamedNodeMapImpl::RemoveNamedItem(
    /* [in] */ const String& name,
    /* [out] */ INode** result)
{
    Int32 i = IndexOfItem(name);

    if (i == -1) {
        //throw new DOMException(DOMException.NOT_FOUND_ERR, null);
        Logger::E("CElementImpl::ElementAttrNamedNodeMapImpl", "RemoveNamedItem");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return mHost->attributes->Remove(i);
}

ECode CElementImpl::ElementAttrNamedNodeMapImpl::RemoveNamedItemNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName,
    /* [out] */ INode** result)
{
    Int32 i = IndexOfItemNS(namespaceURI, localName);

    if (i == -1) {
        //throw new DOMException(DOMException.NOT_FOUND_ERR, null);
        Logger::E("CElementImpl::ElementAttrNamedNodeMapImpl", "NOT_FOUND_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return mHost->attributes->Remove(i);
}

ECode CElementImpl::ElementAttrNamedNodeMapImpl::SetNamedItem(
    /* [in] */ INode* arg,
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    if (IAttr::Probe(arg) == NULL) {
        //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
        Logger::E("CElementImpl::ElementAttrNamedNodeMapImpl", "SetNamedItem");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IAttr> tmp;
    mHost->SetAttributeNode(IAttr::Probe(arg), (IAttr**)&tmp);
    *result = INode::Probe(tmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CElementImpl::ElementAttrNamedNodeMapImpl::SetNamedItemNS(
    /* [in] */ INode* arg,
    /* [out] */ INode** result)
{
    if (IAttr::Probe(arg) == NULL) {
        //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
        Logger::E("CElementImpl::ElementAttrNamedNodeMapImpl", "SetNamedItemNS");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IAttr> tmp;
    mHost->SetAttributeNodeNS(IAttr::Probe(arg), (IAttr**)&tmp);
    *result = INode::Probe(tmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Int32 CElementImpl::ElementAttrNamedNodeMapImpl::IndexOfItem(
    /* [in] */ const String& name)
{
    return mHost->IndexOfAttribute(name);
}

Int32 CElementImpl::ElementAttrNamedNodeMapImpl::IndexOfItemNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName)
{
    return mHost->IndexOfAttributeNS(namespaceURI, localName);
}

CAR_OBJECT_IMPL(CElementImpl)
CAR_INTERFACE_IMPL(CElementImpl, InnerNodeImpl, IElement);

CElementImpl::CElementImpl()
    : namespaceAware(FALSE)
{
    CArrayList::New((IList**)&attributes);
}

ECode CElementImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& qualifiedName)
{
    InnerNodeImpl::constructor(document);
    SetNameNS(this, namespaceURI, qualifiedName);
    return NOERROR;
}

ECode CElementImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& name)
{
    InnerNodeImpl::constructor(document);
    SetName(this, name);
    return NOERROR;
}

ECode CElementImpl::GetNodeName(
    /* [out] */ String * pStr)
{
    return GetTagName(pStr);
}

ECode CElementImpl::GetNodeType(
    /* [out] */ Int16 * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = INode::ELEMENT_NODE;
    return NOERROR;
}

ECode CElementImpl::GetAttributes(
    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
{
    VALIDATE_NOT_NULL(ppNodemap);
    AutoPtr<ElementAttrNamedNodeMapImpl> eannmi = new ElementAttrNamedNodeMapImpl();
    eannmi->constructor(this);
    *ppNodemap = eannmi;
    REFCOUNT_ADD(*ppNodemap);
    return NOERROR;
}

ECode CElementImpl::GetNamespaceURI(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = namespaceURI;
    return NOERROR;
}

ECode CElementImpl::GetPrefix(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = prefix;
    return NOERROR;
}

ECode CElementImpl::SetPrefix(
    /* [in] */ const String& prefix)
{
    this->prefix = ValidatePrefix(prefix, namespaceAware, namespaceURI);
    return NOERROR;
}

ECode CElementImpl::GetLocalName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String(NULL);
    if (namespaceAware) {
        *pStr = localName;
    }
    return NOERROR;
}

ECode CElementImpl::HasAttributes(
    /* [out] */ Boolean * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    Boolean isEmpty;
    *pValue = !(attributes->IsEmpty(&isEmpty), isEmpty);
    return NOERROR;
}

ECode CElementImpl::GetTagName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    if (!prefix.IsNull()) {
        *pStr = prefix + ":" + localName;
    }
    else {
        *pStr = localName;
    }
    return NOERROR;
}

ECode CElementImpl::GetAttribute(
    /* [in] */ const String& name,
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    AutoPtr<IAttr> attr;
    GetAttributeNode(name, (IAttr**)&attr);

    if (attr == NULL) {
        *pStr = String("");
    }
    attr->GetValue(pStr);

    return NOERROR;
}

ECode CElementImpl::SetAttribute(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    AutoPtr<IAttr> attr;
    GetAttributeNode(name, (IAttr**)&attr);

    if (attr == NULL) {
        document->CreateAttribute(name, (IAttr**)&attr);
        AutoPtr<IAttr> tmp;
        SetAttributeNode(attr, (IAttr**)&tmp);
    }

    attr->SetValue(value);
    return NOERROR;
}

ECode CElementImpl::RemoveAttribute(
    /* [in] */ const String& name)
{
    Int32 i = IndexOfAttribute(name);

    if (i != -1) {
        attributes->Remove(i);
    }
    return NOERROR;
}

ECode CElementImpl::GetAttributeNode(
    /* [in] */ const String& name,
    /* [out] */ Org::W3c::Dom::IAttr ** ppAttr)
{
    VALIDATE_NOT_NULL(ppAttr);
    Int32 i = IndexOfAttribute(name);

    if (i == -1) {
        *ppAttr = NULL;
        return NOERROR;
    }

    AutoPtr<IInterface> value;
    attributes->Get(i, (IInterface**)&value);
    *ppAttr = IAttr::Probe(value);
    REFCOUNT_ADD(*ppAttr);
    return NOERROR;
}

ECode CElementImpl::SetAttributeNode(
    /* [in] */ Org::W3c::Dom::IAttr * pNewAttr,
    /* [out] */ Org::W3c::Dom::IAttr ** ppAttr)
{
    VALIDATE_NOT_NULL(ppAttr);
    *ppAttr = NULL;
    AutoPtr<CAttrImpl> newAttrImpl = (CAttrImpl*)pNewAttr;

    if (newAttrImpl->document != this->document) {
        //throw new DOMException(DOMException.WRONG_DOCUMENT_ERR, null);
        Logger::E("CElementImpl", "SetAttributeNodeNS WRONG_DOCUMENT_ERR");
        assert(0);
        return NOERROR;
    }

    AutoPtr<IElement> ownerElement;
    if ((newAttrImpl->GetOwnerElement((IElement**)&ownerElement), ownerElement) != NULL) {
        //throw new DOMException(DOMException.INUSE_ATTRIBUTE_ERR, null);
        Logger::E("CElementImpl", "SetAttributeNodeNS INUSE_ATTRIBUTE_ERR");
        assert(0);
        return NOERROR;
    }

    AutoPtr<CAttrImpl> oldAttrImpl;

    String name;
    pNewAttr->GetName(&name);
    Int32 i = IndexOfAttribute(name);
    if (i != -1) {
        AutoPtr<IInterface> iAttr;
        attributes->Get(i, (IInterface**)&iAttr);
        oldAttrImpl = (CAttrImpl*)IAttr::Probe(iAttr);
        attributes->Remove(i);
    }

    attributes->Add((IObject*)newAttrImpl.Get());
    newAttrImpl->ownerElement = this;

    *ppAttr = oldAttrImpl;
    REFCOUNT_ADD(*ppAttr);
    return NOERROR;
}

ECode CElementImpl::RemoveAttributeNode(
    /* [in] */ Org::W3c::Dom::IAttr * pOldAttr,
    /* [out] */ Org::W3c::Dom::IAttr ** ppAttr)
{
    VALIDATE_NOT_NULL(ppAttr);
    AutoPtr<CAttrImpl> oldAttrImpl = (CAttrImpl*)pOldAttr;

    AutoPtr<IElement> ownerElement;
    oldAttrImpl->GetOwnerElement((IElement**)&ownerElement);
    if (ownerElement.Get() != IElement::Probe(this)) {
        //throw new DOMException(DOMException.NOT_FOUND_ERR, null);
        Logger::E("CElementImpl", "RemoveAttributeNode, NOT_FOUND_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    attributes->Remove((IObject*)oldAttrImpl);
    oldAttrImpl->ownerElement = NULL;

    *ppAttr = oldAttrImpl;
    REFCOUNT_ADD(*ppAttr);
    return NOERROR;
}

ECode CElementImpl::GetElementsByTagName(
    /* [in] */ const String& name,
    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
{
    VALIDATE_NOT_NULL(ppNodelist);
    CNodeListImpl* result = new CNodeListImpl();
    InnerNodeImpl::GetElementsByTagName(result, name);
    *ppNodelist = result;
    REFCOUNT_ADD(*ppNodelist);
    return NOERROR;
}

ECode CElementImpl::GetAttributeNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName,
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    AutoPtr<IAttr> attr;
    GetAttributeNodeNS(namespaceURI, localName, (IAttr**)&attr);

    if (attr == NULL) {
        *pStr = String("");
    }

    return attr->GetValue(pStr);
}

ECode CElementImpl::SetAttributeNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& qualifiedName,
    /* [in] */ const String& value)
{
    AutoPtr<IAttr> attr;
    GetAttributeNodeNS(namespaceURI, qualifiedName, (IAttr**)&attr);

    if (attr == NULL) {
        AutoPtr<IAttr> tmp;
        document->CreateAttributeNS(namespaceURI, qualifiedName, (IAttr**)&attr);
        SetAttributeNodeNS(attr, (IAttr**)&tmp);
    }

    attr->SetValue(value);
    return NOERROR;
}

ECode CElementImpl::RemoveAttributeNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName)
{
    Int32 i = IndexOfAttributeNS(namespaceURI, localName);

    if (i != -1) {
        attributes->Remove(i);
    }
    return NOERROR;
}

ECode CElementImpl::GetAttributeNodeNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName,
    /* [out] */ Org::W3c::Dom::IAttr ** ppAttr)
{
    VALIDATE_NOT_NULL(ppAttr);
    Int32 i = IndexOfAttributeNS(namespaceURI, localName);

    if (i == -1) {
        *ppAttr = NULL;
        return NOERROR;
    }

    AutoPtr<IInterface> value;
    attributes->Get(i, (IInterface**)&value);
    *ppAttr = IAttr::Probe(value);
    REFCOUNT_ADD(*ppAttr);
    return NOERROR;
}

ECode CElementImpl::SetAttributeNodeNS(
    /* [in] */ Org::W3c::Dom::IAttr* newAttr,
    /* [out] */ Org::W3c::Dom::IAttr** ppAttr)
{
    VALIDATE_NOT_NULL(ppAttr);
    *ppAttr = NULL;
    AutoPtr<CAttrImpl> newAttrImpl = (CAttrImpl*)newAttr;

    if (newAttrImpl->document != this->document) {
        //throw new DOMException(DOMException.WRONG_DOCUMENT_ERR, null);
        Logger::E("CElementImpl", "SetAttributeNodeNS WRONG_DOCUMENT_ERR");
        assert(0);
        return NOERROR;
    }

    AutoPtr<IElement> ownerElement;
    if ((newAttrImpl->GetOwnerElement((IElement**)&ownerElement), ownerElement) != NULL) {
        //throw new DOMException(DOMException.INUSE_ATTRIBUTE_ERR, null);
        Logger::E("CElementImpl", "SetAttributeNodeNS INUSE_ATTRIBUTE_ERR");
        assert(0);
        return NOERROR;
    }

    AutoPtr<CAttrImpl> oldAttrImpl;

    String namespaceURI, localName;
    INode::Probe(newAttr)->GetNamespaceURI(&namespaceURI);
    INode::Probe(newAttr)->GetLocalName(&localName);
    Int32 i = IndexOfAttributeNS(namespaceURI, localName);
    if (i != -1) {
        AutoPtr<IInterface> iattr;
        attributes->Get(i, (IInterface**)&iattr);
        oldAttrImpl = (CAttrImpl*)IAttr::Probe(iattr);
        attributes->Remove(i);
    }

    attributes->Add((IObject*)newAttrImpl.Get());
    newAttrImpl->ownerElement = this;

    *ppAttr = oldAttrImpl;
    REFCOUNT_ADD(*ppAttr);
    return NOERROR;
}

ECode CElementImpl::GetElementsByTagNameNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName,
    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
{
    VALIDATE_NOT_NULL(ppNodelist);
    CNodeListImpl* result = new CNodeListImpl();
    InnerNodeImpl::GetElementsByTagNameNS(result, namespaceURI, localName);
    *ppNodelist = result;
    REFCOUNT_ADD(*ppNodelist);
    return NOERROR;
}

ECode CElementImpl::HasAttribute(
    /* [in] */ const String& name,
    /* [out] */ Boolean * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = IndexOfAttribute(name) != -1;
    return NOERROR;
}

ECode CElementImpl::HasAttributeNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName,
    /* [out] */ Boolean * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = IndexOfAttributeNS(namespaceURI, localName) != -1;
    return NOERROR;
}

ECode CElementImpl::GetSchemaTypeInfo(
    /* [out] */ Org::W3c::Dom::ITypeInfo ** ppInfo)
{
    VALIDATE_NOT_NULL(ppInfo);
    *ppInfo = NULL_TYPE_INFO;
    REFCOUNT_ADD(*ppInfo);
    return NOERROR;
}

ECode CElementImpl::SetIdAttribute(
    /* [in] */ const String& name,
    /* [in] */ Boolean isId)
{
    AutoPtr<IAttr> iattr;
    GetAttributeNode(name, (IAttr**)&iattr);
    CAttrImpl* attr = (CAttrImpl*)(iattr.Get());
    if (attr == NULL) {
        //throw new DOMException(DOMException.NOT_FOUND_ERR, "No such attribute: " + name);
        Logger::E("CElementImpl", "SetIdAttribute");
        assert(0);
        return NOERROR;
    }
    attr->isId = isId;
    return NOERROR;
}

ECode CElementImpl::SetIdAttributeNS(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName,
    /* [in] */ Boolean isId)
{
    AutoPtr<IAttr> iattr;
    GetAttributeNodeNS(namespaceURI, localName, (IAttr**)&iattr);

    CAttrImpl* attr = (CAttrImpl*)(iattr.Get());
    if (attr == NULL) {
        //throw new DOMException(DOMException.NOT_FOUND_ERR, "No such attribute: " + namespaceURI +  " " + localName);
        Logger::E("CElementImpl", "SetIdAttributeNS");
        assert(0);
        return NOERROR;
    }
    attr->isId = isId;
    return NOERROR;
}

ECode CElementImpl::SetIdAttributeNode(
    /* [in] */ Org::W3c::Dom::IAttr * pIdAttr,
    /* [in] */ Boolean isId)
{
    ((CAttrImpl*)pIdAttr)->isId = isId;
    return NOERROR;
}

ECode CElementImpl::GetElementById(
    /* [in] */ const String& name,
    /* [out] */ IElement** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    attributes->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        Boolean isId;
        String value;
        AutoPtr<IInterface> iattr;
        attributes->Get(i, (IInterface**)&iattr);
        IAttr* attr = IAttr::Probe(iattr);
        if ((attr->IsId(&isId), isId) && name.Equals((attr->GetValue(&value), value))) {
            *result = this;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    /*
     * TODO: Remove this behavior.
     * The spec explicitly says that this is a bad idea. From
     * Document.getElementById(): "Attributes with the name "ID"
     * or "id" are not of type ID unless so defined.
     */
    String idAttr;
    if (name.Equals((GetAttribute(String("id"), &idAttr), idAttr))) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    Int32 cSize;
    children->GetSize(&cSize);
    for (Int32 i = 0; i < cSize; ++i) {
        AutoPtr<IInterface> inode;
        children->Get(i, (IInterface**)&inode);
        NodeImpl* node = (NodeImpl*)(INode::Probe(inode));
        Int16 nodeType;
        node->GetNodeType(&nodeType);
        if (nodeType == INode::ELEMENT_NODE) {
            AutoPtr<IElement> element;
            ((CElementImpl*)node)->GetElementById(name, (IElement**)&element);
            if (element != NULL) {
                *result = element;
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

Int32 CElementImpl::IndexOfAttribute(
    /* [in] */ const String& name)
{
    Int32 size;
    attributes->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        attributes->Get(i, (IInterface**)&value);
        CAttrImpl* attr = (CAttrImpl*)IAttr::Probe(value);
        String nodeName;
        attr->GetNodeName(&nodeName);
        if (nodeName.Equals(name)) {
            return i;
        }
    }

    return -1;
}

Int32 CElementImpl::IndexOfAttributeNS(
    /* [in] */ const String& _namespaceURI,
    /* [in] */ const String& _localName)
{
    Int32 size;
    attributes->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        attributes->Get(i, (IInterface**)&value);
        CAttrImpl* attr = (CAttrImpl*)IAttr::Probe(value);
        String namespaceURI, localName;
        attr->GetNamespaceURI(&namespaceURI);
        attr->GetLocalName(&localName);
        if (_namespaceURI.Equals(namespaceURI)
                && _localName.Equals(localName)) {
            return i;
        }
    }

    return -1;
}

//ECode CElementImpl::GetNodeValue(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::SetNodeValue(
//    /* [in] */ const String& nodeValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetParentNode(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetChildNodes(
//    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetFirstChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetLastChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetPreviousSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetNextSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetOwnerDocument(
//    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::InsertBefore(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pRefChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::ReplaceChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::RemoveChild(
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::AppendChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::HasChildNodes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::CloneNode(
//    /* [in] */ Boolean deep,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::Normalize()
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::IsSupported(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetBaseURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::CompareDocumentPosition(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Int16 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetTextContent(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::SetTextContent(
//    /* [in] */ const String& textContent)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::IsSameNode(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::LookupPrefix(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::IsDefaultNamespace(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::LookupNamespaceURI(
//    /* [in] */ const String& prefix,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::IsEqualNode(
//    /* [in] */ Org::W3c::Dom::INode * pArg,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetFeature(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::SetUserData(
//    /* [in] */ const String& key,
//    /* [in] */ IObject * pData,
//    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::GetUserData(
//    /* [in] */ const String& key,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CElementImpl::IsParentOf(
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

