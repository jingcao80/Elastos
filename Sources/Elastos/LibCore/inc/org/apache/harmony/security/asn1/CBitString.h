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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CBITSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CBITSTRING_H__

#include "_Org_Apache_Harmony_Security_Asn1_CBitString.h"
#include "Object.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CBitString)
    , public Object
    , public IBitString
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) InitStatic();

    CARAPI SetBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI GetBytes(
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI SetUnusedBits(
        /* [in] */ Int32 unusedBits);

    CARAPI GetUnusedBits(
        /* [out] */ Int32* unusedBits);

    CARAPI GetBit(
        /* [in] */ Int32 bit,
        /* [out] */ Boolean* value);

    CARAPI SetBit(
        /* [in] */ Int32 bit,
        /* [in] */ Boolean value);

    CARAPI ToBooleanArray(
        /* [out, callee] */ ArrayOf<Boolean>** array);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 unusedBits);

    CARAPI constructor(
        /* [in] */ ArrayOf<Boolean>* values);

public:
    /** Sequence of bits padded with unused bits. */
    AutoPtr<ArrayOf<Byte> > mBytes;

    /** Number of unused bits in the last byte. */
    Int32 mUnusedBits;

private:
    static AutoPtr<ArrayOf<Byte> > SET_MASK;

    static AutoPtr<ArrayOf<Byte> > RESET_MASK;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CBITSTRING_H__
