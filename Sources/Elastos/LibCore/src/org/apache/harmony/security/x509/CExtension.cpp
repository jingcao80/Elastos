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

#include "org/apache/harmony/security/x509/CExtension.h"
#include "org/apache/harmony/security/x509/CKeyUsage.h"
#include "org/apache/harmony/security/x509/CBasicConstraints.h"
#include "org/apache/harmony/security/x509/CAlternativeName.h"
#include "org/apache/harmony/security/x509/CCertificatePolicies.h"
#include "org/apache/harmony/security/x509/CAuthorityKeyIdentifier.h"
#include "org/apache/harmony/security/x509/CPolicyConstraints.h"
#include "org/apache/harmony/security/x509/CExtendedKeyUsage.h"
#include "org/apache/harmony/security/x509/CInhibitAnyPolicy.h"
#include "org/apache/harmony/security/x509/CCertificateIssuer.h"
#include "org/apache/harmony/security/x509/CReasonCode.h"
#include "org/apache/harmony/security/x509/CInvalidityDate.h"
#include "org/apache/harmony/security/x509/CCRLNumber.h"
#include "org/apache/harmony/security/x509/CCRLDistributionPoints.h"
#include "org/apache/harmony/security/x509/CIssuingDistributionPoint.h"
#include "org/apache/harmony/security/x509/CSubjectKeyIdentifier.h"
#include "org/apache/harmony/security/x509/CNameConstraints.h"
#include "org/apache/harmony/security/x509/CInfoAccessSyntax.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/asn1/CASN1OctetString.h"
#include "org/apache/harmony/security/asn1/ASN1Oid.h"
#include "org/apache/harmony/security/asn1/CASN1Boolean.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::ASN1Oid;
using Org::Apache::Harmony::Security::Asn1::CASN1Boolean;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::CASN1OctetString;
using Org::Apache::Harmony::Security::X509::CKeyUsage;
using Org::Apache::Harmony::Security::X509::CBasicConstraints;
using Org::Apache::Harmony::Security::X509::CAlternativeName;
using Org::Apache::Harmony::Security::X509::CCertificatePolicies;
using Org::Apache::Harmony::Security::X509::CAuthorityKeyIdentifier;
using Org::Apache::Harmony::Security::X509::CPolicyConstraints;
using Org::Apache::Harmony::Security::X509::CExtendedKeyUsage;
using Org::Apache::Harmony::Security::X509::CInhibitAnyPolicy;
using Org::Apache::Harmony::Security::X509::CCertificateIssuer;
using Org::Apache::Harmony::Security::X509::CReasonCode;
using Org::Apache::Harmony::Security::X509::CInvalidityDate;
using Org::Apache::Harmony::Security::X509::CCRLNumber;
using Org::Apache::Harmony::Security::X509::CCRLDistributionPoints;
using Org::Apache::Harmony::Security::X509::CIssuingDistributionPoint;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Security::Cert::EIID_IExtension;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CExtension::MyASN1OctetString::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    // first - decoded octet string,
    // second - raw encoding of octet string

    AutoPtr<IInterface> obj;
    ASN1OctetString::GetDecodedObject(bis, (IInterface**)&obj);

    AutoPtr<ArrayOf<Byte> > earray;
    bis->GetEncoded((ArrayOf<Byte>**)&earray);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(2);
    array->Set(0, obj);
    array->Set(1, TO_IINTERFACE(CoreUtils::ConvertByteArray(earray)));

    AutoPtr<IArrayOf> tmp = CoreUtils::Convert(array, EIID_IInterface);
    *object = tmp;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CExtension::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);
    AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(obj0);
    Int32 length;
    arrayof->GetLength(&length);
    AutoPtr<ArrayOf<Int32> > oid = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> value;
        arrayof->Get(i, (IInterface**)&value);
        AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
        Int32 num;
        intvalue->GetValue(&num);
        (*oid)[i] = num;
    }

    AutoPtr<IInterface> obj2;
    values->Get(2, (IInterface**)&obj2);
    AutoPtr<IArrayOf> arrayof2 = IArrayOf::Probe(obj2);

    AutoPtr<IInterface> obj20;
    arrayof2->Get(0, (IInterface**)&obj20);
    AutoPtr<IArrayOf> arrayof20 = IArrayOf::Probe(obj20);
    Int32 length20;
    arrayof20->GetLength(&length20);
    AutoPtr<ArrayOf<Byte> > extnValue = ArrayOf<Byte>::Alloc(length20);
    for (Int32 i = 0; i < length20; i++) {
        AutoPtr<IInterface> value;
        arrayof20->Get(i, (IInterface**)&value);
        AutoPtr<IByte> b = IByte::Probe(value);
        Byte num;
        b->GetValue(&num);
        (*extnValue)[i] = num;
    }

    AutoPtr<IInterface> obj21;
    arrayof2->Get(1, (IInterface**)&obj21);
    AutoPtr<IArrayOf> arrayof21 = IArrayOf::Probe(obj21);
    Int32 length21;
    arrayof21->GetLength(&length21);
    AutoPtr<ArrayOf<Byte> > rawExtnValue = ArrayOf<Byte>::Alloc(length21);
    for (Int32 i = 0; i < length21; i++) {
        AutoPtr<IInterface> value;
        arrayof21->Get(i, (IInterface**)&value);
        AutoPtr<IByte> b = IByte::Probe(value);
        Byte num;
        b->GetValue(&num);
        (*rawExtnValue)[i] = num;
    }

    AutoPtr<IExtensionValue> decodedExtValue;
    // decode Key Usage and Basic Constraints extension values
    if (Arrays::Equals(oid, KEY_USAGE)) {
        AutoPtr<IKeyUsage> usage;
        CKeyUsage::New(extnValue, (IKeyUsage**)&usage);
        decodedExtValue = IExtensionValue::Probe(usage);
    }
    else if (Arrays::Equals(oid, BASIC_CONSTRAINTS)) {
        AutoPtr<IBasicConstraints> constraints;
        CBasicConstraints::New(extnValue, (IBasicConstraints**)&constraints);
        decodedExtValue = IExtensionValue::Probe(constraints);
    }

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);
    AutoPtr<IBoolean> objBool = IBoolean::Probe(obj1);
    Boolean res;
    objBool->GetValue(&res);

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);
    AutoPtr<IExtension> tmp;
    CExtension::New(oid, res,
            extnValue, rawExtnValue, array, decodedExtValue, (IExtension**)&tmp);
    *object = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CExtension::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CExtension* ext = (CExtension*)IExtension::Probe(object);
    values->Set(0, TO_IINTERFACE(CoreUtils::Convert(ext->mExtnID)));
    values->Set(1, TO_IINTERFACE(CoreUtils::Convert((ext->mCritical) ? TRUE : FALSE)));
    values->Set(2,  TO_IINTERFACE(CoreUtils::ConvertByteArray(ext->mExtnValue)));
    return NOERROR;
}

