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

#ifndef __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__
#define __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.External.h"
#include "_Org_Apache_Harmony_Xml_Parsers_CSAXParserImpl.h"
#include "elastosx/xml/parsers/SAXParser.h"
#include <elastos/core/Object.h>

using Org::Xml::Sax::IParser;
using Org::Xml::Sax::IXMLReader;
using Elastos::Core::Object;
using Elastos::Utility::IMap;
using Elastosx::Xml::Parsers::SAXParser;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

CarClass(CSAXParserImpl)
    , public SAXParser
{
public:

    CARAPI constructor(
        /* [in] */ IMap* features);

    CARAPI Reset();

    CARAPI GetXMLReader(
        /* [out] */ Org::Xml::Sax::IXMLReader ** ppReader);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean * pIsAware);

    CARAPI IsValidating(
        /* [out] */ Boolean * pIsValidating);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface * pValue);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface ** ppProperty);

    CARAPI GetParser(
        /* [out] */ IParser** parser);

private:
    CARAPI ResetInternal();

private:
    //Map<String, Boolean> initialFeatures;
    AutoPtr<IMap> initialFeatures;
    AutoPtr<IXMLReader> reader;
    AutoPtr<IParser> parser;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_PARSERS_CSAXPARSERIMPL_H__
