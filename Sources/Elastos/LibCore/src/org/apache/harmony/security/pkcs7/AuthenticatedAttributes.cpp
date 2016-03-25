
#include "AuthenticatedAttributes.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CAR_INTERFACE_IMPL_LIGHT(AuthenticatedAttributes::ASN1SetOfDerived, IASN1SetOf)

ECode AuthenticatedAttributes::ASN1SetOfDerived::GetId(
    /* [out] */ Int32* id)
{
    return ASN1ValueCollection::GetId(id);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::GetConstrId(
    /* [out] */ Int32* constrId)
{
    return ASN1ValueCollection::GetConstrId(constrId);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::Decode(encoded, object);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::DecodeEx(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::DecodeEx(encoded, offset, encodingLen, object);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::DecodeEx2(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    return ASN1ValueCollection::DecodeEx2(is, object);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return ASN1ValueCollection::Verify(encoded);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::VerifyEx(
    /* [in] */ IInputStream* is)
{
    return ASN1ValueCollection::VerifyEx(is);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    return ASN1ValueCollection::Encode;
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::DecodeEx3(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return ASN1SetOf::Decode(bis, object);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    return ASN1ValueCollection::CheckTag(identifier, checkTag);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<ArrayOf<Byte> > encoded;
    bis->GetEncoded((ArrayOf<Byte>**)&encoded);
    AutoPtr<IInterface> tmp;
    bis->GetContent((IInterface**)&tmp);
    AutoPtr<IList> lst = IList::Probe(tmp);
    *object = new AuthenticatedAttributes(encoded, lst);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1ValueCollection::EncodeASN(bos);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1SetOf::EncodeContent(bos);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return ASN1SetOf::SetEncodingContent(bos);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1ValueCollection::GetEncodedLength(bos, length);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::ToString(
    /* [out] */ String* result)
{
    return ASN1ValueCollection::ToString(result);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::SetType(
    /* [in] */ IASN1Type* type)
{
    return ASN1ValueCollection::SetType(type);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::GetType(
    /* [out] */ IASN1Type** type)
{
    return ASN1ValueCollection::GetType(type);
}

ECode AuthenticatedAttributes::ASN1SetOfDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    return ASN1ValueCollection::GetValues(object, values);
}

AuthenticatedAttributes::ASN1SetOfDerived::ASN1SetOfDerived(
    /* [in] */ IASN1Type* type)
{
    ASN1SetOf::Init(type);
}

AutoPtr<IASN1SetOf> AuthenticatedAttributes::ASN1 = InitStatic();

AutoPtr<IASN1SetOf> AuthenticatedAttributes::InitStatic()
{
    AutoPtr<IAttributeTypeAndValueHelper> helper;
    CAttributeTypeAndValueHelper::AcquireSingleton((IAttributeTypeAndValueHelper**)&helper);
    AutoPtr<IASN1Sequence> as;
    helper->GetASN1Sequence((IASN1Sequence**)&as);
    AutoPtr<IASN1SetOf> ret = new ASN1SetOfDerived(as.Get());
    return ret;
}

CAR_INTERFACE_IMPL_LIGHT(AuthenticatedAttributes, IInterface)

ECode AuthenticatedAttributes::GetAttributes(
    /* [out] */ IList** attributes)
{
    VALIDATE_NOT_NULL(attributes)
    *attributes = mAuthenticatedAttributes;
    REFCOUNT_ADD(*attributes)
    return NOERROR;

ECode AuthenticatedAttributes::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encoded)
{
    VALIDATE_NOT_NULL(encoded)
    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *encoded = mEncoding;
    REFCOUNT_ADD(*encoded)
    return NOERROR;
}

AuthenticatedAttributes::AuthenticatedAttributes(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [in] */ IList* authenticatedAttributes)
{
    mEncoding = encoding;
    mAuthenticatedAttributes = authenticatedAttributes;
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

