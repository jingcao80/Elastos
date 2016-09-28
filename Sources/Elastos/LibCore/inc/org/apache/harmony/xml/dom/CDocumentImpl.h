
#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDocumentImpl.h"
#include "org/apache/harmony/xml/dom/NodeImpl.h"
#include "org/apache/harmony/xml/dom/InnerNodeImpl.h"
#include <elastos/core/Object.h>

using Org::W3c::Dom::IDOMImplementation;
using Org::W3c::Dom::IDocumentType;
using Elastos::Core::Object;
using Elastos::Utility::IMap;
using Elastos::Utility::IWeakHashMap;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDocumentImpl)
    , public InnerNodeImpl
    , public IDocument
{
public:
    CAR_OBJECT_DECL();
    CAR_INTERFACE_DECL();

    CDocumentImpl();

    CARAPI constructor(
        /* [in] */ IDOMImplementation* impl,
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [in] */ IDocumentType* doctype,
        /* [in] */ const String& inputEncoding);

    CARAPI ImportNode(
        /* [in] */ Org::W3c::Dom::INode * pImportedNode,
        /* [in] */ Boolean deep,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI AdoptNode(
        /* [in] */ Org::W3c::Dom::INode * pSource,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI RenameNode(
        /* [in] */ Org::W3c::Dom::INode * pN,
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [out] */ Org::W3c::Dom::INode ** ppNode);

    CARAPI CreateAttribute(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI CreateAttributeNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [out] */ Org::W3c::Dom::IAttr ** ppAttr);

    CARAPI CreateCDATASection(
        /* [in] */ const String& data,
        /* [out] */ Org::W3c::Dom::ICDATASection ** ppDatasection);

    CARAPI CreateComment(
        /* [in] */ const String& data,
        /* [out] */ Org::W3c::Dom::IComment ** ppComment);

    CARAPI CreateDocumentFragment(
        /* [out] */ Org::W3c::Dom::IDocumentFragment ** ppDocfra);

    CARAPI CreateElement(
        /* [in] */ const String& tagName,
        /* [out] */ Org::W3c::Dom::IElement ** ppElement);

    CARAPI CreateElementNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [out] */ Org::W3c::Dom::IElement** ppElement);

    CARAPI CreateEntityReference(
        /* [in] */ const String& name,
        /* [out] */ Org::W3c::Dom::IEntityReference ** ppEntity);

    CARAPI CreateProcessingInstruction(
        /* [in] */ const String& target,
        /* [in] */ const String& data,
        /* [out] */ Org::W3c::Dom::IProcessingInstruction ** ppProins);

    CARAPI CreateTextNode(
        /* [in] */ const String& data,
        /* [out] */ Org::W3c::Dom::IText ** ppText);

    CARAPI GetDoctype(
        /* [out] */ Org::W3c::Dom::IDocumentType ** ppDoctype);

    CARAPI GetDocumentElement(
        /* [out] */ Org::W3c::Dom::IElement ** ppElement);

    CARAPI GetElementById(
        /* [in] */ const String& elementId,
        /* [out] */ Org::W3c::Dom::IElement ** ppElment);

    CARAPI GetElementsByTagName(
        /* [in] */ const String& tagname,
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI GetElementsByTagNameNS(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localName,
        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);

    CARAPI GetImplementation(
        /* [out] */ Org::W3c::Dom::IDOMImplementation ** ppDom);

    CARAPI GetNodeName(
        /* [out] */ String * pStr);

    CARAPI GetNodeType(
        /* [out] */ Int16 * pValue);

    CARAPI InsertChildAt(
        /* [in] */ INode* toInsert,
        /* [in] */ Int32 index,
        /* [out] */ INode** result);

    CARAPI GetTextContent(
        /* [out] */ String * pStr);

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

    CARAPI GetDomConfig(
        /* [out] */ Org::W3c::Dom::IDOMConfiguration ** ppDomconfig);

    CARAPI NormalizeDocument();

//=================================
//    CARAPI GetNodeValue(
//        /* [out] */ String * pStr);
//
//    CARAPI SetNodeValue(
//        /* [in] */ const String& nodeValue);
//
//    CARAPI GetParentNode(
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI GetChildNodes(
//        /* [out] */ Org::W3c::Dom::INodeList ** ppNodelist);
//
//    CARAPI GetFirstChild(
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI GetLastChild(
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI GetPreviousSibling(
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI GetNextSibling(
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI GetAttributes(
//        /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap);
//
//    CARAPI GetOwnerDocument(
//        /* [out] */ Org::W3c::Dom::IDocument ** ppDoc);
//
//    CARAPI InsertBefore(
//        /* [in] */ Org::W3c::Dom::INode * pNewChild,
//        /* [in] */ Org::W3c::Dom::INode * pRefChild,
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI ReplaceChild(
//        /* [in] */ Org::W3c::Dom::INode * pNewChild,
//        /* [in] */ Org::W3c::Dom::INode * pOldChild,
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI RemoveChild(
//        /* [in] */ Org::W3c::Dom::INode * pOldChild,
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI AppendChild(
//        /* [in] */ Org::W3c::Dom::INode * pNewChild,
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI HasChildNodes(
//        /* [out] */ Boolean * pValue);
//
//    CARAPI CloneNode(
//        /* [in] */ Boolean deep,
//        /* [out] */ Org::W3c::Dom::INode ** ppNode);
//
//    CARAPI Normalize();
//
//    CARAPI IsSupported(
//        /* [in] */ const String& feature,
//        /* [in] */ const String& ver,
//        /* [out] */ Boolean * pValue);
//
//    CARAPI GetNamespaceURI(
//        /* [out] */ String * pStr);
//
//    CARAPI GetPrefix(
//        /* [out] */ String * pStr);
//
//    CARAPI SetPrefix(
//        /* [in] */ const String& prefix);
//
//    CARAPI GetLocalName(
//        /* [out] */ String * pStr);
//
//    CARAPI HasAttributes(
//        /* [out] */ Boolean * pValue);
//
//    CARAPI GetBaseURI(
//        /* [out] */ String * pStr);
//
//    CARAPI CompareDocumentPosition(
//        /* [in] */ Org::W3c::Dom::INode * pOther,
//        /* [out] */ Int16 * pValue);
//
//    CARAPI SetTextContent(
//        /* [in] */ const String& textContent);
//
//    CARAPI IsSameNode(
//        /* [in] */ Org::W3c::Dom::INode * pOther,
//        /* [out] */ Boolean * pValue);
//
//    CARAPI LookupPrefix(
//        /* [in] */ const String& namespaceURI,
//        /* [out] */ String * pStr);
//
//    CARAPI IsDefaultNamespace(
//        /* [in] */ const String& namespaceURI,
//        /* [out] */ Boolean * pValue);
//
//    CARAPI LookupNamespaceURI(
//        /* [in] */ const String& prefix,
//        /* [out] */ String * pStr);
//
//    CARAPI IsEqualNode(
//        /* [in] */ Org::W3c::Dom::INode * pArg,
//        /* [out] */ Boolean * pValue);
//
//    CARAPI GetFeature(
//        /* [in] */ const String& feature,
//        /* [in] */ const String& ver,
//        /* [out] */ IObject ** ppObj);
//
//    CARAPI SetUserData(
//        /* [in] */ const String& key,
//        /* [in] */ IObject * pData,
//        /* [in] */ Org::W3c::Dom::IUserDataHandler * pHandler,
//        /* [out] */ IObject ** ppObj);
//
//    CARAPI GetUserData(
//        /* [in] */ const String& key,
//        /* [out] */ IObject ** ppObj);
//
//    CARAPI IsParentOf(
//        /* [in] */ Org::W3c::Dom::INode * pNode,
//        /* [out] */ Boolean * pIsParentOf);
//
    //CARAPI constructor(
    //    /* [in] */ Org::W3c::Dom::IDOMImplementation * pImpl,
    //    /* [in] */ const String& namespaceURI,
    //    /* [in] */ const String& qualifiedName,
    //    /* [in] */ Org::W3c::Dom::IDocumentType * pDoctype,
    //    /* [in] */ const String& inputEncoding);
private:
    /**
     * Returns a shallow copy of the given node. If the node is an element node,
     * its attributes are always copied.
     *
     * @param node a node belonging to any document or DOM implementation.
     * @param operation the operation type to use when notifying user data
     *     handlers of copied element attributes. It is the caller's
     *     responsibility to notify user data handlers of the returned node.
     * @return a new node whose document is this document and whose DOM
     *     implementation is this DOM implementation.
     */
    CARAPI_(AutoPtr<NodeImpl>) ShallowCopy(
        /* [in] */ Int16 operation,
        /* [in] */ INode* node);

    /**
     * Returns a copy of the given node or subtree with this document as its
     * owner.
     *
     * @param operation either {@link UserDataHandler#NODE_CLONED} or
     *      {@link UserDataHandler#NODE_IMPORTED}.
     * @param node a node belonging to any document or DOM implementation.
     * @param deep true to recursively copy any child nodes; false to do no such
     *      copying and return a node with no children.
     */
    CARAPI_(AutoPtr<INode>) CloneOrImportNode(
        /* [in] */ Int16 operation,
        /* [in] */ INode* node,
        /* [in] */ Boolean deep);

    CARAPI_(void) ChangeDocumentToThis(
        /* [in] */ NodeImpl* node);

    /**
     * Returns a map with the user data objects attached to the specified node.
     * This map is readable and writable.
     */
    //Map<String, UserData> getUserDataMap(NodeImpl node);
    CARAPI_(AutoPtr<IMap>) GetUserDataMap(
        /* [in] */ NodeImpl* node);

    /**
     * Returns a map with the user data objects attached to the specified node.
     * The returned map may be read-only.
     */
    //Map<String, UserData> getUserDataMapForRead(NodeImpl node)
    CARAPI_(AutoPtr<IMap>) GetUserDataMapForRead(
        /* [in] */ NodeImpl* node);

    /**
     * Calls {@link UserDataHandler#handle} on each of the source node's
     * value/handler pairs.
     *
     * <p>If the source node comes from another DOM implementation, user data
     * handlers will <strong>not</strong> be notified. The DOM API provides no
     * mechanism to inspect a foreign node's user data.
     */
    static CARAPI_(void) NotifyUserDataHandlers(
        /* [in] */ Int16 operation,
        /* [in] */ INode* source,
        /* [in] */ NodeImpl* destination);

private:
    AutoPtr<IDOMImplementation> domImplementation;
    AutoPtr<IDOMConfigurationImpl> domConfiguration;

    /*
     * The default values of these fields are specified by the Document
     * interface.
     */
    String documentUri;
    String inputEncoding;
    String xmlEncoding;
    String xmlVersion;// = "1.0";
    Boolean xmlStandalone;// = false;
    Boolean strictErrorChecking;// = true;

    /**
     * A lazily initialized map of user data values for this document's own
     * nodes. The map is weak because the document may live longer than its
     * nodes.
     *
     * <p>Attaching user data directly to the corresponding node would cost a
     * field per node. Under the assumption that user data is rarely needed, we
     * attach user data to the document to save those fields. Xerces also takes
     * this approach.
     */
    //WeakHashMap<NodeImpl, Map<String, UserData>> nodeToUserData;
    AutoPtr<IWeakHashMap> nodeToUserData;

    friend class NodeImpl;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPL_H__
