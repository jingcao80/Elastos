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

#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDOMErrorImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Org::W3c::Dom::IDOMLocator;
using Org::W3c::Dom::IDOMError;
using Org::W3c::Dom::INode;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDOMErrorImpl)
    , public Object
    , public IDOMError
{
private:
    class InnerDOMLocator
        : public Object
        , public IDOMLocator
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI GetLineNumber(
            /* [out] */ Int32* result);

        CARAPI GetColumnNumber(
            /* [out] */ Int32* result);

        CARAPI GetByteOffset(
            /* [out] */ Int32* result);

        CARAPI GetUtf16Offset(
            /* [out] */ Int32* result);

        CARAPI GetRelatedNode(
            /* [out] */ INode** result);

        CARAPI GetUri(
            /* [out] */ String* result);
    };

public:
    CAR_OBJECT_DECL();
    CAR_INTERFACE_DECL();

    CARAPI GetSeverity(
        /* [out] */ Int16 * pValue);

    CARAPI GetMessage(
        /* [out] */ String * pStr);

    CARAPI GetType(
        /* [out] */ String * pStr);

    CARAPI GetRelatedException(
        /* [out] */ IObject ** ppObj);

    CARAPI GetRelatedData(
        /* [out] */ IObject ** ppObj);

    CARAPI GetLocation(
        /* [out] */ Org::W3c::Dom::IDOMLocator ** ppDomloc);

    CARAPI constructor(
        /* [in] */ Int32 severity,
        /* [in] */ const String& type);

private:
    static AutoPtr<IDOMLocator> NULL_DOM_LOCATOR;

    Int16 severity;
    String type;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOMERRORIMPL_H__
