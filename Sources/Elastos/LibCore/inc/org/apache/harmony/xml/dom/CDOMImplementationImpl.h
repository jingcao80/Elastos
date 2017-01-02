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

#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOMIMPLEMENTATIONIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOMIMPLEMENTATIONIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDOMImplementationImpl.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Org::W3c::Dom::IDOMImplementation;
using Org::W3c::Dom::IDocumentType;
using Org::W3c::Dom::IDocument;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDOMImplementationImpl)
    , public Object
    , public IDOMImplementation
{
public:
    CAR_OBJECT_DECL()
    CAR_INTERFACE_DECL()

    CARAPI CreateDocument(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& qualifiedName,
        /* [in] */ IDocumentType* doctype,
        /* [out] */ IDocument** doc);

    CARAPI CreateDocumentType(
        /* [in] */ const String& qualifiedName,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId,
        /* [out] */ IDocumentType** doc);

    CARAPI HasFeature(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ Boolean* value);

    CARAPI GetFeature(
        /* [in] */ const String& feature,
        /* [in] */ const String& ver,
        /* [out] */ IObject** obj);
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOMIMPLEMENTATIONIMPL_H__
