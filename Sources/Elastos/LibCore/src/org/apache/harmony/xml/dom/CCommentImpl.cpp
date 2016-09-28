
#include "org/apache/harmony/xml/dom/CCommentImpl.h"
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"
#include "org/apache/harmony/xml/dom/CDocumentImpl.h"

using Org::W3c::Dom::EIID_IComment;
using Org::W3c::Dom::INode;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_OBJECT_IMPL(CCommentImpl);

CAR_INTERFACE_IMPL_2(CCommentImpl, CharacterDataImpl, IComment, ICommentImpl);

CCommentImpl::CCommentImpl()
{
}

ECode CCommentImpl::constructor(
    /* [in] */ IDocument* document,
    /* [in] */ const String& data)
{
    return CharacterDataImpl::constructor(document, data);
}

ECode CCommentImpl::GetNodeName(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr);
    *pStr = String("#comment");
    return NOERROR;
}

ECode CCommentImpl::GetNodeType(
    /* [out] */ Int16 * pValue)
{
    VALIDATE_NOT_NULL(pValue);
    *pValue = INode::COMMENT_NODE;
    return NOERROR;
}

ECode CCommentImpl::ContainsDashDash(
    /* [out] */ Boolean * pContainsDashDash)
{
    VALIDATE_NOT_NULL(pContainsDashDash);
    String bufferStr = buffer->ToString();
    *pContainsDashDash = bufferStr.IndexOf("--") != -1;
    return NOERROR;
}

//ECode CCommentImpl::GetNodeValue(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::SetNodeValue(
//    /* [in] */ const String& nodeValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetParentNode(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetChildNodes(
//    /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetFirstChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetLastChild(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetPreviousSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetNextSibling(
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetAttributes(
//    /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetOwnerDocument(
//    /* [out] */ Org::W3c::Dom::IDocument ** ppDoc)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::InsertBefore(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pRefChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::ReplaceChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::RemoveChild(
//    /* [in] */ Org::W3c::Dom::INode * pOldChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::AppendChild(
//    /* [in] */ Org::W3c::Dom::INode * pNewChild,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::HasChildNodes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::CloneNode(
//    /* [in] */ Boolean deep,
//    /* [out] */ Org::W3c::Dom::INode ** ppNode)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::Normalize()
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::IsSupported(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetNamespaceURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetPrefix(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::SetPrefix(
//    /* [in] */ const String& prefix)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetLocalName(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::HasAttributes(
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetBaseURI(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::CompareDocumentPosition(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Int16 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetTextContent(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::SetTextContent(
//    /* [in] */ const String& textContent)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::IsSameNode(
//    /* [in] */ Org::W3c::Dom::INode * pOther,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::LookupPrefix(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::IsDefaultNamespace(
//    /* [in] */ const String& namespaceURI,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::LookupNamespaceURI(
//    /* [in] */ const String& prefix,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::IsEqualNode(
//    /* [in] */ Org::W3c::Dom::INode * pArg,
//    /* [out] */ Boolean * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetFeature(
//    /* [in] */ const String& feature,
//    /* [in] */ const String& ver,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::SetUserData(
//    /* [in] */ const String& key,
//    /* [in] */ IObject * pData,
//    /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetUserData(
//    /* [in] */ const String& key,
//    /* [out] */ IObject ** ppObj)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::IsParentOf(
//    /* [in] */ Org::W3c::Dom::INode * pNode,
//    /* [out] */ Boolean * pIsParentOf)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::AppendDataTo(
//    /* [in] */ Elastos::Core::IStringBuilder * pStringBuilder)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetData(
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::SetData(
//    /* [in] */ const String& data)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::GetLength(
//    /* [out] */ Int32 * pValue)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::SubstringData(
//    /* [in] */ Int32 offset,
//    /* [in] */ Int32 count,
//    /* [out] */ String * pStr)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::AppendData(
//    /* [in] */ const String& arg)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::InsertData(
//    /* [in] */ Int32 offset,
//    /* [in] */ const String& arg)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::DeleteData(
//    /* [in] */ Int32 offset,
//    /* [in] */ Int32 count)
//{
//    // TODO: Add your code here
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CCommentImpl::ReplaceData(
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

