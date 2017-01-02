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

#include "org/apache/harmony/security/x509/tsp/TimeStampResp.h"
#include "org/apache/harmony/security/x509/tsp/PKIStatusInfo.h"
#include "org/apache/harmony/security/pkcs7/ContentInfo.h"
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"

using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Pkcs7::ContentInfo;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

ECode TimeStampResp::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj;
    values->Get(0, (IInterface**)&obj);

    AutoPtr<IInterface> obj2;
    values->Get(1, (IInterface**)&obj2);

    AutoPtr<TimeStampResp> tmp = new TimeStampResp();
    tmp->constructor(IPKIStatusInfo::Probe(obj), IContentInfo::Probe(obj2));
    *object = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode TimeStampResp::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    TimeStampResp* resp = (TimeStampResp*)ITimeStampResp::Probe(object);

    values->Set(0, TO_IINTERFACE(resp->mStatus));
    values->Set(1, TO_IINTERFACE(resp->mTimeStampToken));
    return NOERROR;
}

AutoPtr<IASN1Sequence> TimeStampResp::initASN1()
{
    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, IASN1Type::Probe(PKIStatusInfo::ASN1));
    array->Set(1, IASN1Type::Probe(ContentInfo::ASN1));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    tmp->SetOptional(1);
    return IASN1Sequence::Probe(tmp);
}

AutoPtr<IASN1Sequence> TimeStampResp::ASN1 = initASN1();;

CAR_INTERFACE_IMPL(TimeStampResp, Object, ITimeStampResp)

ECode TimeStampResp::ToString(
    /* [out] */ String* pStr)
{
    StringBuilder res;
    res += "-- TimeStampResp:";
    res += "\nstatus:  ";
    res += TO_IINTERFACE(mStatus);
    res += "\ntimeStampToken:  ";
    res += TO_IINTERFACE(mTimeStampToken);
    res += "\n-- TimeStampResp End\n";
    return res.ToString(pStr);
}

ECode TimeStampResp::GetStatus(
    /* [out] */ IPKIStatusInfo** ppStatus)
{
    VALIDATE_NOT_NULL(ppStatus);

    *ppStatus = mStatus;
    REFCOUNT_ADD(*ppStatus);
    return NOERROR;
}

ECode TimeStampResp::GetTimeStampToken(
    /* [out] */ IContentInfo** ppTimeStampToken)
{
    VALIDATE_NOT_NULL(ppTimeStampToken);

    *ppTimeStampToken = mTimeStampToken;
    REFCOUNT_ADD(*ppTimeStampToken);
    return NOERROR;
}

ECode TimeStampResp::constructor(
    /* [in] */ IPKIStatusInfo* status,
    /* [in] */ IContentInfo* timeStampToken)
{
    mStatus = status;
    mTimeStampToken = timeStampToken;
    return NOERROR;
}

ECode TimeStampResp::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1)

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode TimeStampResp::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org