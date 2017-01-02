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

#ifndef __ELASTOS_SECURITY_CERT_CPOLICYQUALIFIERINFO_H__
#define __ELASTOS_SECURITY_CERT_CPOLICYQUALIFIERINFO_H__

#include "_Elastos_Security_Cert_CPolicyQualifierInfo.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CPolicyQualifierInfo)
    , public Object
    , public IPolicyQualifierInfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> **encoded);

    CARAPI GetPolicyQualifierId(
        /* [out] */ String *id);

    CARAPI GetPolicyQualifier(
        /* [out, callee] */ ArrayOf<Byte> **qualifier);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> *encoded);

private:
    // This PolicyQualifierInfo DER encoding
    AutoPtr<ArrayOf<Byte> > mEncoded;
    // This PolicyQualifierInfo policy qualifier id -
    // OID represented as String containing non-negative integers
    // separated by periods
    String mPolicyQualifierId;
    // DER encoding of the policy qualifier - part of encoded
    AutoPtr<ArrayOf<Byte> > mPolicyQualifier;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CPOLICYQUALIFIERINFO_H__
