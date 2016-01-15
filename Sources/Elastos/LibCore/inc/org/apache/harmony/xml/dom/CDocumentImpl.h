
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDocumentImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDocumentImpl)
{
public:
    CARAPI GetNodeName(
        /* [out] */ String * pStr);

    CARAPI GetNodeValue(
        /* [out] */ String * pStr);

    CARAPI SetNodeValue(
        /* [in] */ const String& nodeValue);

    CARAPI GetNodeType(
        /* [out] */ Int16 * pValue);

    CARAPI GetParentNode(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetChildNodes(
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI GetFirstChild(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetLastChild(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetPreviousSibling(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetNextSibling(
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetAttributes(
        /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap);

    CARAPI GetOwnerDocument(
        /* [out] */ Org::W3c::Dom::IDocument ** ppDoc);

    CARAPI InsertBefore(
        /* [in] */ Org::W3c::Dom::INode * pNewChild,
        /* [in] */ Org::W3c::Dom::INode * pRefChild,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI ReplaceChild(
        /* [in] */ Org::W3c::Dom::INode * pNewChild,
        /* [in] */ Org::W3c::Dom::INode * pOldChild,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI RemoveChild(
        /* [in] */ Org::W3c::Dom::INode * pOldChild,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI AppendChild(
        /* [in] */ Org::W3c::Dom::INode * pNewChild,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI HasChildNodes(
        /* [out] */ Boolean * pValue);

    CARAPI CloneNode(
        /* [in] */ Boolean deep,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

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
        /* [in] */ Org::W3c::Dom::INode * pOther,
        /* [out] */ Int16 * pValue);

    CARAPI GetTextContent(
        /* [out] */ String * pStr);

    CARAPI SetTextContent(
        /* [in] */ const String& textContent);

    CARAPI IsSameNode(
        /* [in] */ Org::W3c::Dom::INode * pOther,
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
        /* [in] */ Org::W3c::Dom::INode * pArg,
        /* [out] */ Boolean * pValue);

    CARAPI GetFeature(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ IObject ** ppObj);

    CARAPI SetUserData(
        /* [in] */ const String& key,
        /* [in] */ IObject * pData,
        /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
        /* [out] */ IObject ** ppObj);

    CARAPI GetUserData(
        /* [in] */ const String& key,
        /* [out] */ IObject ** ppObj);

    CARAPI IsParentOf(
        /* [in] */ Org::W3c::Dom::INode * pNode,
        /* [out] */ Boolean * pIsParentOf);

    CARAPI GetDoctype(
        /* [out] */ Org::W3c::Dom::IDocumentType ** ppDoctype);

    CARAPI GetImplementation(
        /* [out] */ Org::W3c::Dom::IDOMImplementation ** ppDom);

    CARAPI GetDocumentElement(
        /* [out] */ Org::W3c::Dom::IElement ** ppElement);

    CARAPI CreateElement(
        /* [in] */ const String& tagName,
        /* [out] */ Org::W3c::Dom::IElement ** ppElement);

    CARAPI CreateDocumentFragment(
        /* [out] */ Org::W3c::Dom::IDocumentFragment ** ppDocfra);

    CARAPI CreateTextNode(
        /* [in] */ const String& data,
        /* [out] */ Org::W3c::Dom::IText ** ppText);

    CARAPI CreateComment(
        /* [in] */ const String& data,
        /* [out] */ Org::W3c::Dom::IComment ** ppComment);

    CARAPI CreateCDATASection(
        /* [in] */ const String& data,
        /* [out] */ Org::W3c::Dom::ICDATASection ** ppDatasection);

    CARAPI CreateProcessingInstruction(
        /* [in] */ const String& target,
        /* [in] */ const String& data,
        /* [out] */ Org::W3c::Dom::IProcessingInstruction ** ppProins);

    CARAPI CreateAttribute(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI CreateEntityReference(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::IEntityReference ** ppEntity);

    CARAPI GetElementsByTagName(
        /* [in] */ const String& tagname,
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI ImportNode(
        /* [in] */ Org::W3c::Dom::INode * pImportedNode,
        /* [in] */ Boolean deep,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI CreateElementNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [out] */ Org::W3c::Dom::IElement ** ppElement);

    CARAPI CreateAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI GetElementsByTagNameNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI GetElementById(
        /* [in] */ const String& elementId,
        /* [out] */ Org::W3c::Dom::IElement ** ppElment);

    CARAPI GetInputEncoding(
        /* [out] */ String * pStr);

    CARAPI GetXmlEncoding(
        /* [out] */ String * pStr);

    CARAPI GetXmlStandalone(
        /* [out] */ Boolean * pValue);

    CARAPI SetXmlStandalone(
        /* [in] */ Boolean xmlStandalone);

    CARAPI GetXmlVersion(
        /* [out] */ String * pStr);

    CARAPI SetXmlVersion(
        /* [in] */ const String& xmlVersion);

    CARAPI GetStrictErrorChecking(
        /* [out] */ Boolean * pValue);

    CARAPI SetStrictErrorChecking(
        /* [in] */ Boolean strictErrorChecking);

    CARAPI GetDocumentURI(
        /* [out] */ String * pStr);

    CARAPI SetDocumentURI(
        /* [in] */ const String& documentURI);

    CARAPI AdoptNode(
        /* [in] */ Org::W3c::Dom::INode * pSource,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI GetDomConfig(
        /* [out] */ Org::W3c::Dom::IDOMConfiguration ** ppDomconfig);

    CARAPI NormalizeDocument();

    CARAPI RenameNode(
        /* [in] */ Org::W3c::Dom::INode * pN,
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI constructor(
        /* [in] */ Org::W3c::Dom::IDOMImplementation * pImpl,
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [in] */ Org::W3c::Dom::IDocumentType * pDoctype,
        /* [in] */ const String& inputEncoding);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPL_H__
