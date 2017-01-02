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

#include "org/apache/harmony/security/x509/CGeneralSubtree.h"
#include "org/apache/harmony/security/x509/CGeneralName.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/asn1/CASN1Integer.h"
#include "org/apache/harmony/security/asn1/CASN1Implicit.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CGeneralSubtree::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    Int32 maximum = -1; // is optional maximum missing?
    AutoPtr<IInterface> obj2;
    values->Get(2, (IInterface**)&obj2);
    if (obj2 != NULL) {
        CASN1Integer::ToIntValue(obj2, &maximum); // no!
    }

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);
    Int32 min;
    CASN1Integer::ToIntValue(obj1, &min);
    AutoPtr<IGeneralSubtree> tmp;
    CGeneralSubtree::New(IGeneralName::Probe(obj0), min, maximum, (IGeneralSubtree**)&tmp);
    *object = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CGeneralSubtree::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CGeneralSubtree* gs = (CGeneralSubtree*)IGeneralSubtree::Probe(object);
    values->Set(0, TO_IINTERFACE(gs->mBase));

    AutoPtr<IInterface> value1;
    CASN1Integer::FromIntValue(gs->mMinimum, (IInterface**)&value1);
    values->Set(1, value1);

    if (gs->mMaximum > -1) {
        AutoPtr<IInterface> value2;
        CASN1Integer::FromIntValue(gs->mMaximum, (IInterface**)&value2);
        values->Set(2, value2);
    }
    return NOERROR;
}

AutoPtr<IASN1Sequence> CGeneralSubtree::initASN1()
{
    AutoPtr<IASN1Integer> integer1;
    CASN1Integer::GetInstance((IASN1Integer**)&integer1);
    AutoPtr<IASN1Implicit> implicit1;
    CASN1Implicit::New(0, IASN1Type::Probe(integer1), (IASN1Implicit**)&implicit1);

    AutoPtr<IASN1Integer> integer2;
    CASN1Integer::GetInstance((IASN1Integer**)&integer2);
    AutoPtr<IASN1Implicit> implicit2;
    CASN1Implicit::New(1, IASN1Type::Probe(integer2), (IASN1Implicit**)&implicit2);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(3);
    array->Set(0, IASN1Type::Probe(CGeneralName::ASN1));
    array->Set(1, IASN1Type::Probe(implicit1));
    array->Set(2, IASN1Type::Probe(implicit2));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    AutoPtr<ArrayOf<Byte> > array2 = ArrayOf<Byte>::Alloc(1);
    (*array2)[0] = 0;
    AutoPtr<IArrayOf> arrayof = CoreUtils::ConvertByteArray(array2);
    tmp->SetDefault(TO_IINTERFACE(arrayof), 1);  // minimum 0
    tmp->SetOptional(2);                 // maximum optional
    return IASN1Sequence::Probe(tmp);
}

AutoPtr<IASN1Sequence> CGeneralSubtree::ASN1 = initASN1();

CAR_OBJECT_IMPL(CGeneralSubtree)

CAR_INTERFACE_IMPL(CGeneralSubtree, Object, IGeneralSubtree)

ECode CGeneralSubtree::GetBase(
    /* [out] */ IGeneralName** ppBase)
{
    VALIDATE_NOT_NULL(ppBase);

    *ppBase = mBase;
    REFCOUNT_ADD(*ppBase);
    return NOERROR;
}

ECode CGeneralSubtree::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CGeneralSubtree::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("General Subtree: [\n"));
    sb->Append(prefix);
    sb->Append(String("  base: "));
    sb->Append(mBase);
    sb->Append('\n');
    sb->Append(prefix);
    sb->Append(String("  minimum: "));
    sb->Append(mMinimum);
    sb->Append('\n');
    if (mMaximum >= 0) {
        sb->Append(prefix);
        sb->Append(String("  maximum: "));
        sb->Append(mMaximum);
        sb->Append('\n');
    }
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CGeneralSubtree::constructor(
    /* [in] */ IGeneralName* base,
    /* [in] */ Int32 minimum,
    /* [in] */ Int32 maximum)
{
    mBase = base;
    mMinimum = minimum;
    mMaximum = maximum;
    return NOERROR;
}

ECode CGeneralSubtree::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CGeneralSubtree::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org