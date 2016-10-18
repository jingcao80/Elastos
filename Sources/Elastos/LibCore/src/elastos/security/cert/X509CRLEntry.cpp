
#include "X509CRLEntry.h"
#include <elastos/utility/Arrays.h>
#include "org/apache/harmony/security/asn1/CASN1OctetString.h"
#include "org/apache/harmony/security/x509/CReasonCode.h"

using Elastos::Core::IByte;
using Elastos::Utility::Arrays;
using Org::Apache::Harmony::Security::Asn1::CASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X509::CReasonCode;
using Org::Apache::Harmony::Security::X509::IReasonCode;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL_2(X509CRLEntry, Object, IX509CRLEntry, IX509Extension)

X509CRLEntry::X509CRLEntry()
{}

ECode X509CRLEntry::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    if (reinterpret_cast<X509CRLEntry*>(IX509CRLEntry::Probe(other)) == this) {
        *isEqual = TRUE;
        return NOERROR;
    }
    if (other == NULL || !IX509CRLEntry::Probe(other)) {
        *isEqual = FALSE;
        return NOERROR;
    }
    AutoPtr<X509CRLEntry> obj = reinterpret_cast<X509CRLEntry*>(IX509CRLEntry::Probe(other));
    AutoPtr<ArrayOf<Byte> > encode, oEncode;
    ECode ec = GetEncoded((ArrayOf<Byte>**)&encode);
    FAIL_GOTO(ec, FAIL_PROCESS)
    ec = obj->GetEncoded((ArrayOf<Byte>**)&oEncode);
    FAIL_GOTO(ec, FAIL_PROCESS)
    *isEqual = Arrays::Equals(encode, oEncode);
    return NOERROR;

FAIL_PROCESS:
    if (ec == (ECode)E_CRL_EXCEPTION) {
        *isEqual = FALSE;
        return NOERROR;
    }
    return ec;
}

ECode X509CRLEntry::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 res = 0;
    AutoPtr<ArrayOf<Byte> > array;
    ECode ec = GetEncoded((ArrayOf<Byte>**)&array);
    FAIL_GOTO(ec, FAIL_PROCESS)
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        res += (*array)[i] & 0xFF;
    }
FAIL_PROCESS:
    if (ec == (ECode)E_CRL_EXCEPTION) {
        ;
    }
    else
    {
        FAIL_RETURN(ec)
    }
    *hashCode = res;
    return NOERROR;
}

ECode X509CRLEntry::GetCertificateIssuer(
    /* [out] */ IX500Principal** issuer)
{
    return NOERROR;
}

ECode X509CRLEntry::GetRevocationReason(
    /* [out] */ CRLReason* reason)
{
    VALIDATE_NOT_NULL(reason);
    AutoPtr<ArrayOf<Byte> > reasonBytes;
    GetExtensionValue(String("2.5.29.21"), (ArrayOf<Byte>**)&reasonBytes);
    if (reasonBytes == NULL) {
        *reason = CRLReason_NULL;
        return NOERROR;
    }

    // // try {
    Int32 count = 0;
    AutoPtr<IASN1OctetString> asn;
    AutoPtr<IInterface> obj;
    AutoPtr<ArrayOf<Byte> > rawBytes;
    AutoPtr<IReasonCode> rc;
    FAIL_GOTO(CASN1OctetString::GetInstance((IASN1OctetString**)&asn), fail);
    FAIL_GOTO(IASN1Type::Probe(asn)->Decode(reasonBytes, (IInterface**)&obj), fail);
    assert(IArrayOf::Probe(obj) != NULL);
    IArrayOf::Probe(obj)->GetLength(&count);
    rawBytes = ArrayOf<Byte>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> item;
        IArrayOf::Probe(obj)->Get(i, (IInterface**)&item);
        assert(IByte::Probe(item) != NULL);
        IByte::Probe(item)->GetValue(&((*rawBytes)[i]));
    }

    FAIL_GOTO(CReasonCode::New(rawBytes, (IReasonCode**)&rc), fail);
    *reason = ((CReasonCode*)rc.Get())->GetReason();
    return NOERROR;
    // } catch (IOException e) {
    //     *reason = CRLReason_NULL;
    //     return NOERROR;
    // }

fail:
    *reason = CRLReason_NULL;
    return NOERROR;
}

} // end Cert
} // end Security
} // end Elastos