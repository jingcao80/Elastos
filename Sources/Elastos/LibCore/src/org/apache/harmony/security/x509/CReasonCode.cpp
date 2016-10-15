
#include "org/apache/harmony/security/x509/CReasonCode.h"
#include "CoreUtils.h"
#include "CASN1Enumerated.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Security::Cert::CRLReason_NULL;
using Org::Apache::Harmony::Security::Asn1::CASN1Enumerated;
using Org::Apache::Harmony::Security::Asn1::IASN1Enumerated;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

static AutoPtr<IASN1Type> InitValue()
{
    AutoPtr<IASN1Enumerated> ae;
    CASN1Enumerated::GetInstance((IASN1Enumerated**)&ae);
    return IASN1Type::Probe(ae);
}

AutoPtr<IASN1Type> CReasonCode::ASN1 = InitValue();
CAR_OBJECT_IMPL(CReasonCode)
CAR_INTERFACE_IMPL_2(CReasonCode, Object, IReasonCode, IExtensionValue)
CReasonCode::CReasonCode()
    : mCode(0)
{}

ECode CReasonCode::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mEncoding == NULL) {
        AutoPtr<ArrayOf<Byte> > bs = ArrayOf<Byte>::Alloc(1);
        (*bs)[0] = mCode;
        ASN1->Encode(CoreUtils::ConvertByteArray(bs), (ArrayOf<Byte>**)&mEncoding);
    }
    *result = mEncoding;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CReasonCode::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    assert(0 && "TODO");
    // sb.append(prefix).append("Reason Code: [ ");
    // switch (code) {
    // case UNSPECIFIED:
    //     sb.append("unspecified");
    //     break;
    // case KEY_COMPROMISE:
    //     sb.append("keyCompromise");
    //     break;
    // case CA_COMPROMISE:
    //     sb.append("cACompromise");
    //     break;
    // case AFFILIATION_CHANGED:
    //     sb.append("affiliationChanged");
    //     break;
    // case SUPERSEDED:
    //     sb.append("superseded");
    //     break;
    // case CESSATION_OF_OPERATION:
    //     sb.append("cessationOfOperation");
    //     break;
    // case CERTIFICATE_HOLD:
    //     sb.append("certificateHold");
    //     break;
    // case REMOVE_FROM_CRL:
    //     sb.append("removeFromCRL");
    //     break;
    // case PRIVILEGE_WITHDRAWN:
    //     sb.append("privilegeWithdrawn");
    //     break;
    // case AA_COMPROMISE:
    //     sb.append("aACompromise");
    //     break;
    // }
    // sb.append(" ]\n");
    return NOERROR;
}

ECode CReasonCode::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);
    AutoPtr<IInterface> o;
    ASN1->Decode(encoding, (IInterface**)&o);
    AutoPtr<IInterface> bo;
    IArrayOf::Probe(o)->Get(0, (IInterface**)&bo);
    IByte::Probe(bo)->GetValue(&mCode);
    return NOERROR;
}

CRLReason CReasonCode::GetReason()
{
    // CRLReason[] values = CRLReason.values();
    if (mCode < 0 || mCode > /*values.length*/12) {
        return CRLReason_NULL;
    }

    //TODO: reutn values[mCode] ???????????????, we add the item:CRLReason_NULL
    return 1 + mCode;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org