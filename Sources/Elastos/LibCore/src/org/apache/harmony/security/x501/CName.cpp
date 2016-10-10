
#include "CName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

ECode CName::ASN1SequenceOfDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** object)
{
    AutoPtr<IInterface> c;
    in->GetContent((IInterface**)&c);
    return CName::New(IList::Probe(GetContent), object);
}

ECode CName::ASN1SequenceOfDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);
    *values = ICollection::Probe((CName*)IName::Probe(object)->mRdn);
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode CName::ASN1SequenceOfDerived::constructor(
    /* [in] */ IASN1SetOf* set)
{
    return ASN1SequenceOf::constructor(set);
}

CName::ASN1SequenceOfDerived::ASN1SequenceOfDerived()
{}

AutoPtr<IASN1SetOf> CName::ASN1_RDN = InitASN1_RDN();
AutoPtr<IASN1SequenceOf> CName::ASN1 = InitASN1();

CAR_OBJECT_IMPL(CName)
CAR_INTERFACE_IMPL(CName, Object, IName)
AutoPtr<IASN1SetOf> CName::InitASN1_RDN()
{
    AutoPtr<IASN1SetOf> as;
    CASN1SetOf::New(CAttributeTypeAndValue::ASN1, (IASN1SetOf**)&as);
    return as;
}

AutoPtr<IASN1SequenceOf> CName::InitASN1()
{
    AutoPtr<ASN1SequenceOfDerived> as = new ASN1SequenceOfDerived();
    as->constructor(ASN1_RDN);
    return as;
}

ECode CName::GetX500Principal(
    /* [out] */ Elastosx::Security::Auth::X500::IX500Principal ** ppX500Principal)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::GetName(
    /* [in] */ const String& format,
    /* [out] */ String * pName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::constructor(
    /* [in] */ ArrayOf<Byte> * pEncoding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::constructor(
    /* [in] */ const String& name)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CName::constructor(
    /* [in] */ Elastos::Utility::IList * pRdn)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

