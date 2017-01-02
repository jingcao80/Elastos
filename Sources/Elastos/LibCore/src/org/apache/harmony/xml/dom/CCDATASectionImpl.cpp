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

#include "Elastos.CoreLibrary.External.h"
#include "org/apache/harmony/xml/dom/CCDATASectionImpl.h"
#include "org/apache/harmony/xml/dom/NodeImpl.h"
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "elastos/core/StringUtils.h"

using Elastos::Core::StringUtils;
using Org::W3c::Dom::EIID_ICDATASection;
using Org::W3c::Dom::INode;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_OBJECT_IMPL(CCDATASectionImpl)
CAR_INTERFACE_IMPL_2(CCDATASectionImpl, TextImpl, ICDATASection, ICDATASectionImpl);

CCDATASectionImpl::CCDATASectionImpl()
{
}

ECode CCDATASectionImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& data)
{
    return TextImpl::constructor(document, data);
}

ECode CCDATASectionImpl::GetNodeName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String("#cdata-section");
    return NOERROR;
}

ECode CCDATASectionImpl::GetNodeType(
    /* [out] */ Int16* pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = INode::CDATA_SECTION_NODE;
    return NOERROR;
}

ECode CCDATASectionImpl::Split()
{
    Boolean needsSplitting;
    if (!NeedsSplitting(&needsSplitting)) {
        return NOERROR;
    }

    AutoPtr<INode> parent;
    GetParentNode((INode**)&parent);
    String data;
    GetData(&data);
    AutoPtr<ArrayOf<String> > parts;
    StringUtils::Split(data, String("\\]\\]>"), (ArrayOf<String>**)&parts);
    AutoPtr<INode> tmp;
    AutoPtr<CCDATASectionImpl> dsi = new CCDATASectionImpl();
    dsi->constructor(document, (*parts)[0] + "]]");
    AutoPtr<INode> doc = INode::Probe(dsi);
    parent->InsertBefore(doc, INode::Probe(this), (INode**)&tmp);
    for (Int32 p = 1; p < parts->GetLength() - 1; p++) {
        AutoPtr<CCDATASectionImpl> dsi = new CCDATASectionImpl();
        dsi->constructor(document, String(">") + (*parts)[p] + "]]");
        AutoPtr<INode> doc = INode::Probe(dsi);
        tmp = NULL;
        parent->InsertBefore(doc, INode::Probe(this), (INode**)&tmp);
    }
    SetData(String(">") + (*parts)[parts->GetLength() - 1]);
    return NOERROR;
}

ECode CCDATASectionImpl::NeedsSplitting(
    /* [out] */ Boolean * pNeedsSplitting)
{
    VALIDATE_NOT_NULL(pNeedsSplitting);
    String bufferStr = buffer->ToString();
    *pNeedsSplitting = bufferStr.IndexOf("]]>") != -1;
    return NOERROR;
}

ECode CCDATASectionImpl::ReplaceWithText(
    /* [out] */ Org::W3c::Dom::IText ** ppReplaceWithText)
{
    VALIDATE_NOT_NULL(ppReplaceWithText);
    String data;
    GetData(&data);
    AutoPtr<TextImpl> replacement = new TextImpl();
    replacement->constructor(document, data);
    AutoPtr<INode> tmp;
    parent->InsertBefore(INode::Probe(replacement), INode::Probe(this), (INode**)&tmp);
    tmp = NULL;
    parent->RemoveChild(INode::Probe(this), (INode**)&tmp);
    *ppReplaceWithText = replacement.Get();
    REFCOUNT_ADD(*ppReplaceWithText);
    return NOERROR;
}

//ECode CCDATASectionImpl::GetNodeValue(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::SetNodeValue(
//    /* [in] */ const String& nodeValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetParentNode(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetChildNodes(
//    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetFirstChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetLastChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetPreviousSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetNextSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetAttributes(
//    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetOwnerDocument(
//    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::InsertBefore(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pRefChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::ReplaceChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::RemoveChild(
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::AppendChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::HasChildNodes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::CloneNode(
//    /* [in] */ Boolean deep,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::Normalize()
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::IsSupported(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetNamespaceURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetPrefix(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::SetPrefix(
//    /* [in] */ const String& prefix)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetLocalName(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::HasAttributes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetBaseURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::CompareDocumentPosition(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Int16 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetTextContent(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::SetTextContent(
//    /* [in] */ const String& textContent)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::IsSameNode(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::LookupPrefix(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::IsDefaultNamespace(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::LookupNamespaceURI(
//    /* [in] */ const String& prefix,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::IsEqualNode(
//    /* [in] */ Org::W3c::Dom::INode * pArg,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetFeature(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::SetUserData(
//    /* [in] */ const String& key,
//    /* [in] */ IObject * pData,
//    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetUserData(
//    /* [in] */ const String& key,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::IsParentOf(
//    /* [in] */ Org::W3c::Dom::INode * pNode,
//    /* [out] */ Boolean * pIsParentOf)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::AppendDataTo(
//    /* [in] */ Elastos::Core::IStringBuilder * pStringBuilder)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetData(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::SetData(
//    /* [in] */ const String& data)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetLength(
//    /* [out] */ Int32 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::SubstringData(
//    /* [in] */ Int32 offset,
//    /* [in] */ Int32 count,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::AppendData(
//    /* [in] */ const String& arg)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::InsertData(
//    /* [in] */ Int32 offset,
//    /* [in] */ const String& arg)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::DeleteData(
//    /* [in] */ Int32 offset,
//    /* [in] */ Int32 count)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::ReplaceData(
//    /* [in] */ Int32 offset,
//    /* [in] */ Int32 count,
//    /* [in] */ const String& arg)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::SplitText(
//    /* [in] */ Int32 offset,
//    /* [out] */ Org::W3c::Dom::IText ** ppText)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::IsElementContentWhitespace(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::GetWholeText(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::ReplaceWholeText(
//    /* [in] */ const String& content,
//    /* [out] */ Org::W3c::Dom::IText ** ppText)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCDATASectionImpl::constructor(
//    /* [in] */ Org::W3c::Dom::IDocument * pDocument,
//    /* [in] */ const String& data)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}

}
}
}
}
}