AutoPtr<IASN1Sequence> CExtension::initASN1()
{
    AutoPtr<IASN1Type> instance1;
    ASN1Oid::GetInstance((IASN1Type**)&instance1);
    AutoPtr<IASN1Type> instance2;
    CASN1Boolean::GetInstance((IASN1Type**)&instance2);

    AutoPtr<ASN1OctetString> str = new MyASN1OctetString();
    str->constructor();

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(3);
    array->Set(0, IASN1Type::Probe(instance1));
    array->Set(1, IASN1Type::Probe(instance2));
    array->Set(2, IASN1Type::Probe(str));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    tmp->SetDefault(TO_IINTERFACE(CoreUtils::Convert(FALSE)), 1);
    return IASN1Sequence::Probe(tmp);
}

INIT_PROI_4 AutoPtr<IASN1Sequence> CExtension::ASN1 = initASN1();

static AutoPtr<ArrayOf<Int32> > initSUBJ_DIRECTORY_ATTRS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 9;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::SUBJ_DIRECTORY_ATTRS = initSUBJ_DIRECTORY_ATTRS();

static AutoPtr<ArrayOf<Int32> > initSUBJ_KEY_ID()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 14;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::SUBJ_KEY_ID = initSUBJ_KEY_ID();

static AutoPtr<ArrayOf<Int32> > initKEY_USAGED()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 15;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::KEY_USAGE = initKEY_USAGED();

static AutoPtr<ArrayOf<Int32> > initPRIVATE_KEY_USAGE_PERIOD()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 16;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::PRIVATE_KEY_USAGE_PERIOD = initPRIVATE_KEY_USAGE_PERIOD();

