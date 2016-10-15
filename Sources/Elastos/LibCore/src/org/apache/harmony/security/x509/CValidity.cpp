
#include "org/apache/harmony/security/x509/CValidity.h"
#include "Time.h"
#include "CBerInputStream.h"

using Elastos::Core::IArrayOf;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CValidity::ASN1SequenceWrapper::ASN1SequenceWrapper()
{}

ECode CValidity::ASN1SequenceWrapper::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(((CBerInputStream*)in)->mContent);
    AutoPtr<IInterface> item1;
    values->Get(0, (IInterface**)&item1);

    AutoPtr<IInterface> item2;
    values->Get(1, (IInterface**)&item2);
    AutoPtr<IValidity> v;
    CValidity::New(IDate::Probe(item1), IDate::Probe(item2), (IValidity**)&v);
    *result = v;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CValidity::ASN1SequenceWrapper::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CValidity* validity = (CValidity*) IValidity::Probe(object);
    (*values)[0] = validity->mNotBefore;
    (*values)[1] = validity->mNotAfter;
    return NOERROR;
}

AutoPtr<IASN1Sequence> CValidity::ASN1 = InitASN1();

CAR_OBJECT_IMPL(CValidity)
CAR_INTERFACE_IMPL(CValidity, Object, IValidity)
AutoPtr<IASN1Sequence> CValidity::InitASN1()
{
    AutoPtr<ASN1SequenceWrapper> a = new ASN1SequenceWrapper();
    AutoPtr<ArrayOf<IASN1Type*> > at = ArrayOf<IASN1Type*>::Alloc(2);
    at->Set(0, IASN1Type::Probe(Time::ASN1));
    at->Set(1, IASN1Type::Probe(Time::ASN1));
    a->constructor(at);
    return a;
}

ECode CValidity::GetNotBefore(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNotBefore;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CValidity::GetNotAfter(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNotAfter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CValidity::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    *result = mEncoding;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CValidity::constructor(
    /* [in] */ IDate* notBefore,
    /* [in] */ IDate* notAfter)
{
    mNotBefore = notBefore;
    mNotAfter = notAfter;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org