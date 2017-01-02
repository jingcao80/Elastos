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

#include "org/apache/harmony/security/x509/tsp/TimeStampReq.h"
#include "org/apache/harmony/security/x509/tsp/CTimeStampReq.h"
#include "org/apache/harmony/security/x509/tsp/CMessageImprint.h"
#include "org/apache/harmony/security/x509/CGeneralName.h"
#include "org/apache/harmony/security/x509/CExtensions.h"
#include "org/apache/harmony/security/asn1/ASN1Oid.h"
#include "org/apache/harmony/security/asn1/CASN1Explicit.h"
#include "org/apache/harmony/security/asn1/CASN1Implicit.h"
#include "org/apache/harmony/security/asn1/CASN1Integer.h"
#include "org/apache/harmony/security/asn1/CASN1Boolean.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/asn1/CASN1GeneralizedTime.h"
#include "elastos/math/CBigInteger.h"
#include "elastos/math/CBigIntegerHelper.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"
#include "core/CBoolean.h"
#include "core/CInteger32.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Oid;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::CASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Boolean;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::Asn1::IASN1GeneralizedTime;
using Org::Apache::Harmony::Security::Asn1::CASN1GeneralizedTime;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CBoolean;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::Utility::IDate;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

ECode TimeStampReq::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj2;
    values->Get(2, (IInterface**)&obj2);
    String objID;
    if (obj2 == NULL) {
        objID = String(NULL);
    }
    else {
        AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(obj2);

        Int32 length;
        arrayof->GetLength(&length);
        AutoPtr<ArrayOf<Int32> > intarray = ArrayOf<Int32>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IInterface> obj;
            arrayof->Get(i, (IInterface**)&obj);
            AutoPtr<IInteger32> intobj = IInteger32::Probe(obj);
            Int32 value;
            intobj->GetValue(&value);
            (*intarray)[i] = value;
        }
        CObjectIdentifier::ToString(intarray, &objID);
    }

    AutoPtr<IInterface> obj3;
    values->Get(3, (IInterface**)&obj3);
    AutoPtr<IBigInteger> nonce;
    if (obj3 == NULL) {
        nonce = NULL;
    }
    else {
        AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(obj3);
        Int32 length;
        arrayof->GetLength(&length);
        AutoPtr<ArrayOf<Byte> > barray = ArrayOf<Byte>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IInterface> obj;
            arrayof->Get(i, (IInterface**)&obj);
            AutoPtr<IByte> b = IByte::Probe(obj);
            Byte value;
            b->GetValue(&value);
            (*barray)[i] = value;
        }
        AutoPtr<IBigInteger> bi;
        CBigInteger::New(*barray, (IBigInteger**)&nonce);
    }

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);
    Int32 intValue;
    CASN1Integer::ToIntValue(obj0, &intValue);

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);

    AutoPtr<IInterface> obj4;
    values->Get(4, (IInterface**)&obj4);
    AutoPtr<IBoolean > b = IBoolean::Probe(obj4);
    Boolean res;
    b->GetValue(&res);

    AutoPtr<IInterface> obj5;
    values->Get(5, (IInterface**)&obj5);

    AutoPtr<ArrayOf<Byte> > encoded;
    bis->GetEncoded((ArrayOf<Byte>**)&encoded);

    AutoPtr<ITimeStampReq> timeStampReq;
    if (obj5 == NULL) {
        CTimeStampReq::New(
                intValue,
                IMessageImprint::Probe(obj1),
                objID,
                nonce,
                res,
                NULL,
                encoded,
                (ITimeStampReq**)&timeStampReq);
    }
    else {
        CTimeStampReq::New(
                intValue,
                IMessageImprint::Probe(obj1),
                objID,
                nonce,
                res,
                IExtensions::Probe(obj5),
                encoded,
                (ITimeStampReq**)&timeStampReq);
    }

    *object = TO_IINTERFACE(timeStampReq);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TimeStampReq::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    TimeStampReq* req = (TimeStampReq*)ITimeStampReq::Probe(object);

    AutoPtr<IInterface> obj0;
    CASN1Integer::FromIntValue(req->mVersion, (IInterface**)&obj0);
    values->Set(0,  obj0);
    values->Set(1,  TO_IINTERFACE(req->mMessageImprint));

    if (req->mReqPolicy.IsNull()) {
        values->Set(2, NULL);
    }
    else {
        AutoPtr<ArrayOf<Int32> > array;
        CObjectIdentifier::ToIntArray(req->mReqPolicy, (ArrayOf<Int32>**)&array);
        AutoPtr<IArrayOf> arrayof = CoreUtils::Convert(array);
        values->Set(2, TO_IINTERFACE(arrayof));
    }

    if (req->mNonce == NULL) {
        values->Set(3, NULL);
    }
    else {
        AutoPtr<ArrayOf<Byte> > array;
        req->mNonce->ToByteArray((ArrayOf<Byte>**)&array);
        AutoPtr<IArrayOf> arrayof = CoreUtils::ConvertByteArray(array);
        values->Set(3, TO_IINTERFACE(arrayof));
    }

    if (req->mCertReq == NULL) {
        values->Set(4, TO_IINTERFACE(CoreUtils::Convert(FALSE)));
    }
    else {
        values->Set(4, TO_IINTERFACE(req->mCertReq));
    }

    values->Set(5,  TO_IINTERFACE(req->mExtensions));
    return NOERROR;
}

