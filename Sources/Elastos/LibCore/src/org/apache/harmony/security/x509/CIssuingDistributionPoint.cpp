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

#include "org/apache/harmony/security/x509/CIssuingDistributionPoint.h"
#include "org/apache/harmony/security/x509/CDistributionPointName.h"
#include "org/apache/harmony/security/x509/CReasonFlags.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CASN1Explicit.h"
#include "org/apache/harmony/security/asn1/CASN1Implicit.h"
#include "org/apache/harmony/security/asn1/CASN1Boolean.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Boolean;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CIssuingDistributionPoint::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);

    AutoPtr<IInterface> obj3;
    values->Get(3, (IInterface**)&obj3);

    AutoPtr<IIssuingDistributionPoint> point;
    CIssuingDistributionPoint::New(IDistributionPointName::Probe(obj0),
            IReasonFlags::Probe(obj3), (IIssuingDistributionPoint**)&point);
    CIssuingDistributionPoint* idp = (CIssuingDistributionPoint*)point.Get();

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);
    idp->mEncoding = array;

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);
    if (obj1 != NULL) {
        Boolean res;
        IBoolean::Probe(obj1)->GetValue(&res);
        idp->SetOnlyContainsUserCerts(res);
    }

    AutoPtr<IInterface> obj2;
    values->Get(2, (IInterface**)&obj2);
    if (obj2 != NULL) {
        Boolean res;
        IBoolean::Probe(obj2)->GetValue(&res);
        idp->SetOnlyContainsCACerts(res);
    }

    AutoPtr<IInterface> obj4;
    values->Get(4, (IInterface**)&obj4);
    if (obj4 != NULL) {
        Boolean res;
        IBoolean::Probe(obj4)->GetValue(&res);
        idp->SetIndirectCRL(res);
    }

    AutoPtr<IInterface> obj5;
    values->Get(5, (IInterface**)&obj5);
    if (obj5 != NULL) {
        Boolean res;
        IBoolean::Probe(obj5)->GetValue(&res);
        idp->SetOnlyContainsAttributeCerts(res);
    }
    *object = TO_IINTERFACE(idp);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CIssuingDistributionPoint::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CIssuingDistributionPoint* idp = (CIssuingDistributionPoint*)IIssuingDistributionPoint::Probe(object);
    values->Set(0, TO_IINTERFACE(idp->mDistributionPoint));

    Boolean res;
    idp->mOnlyContainsUserCerts->GetValue(&res);
    values->Set(1, res ? TO_IINTERFACE(CoreUtils::Convert(TRUE)) : NULL);

    idp->mOnlyContainsCACerts->GetValue(&res);
    values->Set(2, res ? TO_IINTERFACE(CoreUtils::Convert(TRUE)) : NULL);

    values->Set(3, TO_IINTERFACE(idp->mOnlySomeReasons));

    idp->mIndirectCRL->GetValue(&res);
    values->Set(4, res ? TO_IINTERFACE(CoreUtils::Convert(TRUE)) : NULL);

    idp->mOnlyContainsAttributeCerts->GetValue(&res);
    values->Set(5, res ? TO_IINTERFACE(CoreUtils::Convert(TRUE)) : NULL);
    return NOERROR;
}

AutoPtr<IASN1Type> CIssuingDistributionPoint::initASN1()
{
    // ASN.1 prohibits implicitly tagged CHOICE
    AutoPtr<IASN1Explicit> explicit0;
    CASN1Explicit::New(0, IASN1Type::Probe(CDistributionPointName::ASN1),
            (IASN1Explicit**)&explicit0);

    AutoPtr<IASN1Type> instance1;
    CASN1Boolean::GetInstance((IASN1Type**)&instance1);

    AutoPtr<IASN1Implicit> implicit1;
    CASN1Implicit::New(1, instance1, (IASN1Implicit**)&implicit1);

    AutoPtr<IASN1Implicit> implicit2;
    CASN1Implicit::New(2, instance1, (IASN1Implicit**)&implicit2);

    AutoPtr<IASN1Implicit> implicit3;
    CASN1Implicit::New(3, IASN1Type::Probe(CReasonFlags::ASN1), (IASN1Implicit**)&implicit3);

    AutoPtr<IASN1Implicit> implicit4;
    CASN1Implicit::New(4, instance1, (IASN1Implicit**)&implicit4);

    AutoPtr<IASN1Implicit> implicit5;
    CASN1Implicit::New(5, instance1, (IASN1Implicit**)&implicit5);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(6);
    array->Set(0, IASN1Type::Probe(explicit0));
    array->Set(1, IASN1Type::Probe(implicit1));
    array->Set(2, IASN1Type::Probe(implicit2));
    array->Set(3, IASN1Type::Probe(implicit3));
    array->Set(4, IASN1Type::Probe(implicit4));
    array->Set(5, IASN1Type::Probe(implicit5));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    tmp->SetOptional(0);
    tmp->SetOptional(3);
    tmp->SetDefault(TO_IINTERFACE(CoreUtils::Convert(FALSE)), 1);
    tmp->SetDefault(TO_IINTERFACE(CoreUtils::Convert(FALSE)), 2);
    tmp->SetDefault(TO_IINTERFACE(CoreUtils::Convert(FALSE)), 4);
    tmp->SetDefault(TO_IINTERFACE(CoreUtils::Convert(FALSE)), 5);
    return IASN1Type::Probe(tmp);
}

AutoPtr<IASN1Type> CIssuingDistributionPoint::ASN1 = initASN1();

CAR_OBJECT_IMPL(CIssuingDistributionPoint)

CAR_INTERFACE_IMPL(CIssuingDistributionPoint, ExtensionValue, IIssuingDistributionPoint)

ECode CIssuingDistributionPoint::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CIssuingDistributionPoint::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("Issuing Distribution Point: [\n"));
    if (mDistributionPoint != NULL) {
        mDistributionPoint->DumpValue(sb, String("  ") + prefix);
    }
    sb->Append(prefix);
    sb->Append(String("  onlyContainsUserCerts: "));
    sb->Append(mOnlyContainsUserCerts);
    sb->Append('\n');
    sb->Append(prefix);
    sb->Append(String("  onlyContainsCACerts: "));
    sb->Append(mOnlyContainsCACerts);
    sb->Append('\n');
    if (mOnlySomeReasons != NULL) {
        mOnlySomeReasons->DumpValue(sb, prefix + String("  "));
    }
    sb->Append(prefix);
    sb->Append(String("  indirectCRL: "));
    sb->Append(mIndirectCRL);
    sb->Append('\n');
    sb->Append(prefix);
    sb->Append(String("  onlyContainsAttributeCerts: "));
    sb->Append(mOnlyContainsAttributeCerts);
    sb->Append('\n');
    return NOERROR;
}

ECode CIssuingDistributionPoint::SetOnlyContainsUserCerts(
    /* [in] */ Boolean onlyContainsUserCerts)
{
    mOnlyContainsUserCerts = CoreUtils::Convert(onlyContainsUserCerts);
    return NOERROR;
}

ECode CIssuingDistributionPoint::SetOnlyContainsCACerts(
    /* [in] */ Boolean onlyContainsCACerts)
{
    mOnlyContainsCACerts = CoreUtils::Convert(onlyContainsCACerts);
    return NOERROR;
}

ECode CIssuingDistributionPoint::SetIndirectCRL(
    /* [in] */ Boolean indirectCRL)
{
    mIndirectCRL = CoreUtils::Convert(indirectCRL);
    return NOERROR;
}

ECode CIssuingDistributionPoint::SetOnlyContainsAttributeCerts(
    /* [in] */ Boolean onlyContainsAttributeCerts)
{
    mOnlyContainsAttributeCerts = CoreUtils::Convert(onlyContainsAttributeCerts);
    return NOERROR;
}

ECode CIssuingDistributionPoint::constructor(
    /* [in] */ IDistributionPointName* distributionPoint,
    /* [in] */ IReasonFlags* onlySomeReasons)
{
    mDistributionPoint = distributionPoint;
    mOnlySomeReasons = onlySomeReasons;
    return NOERROR;
}

ECode CIssuingDistributionPoint::Decode(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ IIssuingDistributionPoint** ppObject)
{
    VALIDATE_NOT_NULL(ppObject);

    AutoPtr<IInterface> obj;
    ASN1->Decode(encoding, (IInterface**)&obj);

    CIssuingDistributionPoint* idp = (CIssuingDistributionPoint*)IIssuingDistributionPoint::Probe(obj);
    idp->mEncoding = encoding;
    *ppObject = IIssuingDistributionPoint::Probe(idp);
    REFCOUNT_ADD(*ppObject);
    return NOERROR;
}

ECode CIssuingDistributionPoint::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CIssuingDistributionPoint::SetASN1(
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