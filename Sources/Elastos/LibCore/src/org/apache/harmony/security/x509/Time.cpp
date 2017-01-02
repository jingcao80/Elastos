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

#include "org/apache/harmony/security/x509/Time.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "CASN1GeneralizedTime.h"
#include "CASN1UTCTime.h"

using Elastos::Utility::IDate;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::CASN1GeneralizedTime;
using Org::Apache::Harmony::Security::Asn1::CASN1UTCTime;
using Org::Apache::Harmony::Security::Asn1::IASN1GeneralizedTime;
using Org::Apache::Harmony::Security::Asn1::IASN1UTCTime;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

Time::ASN1ChoiceWrapper::ASN1ChoiceWrapper()
{}

ECode Time::ASN1ChoiceWrapper::GetIndex(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // choose encoding method (see RFC 3280 p. 22)
    Int64 time = 0;
    IDate::Probe(object)->GetTime(&time);
    if (time < JAN_01_2050) {
        *result = 1; // it is before 2050, so encode as UTCTime
        return NOERROR;
    }

    *result = 0; // it is after 2050, encode as GeneralizedTime
    return NOERROR;
}

ECode Time::ASN1ChoiceWrapper::GetObjectToEncode(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = object;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

const Int64 Time::JAN_01_2050 = 2524608000000L;
INIT_PROI_4 AutoPtr<IASN1Choice> Time::ASN1 = InitASN1();
AutoPtr<IASN1Choice> Time::InitASN1()
{
    AutoPtr<ASN1ChoiceWrapper> a = new ASN1ChoiceWrapper();
    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    AutoPtr<IASN1GeneralizedTime> at;
    CASN1GeneralizedTime::GetInstance((IASN1GeneralizedTime**)&at);
    AutoPtr<IASN1UTCTime> ct;
    CASN1UTCTime::GetInstance((IASN1UTCTime**)&ct);
    array->Set(0, IASN1Type::Probe(at));
    array->Set(1, IASN1Type::Probe(ct));
    a->constructor(array);
    return a;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org