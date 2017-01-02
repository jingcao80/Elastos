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

#include "org/apache/harmony/xml/dom/TextImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"
#include "elastos/core/StringBuilder.h"

using Org::W3c::Dom::EIID_IText;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_INTERFACE_IMPL(TextImpl, CharacterDataImpl, IText);

TextImpl::TextImpl()
{
}

ECode TextImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& data)
{
    return CharacterDataImpl::constructor(document, data);
}

ECode TextImpl::GetNodeName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String("#text");
    return NOERROR;
}

ECode TextImpl::GetNodeType(
    /* [out] */ Int16 * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = INode::TEXT_NODE;
    return NOERROR;
}

ECode TextImpl::SplitText(
    /* [in] */ Int32 offset,
    /* [out] */ Org::W3c::Dom::IText ** ppText)
{
    VALIDATE_NOT_NULL(ppText);
    AutoPtr<IText> newText;
    Int32 length;
    GetLength(&length);
    String substr;
    SubstringData(offset, length - offset, &substr);
    document->CreateTextNode(substr, (IText**)&newText);
    DeleteData(0, offset);

    AutoPtr<INode> refNode;
    GetNextSibling((INode**)&refNode);
    if (refNode == NULL) {
        AutoPtr<INode> pNode;
        GetParentNode((INode**)&pNode);
        AutoPtr<INode> tmp;
        pNode->AppendChild(INode::Probe(newText), (INode**)&tmp);
    } else {
        AutoPtr<INode> pNode;
        GetParentNode((INode**)&pNode);
        AutoPtr<INode> tmp;
        pNode->InsertBefore(INode::Probe(newText), INode::Probe(refNode), (INode**)&tmp);
    }

    *ppText = this;
    REFCOUNT_ADD(*ppText);
    return NOERROR;
}

ECode TextImpl::IsElementContentWhitespace(
    /* [out] */ Boolean * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = FALSE;
    return NOERROR;
}

ECode TextImpl::GetWholeText(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    // TODO: support entity references. This code should expand through
    // the child elements of entity references.
    //     http://code.google.com/p/android/issues/detail?id=6807

    AutoPtr<StringBuilder> result = new StringBuilder();
    AutoPtr<TextImpl> n = FirstTextNodeInCurrentRun();
    AutoPtr<TextImpl> tmp;
    for (; n != NULL; n = tmp->NextTextNode()) {
        tmp = n;
        n->AppendDataTo(result);
    }
    *pStr = result->ToString();
    return NOERROR;
}

ECode TextImpl::ReplaceWholeText(
    /* [in] */ const String& content,
    /* [out] */ Org::W3c::Dom::IText ** ppText)
{
    VALIDATE_NOT_NULL(ppText);
    // TODO: support entity references. This code should expand and replace
    // the child elements of entity references.
    //     http://code.google.com/p/android/issues/detail?id=6807

    AutoPtr<INode> parent;
    GetParentNode((INode**)&parent);
    AutoPtr<IText> result;

    // delete all nodes in the current run of text...
    for (AutoPtr<TextImpl> n = FirstTextNodeInCurrentRun(); n != NULL; ) {

        // ...except the current node if we have content for it
        if (n.Get() == this && !content.IsNull() && content.GetLength() > 0) {
            SetData(content);
            result = this;
            n = n->NextTextNode();

        } else {
            AutoPtr<INode> toRemove = INode::Probe(n); // because removeChild() detaches siblings
            n = n->NextTextNode();
            AutoPtr<INode> tmp;
            parent->RemoveChild(toRemove, (INode**)&tmp);
        }
    }

    *ppText = result;
    REFCOUNT_ADD(*ppText);
    return NOERROR;
}

