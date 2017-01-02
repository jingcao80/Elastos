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

#include "org/apache/harmony/security/x509/CSubjectKeyIdentifier.h"
#include "CoreUtils.h"
#include "ASN1OctetString.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Org::Apache::Harmony::Security::Asn1::ASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CSubjectKeyIdentifier)
CAR_INTERFACE_IMPL(CSubjectKeyIdentifier, ExtensionValue, ISubjectKeyIdentifier)

ECode CSubjectKeyIdentifier::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mEncoding == NULL) {
        AutoPtr<IASN1OctetString> as;
        ASN1OctetString::GetInstance((IASN1OctetString**)&as);
        IASN1Type::Probe(as)->Encode(CoreUtils::ConvertByteArray(mKeyIdentifier)
                    , (ArrayOf<Byte>**)&mEncoding);
    }
    *result = mEncoding;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSubjectKeyIdentifier::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    assert(0 && "TODO");
    // sb.append(prefix).append("SubjectKeyIdentifier: [\n");
    // sb.append(Array.toString(keyIdentifier, prefix));
    // sb.append(prefix).append("]\n");
    return NOERROR;
}

ECode CSubjectKeyIdentifier::GetKeyIdentifier(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeyIdentifier;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSubjectKeyIdentifier::Decode(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ ISubjectKeyIdentifier** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IASN1OctetString> as;
    ASN1OctetString::GetInstance((IASN1OctetString**)&as);
    AutoPtr<IInterface> obj;
    IASN1Type::Probe(as)->Decode(encoding, (IInterface**)&obj);

    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > bs = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> o;
        array->Get(i, (IInterface**)&o);
        Byte b;
        IByte::Probe(o)->GetValue(&b);
        (*bs)[i] = b;
    }

    AutoPtr<ISubjectKeyIdentifier> res;
    CSubjectKeyIdentifier::New(bs, (ISubjectKeyIdentifier**)&res);
    ((CSubjectKeyIdentifier*)res.Get())->mEncoding = encoding;
    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSubjectKeyIdentifier::constructor(
    /* [in] */ ArrayOf<Byte>* keyIdentifier)
{
    mKeyIdentifier = keyIdentifier;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org