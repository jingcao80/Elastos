
#include "org/apache/harmony/security/x509/CReasonFlags.h"
#include "CArrayOf.h"
#include "CBoolean.h"
#include "CBerOutputStream.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CBoolean;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::IArrayOf;
using Elastos::Core::IBoolean;
using Org::Apache::Harmony::Security::Asn1::CBerOutputStream;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CReasonFlags::ASN1NamedBitListWrapper::ASN1NamedBitListWrapper()
{}

ECode CReasonFlags::ASN1NamedBitListWrapper::constructor(
    /* [in] */ Int32 minBits)
{
    return ASN1NamedBitList::constructor(minBits);
}

ECode CReasonFlags::ASN1NamedBitListWrapper::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    ASN1NamedBitList::GetDecodedObject(in, (IInterface**)&obj);

    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Boolean> > toEncode = ArrayOf<Boolean>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> o;
        array->Get(i, (IInterface**)&o);
        Boolean v;
        IBoolean::Probe(o)->GetValue(&v);
        (*toEncode)[i] = v;
    }

    AutoPtr<IReasonFlags> rf;
    CReasonFlags::New(toEncode, (IReasonFlags**)&rf);
    *result = rf;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CReasonFlags::ASN1NamedBitListWrapper::SetEncodingContent(
    /* [in] */ IBerOutputStream* out)
{
    AutoPtr<ArrayOf<Boolean> > flags =
            ((CReasonFlags*) IReasonFlags::Probe(((CBerOutputStream*)out)->mContent))->mFlags;

    Int32 len = flags->GetLength();
    AutoPtr<IArrayOf> arrObj;
    CArrayOf::New(EIID_IBoolean, len, (IArrayOf**)&arrObj);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IBoolean> b;
        CBoolean::New((*flags)[i], (IBoolean**)&b);
        arrObj->Set(i, b);
    }

    ((CBerOutputStream*)out)->mContent = arrObj;
    ASN1NamedBitList::SetEncodingContent(out);
    return NOERROR;
}

const String CReasonFlags::REASONS[] = {
    String("unused"),
    String("keyCompromise"),
    String("cACompromise"),
    String("affiliationChanged"),
    String("superseded"),
    String("cessationOfOperation"),
    String("certificateHold"),
    String("privilegeWithdrawn"),
    String("aACompromise")
};


INIT_PROI_7 AutoPtr<IASN1BitString> CReasonFlags::ASN1 = InitASN1();
CAR_OBJECT_IMPL(CReasonFlags)
CAR_INTERFACE_IMPL(CReasonFlags, Object, IReasonFlags)

AutoPtr<IASN1BitString> CReasonFlags::InitASN1()
{
    AutoPtr<ASN1NamedBitListWrapper> w = new ASN1NamedBitListWrapper();
    w->constructor(9/*REASONS.length*/);
    return w;
}

ECode CReasonFlags::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    assert(0 && "TODO");
    // sb.append(prefix);
    // sb.append("ReasonFlags [\n");
    // for (int i=0; i<flags.length; i++) {
    //     if (flags[i]) {
    //         sb.append(prefix).append("  ").append(REASONS[i]).append('\n');
    //     }
    // }
    // sb.append(prefix);
    // sb.append("]\n");
    return NOERROR;
}

ECode CReasonFlags::constructor(
    /* [in] */ ArrayOf<Boolean>* flags)
{
    mFlags = flags;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org