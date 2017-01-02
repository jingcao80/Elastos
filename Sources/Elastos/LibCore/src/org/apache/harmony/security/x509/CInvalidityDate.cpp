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

#include "org/apache/harmony/security/x509/CInvalidityDate.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CASN1GeneralizedTime.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1GeneralizedTime;
using Org::Apache::Harmony::Security::Asn1::CASN1GeneralizedTime;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICloneable;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

AutoPtr<IASN1Type> CInvalidityDate::initASN1()
{
    AutoPtr<IASN1GeneralizedTime> time;
    CASN1GeneralizedTime::GetInstance((IASN1GeneralizedTime**)&time);
    return IASN1Type::Probe(time);
}

AutoPtr<IASN1Type> CInvalidityDate::ASN1 = initASN1();

CAR_OBJECT_IMPL(CInvalidityDate)

CAR_INTERFACE_IMPL(CInvalidityDate, ExtensionValue, IInvalidityDate)

ECode CInvalidityDate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(mDate), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CInvalidityDate::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("Invalidity Date: [ "));
    sb->Append(mDate);
    sb->Append(String(" ]\n"));
    return NOERROR;
}

ECode CInvalidityDate::GetDate(
    /* [out] */ IDate** ppDate)
{
    VALIDATE_NOT_NULL(ppDate);

    *ppDate = mDate;
    REFCOUNT_ADD(*ppDate);
    return NOERROR;
}

ECode CInvalidityDate::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);
    AutoPtr<IInterface> obj;
    ASN1->Decode(encoding, (IInterface**)&obj);
    mDate = IDate::Probe(obj);
    return NOERROR;
}

ECode CInvalidityDate::constructor(
    /* [in] */ IDate* date)
{
    AutoPtr<IInterface> obj;
    ICloneable::Probe(date)->Clone((IInterface**)&obj);
    mDate = IDate::Probe(obj);
    return NOERROR;
}

ECode CInvalidityDate::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CInvalidityDate::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org