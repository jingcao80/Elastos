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

#ifndef __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Xml_Parsers_CDocumentBuilderFactoryImpl.h"
#include "elastosx/xml/parsers/DocumentBuilderFactory.h"

using Elastosx::Xml::Parsers::DocumentBuilderFactory;
using Org::W3c::Dom::IDOMImplementation;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

CarClass(CDocumentBuilderFactoryImpl)
    , public DocumentBuilderFactory
{
public:

    CARAPI NewDocumentBuilder(
        /* [out] */ Elastosx::Xml::Parsers::IDocumentBuilder ** ppBuilder);

    CARAPI SetAttribute(
        /* [in] */ const String& name,
        /* [in] */ IObject * pValue);

    CARAPI GetAttribute(
        /* [in] */ const String& name,
        /* [out] */ IInterface ** ppAttri);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean * pFeature);

private:
    static const String NAMESPACES;// = "http://xml.org/sax/features/namespaces";
    static const String VALIDATION;// = "http://xml.org/sax/features/validation";
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_PARSERS_CDOCUMENTBUILDERFACTORYIMPL_H__