AutoPtr<IASN1Sequence> TimeStampReq::initASN1()
{
    AutoPtr<IASN1Integer> instance;
    CASN1Integer::GetInstance((IASN1Integer**)&instance);

    AutoPtr<IASN1Type> type;
    ASN1Oid::GetInstance((IASN1Type**)&type);

    AutoPtr<IASN1Type> type2;
    CASN1Boolean::GetInstance((IASN1Type**)&type2);

    AutoPtr<IASN1Implicit> implicit;
    CASN1Implicit::New(0, CExtensions::ASN1, (IASN1Implicit**)&implicit);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(6);
    array->Set(0, IASN1Type::Probe(instance)); // version
    array->Set(1, IASN1Type::Probe(CMessageImprint::ASN1)); // messageImprint
    array->Set(2, type); // reqPolicy
    array->Set(3, IASN1Type::Probe(instance)); // nonce
    array->Set(4, type2); // certReq
    array->Set(5, IASN1Type::Probe(implicit)); // extensions

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    AutoPtr<IBoolean> ib = CoreUtils::Convert(FALSE);
    tmp->SetDefault(TO_IINTERFACE(ib), 4);
    tmp->SetOptional(2);
    tmp->SetOptional(3);
    tmp->SetOptional(5);
    return IASN1Sequence::Probe(tmp);
}

AutoPtr<IASN1Sequence> TimeStampReq::ASN1 = initASN1();

CAR_INTERFACE_IMPL(TimeStampReq, Object, ITimeStampReq)

ECode TimeStampReq::ToString(
    /* [out] */ String* pStr)
{
    VALIDATE_NOT_NULL(pStr);

    StringBuilder res;
    res += "-- TimeStampReq:";
    res += "\nversion : ";
    res += mVersion;
    res += "\nmessageImprint:  ";
    res += TO_IINTERFACE(mMessageImprint);
    res += "\nreqPolicy:  ";
    res += mReqPolicy;
    res += "\nnonce:  ";
    res += TO_IINTERFACE(mNonce);
    res += "\ncertReq:  ";
    res += mCertReq;
    res += "\nextensions:  ";
    res += TO_IINTERFACE(mExtensions);
    res += "\n-- TimeStampReq End\n";
    return res.ToString(pStr);
}

ECode TimeStampReq::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        ((ASN1Sequence*)ASN1.Get())->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode TimeStampReq::GetCertReq(
    /* [out] */ Boolean* pCertReq)
{
    VALIDATE_NOT_NULL(pCertReq);

    return mCertReq->GetValue(pCertReq);
}

ECode TimeStampReq::GetExtensions(
    /* [out] */ IExtensions** ppExtensions)
{
    VALIDATE_NOT_NULL(ppExtensions);

    *ppExtensions = mExtensions;
    REFCOUNT_ADD(*ppExtensions);
    return NOERROR;
}

ECode TimeStampReq::GetMessageImprint(
    /* [out] */ IMessageImprint** ppMessageImprint)
{
    VALIDATE_NOT_NULL(ppMessageImprint);

    *ppMessageImprint = mMessageImprint;
    REFCOUNT_ADD(*ppMessageImprint);
    return NOERROR;
}

ECode TimeStampReq::GetNonce(
    /* [out] */ IBigInteger** ppNonce)
{
    VALIDATE_NOT_NULL(ppNonce);

    *ppNonce = mNonce;
    REFCOUNT_ADD(*ppNonce);
    return NOERROR;
}

ECode TimeStampReq::GetReqPolicy(
    /* [out] */ String* pReqPolicy)
{
    VALIDATE_NOT_NULL(pReqPolicy);

    *pReqPolicy = mReqPolicy;
    return NOERROR;
}

ECode TimeStampReq::GetVersion(
    /* [out] */ Int32* pVer)
{
    VALIDATE_NOT_NULL(pVer);

    *pVer = mVersion;
    return NOERROR;
}

ECode TimeStampReq::constructor(
    /* [in] */ Int32 version,
    /* [in] */ IMessageImprint* messageImprint,
    /* [in] */ const String& reqPolicy,
    /* [in] */ IBigInteger* nonce,
    /* [in] */ Boolean certReq,
    /* [in] */ IExtensions* extensions)
{
    mVersion = version;
    mMessageImprint = messageImprint;
    mReqPolicy = reqPolicy;
    mNonce = nonce;
    mCertReq = CoreUtils::Convert(certReq);
    mExtensions = extensions;
    return NOERROR;
}

ECode TimeStampReq::constructor(
    /* [in] */ Int32 version,
    /* [in] */ IMessageImprint* messageImprint,
    /* [in] */ const String& reqPolicy,
    /* [in] */ IBigInteger* nonce,
    /* [in] */ Boolean certReq,
    /* [in] */ IExtensions* extensions,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(version, messageImprint, reqPolicy, nonce, certReq, extensions);
    mEncoding = encoding;
    return NOERROR;
}

ECode TimeStampReq::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1)

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode TimeStampReq::SetASN1(
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