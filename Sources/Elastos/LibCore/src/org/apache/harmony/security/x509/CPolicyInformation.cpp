
#include "org/apache/harmony/security/x509/CPolicyInformation.h"
#include "ASN1Oid.h"
#include "CASN1Any.h"
#include "CObjectIdentifier.h"
#include "CBerInputStream.h"
#include "CoreUtils.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Org::Apache::Harmony::Security::Asn1::ASN1Oid;
using Org::Apache::Harmony::Security::Asn1::CASN1Any;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::Asn1::IASN1Any;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CPolicyInformation::ASN1SequenceWrapper::ASN1SequenceWrapper()
{}

ECode CPolicyInformation::ASN1SequenceWrapper::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* types)
{
    ASN1Sequence::constructor(types);
    SetOptional(1);
    return NOERROR;
}

ECode CPolicyInformation::ASN1SequenceWrapper::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CPolicyInformation* pi = (CPolicyInformation*) IPolicyInformation::Probe(object);
    AutoPtr<ArrayOf<Int32> > ivs;
    CObjectIdentifier::ToIntArray(pi->mPolicyIdentifier, (ArrayOf<Int32>**)&ivs);
    values->Set(0, CoreUtils::Convert(ivs));
    return NOERROR;
}

ECode CPolicyInformation::ASN1SequenceWrapper::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result)
{
    AutoPtr<IArrayOf> values = IArrayOf::Probe(((CBerInputStream*)in)->mContent);
    AutoPtr<IInterface> o;
    values->Get(0, (IInterface**)&o);
    Int32 v = 0;
    IInteger32::Probe(o)->GetValue(&v);
    AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(1);
    (*ivs)[0] = v;
    AutoPtr<IPolicyInformation> pi;
    String s;
    CObjectIdentifier::ToString(ivs, &s);
    CPolicyInformation::New(s, (IPolicyInformation**)&pi);
    *result = pi;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

INIT_PROI_4 AutoPtr<IASN1Sequence> CPolicyInformation::ASN1 = InitASN1();
CAR_OBJECT_IMPL(CPolicyInformation)
CAR_INTERFACE_IMPL(CPolicyInformation, Object, IPolicyInformation)

AutoPtr<IASN1Sequence> CPolicyInformation::InitASN1()
{
    AutoPtr<ArrayOf<IASN1Type*> > types = ArrayOf<IASN1Type*>::Alloc(2);
    AutoPtr<IASN1Type> t1;
    ASN1Oid::GetInstance((IASN1Type**)&t1);
    types->Set(0, t1);
    AutoPtr<IASN1Any> aa;
    CASN1Any::GetInstance((IASN1Any**)&aa);
    types->Set(1, IASN1Type::Probe(aa));

    // Contains only ASN1 DER decoder currently
    AutoPtr<ASN1SequenceWrapper> as = new ASN1SequenceWrapper();
    as->constructor(types);
    return as;
}

ECode CPolicyInformation::GetPolicyIdentifier(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPolicyIdentifier;
    return NOERROR;
}

ECode CPolicyInformation::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    *result = mEncoding;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPolicyInformation::DumpValue(
    /* [in] */ IStringBuilder* sb)
{
    sb->Append(String("Policy Identifier ["));
    sb->Append(mPolicyIdentifier);
    sb->AppendChar(']');
    return NOERROR;
}

ECode CPolicyInformation::constructor(
    /* [in] */ const String& policyIdentifier)
{
    mPolicyIdentifier = policyIdentifier;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org