static AutoPtr<ArrayOf<Int32> > initSUBJECT_ALT_NAME()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 17;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::SUBJECT_ALT_NAME = initSUBJECT_ALT_NAME();

static AutoPtr<ArrayOf<Int32> > initISSUER_ALTERNATIVE_NAME()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 18;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::ISSUER_ALTERNATIVE_NAME = initISSUER_ALTERNATIVE_NAME();

static AutoPtr<ArrayOf<Int32> > initBASIC_CONSTRAINTS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 19;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::BASIC_CONSTRAINTS = initBASIC_CONSTRAINTS();

static AutoPtr<ArrayOf<Int32> > initNAME_CONSTRAINTS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 30;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::NAME_CONSTRAINTS = initNAME_CONSTRAINTS();

static AutoPtr<ArrayOf<Int32> > initCRL_DISTR_POINTS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 31;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::CRL_DISTR_POINTS = initCRL_DISTR_POINTS();

static AutoPtr<ArrayOf<Int32> > initCERTIFICATE_POLICIES()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 32;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::CERTIFICATE_POLICIES = initCERTIFICATE_POLICIES();

static AutoPtr<ArrayOf<Int32> > initPOLICY_MAPPINGS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 33;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::POLICY_MAPPINGS = initPOLICY_MAPPINGS();

static AutoPtr<ArrayOf<Int32> > initAUTH_KEY_ID()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 35;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::AUTH_KEY_ID = initAUTH_KEY_ID();

static AutoPtr<ArrayOf<Int32> > initPOLICY_CONSTRAINTS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 36;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::POLICY_CONSTRAINTS = initPOLICY_CONSTRAINTS();

static AutoPtr<ArrayOf<Int32> > initEXTENDED_KEY_USAGE()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 37;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::EXTENDED_KEY_USAGE = initEXTENDED_KEY_USAGE();

static AutoPtr<ArrayOf<Int32> > initFRESHEST_CRL()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 46;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::FRESHEST_CRL = initFRESHEST_CRL();

static AutoPtr<ArrayOf<Int32> > initINHIBIT_ANY_POLICY()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;;
    (*array)[1] = 5;;
    (*array)[2] = 29;;
    (*array)[3] = 54;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::INHIBIT_ANY_POLICY = initINHIBIT_ANY_POLICY();

static AutoPtr<ArrayOf<Int32> > initAUTHORITY_INFO_ACCESS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(9);
    (*array)[0] = 1;
    (*array)[1] = 3;
    (*array)[2] = 6;
    (*array)[3] = 1;
    (*array)[4] = 5;
    (*array)[5] = 5;
    (*array)[6] = 7;
    (*array)[7] = 1;
    (*array)[8] = 1;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::AUTHORITY_INFO_ACCESS = initAUTHORITY_INFO_ACCESS();

static AutoPtr<ArrayOf<Int32> > initSUBJECT_INFO_ACCESS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(9);
    (*array)[0] = 1;
    (*array)[1] = 3;
    (*array)[2] = 6;
    (*array)[3] = 1;
    (*array)[4] = 5;
    (*array)[5] = 5;
    (*array)[6] = 7;
    (*array)[7] = 1;
    (*array)[8] = 11;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::SUBJECT_INFO_ACCESS = initSUBJECT_INFO_ACCESS();

static AutoPtr<ArrayOf<Int32> > initISSUING_DISTR_POINT()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 28;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::ISSUING_DISTR_POINT = initISSUING_DISTR_POINT();

static AutoPtr<ArrayOf<Int32> > initCRL_NUMBER()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 20;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::CRL_NUMBER = initCRL_NUMBER();

static AutoPtr<ArrayOf<Int32> > initCERTIFICATE_ISSUER()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 29;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::CERTIFICATE_ISSUER = initCERTIFICATE_ISSUER();

static AutoPtr<ArrayOf<Int32> > initINVALIDITY_DATE()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 24;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::INVALIDITY_DATE = initINVALIDITY_DATE();

static AutoPtr<ArrayOf<Int32> > initREASON_CODE()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 21;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::REASON_CODE = initREASON_CODE();

static AutoPtr<ArrayOf<Int32> > initISSUING_DISTR_POINTS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = 2;
    (*array)[1] = 5;
    (*array)[2] = 29;
    (*array)[3] = 28;
    return array;
}
AutoPtr<ArrayOf<Int32> > CExtension::ISSUING_DISTR_POINTS = initISSUING_DISTR_POINTS();

