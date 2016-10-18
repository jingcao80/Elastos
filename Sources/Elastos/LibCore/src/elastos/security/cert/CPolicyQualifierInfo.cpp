
#include "CPolicyQualifierInfo.h"
#include "StringBuilder.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/x509/CPolicyQualifierInfoHelper.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X509::CPolicyQualifierInfoHelper;
using Org::Apache::Harmony::Security::X509::IPolicyQualifierInfoHelper;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPolicyQualifierInfo)
CAR_INTERFACE_IMPL(CPolicyQualifierInfo, Object, IPolicyQualifierInfo)
ECode CPolicyQualifierInfo::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> **encoded)
{
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(mEncoded->GetLength());
    ret->Copy(0, mEncoded, 0, mEncoded->GetLength());
    *encoded = ret;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

ECode CPolicyQualifierInfo::GetPolicyQualifierId(
    /* [out] */ String *id)
{
    VALIDATE_NOT_NULL(id)
    *id = mPolicyQualifierId;
    return NOERROR;
}

ECode CPolicyQualifierInfo::GetPolicyQualifier(
    /* [out, callee] */ ArrayOf<Byte> **qualifier)
{
    VALIDATE_NOT_NULL(qualifier)
    if (!mPolicyQualifier) {
        *qualifier = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(mPolicyQualifier->GetLength());
    ret->Copy(0, mPolicyQualifier, 0, mPolicyQualifier->GetLength());
    *qualifier = ret;
    REFCOUNT_ADD(*qualifier)
    return NOERROR;
}

ECode CPolicyQualifierInfo::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("PolicyQualifierInfo: [\npolicyQualifierId: ");
    sb.Append(mPolicyQualifierId);
    sb.Append("\npolicyQualifier: \n");
    //Apache...Todo late
    //sb.append(Array.toString(policyQualifier, " "));
    sb.Append("]");
    return sb.ToString(str);

}

ECode CPolicyQualifierInfo::constructor(
    /* [in] */ ArrayOf<Byte> *encoded)
{
    if (!encoded) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (encoded->GetLength() == 0) {
        return E_IO_EXCEPTION;
    }
    mEncoded = ArrayOf<Byte>::Alloc(encoded->GetLength());
    mEncoded->Copy(0, encoded, 0, mEncoded->GetLength());

    // DER Decoding:
    AutoPtr<IPolicyQualifierInfoHelper> helper;
    CPolicyQualifierInfoHelper::AcquireSingleton((IPolicyQualifierInfoHelper**)&helper);
    AutoPtr<IASN1Sequence> instance;
    helper->GetASN1((IASN1Sequence**)&instance);
    AutoPtr<IInterface> obj;
    IASN1Type::Probe(instance)->Decode(mEncoded, (IInterface**)&obj);
    AutoPtr<IArrayOf> decoded = IArrayOf::Probe(obj);

    AutoPtr<IInterface> v0;
    decoded->Get(0, (IInterface**)&v0);
    AutoPtr<IArrayOf> ivs = IArrayOf::Probe(v0);
    Int32 len = 0;
    ivs->GetLength(&len);
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> o;
        ivs->Get(i, (IInterface**)&o);
        IInteger32::Probe(o)->GetValue(&((*array)[i]));
    }
    CObjectIdentifier::ToString(array, &mPolicyQualifierId);

    AutoPtr<IInterface> v1;
    decoded->Get(1, (IInterface**)&v1);
    ivs = IArrayOf::Probe(v1);
    ivs->GetLength(&len);
    mPolicyQualifier = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> o;
        ivs->Get(i, (IInterface**)&o);
        IByte::Probe(o)->GetValue(&((*mPolicyQualifier)[i]));
    }

    return NOERROR;
}

}
}
}
