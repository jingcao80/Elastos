
#include "org/apache/harmony/security/x509/CPolicyConstraints.h"
#include "CASN1Implicit.h"
#include "CASN1Integer.h"
#include "CoreUtils.h"
#include "CBerInputStream.h"
#include "CBigInteger.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Math::CBigInteger;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CPolicyConstraints::ASN1SequenceWrapper::ASN1SequenceWrapper()
{}

ECode CPolicyConstraints::ASN1SequenceWrapper::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* types)
{
    ASN1Sequence::constructor(types);
    SetOptional(0);
    SetOptional(1);
    return NOERROR;
}

ECode CPolicyConstraints::ASN1SequenceWrapper::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CPolicyConstraints* pc = (CPolicyConstraints*) IPolicyConstraints::Probe(object);

    AutoPtr<ArrayOf<Byte> > bs;
    pc->mRequireExplicitPolicy->ToByteArray((ArrayOf<Byte>**)&bs);
    values->Set(0, CoreUtils::ConvertByteArray(bs));

    bs = NULL;
    pc->mInhibitPolicyMapping->ToByteArray((ArrayOf<Byte>**)&bs);
    values->Set(1, CoreUtils::ConvertByteArray(bs));
    return NOERROR;
}

ECode CPolicyConstraints::ASN1SequenceWrapper::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result)
{
    AutoPtr<IArrayOf> values = IArrayOf::Probe(((CBerInputStream*)in)->mContent);
    AutoPtr<IBigInteger> requireExplicitPolicy;
    AutoPtr<IBigInteger> inhibitPolicyMapping;

    AutoPtr<IInterface> v0;
    values->Get(0, (IInterface**)&v0);
    AutoPtr<IInterface> v1;
    values->Get(1, (IInterface**)&v1);

    Int32 len = 0;
    AutoPtr<ArrayOf<Byte> > bs;
    if (v0 != NULL) {
        IArrayOf::Probe(v0)->GetLength(&len);
        bs = ArrayOf<Byte>::Alloc(len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> o;
            IArrayOf::Probe(v0)->Get(i, (IInterface**)&o);
            Byte v;
            IByte::Probe(o)->GetValue(&v);
            (*bs)[i] = v;
        }
        CBigInteger::New(*bs, (IBigInteger**)&requireExplicitPolicy);
    }
    if (v1 != NULL) {
        bs = NULL;
        IArrayOf::Probe(v1)->GetLength(&len);
        bs = ArrayOf<Byte>::Alloc(len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> o;
            IArrayOf::Probe(v1)->Get(i, (IInterface**)&o);
            Byte v;
            IByte::Probe(o)->GetValue(&v);
            (*bs)[i] = v;
        }

        CBigInteger::New(*bs, (IBigInteger**)&inhibitPolicyMapping);
    }

    bs = NULL;
    in->GetEncoded((ArrayOf<Byte>**)&bs);
    AutoPtr<IPolicyConstraints> pc;
    CPolicyConstraints::New(requireExplicitPolicy, inhibitPolicyMapping,
            bs, (IPolicyConstraints**)&pc);
    *result = pc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IASN1Sequence> CPolicyConstraints::ASN1 = InitASN1();
CAR_OBJECT_IMPL(CPolicyConstraints)
CAR_INTERFACE_IMPL(CPolicyConstraints, ExtensionValue, IPolicyConstraints)

AutoPtr<IASN1Sequence> CPolicyConstraints::InitASN1()
{
    AutoPtr<ArrayOf<IASN1Type*> > types = ArrayOf<IASN1Type*>::Alloc(2);

    AutoPtr<IASN1Implicit> ai;
    AutoPtr<IASN1Integer> instance;
    CASN1Integer::GetInstance((IASN1Integer**)&instance);
    assert(instance != NULL);
    CASN1Implicit::New(0, IASN1Type::Probe(instance), (IASN1Implicit**)&ai);
    types->Set(0, IASN1Type::Probe(ai));

    ai = NULL;
    CASN1Implicit::New(1, IASN1Type::Probe(instance), (IASN1Implicit**)&ai);
    types->Set(1, IASN1Type::Probe(ai));

    AutoPtr<ASN1SequenceWrapper> as = new ASN1SequenceWrapper();
    as->constructor(types);
    return as;
}

ECode CPolicyConstraints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CPolicyConstraints::DumpValue(
    /* [in] */ IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    assert(0 && "TODO");
    // sb.append(prefix).append("PolicyConstraints: [\n");
    // if (requireExplicitPolicy != null) {
    //     sb.append(prefix).append("  requireExplicitPolicy: ").append(requireExplicitPolicy).append('\n');
    // }
    // if (inhibitPolicyMapping != null) {
    //     sb.append(prefix).append("  inhibitPolicyMapping: ").append(inhibitPolicyMapping).append('\n');
    // }
    // sb.append(prefix).append("]\n");
    return NOERROR;
}

ECode CPolicyConstraints::constructor(
    /* [in] */ IBigInteger* requireExplicitPolicy,
    /* [in] */ IBigInteger* inhibitPolicyMapping)
{
    mRequireExplicitPolicy = requireExplicitPolicy;
    mInhibitPolicyMapping = inhibitPolicyMapping;
    return NOERROR;
}

ECode CPolicyConstraints::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    ExtensionValue::constructor(encoding);
    AutoPtr<IInterface> o;
    IASN1Type::Probe(ASN1)->Decode(encoding, (IInterface**)&o);
    CPolicyConstraints* pc = (CPolicyConstraints*) IPolicyConstraints::Probe(o);
    mRequireExplicitPolicy = pc->mRequireExplicitPolicy;
    mInhibitPolicyMapping = pc->mInhibitPolicyMapping;
    return NOERROR;
}

ECode CPolicyConstraints::constructor(
    /* [in] */ IBigInteger* requireExplicitPolicy,
    /* [in] */ IBigInteger* inhibitPolicyMapping,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(requireExplicitPolicy, inhibitPolicyMapping);
    mEncoding = encoding;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org