CAR_OBJECT_IMPL(CExtension)

CAR_INTERFACE_IMPL_2(CExtension, Object, IX509Extension, IExtension)

ECode CExtension::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);

    if (mExtnID_str.IsNull()) {
        CObjectIdentifier::ToString(mExtnID, &mExtnID_str);
    }
    *id = mExtnID_str;
    return NOERROR;
}

ECode CExtension::IsCritical(
    /* [out] */ Boolean* isCritical)
{
    VALIDATE_NOT_NULL(isCritical);

    *isCritical = mCritical;
    return NOERROR;
}

ECode CExtension::GetValue(
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value);

    *value = mExtnValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CExtension::Encode(
    /* [in] */ IOutputStream* outValue)
{
    AutoPtr<ArrayOf<Byte> > array;
    GetEncoded((ArrayOf<Byte>**)&array);
    return outValue->Write(array);
}

ECode CExtension::GetRawExtnValue(
    /* [out, callee] */ ArrayOf<Byte>** ppRawExtnValue)
{
    VALIDATE_NOT_NULL(ppRawExtnValue);

    if (mRawExtnValue == NULL) {
        AutoPtr<IASN1OctetString> instance;
        CASN1OctetString::GetInstance((IASN1OctetString**)&instance);
        IASN1Type::Probe(instance)->Encode(TO_IINTERFACE(CoreUtils::ConvertByteArray(mExtnValue)),
                (ArrayOf<Byte>**)&mRawExtnValue);
    }
    *ppRawExtnValue = mRawExtnValue;
    REFCOUNT_ADD(*ppRawExtnValue);
    return NOERROR;
}

