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

#ifndef __ORG_APACHE_HARMONY_XML_DOM_CDOMCONFIGURATIONIMPL_H__
#define __ORG_APACHE_HARMONY_XML_DOM_CDOMCONFIGURATIONIMPL_H__

#include "_Org_Apache_Harmony_Xml_Dom_CDOMConfigurationImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Org::W3c::Dom::IDOMConfiguration;
using Org::W3c::Dom::IDOMErrorHandler;
using Org::W3c::Dom::IDOMStringList;
using Elastos::Utility::IMap;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CarClass(CDOMConfigurationImpl)
    , public Object
    , public IDOMConfiguration
    , public IDOMConfigurationImpl
{
private:
    class FixedParameter
        : public Object
        , public IParameter
    {
    public:
        CAR_INTERFACE_DECL();

        FixedParameter(
            /* [in] */ IObject* oValue);

        CARAPI Get(
            /* [in] */ IDOMConfigurationImpl* config,
            /* [out] */ IObject** result);

        CARAPI Set(
            /* [in] */ IDOMConfigurationImpl* config,
            /* [in] */ IObject* value);

        CARAPI CanSet(
            /* [in] */ IDOMConfigurationImpl* config,
            /* [in] */ IObject* value,
            /* [out] */ Boolean* result);
    private:
        AutoPtr<IObject> onlyValue;
    };

    class BooleanParameter
        : public Object
        , public IParameter
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI Get(
            /* [in] */ IDOMConfigurationImpl* config,
            /* [out] */ IObject** result) = 0;

        CARAPI Set(
            /* [in] */ IDOMConfigurationImpl* config,
            /* [in] */ IObject* value) = 0;

        CARAPI CanSet(
            /* [in] */ IDOMConfigurationImpl* config,
            /* [in] */ IObject* value,
            /* [out] */ Boolean* result);
    };

    class InnerDOMStringList
        : public Object
        , public IDOMStringList
    {
    public:
        CAR_INTERFACE_DECL();

        InnerDOMStringList(
            /* [in] */ ArrayOf<String>* keys);

        CARAPI Item(
           /* [in] */ Int32 index,
           /* [out] */ String* str);

        CARAPI GetLength(
           /* [out] */ Int32* value);

        CARAPI Contains(
           /* [in] */ const String& str,
           /* [out] */ Boolean* value);
    private:
        AutoPtr<ArrayOf<String> > result;
    };
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CDOMConfigurationImpl();

    CARAPI SetParameter(
        /* [in] */ const String& name,
        /* [in] */ IObject * pValue);

    CARAPI GetParameter(
        /* [in] */ const String& name,
        /* [out] */ IObject ** ppObj);

    CARAPI CanSetParameter(
        /* [in] */ const String& name,
        /* [in] */ IObject * pValue,
        /* [out] */ Boolean * pReslut);

    CARAPI GetParameterNames(
        /* [out] */ Org::W3c::Dom::IDOMStringList ** ppDomlist);

    CARAPI Normalize(
        /* [in] */ Org::W3c::Dom::INode * pNode);

private:
    CARAPI_(void) CheckTextValidity(
        /* [in] */ ICharSequence* s);

    CARAPI_(Boolean) IsValid(
        /* [in] */ ICharSequence* text);

    CARAPI_(void) Report(
        /* [in] */ Int16 severity,
        /* [in] */ const String& type);

private:
    static AutoPtr<IMap> PARAMETERS;// = new TreeMap<String, Parameter>(String.CASE_INSENSITIVE_ORDER); // <String, Parameter>
    Boolean cdataSections;// = true;
    Boolean comments;// = true;
    Boolean datatypeNormalization;// = false;
    Boolean entities;// = true;
    AutoPtr<IDOMErrorHandler> errorHandler;
    Boolean namespaces;// = true;
    String schemaLocation;
    String schemaType;
    Boolean splitCdataSections;// = true;
    Boolean validate;// = false;
    Boolean wellFormed;// = true;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XML_DOM_CDOMCONFIGURATIONIMPL_H__
