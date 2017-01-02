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
#include "org/apache/harmony/xml/dom/CAttrImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"

using Org::W3c::Dom::INode;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_OBJECT_IMPL(CAttrImpl);
CAR_INTERFACE_IMPL(CAttrImpl, NodeImpl, IAttr);

CAttrImpl::CAttrImpl()
    : isId(FALSE)
    , namespaceAware(FALSE)
    , value("")
{
}

ECode CAttrImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& qualifiedName)
{
    NodeImpl::constructor(document);
    SetNameNS(this, namespaceURI, qualifiedName);
    return NOERROR;
}

ECode CAttrImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& name)
{
    NodeImpl::constructor(document);
    SetName(this, name);
    return NOERROR;
}

ECode CAttrImpl::GetNodeName(
    /* [out] */ String * pStr)
{
    return GetName(pStr);;
}

ECode CAttrImpl::GetNodeValue(
    /* [out] */ String * pStr)
{
    return GetValue(pStr);
}

ECode CAttrImpl::SetNodeValue(
    /* [in] */ const String& nodeValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetNodeType(
    /* [out] */ Int16 * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = INode::ATTRIBUTE_NODE;
    return NOERROR;
}

ECode CAttrImpl::GetParentNode(
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetChildNodes(
    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetFirstChild(
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetLastChild(
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetPreviousSibling(
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetNextSibling(
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetAttributes(
    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetOwnerDocument(
    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::InsertBefore(
    /* [in] */ Org::W3c::Dom::INode * pNewChild,
    /* [in] */ Org::W3c::Dom::INode * pRefChild,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::ReplaceChild(
    /* [in] */ Org::W3c::Dom::INode * pNewChild,
    /* [in] */ Org::W3c::Dom::INode * pOldChild,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::RemoveChild(
    /* [in] */ Org::W3c::Dom::INode * pOldChild,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::AppendChild(
    /* [in] */ Org::W3c::Dom::INode * pNewChild,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::HasChildNodes(
    /* [out] */ Boolean * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::CloneNode(
    /* [in] */ Boolean deep,
    /* [out] */ Org::W3c::Dom::INode ** ppNode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::Normalize()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::IsSupported(
    /* [in] */ const String& feature,
    /* [in] */ const String& ver,
    /* [out] */ Boolean * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetNamespaceURI(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = namespaceURI;
    return NOERROR;
}

ECode CAttrImpl::GetPrefix(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = prefix;
    return NOERROR;
}

ECode CAttrImpl::SetPrefix(
    /* [in] */ const String& _prefix)
{
    prefix = ValidatePrefix(_prefix, namespaceAware, namespaceURI);
    return NOERROR;
}

ECode CAttrImpl::GetLocalName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String(NULL);
    if (namespaceAware) {
        *pStr = localName;
    }
    return NOERROR;
}

ECode CAttrImpl::HasAttributes(
    /* [out] */ Boolean * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetBaseURI(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::CompareDocumentPosition(
    /* [in] */ Org::W3c::Dom::INode * pOther,
    /* [out] */ Int16 * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetTextContent(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::SetTextContent(
    /* [in] */ const String& textContent)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::IsSameNode(
    /* [in] */ Org::W3c::Dom::INode * pOther,
    /* [out] */ Boolean * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::LookupPrefix(
    /* [in] */ const String& namespaceURI,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::IsDefaultNamespace(
    /* [in] */ const String& namespaceURI,
    /* [out] */ Boolean * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::LookupNamespaceURI(
    /* [in] */ const String& prefix,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::IsEqualNode(
    /* [in] */ Org::W3c::Dom::INode * pArg,
    /* [out] */ Boolean * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetFeature(
    /* [in] */ const String& feature,
    /* [in] */ const String& ver,
    /* [out] */ IObject ** ppObj)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::SetUserData(
    /* [in] */ const String& key,
    /* [in] */ IObject * pData,
    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
    /* [out] */ IObject ** ppObj)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetUserData(
    /* [in] */ const String& key,
    /* [out] */ IObject ** ppObj)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAttrImpl::GetName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String(NULL);
    if (!prefix.IsNull()) {
        *pStr = prefix + ":" + localName;
    }
    else {
        *pStr = localName;
    }
    return NOERROR;
}

ECode CAttrImpl::GetSpecified(
    /* [out] */ Boolean * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = !value.IsNull();
    return NOERROR;
}

ECode CAttrImpl::GetValue(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = value;
    return NOERROR;
}

ECode CAttrImpl::SetValue(
    /* [in] */ const String& _value)
{
    value = _value;
    return NOERROR;
}

ECode CAttrImpl::GetOwnerElement(
    /* [out] */ Org::W3c::Dom::IElement ** ppElement)
{
    VALIDATE_NOT_NULL(ppElement);
    *ppElement = ownerElement;
    REFCOUNT_ADD(*ppElement);
    return NOERROR;
}

ECode CAttrImpl::GetSchemaTypeInfo(
    /* [out] */ Org::W3c::Dom::ITypeInfo ** ppInfo)
{
    VALIDATE_NOT_NULL(ppInfo);
    *ppInfo = NULL_TYPE_INFO;
    REFCOUNT_ADD(*ppInfo);
    return NOERROR;
}

ECode CAttrImpl::IsId(
    /* [out] */ Boolean * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = isId;
    return NOERROR;
}

}
}
}
}
}

