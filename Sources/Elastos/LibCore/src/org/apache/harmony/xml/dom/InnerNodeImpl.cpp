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
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"
#include "org/apache/harmony/xml/dom/CTextImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/CElementImpl.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/utility/CArrayList.h"
#include <elastos/utility/logging/Logger.h>

//import java.util.List;
//import libcore.util.Objects;
//import org.w3c.dom.DOMException;
//import org.w3c.dom.Node;
//import org.w3c.dom.NodeList;
using Org::W3c::Dom::IDocumentFragment;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

InnerNodeImpl::InnerNodeImpl()
{
}

ECode InnerNodeImpl::constructor(
    /* [in] */ IDocument* document)
{
    LeafNodeImpl::constructor(document);
    CArrayList::New((IList**)&children);
    return NOERROR;
}

ECode InnerNodeImpl::AppendChild(
    /* [in] */ INode* newChild,
    /* [out] */ INode**result)
{
    Int32 size;
    children->GetSize(&size);
    return InsertChildAt(newChild, size, result);
}

ECode InnerNodeImpl::GetChildNodes(
    /* [out] */  INodeList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<INodeList> list;
    CNodeListImpl::New((INodeList**)&list);
    CNodeListImpl* nli = (CNodeListImpl*)list.Get();

    Int32 size;
    children->GetSize(&size);
    for(Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        children->Get(i, (IInterface**)&value);
        INode* node = INode::Probe(value);
        nli->Add((NodeImpl*)node);
    }

    *result = list;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode InnerNodeImpl::GetFirstChild(
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Boolean isEmpty;
    children->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<IInterface> value;
        children->Get(0, (IInterface**)&value);
        INode* node = INode::Probe(value);
        *result = node;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode InnerNodeImpl::GetLastChild(
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Boolean isEmpty;
    children->IsEmpty(&isEmpty);
    if (!isEmpty) {
        Int32 size;
        children->GetSize(&size);
        AutoPtr<IInterface> value;
        children->Get(size - 1, (IInterface**)&value);
        INode* node = INode::Probe(value);
        *result = node;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode InnerNodeImpl::GetNextSibling(
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    if (parent == NULL || (index + 1) >= (((InnerNodeImpl*)(parent.Get()))->children->GetSize(&size), size)) {
        return NOERROR;
    }

    AutoPtr<IInterface> value;
    children->Get(index + 1, (IInterface**)&value);
    INode* node = INode::Probe(value);
    *result = node;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode InnerNodeImpl::HasChildNodes(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    children->GetSize(&size);
    *result = size != 0;
    return NOERROR;
}

ECode InnerNodeImpl::InsertBefore(
    /* [in] */ INode* newChild,
    /* [in] */ INode* refChild,
    /* [out] */ INode** result)
{
    LeafNodeImpl* refChildImpl = (LeafNodeImpl*)refChild;

    if (refChildImpl == NULL) {
        return AppendChild(newChild, result);
    }

    if (refChildImpl->document != document) {
        //throw new DOMException(DOMException.WRONG_DOCUMENT_ERR, null);
        Logger::E("InnerNodeImpl", "InsertBefore WRONG_DOCUMENT_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (refChildImpl->parent.Get() != this) {
        //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
        Logger::E("InnerNodeImpl", "InsertBefore HIERARCHY_REQUEST_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return InsertChildAt(newChild, refChildImpl->index, result);
}

ECode InnerNodeImpl::InsertChildAt(
    /* [in] */ INode* newChild,
    /* [in] */ Int32 index,
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (IDocumentFragment::Probe(newChild) != NULL) {
        AutoPtr<INodeList> toAdd;
        newChild->GetChildNodes((INodeList**)&toAdd);
        Int32 length;
        toAdd->GetLength(&length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<INode> node;
            toAdd->Item(i, (INode**)&node);
            AutoPtr<INode> tmp;
            InsertChildAt(node, index + i, (INode**)&tmp);
        }
        *result = newChild;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    LeafNodeImpl* toInsert = (LeafNodeImpl*)newChild;
    if (toInsert->document != NULL && document != NULL && toInsert->document != document) {
        //throw new DOMException(DOMException.WRONG_DOCUMENT_ERR, null);
        Logger::E("InnerNodeImpl", "InsertChildAt WRONG_DOCUMENT_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Boolean isParentOf;
    if (toInsert->IsParentOf(this, &isParentOf), isParentOf) {
        //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
        Logger::E("InnerNodeImpl", "InsertChildAt HIERARCHY_REQUEST_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (toInsert->parent != NULL) {
        Int32 oldIndex = toInsert->index;
        InnerNodeImpl* p = (InnerNodeImpl*)(toInsert->parent.Get());
        p->children->Remove(oldIndex);
        p->RefreshIndices(oldIndex);
    }

    children->Add(index, TO_IINTERFACE(toInsert));
    toInsert->parent = this;
    RefreshIndices(index);

    *result = newChild;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode InnerNodeImpl::IsParentOf(
    /* [in] */ INode* node,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    LeafNodeImpl* nodeImpl = (LeafNodeImpl*) node;

    while (nodeImpl != NULL) {
        if (nodeImpl == this) {
            *result = TRUE;
            return NOERROR;
        }

        nodeImpl = (LeafNodeImpl*)(nodeImpl->parent.Get());
    }

    *result = FALSE;
    return NOERROR;
}

ECode InnerNodeImpl::Normalize()
{
    AutoPtr<INode> next, node;
    for (GetFirstChild((INode**)&node); node != NULL; node = next) {
        node->GetNextSibling((INode**)&next);
        node->Normalize();

        Int16 nodeType;
        node->GetNodeType(&nodeType);
        if (nodeType == INode::TEXT_NODE) {
            AutoPtr<TextImpl> tmp;
            ((CTextImpl*) IText::Probe(node))->Minimize((TextImpl**)&tmp);
        }
    }
    return NOERROR;
}

ECode InnerNodeImpl::RefreshIndices(
    /* [in] */ Int32 fromIndex)
{
    Int32 size;
    children->GetSize(&size);
    for (Int32 i = fromIndex; i < size; i++) {
        AutoPtr<IInterface> v;
        children->Get(i, (IInterface**)&v);
        INode* node = INode::Probe(v);
        LeafNodeImpl* ni = (LeafNodeImpl*)node;
        ni->index = i;
    }
    return NOERROR;
}

ECode InnerNodeImpl::RemoveChild(
    /* [in] */ INode* oldChild,
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    LeafNodeImpl* oldChildImpl = (LeafNodeImpl*) oldChild;

    if (oldChildImpl->document != document) {
        //throw new DOMException(DOMException.WRONG_DOCUMENT_ERR, null);
        Logger::E("InnerNodeImpl", "RemoveChild WRONG_DOCUMENT_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (oldChildImpl->parent.Get() != this) {
        //throw new DOMException(DOMException.HIERARCHY_REQUEST_ERR, null);
        Logger::E("InnerNodeImpl", "RemoveChild HIERARCHY_REQUEST_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 index = oldChildImpl->index;
    children->Remove(index);
    oldChildImpl->parent = NULL;
    RefreshIndices(index);

    *result = oldChild;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode InnerNodeImpl::ReplaceChild(
    /* [in] */ INode* newChild,
    /* [in] */ INode* oldChild,
    /* [out] */ INode** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 index = ((LeafNodeImpl*) oldChild)->index;
    AutoPtr<INode> tmp;
    RemoveChild(oldChild, (INode**)&tmp);
    tmp = NULL;
    InsertChildAt(newChild, index, (INode**)&tmp);
    *result = oldChild;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode InnerNodeImpl::GetTextContent(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<INode> child;
    GetFirstChild((INode**)&child);
    if (child == NULL) {
        *result = String("");
        return NOERROR;
    }

    AutoPtr<INode> next;
    child->GetNextSibling((INode**)&next);
    if (next == NULL) {
        Boolean hasTextContent;
        HasTextContent(child, &hasTextContent);
        if (hasTextContent) {
            child->GetTextContent(result);
        }
        else {
            *result = String("");
        }
        return NOERROR;
    }

    AutoPtr<StringBuilder> buf = new StringBuilder();
    GetTextContent(buf);
    *result = buf->ToString();
    return NOERROR;
}

ECode InnerNodeImpl::GetTextContent(
    /* [in] */ IStringBuilder* buf)
{
    AutoPtr<INode> child;
    GetFirstChild((INode**)&child);
    while (child != NULL) {
        Boolean hasTextContent;
        if (HasTextContent(child, &hasTextContent), hasTextContent) {
            ((NodeImpl*) child.Get())->GetTextContent(buf);
        }
        AutoPtr<INode> tmp;
        child->GetNextSibling((INode**)&tmp);
        child = tmp;
    }
    return NOERROR;
}

ECode InnerNodeImpl::HasTextContent(
    /* [in] */ INode* child,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO: skip text nodes with ignorable whitespace?
    Int16 nodeType;
    child->GetNodeType(&nodeType);
    *result = nodeType != INode::COMMENT_NODE
            && nodeType != INode::PROCESSING_INSTRUCTION_NODE;
    return NOERROR;
}

ECode InnerNodeImpl::GetElementsByTagName(
    /* [in] */ CNodeListImpl* out,
    /* [in] */ const String& name)
{
    Int32 size;
    children->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> value;
        children->Get(i, (IInterface**)&value);
        NodeImpl* node = (NodeImpl*)INode::Probe(value);
        Int16 nodeType;
        node->GetNodeType(&nodeType);
        if (nodeType == INode::ELEMENT_NODE) {
            CElementImpl* element = (CElementImpl*)IElement::Probe(node);
            String nodeName;
            element->GetNodeName(&nodeName);
            if (MatchesNameOrWildcard(name, nodeName)) {
                out->Add(element);
            }
            element->InnerNodeImpl::GetElementsByTagName(out, name);
        }
    }
    return NOERROR;
}

ECode InnerNodeImpl::GetElementsByTagNameNS(
    /* [in] */ CNodeListImpl* out,
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localName)
{
    Int32 size;
    children->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> value;
        children->Get(i, (IInterface**)&value);
        NodeImpl* node = (NodeImpl*)INode::Probe(value);
        Int16 nodeType;
        node->GetNodeType(&nodeType);
        if (nodeType == INode::ELEMENT_NODE) {
            CElementImpl* element = (CElementImpl*)IElement::Probe(node);
            String nsURI, localName;
            element->GetNamespaceURI(&nsURI);
            element->GetLocalName(&localName);
            if (MatchesNameOrWildcard(namespaceURI, nsURI)
                    && MatchesNameOrWildcard(localName, localName)) {
                out->Add(element);
            }
            element->InnerNodeImpl::GetElementsByTagNameNS(out, namespaceURI, localName);
        }
    }
    return NOERROR;
}

Boolean InnerNodeImpl::MatchesNameOrWildcard(
    /* [in] */ const String& pattern,
    /* [in] */ const String& s)
{
    return String("*").Equals(pattern) || pattern.Equals(s);//Objects.equal(pattern, s);
}

} // Dom
} // Xml
} // Harmony
} // Apache
} // Org
