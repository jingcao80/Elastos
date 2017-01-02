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

#include "org/apache/harmony/xml/dom/CEntityReferenceImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"

using Org::W3c::Dom::EIID_IEntityReference;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_OBJECT_IMPL(CEntityReferenceImpl);
CAR_INTERFACE_IMPL(CEntityReferenceImpl, LeafNodeImpl, IEntityReference);

ECode CEntityReferenceImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& name)
{
    LeafNodeImpl::constructor(document);
    this->name = name;
    return NOERROR;
}

ECode CEntityReferenceImpl::GetNodeName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = name;
    return NOERROR;
}

ECode CEntityReferenceImpl::GetNodeType(
    /* [out] */ Int16 * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = INode::ENTITY_REFERENCE_NODE;
    return NOERROR;
}

//ECode CEntityReferenceImpl::GetNodeValue(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::SetNodeValue(
//    /* [in] */ const String& nodeValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetParentNode(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetChildNodes(
//    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetFirstChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetLastChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetPreviousSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetNextSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetAttributes(
//    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetOwnerDocument(
//    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::InsertBefore(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pRefChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::ReplaceChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::RemoveChild(
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::AppendChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::HasChildNodes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::CloneNode(
//    /* [in] */ Boolean deep,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::Normalize()
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::IsSupported(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetNamespaceURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetPrefix(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::SetPrefix(
//    /* [in] */ const String& prefix)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetLocalName(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::HasAttributes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetBaseURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::CompareDocumentPosition(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Int16 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetTextContent(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::SetTextContent(
//    /* [in] */ const String& textContent)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::IsSameNode(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::LookupPrefix(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::IsDefaultNamespace(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::LookupNamespaceURI(
//    /* [in] */ const String& prefix,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::IsEqualNode(
//    /* [in] */ Org::W3c::Dom::INode * pArg,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetFeature(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::SetUserData(
//    /* [in] */ const String& key,
//    /* [in] */ IObject * pData,
//    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::GetUserData(
//    /* [in] */ const String& key,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CEntityReferenceImpl::IsParentOf(
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

