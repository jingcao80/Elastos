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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__

#include "_Org_Apache_Harmony_Security_X509_CDistributionPointName.h"
#include "org/apache/harmony/security/asn1/ASN1Choice.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Choice;
using Org::Apache::Harmony::Security::Asn1::ASN1Choice;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::X501::IName;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CDistributionPointName)
    , public Object
    , public IDistributionPointName
{
private:
    class MyASN1Choice
        : public ASN1Choice
    {
    public:
        CARAPI GetIndex(
            /* [in] */ IInterface* object,
            /* [out] */ Int32* index);

        CARAPI GetObjectToEncode(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** outobject);
    protected:
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ IGeneralNames* pFullName);

    CARAPI constructor(
        /* [in] */ IName* pNameRelativeToCRLIssuer);

    static CARAPI GetASN1(
        /* [out] */ IASN1Choice** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Choice* pAsn1);

private:
    static CARAPI_(AutoPtr<IASN1Choice>) initASN1();

public:
    static AutoPtr<IASN1Choice> ASN1;

private:
    AutoPtr<IGeneralNames> mFullName;

    AutoPtr<IName> mNameRelativeToCRLIssuer;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CDISTRIBUTIONPOINTNAME_H__
