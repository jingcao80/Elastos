
#ifndef __ORG_W3C_DOM_CTEXT_H__
#define __ORG_W3C_DOM_CTEXT_H__

#include "_Org_W3c_Dom_CText.h"
#include <elastos/core/Object.h>

namespace Org {
namespace W3c {
namespace Dom {

CarClass(CText)
    , public Object
    , public IText
{
public:
    CAR_INTERFACE_DECL();

    CARAPI GetNodeName(
        /* [out] */ String * pStr);

    CARAPI GetNodeValue(
        /* [out] */ String * pStr);

    CARAPI SetNodeValue(
        /* [in] */ const String& nodeValue);

    CARAPI GetNodeType(
        /* [out] */ Int16 * pValue);

    CARAPI GetParentNode(
        /* [out] */ INode ** ppNode);

    CARAPI GetChildNodes(
        /* [out] */ INodeList ** ppNodelist);

    CARAPI GetFirstChild(
        /* [out] */ INode ** ppNode);

    CARAPI GetLastChild(
        /* [out] */ INode ** ppNode);

    CARAPI GetPreviousSibling(
        /* [out] */ INode ** ppNode);

    CARAPI GetNextSibling(
        /* [out] */ INode ** ppNode);

    CARAPI GetAttributes(
        /* [out] */ INamedNodeMap ** ppNodemap);

    CARAPI GetOwnerDocument(
        /* [out] */ IDocument ** ppDoc);

    CARAPI InsertBefore(
        /* [in] */ INode * pNewChild,
        /* [in] */ INode * pRefChild,
        /* [out] */ INode ** ppNode);

    CARAPI ReplaceChild(
        /* [in] */ INode * pNewChild,
        /* [in] */ INode * pOldChild,
        /* [out] */ INode ** ppNode);

    CARAPI RemoveChild(
        /* [in] */ INode * pOldChild,
        /* [out] */ INode ** ppNode);

    CARAPI AppendChild(
        /* [in] */ INode * pNewChild,
        /* [out] */ INode ** ppNode);

    CARAPI HasChildNodes(
        /* [out] */ Boolean * pValue);

    CARAPI CloneNode(
        /* [in] */ Boolean deep,
        /* [out] */ INode ** ppNode);

    CARAPI Normalize();

    CARAPI IsSupported(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ Boolean * pValue);

    CARAPI GetNamespaceURI(
        /* [out] */ String * pStr);

    CARAPI GetPrefix(
        /* [out] */ String * pStr);

    CARAPI SetPrefix(
        /* [in] */ const String& prefix);

    CARAPI GetLocalName(
        /* [out] */ String * pStr);

    CARAPI HasAttributes(
        /* [out] */ Boolean * pValue);

    CARAPI GetBaseURI(
        /* [out] */ String * pStr);

    CARAPI CompareDocumentPosition(
        /* [in] */ INode * pOther,
        /* [out] */ Int16 * pValue);

    CARAPI GetTextContent(
        /* [out] */ String * pStr);

    CARAPI SetTextContent(
        /* [in] */ const String& textContent);

    CARAPI IsSameNode(
        /* [in] */ INode * pOther,
        /* [out] */ Boolean * pValue);

    CARAPI LookupPrefix(
        /* [in] */ const String& namespaceURI,
        /* [out] */ String * pStr);

    CARAPI IsDefaultNamespace(
        /* [in] */ const String& namespaceURI,
        /* [out] */ Boolean * pValue);

    CARAPI LookupNamespaceURI(
        /* [in] */ const String& prefix,
        /* [out] */ String * pStr);

    CARAPI IsEqualNode(
        /* [in] */ INode * pArg,
        /* [out] */ Boolean * pValue);

    CARAPI GetFeature(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ IObject ** ppObj);

    CARAPI SetUserData(
        /* [in] */ const String& key,
        /* [in] */ IObject * pData,
        /* [in] */ IUserDataHandler * pHandler,
        /* [out] */ IObject ** ppObj);

    CARAPI GetUserData(
        /* [in] */ const String& key,
        /* [out] */ IObject ** ppObj);

    CARAPI GetData(
        /* [out] */ String * pStr);

    CARAPI SetData(
        /* [in] */ const String& data);

    CARAPI GetLength(
        /* [out] */ Int32 * pValue);

    CARAPI SubstringData(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ String * pStr);

    CARAPI AppendData(
        /* [in] */ const String& arg);

    CARAPI InsertData(
        /* [in] */ Int32 offset,
        /* [in] */ const String& arg);

    CARAPI DeleteData(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI ReplaceData(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ const String& arg);

    CARAPI SplitText(
        /* [in] */ Int32 offset,
        /* [out] */ IText ** ppText);

    CARAPI IsElementContentWhitespace(
        /* [out] */ Boolean * pValue);

    CARAPI GetWholeText(
        /* [out] */ String * pStr);

    CARAPI ReplaceWholeText(
        /* [in] */ const String& content,
        /* [out] */ IText ** ppText);

private:
    // TODO: Add your private member variables here.
};

}
}
}

#endif // __ORG_W3C_DOM_CTEXT_H__
