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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SEQUENCE_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SEQUENCE_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Sequence.h"
#include <ASN1Sequence.h>
#include <Asn1TypeMacro.h>

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Sequence), public ASN1Sequence
{
public:
    CAR_OBJECT_DECL()

    ASN1TYPE_METHODS_DECL()

    CARAPI SetType(
        /* [in] */ ArrayOf<IASN1Type *>* type);

    CARAPI GetType(
        /* [out, callee] */ ArrayOf<IASN1Type *>** type);

    CARAPI SetOPTIONAL(
        /* [in] */ ArrayOf<Boolean>* optional);

    CARAPI GetOPTIONAL(
        /* [out, callee] */ ArrayOf<Boolean>** optional);

    CARAPI SetDEFAULT(
        /* [in] */ ArrayOf<IInterface *>* def);

    CARAPI GetDEFAULT(
        /* [out, callee] */ ArrayOf<IInterface *>** def);

    CARAPI GetValues(
        /* [in] */ IInterface* object,
        /* [in] */ ArrayOf<IInterface*>* values);

    CARAPI constructor(
        /* [in] */ ArrayOf<IASN1Type *>* type);

private:
    // TODO: Add your private member variables here.
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SEQUENCE_H__
