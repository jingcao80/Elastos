
#include "org/apache/harmony/security/x509/tsp/TSTInfo.h"
#include "org/apache/harmony/security/x509/tsp/CTSTInfo.h"
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
using Elastos::Core::IBoolean;
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

ECode TSTInfo::MyASN1Sequence1::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<ArrayOf<Int32> > accuracy = ArrayOf<Int32>::Alloc(3);
    for (Int32 i = 0; i < 3; i++) {
        AutoPtr<IInterface> obj;
        values->Get(i, (IInterface**)&obj);
        if (obj != NULL) {
            Int32 toIntValue;
            CASN1Integer::ToIntValue(obj, &toIntValue);
            (*accuracy)[i] = toIntValue;

            if (i > 0 && ((*accuracy)[i] < 0 || (*accuracy)[i] > 999)) {
                //throw new RuntimeException("Time-stamp accuracy value is incorrect: " + accuracy[i]);
                return E_RUNTIME_EXCEPTION;
            }
        }
    }

    AutoPtr<IArrayOf> array = CoreUtils::Convert(accuracy);
    *object = TO_IINTERFACE(array);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TSTInfo::MyASN1Sequence1::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(object);
    for (Int32 i = 0; i < 3; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        AutoPtr<IInteger32> intobj = IInteger32::Probe(obj);
        Int32 value;
        intobj->GetValue(&value);
        if (i > 0 && (value < 0 || value > 999)) {
            //throw new RuntimeException("Time-stamp accuracy value is incorrect: " + accuracy[i]);
            return E_RUNTIME_EXCEPTION;
        }
        AutoPtr<IBigInteger> bi;
        CBigInteger::ValueOf((Int64)value, (IBigInteger**)&bi);
        AutoPtr<ArrayOf<Byte> > bytes;
        bi->ToByteArray((ArrayOf<Byte>**)&bytes);
        AutoPtr<IArrayOf> array2 = CoreUtils::ConvertByteArray(bytes);
        values->Set(i, TO_IINTERFACE(array2));
    }
    return NOERROR;
}

AutoPtr<IASN1Sequence> TSTInfo::initACCURACY()
{
    AutoPtr<IASN1Integer> integer;
    CASN1Integer::GetInstance((IASN1Integer**)&integer);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(3);
    array->Set(0, IASN1Type::Probe(integer));
    array->Set(1, IASN1Type::Probe(integer));
    array->Set(2, IASN1Type::Probe(integer));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence1();
    tmp->constructor(array);
    tmp->SetOptional(0);
    tmp->SetOptional(1);
    tmp->SetOptional(2);
    return IASN1Sequence::Probe(tmp);
}