ECode CExtension::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        IASN1Type::Probe(CExtension::ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CExtension::Equals(
    /* [in] */ IInterface* ext,
    /* [out] */ Boolean* pResult)
{
    VALIDATE_NOT_NULL(pResult);

    if (IExtension::Probe(ext) == NULL) {
        *pResult = FALSE;
        return NOERROR;
    }
    CExtension* extension = (CExtension*)IExtension::Probe(ext);
    *pResult = Arrays::Equals(mExtnID, extension->mExtnID)
        && (mCritical == extension->mCritical)
        && Arrays::Equals(mExtnValue, extension->mExtnValue);
    return NOERROR;
}

ECode CExtension::GetHashCode(
    /* [out] */ Int32* pHashCode)
{
    VALIDATE_NOT_NULL(pHashCode);

    *pHashCode = (Arrays::GetHashCode(mExtnID) * 37 +
            (mCritical ? 1 : 0)) * 37 + Arrays::GetHashCode(mExtnValue);
    return NOERROR;
}

ECode CExtension::GetDecodedExtensionValue(
    /* [out] */ IExtensionValue** ppExtensionValue)
{
    VALIDATE_NOT_NULL(ppExtensionValue);

    if (!mValueDecoded) {
        DecodeExtensionValue();
    }
    *ppExtensionValue = mExtnValueObject;
    REFCOUNT_ADD(*ppExtensionValue);
    return NOERROR;
}

ECode CExtension::GetKeyUsageValue(
    /* [out] */ IKeyUsage** ppUsageValue)
{
    VALIDATE_NOT_NULL(ppUsageValue);

    if (!mValueDecoded) {
        //try {
        DecodeExtensionValue();
        //} catch (IOException ignored) {
        //}
    }
    if (IKeyUsage::Probe(mExtnValueObject) != NULL) {
        *ppUsageValue = IKeyUsage::Probe(mExtnValueObject);
        REFCOUNT_ADD(*ppUsageValue);
        return NOERROR;
    }
    else {
        *ppUsageValue = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode CExtension::GetBasicConstraintsValue(
    /* [out] */ IBasicConstraints** ppBasicConstrainsValue)
{
    VALIDATE_NOT_NULL(ppBasicConstrainsValue);
    *ppBasicConstrainsValue = NULL;

    if (!mValueDecoded) {
        //try {
        DecodeExtensionValue();
        //} catch (IOException ignored) {
        //}
    }
    if (IBasicConstraints::Probe(mExtnValueObject) != NULL) {
        *ppBasicConstrainsValue = IBasicConstraints::Probe(mExtnValueObject);
        REFCOUNT_ADD(*ppBasicConstrainsValue);
        return NOERROR;
    }
    else {
        *ppBasicConstrainsValue = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode CExtension::DecodeExtensionValue()
{
    if (mValueDecoded) {
        return NOERROR;
    }

    if (Arrays::Equals(mExtnID, SUBJ_KEY_ID)) {
        AutoPtr<ISubjectKeyIdentifier> tmp;
        CSubjectKeyIdentifier::Decode(mExtnValue, (ISubjectKeyIdentifier**)&tmp);
        mExtnValueObject = IExtensionValue::Probe(tmp);
    }
    else if (Arrays::Equals(mExtnID, KEY_USAGE)) {
        AutoPtr<IKeyUsage> usage;
        CKeyUsage::New(mExtnValue, (IKeyUsage**)&usage);
        mExtnValueObject = IExtensionValue::Probe(usage);
    }
    else if (Arrays::Equals(mExtnID, SUBJECT_ALT_NAME)) {
        AutoPtr<IAlternativeName> name;
        CAlternativeName::New(IAlternativeName::SUBJECT, mExtnValue, (IAlternativeName**)&name);
        mExtnValueObject = IExtensionValue::Probe(name);
    }
    else if (Arrays::Equals(mExtnID, ISSUER_ALTERNATIVE_NAME)) {
        AutoPtr<IAlternativeName> name;
        CAlternativeName::New(IAlternativeName::SUBJECT, mExtnValue, (IAlternativeName**)&name);
        mExtnValueObject = IExtensionValue::Probe(name);
    }
    else if (Arrays::Equals(mExtnID, BASIC_CONSTRAINTS)) {
        AutoPtr<IBasicConstraints> constraints;
        CBasicConstraints::New(mExtnValue, (IBasicConstraints**)&constraints);
        mExtnValueObject = IExtensionValue::Probe(constraints);
    }
    else if (Arrays::Equals(mExtnID, NAME_CONSTRAINTS)) {
        AutoPtr<INameConstraints> tmp;
        CNameConstraints::Decode(mExtnValue, (INameConstraints**)&tmp);
        mExtnValueObject = IExtensionValue::Probe(tmp);
    }
    else if (Arrays::Equals(mExtnID, CERTIFICATE_POLICIES)) {
        AutoPtr<ICertificatePolicies> tmp;
        CCertificatePolicies::Decode(mExtnValue, (ICertificatePolicies**)&tmp);
        mExtnValueObject = IExtensionValue::Probe(tmp);
    }
    else if (Arrays::Equals(mExtnID, AUTH_KEY_ID)) {
        AutoPtr<IAuthorityKeyIdentifier> tmp;
        CAuthorityKeyIdentifier::Decode(mExtnValue, (IAuthorityKeyIdentifier**)&tmp);
        mExtnValueObject = IExtensionValue::Probe(tmp);
    }
    else if (Arrays::Equals(mExtnID, POLICY_CONSTRAINTS)) {
        AutoPtr<IPolicyConstraints> constraints;
        CPolicyConstraints::New(mExtnValue, (IPolicyConstraints**)&constraints);
        mExtnValueObject = IExtensionValue::Probe(constraints);
    }
    else if (Arrays::Equals(mExtnID, EXTENDED_KEY_USAGE)) {
        AutoPtr<IExtendedKeyUsage> keyUsage;
        CExtendedKeyUsage::New(mExtnValue, (IExtendedKeyUsage**)&keyUsage);
        mExtnValueObject = IExtensionValue::Probe(keyUsage);
    }
    else if (Arrays::Equals(mExtnID, INHIBIT_ANY_POLICY)) {
        AutoPtr<IInhibitAnyPolicy> policy;
        CInhibitAnyPolicy::New(mExtnValue, (IInhibitAnyPolicy**)&policy);
        mExtnValueObject = IExtensionValue::Probe(policy);
    }
    else if (Arrays::Equals(mExtnID, CERTIFICATE_ISSUER)) {
        AutoPtr<ICertificateIssuer> issuer;
        CCertificateIssuer::New(mExtnValue, (ICertificateIssuer**)&issuer);
        mExtnValueObject = IExtensionValue::Probe(issuer);
    }
    else if (Arrays::Equals(mExtnID, CRL_DISTR_POINTS)) {
        AutoPtr<ICRLDistributionPoints> tmp;
        CCRLDistributionPoints::Decode(mExtnValue, (ICRLDistributionPoints**)&tmp);
        mExtnValueObject = IExtensionValue::Probe(tmp);
    }
    else if (Arrays::Equals(mExtnID, CERTIFICATE_ISSUER)) {
        AutoPtr<IReasonCode> code;
        CReasonCode::New(mExtnValue, (IReasonCode**)&code);
        mExtnValueObject = IExtensionValue::Probe(code);
    }
    else if (Arrays::Equals(mExtnID, INVALIDITY_DATE)) {
        AutoPtr<IInvalidityDate> date;
        CInvalidityDate::New(mExtnValue, (IInvalidityDate**)&date);
        mExtnValueObject = IExtensionValue::Probe(date);
    }
    else if (Arrays::Equals(mExtnID, REASON_CODE)) {
        AutoPtr<IReasonCode> code;
        CReasonCode::New(mExtnValue, (IReasonCode**)&code);
        mExtnValueObject = IExtensionValue::Probe(code);
    }
    else if (Arrays::Equals(mExtnID, CRL_NUMBER)) {
        AutoPtr<ICRLNumber> number;
        CCRLNumber::New(mExtnValue, (ICRLNumber**)&number);
        mExtnValueObject = IExtensionValue::Probe(number);
    }
    else if (Arrays::Equals(mExtnID, ISSUING_DISTR_POINTS)) {
        AutoPtr<IIssuingDistributionPoint> tmp;
        CIssuingDistributionPoint::Decode(mExtnValue, (IIssuingDistributionPoint**)&tmp);
        mExtnValueObject = IExtensionValue::Probe(tmp);
    }
    else if (Arrays::Equals(mExtnID, AUTHORITY_INFO_ACCESS)) {
        AutoPtr<IInfoAccessSyntax> tmp;
        CInfoAccessSyntax::Decode(mExtnValue, (IInfoAccessSyntax**)&tmp);
        mExtnValueObject = IExtensionValue::Probe(tmp);
    }
    else if (Arrays::Equals(mExtnID, SUBJECT_INFO_ACCESS)) {
        AutoPtr<IInfoAccessSyntax> tmp;
        CInfoAccessSyntax::Decode(mExtnValue, (IInfoAccessSyntax**)&tmp);
        mExtnValueObject = IExtensionValue::Probe(tmp);
    }
    mValueDecoded = TRUE;
    return NOERROR;
}

ECode CExtension::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(String("OID: "));
    String id;
    GetId(&id);
    sb->Append(id);
    sb->Append(String(", Critical: "));
    sb->Append(mCritical);
    sb->Append('\n');
    if (!mValueDecoded) {
        //try {
        DecodeExtensionValue();
        //} catch (IOException ignored) {
        //}
    }
    if (mExtnValueObject != NULL) {
        mExtnValueObject->DumpValue(sb, prefix);
        return NOERROR;
    }
    // else: dump unparsed hex representation
    sb->Append(prefix);
    if (Arrays::Equals(mExtnID, SUBJ_DIRECTORY_ATTRS)) {
        sb->Append(String("Subject Directory Attributes Extension"));
    }
    else if (Arrays::Equals(mExtnID, SUBJ_KEY_ID)) {
        sb->Append(String("Subject Key Identifier Extension"));
    }
    else if (Arrays::Equals(mExtnID, KEY_USAGE)) {
        sb->Append(String("Key Usage Extension"));
    }
    else if (Arrays::Equals(mExtnID, PRIVATE_KEY_USAGE_PERIOD)) {
        sb->Append(String("Private Key Usage Period Extension"));
    }
    else if (Arrays::Equals(mExtnID, SUBJECT_ALT_NAME)) {
        sb->Append(String("Subject Alternative Name Extension"));
    }
    else if (Arrays::Equals(mExtnID, ISSUER_ALTERNATIVE_NAME)) {
        sb->Append(String("Issuer Alternative Name Extension"));
    }
    else if (Arrays::Equals(mExtnID, BASIC_CONSTRAINTS)) {
        sb->Append(String("Basic Constraints Extension"));
    }
    else if (Arrays::Equals(mExtnID, NAME_CONSTRAINTS)) {
        sb->Append(String("Name Constraints Extension"));
    }
    else if (Arrays::Equals(mExtnID, CRL_DISTR_POINTS)) {
        sb->Append(String("CRL Distribution Points Extension"));
    }
    else if (Arrays::Equals(mExtnID, CERTIFICATE_POLICIES)) {
        sb->Append(String("Certificate Policies Extension"));
    }
    else if (Arrays::Equals(mExtnID, POLICY_MAPPINGS)) {
        sb->Append(String("Policy Mappings Extension"));
    }
    else if (Arrays::Equals(mExtnID, AUTH_KEY_ID)) {
        sb->Append(String("Authority Key Identifier Extension"));
    }
    else if (Arrays::Equals(mExtnID, POLICY_CONSTRAINTS)) {
        sb->Append(String("Policy Constraints Extension"));
    }
    else if (Arrays::Equals(mExtnID, EXTENDED_KEY_USAGE)) {
        sb->Append(String("Extended Key Usage Extension"));
    }
    else if (Arrays::Equals(mExtnID, INHIBIT_ANY_POLICY)) {
        sb->Append(String("Inhibit Any-Policy Extension"));
    }
    else if (Arrays::Equals(mExtnID, AUTHORITY_INFO_ACCESS)) {
        sb->Append(String("Authority Information Access Extension"));
    }
    else if (Arrays::Equals(mExtnID, SUBJECT_INFO_ACCESS)) {
        sb->Append(String("Subject Information Access Extension"));
    }
    else if (Arrays::Equals(mExtnID, INVALIDITY_DATE)) {
        sb->Append(String("Invalidity Date Extension"));
    }
    else if (Arrays::Equals(mExtnID, CRL_NUMBER)) {
        sb->Append(String("CRL Number Extension"));
    }
    else if (Arrays::Equals(mExtnID, REASON_CODE)) {
        sb->Append(String("Reason Code Extension"));
    }
    else {
        sb->Append(String("Unknown Extension"));
    }
    sb->Append('\n');
    sb->Append(prefix);
    sb->Append(String("Unparsed Extension Value:\n"));
    assert(0);
    //sb->Append(Array.toString(extnValue, prefix));
    return NOERROR;
}

CExtension::CExtension()
    : mExtnID_str(NULL)
    , mCritical(FALSE)
    , mValueDecoded(FALSE)
{
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ Boolean critical,
    /* [in] */ IExtensionValue* extnValueObject)
{
    mExtnID_str = extnID;
    CObjectIdentifier::ToIntArray(extnID, (ArrayOf<Int32>**)&mExtnID);
    mCritical = critical;
    mExtnValueObject = extnValueObject;
    mValueDecoded = TRUE;
    extnValueObject->GetEncoded((ArrayOf<Byte>**)&mExtnValue);
    return NOERROR;
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ Boolean critical,
    /* [in] */ ArrayOf<Byte>* extnValue)
{
    mExtnID_str = extnID;
    CObjectIdentifier::ToIntArray(extnID, (ArrayOf<Int32>**)&mExtnID);
    mCritical = critical;
    mExtnValue = extnValue;
    return NOERROR;
}

ECode CExtension::constructor(
    /* [in] */ ArrayOf<Int32>* extnID,
    /* [in] */ Boolean critical,
    /* [in] */ ArrayOf<Byte>* extnValue)
{
    mExtnID = extnID;
    mCritical = critical;
    mExtnValue = extnValue;
    return NOERROR;
}

ECode CExtension::constructor(
    /* [in] */ const String& extnID,
    /* [in] */ ArrayOf<Byte>* extnValue)
{
    return constructor(extnID, NON_CRITICAL, extnValue);
}

ECode CExtension::constructor(
    /* [in] */ ArrayOf<Int32>* extnID,
    /* [in] */ ArrayOf<Byte>* extnValue)
{
    return constructor(extnID, NON_CRITICAL, extnValue);
}

ECode CExtension::constructor(
    /* [in] */ ArrayOf<Int32>* extnID,
    /* [in] */ Boolean critical,
    /* [in] */ ArrayOf<Byte>* extnValue,
    /* [in] */ ArrayOf<Byte>* rawExtnValue,
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [in] */ IExtensionValue* decodedExtValue)
{
    constructor(extnID, critical, extnValue);
    mRawExtnValue = rawExtnValue;
    mEncoding = encoding;
    mExtnValueObject = decodedExtValue;
    mValueDecoded = (decodedExtValue != NULL);
    return NOERROR;
}

ECode CExtension::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CExtension::SetASN1(
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