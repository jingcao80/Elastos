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

#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTTYPEIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTTYPEIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDocumentTypeImpl.h"
#include <elastos/core/Object.h>
#include "org/apache/harmony/xml/dom/LeafNodeImpl.h"

using Org::W3c::Dom::IDocumentType;
using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDocumentTypeImpl)
    , public LeafNodeImpl
    , public IDocumentType
{
public:
    CAR_OBJECT_DECL();
    CAR_INTERFACE_DECL();

    CDocumentTypeImpl();

    CARAPI constructor(
        /* [in] */ Org::W3c::Dom::IDocument * pDocument,
        /* [in] */ const String& qualifiedName,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    CARAPI GetNodeName(
        /* [out] */ String * pStr);

    CARAPI GetNodeType(
        /* [out] */ Int16 * pValue);

    CARAPI GetEntities(
        /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap);

    CARAPI GetInternalSubset(
        /* [out] */ String * pStr);

    CARAPI GetName(
        /* [out] */ String * pStr);

    CARAPI GetNotations(
        /* [out] */ Org::W3c::Dom::INamedNodeMap ** ppNodemap);

    CARAPI GetPublicId(
        /* [out] */ String * pStr);

    CARAPI GetSystemId(
        /* [out] */ String * pStr);

    CARAPI GetTextContent(
        /* [out] */ String * pStr);

    //=====================================
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
private:
    String qualifiedName;
    String publicId;
    String systemId;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTTYPEIMPL_H__