ECode TSTInfo::MyASN1Sequence2::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj7;
    values->Get(7, (IInterface**)&obj7);
    AutoPtr<IBigInteger> nonce;
    if (obj7 == NULL) {
        nonce = NULL;
    }
    else {
        AutoPtr<IArrayOf> array = IArrayOf::Probe(obj7);
        Int32 length;
        array->GetLength(&length);
        AutoPtr<ArrayOf<Byte> > barray = ArrayOf<Byte>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IInterface> obj;
            array->Get(i, (IInterface**)&obj);
            AutoPtr<IByte> b = IByte::Probe(obj);
            Byte value;
            b->GetValue(&value);
            (*barray)[i] = value;
        }
        CBigInteger::New(*barray, (IBigInteger**)&nonce);
    }

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);
    Int32 toIntValue;
    CASN1Integer::ToIntValue(obj0, &toIntValue);

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);
    AutoPtr<IArrayOf> array1 = IArrayOf::Probe(obj1);
    Int32 length1;
    array1->GetLength(&length1);
    AutoPtr<ArrayOf<Int32> > intarray1 = ArrayOf<Int32>::Alloc(length1);
    for (Int32 i = 0; i < length1; i++) {
        AutoPtr<IInterface> obj;
        array1->Get(i, (IInterface**)&obj);
        AutoPtr<IInteger32> intobj = IInteger32::Probe(obj);
        Int32 value;
        intobj->GetValue(&value);
        (*intarray1)[i] = value;
    }
    String str;
    CObjectIdentifier::ToString(intarray1, &str);

    AutoPtr<IInterface> obj2;
    values->Get(2, (IInterface**)&obj2);

    AutoPtr<IInterface> obj3;
    values->Get(3, (IInterface**)&obj3);
    AutoPtr<IArrayOf> array3 = IArrayOf::Probe(obj3);
    Int32 length3;
    array3->GetLength(&length3);
    AutoPtr<ArrayOf<Byte> > barray3 = ArrayOf<Byte>::Alloc(length3);
    for (Int32 i = 0; i < length3; i++) {
        AutoPtr<IInterface> obj;
        array3->Get(i, (IInterface**)&obj);
        AutoPtr<IByte> b = IByte::Probe(obj);
        Byte value;
        b->GetValue(&value);
        (*barray3)[i] = value;
    }
    AutoPtr<IBigInteger> bi;
    CBigInteger::New(*barray3, (IBigInteger**)&bi);

    AutoPtr<IInterface> obj4;
    values->Get(4, (IInterface**)&obj4);

    AutoPtr<IInterface> obj5;
    values->Get(5, (IInterface**)&obj5);
    AutoPtr<IArrayOf> array5 = IArrayOf::Probe(obj5);
    Int32 length5;
    array5->GetLength(&length5);
    AutoPtr<ArrayOf<Int32> > intarray5 = ArrayOf<Int32>::Alloc(length5);
    for (Int32 i = 0; i < length5; i++) {
        AutoPtr<IInterface> obj;
        array5->Get(i, (IInterface**)&obj);
        AutoPtr<IInteger32> intobj = IInteger32::Probe(obj);
        Int32 value;
        intobj->GetValue(&value);
        (*intarray5)[i] = value;
    }

    AutoPtr<IInterface> obj6;
    values->Get(6, (IInterface**)&obj6);
    AutoPtr<IBoolean> ib = IBoolean::Probe(obj6);
    Boolean res;
    ib->GetValue(&res);

    AutoPtr<IInterface> obj8;
    values->Get(8, (IInterface**)&obj8);

    AutoPtr<IInterface> obj9;
    values->Get(9, (IInterface**)&obj9);

    AutoPtr<ITSTInfo> info;
    CTSTInfo::New(toIntValue, str, IMessageImprint::Probe(obj2),
            bi, IDate::Probe(obj4), intarray5, res, nonce,
            IGeneralName::Probe(obj8), IExtensions::Probe(obj9),
            (ITSTInfo**)&info);
    *object = TO_IINTERFACE(info);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TSTInfo::MyASN1Sequence2::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CTSTInfo* info = (CTSTInfo*)ITSTInfo::Probe(object);

    AutoPtr<IInterface> fromIntValue;
    CASN1Integer::FromIntValue(info->mVersion, (IInterface**)&fromIntValue);
    values->Set(0, fromIntValue);

    AutoPtr<ArrayOf<Int32> > intArray;
    CObjectIdentifier::ToIntArray(info->mPolicy, (ArrayOf<Int32>**)&intArray);
    AutoPtr<IArrayOf> array = CoreUtils::Convert(intArray);
    values->Set(1, TO_IINTERFACE(array));

    values->Set(2, TO_IINTERFACE(info->mMessageImprint));

    AutoPtr<ArrayOf<Byte> > narray;
    info->mSerialNumber->ToByteArray((ArrayOf<Byte>**)&narray);
    AutoPtr<IArrayOf> array2 = CoreUtils::ConvertByteArray(narray);
    values->Set(3, TO_IINTERFACE(array2));

    values->Set(4, TO_IINTERFACE(info->mGenTime));

    AutoPtr<IArrayOf> array3 = CoreUtils::Convert(info->mAccuracy);
    values->Set(5, TO_IINTERFACE(array3));

    AutoPtr<IBoolean> ib = CoreUtils::Convert(info->mOrdering);
    values->Set(6, TO_IINTERFACE(ib));

    if (info->mNonce == NULL) {
        values->Set(7, NULL);
    }
    else {
        AutoPtr<ArrayOf<Byte> > barray;
        info->mNonce->ToByteArray((ArrayOf<Byte>**)&barray);
        AutoPtr<IArrayOf> array = CoreUtils::ConvertByteArray(barray);
        values->Set(7, TO_IINTERFACE(array));
    }

    values->Set(8, TO_IINTERFACE(info->mTsa));

    values->Set(9, TO_IINTERFACE(info->mExtensions));
    return NOERROR;
}