AutoPtr<TextImpl> TextImpl::FirstTextNodeInCurrentRun()
{
    AutoPtr<TextImpl> firstTextInCurrentRun = this;
    AutoPtr<INode> p, tmp;
    for (GetPreviousSibling((INode**)&p); p != NULL; tmp->GetPreviousSibling((INode**)&p)) {
        tmp = p;
        Int16 nodeType;
        p->GetNodeType(&nodeType);
        if (nodeType == INode::TEXT_NODE || nodeType == INode::CDATA_SECTION_NODE) {
            firstTextInCurrentRun = (TextImpl*)(IText::Probe(p));
        } else {
            break;
        }
        p = NULL;
    }
    return firstTextInCurrentRun;
}

AutoPtr<TextImpl> TextImpl::NextTextNode()
{
    AutoPtr<INode> nextSibling;
    GetNextSibling((INode**)&nextSibling);
    if (nextSibling == NULL) {
        return NULL;
    }

    Int16 nodeType;
    nextSibling->GetNodeType(&nodeType);
    if (nodeType == INode::TEXT_NODE || nodeType == INode::CDATA_SECTION_NODE)
    {
        return (TextImpl*)IText::Probe(nextSibling);
    }
    else
    {
        return NULL;
    }
}

ECode TextImpl::Minimize(
    /* [out] */ TextImpl** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 length;
    if ((GetLength(&length), length) == 0) {
        AutoPtr<INode> tmp;
        parent->RemoveChild(INode::Probe(this), (INode**)&tmp);
        return NOERROR;
    }

    AutoPtr<INode> previous;
    GetPreviousSibling((INode**)&previous);

    Int16 nodeType;
    if (previous == NULL || (previous->GetNodeType(&nodeType), nodeType) != INode::TEXT_NODE) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<TextImpl> previousText = (TextImpl*)IText::Probe(previous);
    previousText->buffer->Append(buffer->ToString());
    AutoPtr<INode> tmp;
    parent->RemoveChild(INode::Probe(this), (INode**)&tmp);
    *result = previousText;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//ECode TextImpl::GetNodeValue(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::SetNodeValue(
//    /* [in] */ const String& nodeValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetParentNode(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetChildNodes(
//    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetFirstChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetLastChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetPreviousSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetNextSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetAttributes(
//    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetOwnerDocument(
//    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::InsertBefore(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pRefChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::ReplaceChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::RemoveChild(
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::AppendChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::HasChildNodes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::CloneNode(
//    /* [in] */ Boolean deep,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::Normalize()
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::IsSupported(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetNamespaceURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetPrefix(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::SetPrefix(
//    /* [in] */ const String& prefix)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetLocalName(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::HasAttributes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetBaseURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::CompareDocumentPosition(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Int16 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetTextContent(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::SetTextContent(
//    /* [in] */ const String& textContent)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::IsSameNode(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::LookupPrefix(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::IsDefaultNamespace(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::LookupNamespaceURI(
//    /* [in] */ const String& prefix,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::IsEqualNode(
//    /* [in] */ Org::W3c::Dom::INode * pArg,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetFeature(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::SetUserData(
//    /* [in] */ const String& key,
//    /* [in] */ IObject * pData,
//    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetUserData(
//    /* [in] */ const String& key,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::IsParentOf(
//    /* [in] */ Org::W3c::Dom::INode * pNode,
//    /* [out] */ Boolean * pIsParentOf)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::AppendDataTo(
//    /* [in] */ Elastos::Core::IStringBuilder * pStringBuilder)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetData(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::SetData(
//    /* [in] */ const String& data)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::GetLength(
//    /* [out] */ Int32 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::SubstringData(
//    /* [in] */ Int32 offset,
//    /* [in] */ Int32 count,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::AppendData(
//    /* [in] */ const String& arg)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::InsertData(
//    /* [in] */ Int32 offset,
//    /* [in] */ const String& arg)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::DeleteData(
//    /* [in] */ Int32 offset,
//    /* [in] */ Int32 count)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode TextImpl::ReplaceData(
//    /* [in] */ Int32 offset,
//    /* [in] */ Int32 count,
//    /* [in] */ const String& arg)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}

}
}
}
}
}

