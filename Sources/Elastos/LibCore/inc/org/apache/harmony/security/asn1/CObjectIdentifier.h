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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CObjectIdentifier.h"
#include "Object.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CObjectIdentifier)
    , public Object
    , public IObjectIdentifier
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI Validate(
        /* [in] */ ArrayOf<Int32>* oid);

    static CARAPI ToString(
        /* [in] */ ArrayOf<Int32>* oid,
        /* [out] */ String* str);

    static CARAPI ToIntArray(
        /* [in] */ const String& str,
        /* [out, callee] */ ArrayOf<Int32>** intArray);

    static CARAPI IsOID(
        /* [in] */ const String& str,
        /* [out] */ Boolean* isOID);

    CARAPI Equals(
        /* [in] */ PInterface obj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* oid);

    CARAPI constructor(
        /* [in] */ const String& strOid);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetOid();

private:
    static CARAPI ToIntArray(
        /* [in] */ const String& str,
        /* [in] */ Boolean shouldThrow,
        /* [out, callee] */ ArrayOf<Int32>** ret);

private:
        /** OID as array of integers */
    AutoPtr<ArrayOf<Int32> > mOid;

    /** OID as string */
    String mSoid;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIER_H__