AutoPtr<IASN1Sequence> TSTInfo::initASN1()
{
    AutoPtr<IASN1Integer> integer;
    CASN1Integer::GetInstance((IASN1Integer**)&integer);

    AutoPtr<IASN1Type> instance;
    ASN1Oid::GetInstance((IASN1Type**)&instance);

    AutoPtr<IASN1GeneralizedTime> instance2;
    CASN1GeneralizedTime::GetInstance((IASN1GeneralizedTime**)&instance2);

    AutoPtr<IASN1Type> instance3;
    CASN1Boolean::GetInstance((IASN1Type**)&instance3);

    AutoPtr<IASN1Explicit> type;
    CASN1Explicit::New(0, IASN1Type::Probe(CGeneralName::ASN1), (IASN1Explicit**)&type);

    AutoPtr<IASN1Implicit> type2;
    CASN1Implicit::New(1, CExtensions::ASN1, (IASN1Implicit**)&type2);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(10);
    array->Set(0, IASN1Type::Probe(integer));
    array->Set(1, instance);
    array->Set(2, IASN1Type::Probe(CMessageImprint::ASN1));
    array->Set(3, IASN1Type::Probe(integer));
    array->Set(4, IASN1Type::Probe(instance2));
    array->Set(5, IASN1Type::Probe(ACCURACY));
    array->Set(6, IASN1Type::Probe(instance3));
    array->Set(7, IASN1Type::Probe(integer));
    array->Set(8, IASN1Type::Probe(type));
    array->Set(9, IASN1Type::Probe(type2));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence2();
    tmp->constructor(array);
    tmp->SetOptional(5);
    AutoPtr<IBoolean> ib = CoreUtils::Convert(FALSE);
    tmp->SetDefault(TO_IINTERFACE(ib), 6);
    tmp->SetOptional(7);
    tmp->SetOptional(8);
    tmp->SetOptional(9);
    return IASN1Sequence::Probe(tmp);
}

AutoPtr<IASN1Sequence> TSTInfo::ACCURACY = initACCURACY();

AutoPtr<IASN1Sequence> TSTInfo::ASN1 = initASN1();

CAR_INTERFACE_IMPL(TSTInfo, Object, ITSTInfo)

ECode TSTInfo::ToString(
    /* [out] */ String* pStr)
{
    VALIDATE_NOT_NULL(pStr);

    StringBuilder res;
    res += "-- TSTInfo:";
    res += "\nversion:  ";
    res += mVersion;
    res += "\npolicy:  ";
    res += mPolicy;
    res += "\nmessageImprint:  ";
    res += TO_IINTERFACE(mMessageImprint);
    res += "\nserialNumber:  ";
    res += TO_IINTERFACE(mSerialNumber);
    res += "\ngenTime:  ";
    res += TO_IINTERFACE(mGenTime);
    res += "\naccuracy:  ";
    if (mAccuracy != NULL) {
        res += (*mAccuracy)[0];
        res += " sec, ";
        res += (*mAccuracy)[1];
        res += " millis, ";
        res += (*mAccuracy)[2];
        res += " micros";
    }
    res += "\nordering:  ";
    res += mOrdering;
    res += "\nnonce:  ";
    res += TO_IINTERFACE(mNonce);
    res += "\ntsa:  ";
    res += TO_IINTERFACE(mTsa);
    res += "\nextensions:  ";
    res += TO_IINTERFACE(mExtensions);
    res += "\n-- TSTInfo End\n";
    return res.ToString(pStr);
}

