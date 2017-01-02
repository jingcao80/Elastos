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

#include "org/apache/harmony/security/x509/CInhibitAnyPolicy.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CASN1Integer.h"
#include <elastos/core/CoreUtils.h>
#include "elastos/math/CBigInteger.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CInhibitAnyPolicy)

CAR_INTERFACE_IMPL(CInhibitAnyPolicy, ExtensionValue, IInhibitAnyPolicy)

ECode CInhibitAnyPolicy::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        AutoPtr<IASN1Integer> integer;
        CASN1Integer::GetInstance((IASN1Integer**)&integer);
        AutoPtr<IInterface> obj;
        CASN1Integer::FromIntValue(mSkipCerts, (IInterface**)&obj);
        IASN1Type::Probe(integer)->Encode(obj, (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CInhibitAnyPolicy::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("Inhibit Any-Policy: "));
    sb->Append(mSkipCerts);
    sb->Append('\n');
    return NOERROR;
}

ECode CInhibitAnyPolicy::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);

    AutoPtr<IASN1Integer> integer;
    CASN1Integer::GetInstance((IASN1Integer**)&integer);
    AutoPtr<IInterface> obj;
    IASN1Type::Probe(integer)->Decode(encoding, (IInterface**)&obj);
    AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(obj);
    Int32 length;
    arrayof->GetLength(&length);
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> value;
        arrayof->Get(i, (IInterface**)&value);
        AutoPtr<IByte> b = IByte::Probe(value);
        Byte num;
        b->GetValue(&num);
        (*array)[i] = num;
    }

    AutoPtr<IBigInteger> bigInteger;
    CBigInteger::New(*array, (IBigInteger**)&bigInteger);
    return INumber::Probe(bigInteger)->Int32Value(&mSkipCerts);
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org