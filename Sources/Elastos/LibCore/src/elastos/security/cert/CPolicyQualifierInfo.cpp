
#include "CPolicyQualifierInfo.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPolicyQualifierInfo);
CAR_INTERFACE_IMPL(CPolicyQualifierInfo, Object, IPolicyQualifierInfo);
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
    sb.AppendString(mPolicyQualifierId);
    sb.AppendCStr("\npolicyQualifier: \n");
    //Apache...Todo late
    //sb.append(Array.toString(policyQualifier, " "));
    sb.AppendCStr("]");
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
    //Apache...Todo late
    /*
    Object[] decoded = (Object[]) org.apache.harmony.security.x509.PolicyQualifierInfo.ASN1
                .decode(this.encoded);
    policyQualifierId = ObjectIdentifier.toString((int[]) decoded[0]);
    policyQualifier = (byte[]) decoded[1];
    */
    return E_NOT_IMPLEMENTED;
}

}
}
}