ECode TSTInfo::GetAccuracy(
    /* [out, callee] */ ArrayOf<Int32>** ppAccuracy)
{
    VALIDATE_NOT_NULL(ppAccuracy);

    *ppAccuracy = mAccuracy;
    REFCOUNT_ADD(*ppAccuracy);
    return NOERROR;
}

ECode TSTInfo::GetExtensions(
    /* [out] */ IExtensions** ppExtensions)
{
    VALIDATE_NOT_NULL(ppExtensions);

    *ppExtensions = mExtensions;
    REFCOUNT_ADD(*ppExtensions);
    return NOERROR;
}

ECode TSTInfo::GetGenTime(
    /* [out] */ IDate** ppDate)
{
    VALIDATE_NOT_NULL(ppDate);

    *ppDate = mGenTime;
    REFCOUNT_ADD(*ppDate);
    return NOERROR;
}

ECode TSTInfo::GetMessageImprint(
    /* [out] */ IMessageImprint** ppMessageImprint)
{
    VALIDATE_NOT_NULL(ppMessageImprint);

    *ppMessageImprint = mMessageImprint;
    REFCOUNT_ADD(*ppMessageImprint);
    return NOERROR;
}

ECode TSTInfo::GetNonce(
    /* [out] */ IBigInteger** ppNonce)
{
    VALIDATE_NOT_NULL(ppNonce);

    *ppNonce = mNonce;
    REFCOUNT_ADD(*ppNonce);
    return NOERROR;
}

ECode TSTInfo::GetOrdering(
    /* [out] */ Boolean* pOrdering)
{
    VALIDATE_NOT_NULL(pOrdering);

    *pOrdering = mOrdering;
    return NOERROR;
}

ECode TSTInfo::GetPolicy(
    /* [out] */ String* pPolicy)
{
    VALIDATE_NOT_NULL(pPolicy);

    *pPolicy = mPolicy;
    return NOERROR;
}

ECode TSTInfo::GetSerialNumber(
    /* [out] */ IBigInteger** ppSerialNumber)
{
    VALIDATE_NOT_NULL(ppSerialNumber);

    *ppSerialNumber = mSerialNumber;
    REFCOUNT_ADD(*ppSerialNumber);
    return NOERROR;
}

ECode TSTInfo::GetTsa(
    /* [out] */ IGeneralName** ppTsa)
{
    VALIDATE_NOT_NULL(ppTsa);

    *ppTsa = mTsa;
    REFCOUNT_ADD(*ppTsa);
    return NOERROR;
}

ECode TSTInfo::GetVersion(
    /* [out] */ Int32* pVer)
{
    VALIDATE_NOT_NULL(pVer);

    *pVer = mVersion;
    return NOERROR;
}

ECode TSTInfo::constructor(
    /* [in] */ Int32 version,
    /* [in] */ const String& policy,
    /* [in] */ IMessageImprint* messageImprint,
    /* [in] */ IBigInteger* serialNumber,
    /* [in] */ IDate* genTime,
    /* [in] */ ArrayOf<Int32>* accuracy,
    /* [in] */ Boolean ordering,
    /* [in] */ IBigInteger* nonce,
    /* [in] */ IGeneralName* tsa,
    /* [in] */ IExtensions* extensions)
{
    mVersion = version;
    mPolicy = policy;
    mMessageImprint = messageImprint;
    mSerialNumber = serialNumber;
    mGenTime = genTime;
    mAccuracy = accuracy;
    mOrdering = ordering;
    mNonce = nonce;
    mTsa = tsa;
    mExtensions = extensions;
    return NOERROR;
}

ECode TSTInfo::GetACCURACY(
    /* [out] */ IASN1Sequence** ppAccuracy)
{
    VALIDATE_NOT_NULL(ppAccuracy)

    *ppAccuracy = ACCURACY;
    REFCOUNT_ADD(*ppAccuracy);
    return NOERROR;
}

ECode TSTInfo::SetACCURACY(
    /* [in] */ IASN1Sequence* pAccuracy)
{
    ACCURACY = pAccuracy;
    return NOERROR;
}

ECode TSTInfo::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1)

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode TSTInfo::SetASN1(
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