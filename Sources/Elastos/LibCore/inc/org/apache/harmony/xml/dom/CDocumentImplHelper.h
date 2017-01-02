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

#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPLHELPER_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPLHELPER_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDocumentImplHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDocumentImplHelper)
    , public Singleton
    , public IDocumentImplHelper
{
public:
    CAR_SINGLETON_DECL();
    CAR_INTERFACE_DECL();

    CARAPI IsXMLIdentifier(
        /* [in] */ const String& s,
        /* [out] */ Boolean * pResult);

private:
    CARAPI_(Boolean) IsXMLIdentifierStart(
        /* [in] */ Char32 c);

    CARAPI_(Boolean) IsXMLIdentifierPart(
        /* [in] */ Char32 c);
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOCUMENTIMPLHELPER_H__
