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

#include "org/apache/harmony/security/x509/CBasicConstraints.h"
#include "org/apache/harmony/security/asn1/CASN1Boolean.h"
#include "org/apache/harmony/security/asn1/CASN1Integer.h"
#include "elastos/core/Math.h"
#include "elastos/math/CBigInteger.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Boolean;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::INumber;
using Elastos::Core::IBoolean;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::Utility::Arrays;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CBasicConstraints::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    *object = TO_IINTERFACE(con);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CBasicConstraints::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(object);
    AutoPtr<IInterface> obj0;
    array->Get(0, (IInterface**)&obj0);
    values->Set(0, obj0);

    AutoPtr<IInterface> obj1;
    array->Get(0, (IInterface**)&obj1);
    AutoPtr<IBigInteger> b = IBigInteger::Probe(obj1);
    AutoPtr<ArrayOf<Byte> > barray;
    b->ToByteArray((ArrayOf<Byte>**)&barray);
    AutoPtr<IArrayOf> arrayof = CoreUtils::ConvertByteArray(barray);
    values->Set(1, TO_IINTERFACE(arrayof));

    return NOERROR;
}

AutoPtr<IASN1Type> CBasicConstraints::initASN1()
{
    AutoPtr<IASN1Type> instance1;
    CASN1Boolean::GetInstance((IASN1Type**)&instance1);
    AutoPtr<IASN1Integer> instance2;
    CASN1Integer::GetInstance((IASN1Integer**)&instance2);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, instance1);
    array->Set(1, IASN1Type::Probe(instance2));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    tmp->SetDefault(TO_IINTERFACE(CoreUtils::Convert(FALSE)), 0);
    tmp->SetOptional(1);
    return IASN1Type::Probe(tmp);
}

AutoPtr<IASN1Type> CBasicConstraints::ASN1 = initASN1();

CAR_OBJECT_IMPL(CBasicConstraints)

CAR_INTERFACE_IMPL(CBasicConstraints, ExtensionValue, IBasicConstraints)

CBasicConstraints::CBasicConstraints()
    : mCa(FALSE)
    , mPathLenConstraint(Elastos::Core::Math::INT32_MAX_VALUE)
{
}

ECode CBasicConstraints::GetCa(
    /* [out] */ Boolean* ca)
{
    VALIDATE_NOT_NULL(ca);

    *ca = mCa;
    return NOERROR;
}

ECode CBasicConstraints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        AutoPtr<IBoolean> bobj = CoreUtils::Convert(mCa);
        AutoPtr<IBigInteger> iobj;
        CBigInteger::ValueOf(mPathLenConstraint, (IBigInteger**)&iobj);
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(2);
        array->Set(0, TO_IINTERFACE(bobj));
        array->Set(1, TO_IINTERFACE(iobj));
        AutoPtr<IArrayOf> arrayof = CoreUtils::Convert(array, EIID_IInterface);
        ASN1->Encode(arrayof, (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CBasicConstraints::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("BasicConstraints [\n"));
    sb->Append(prefix);
    sb->Append(String("  CA: "));
    sb->Append(mCa);
    sb->Append(String("\n  "));
    sb->Append(prefix);
    sb->Append(String("pathLenConstraint: "));
    sb->Append(mPathLenConstraint);
    sb->Append('\n');
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CBasicConstraints::GetPathLenConstraint(
    /* [out] */ Int32* pPathLenConstraint)
{
    VALIDATE_NOT_NULL(pPathLenConstraint);

    *pPathLenConstraint = mPathLenConstraint;
    return NOERROR;
}

ECode CBasicConstraints::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);

    AutoPtr<IInterface> obj;
    ASN1->Decode(encoding, (IInterface**)&obj);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(obj);

    AutoPtr<IInterface> obj2;
    values->Get(0, (IInterface**)&obj2);
    AutoPtr<IBoolean> bobj = IBoolean::Probe(obj2);
    bobj->GetValue(&mCa);

    AutoPtr<IInterface> obj3;
    values->Get(1, (IInterface**)&obj3);
    if (obj3 != NULL) {
        AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(obj3);
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
        AutoPtr<IBigInteger> bi;
        CBigInteger::New(*array, (IBigInteger**)&bi);
        INumber::Probe(bi)->Int32Value(&mPathLenConstraint);
    }
    return NOERROR;
}

ECode CBasicConstraints::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CBasicConstraints::SetASN1(
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