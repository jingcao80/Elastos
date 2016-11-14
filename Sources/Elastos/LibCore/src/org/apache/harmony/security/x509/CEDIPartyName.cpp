
#include "org/apache/harmony/security/x509/CEDIPartyName.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CASN1Explicit.h"
#include "org/apache/harmony/security/x501/DirectoryString.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Explicit;
using Org::Apache::Harmony::Security::X501::DirectoryString;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CEDIPartyName::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);
    AutoPtr<ICharSequence> c0 = ICharSequence::Probe(obj0);
    String str0;
    c0->ToString(&str0);

    AutoPtr<IInterface> obj1;
    values->Get(0, (IInterface**)&obj1);
    AutoPtr<ICharSequence> c1 = ICharSequence::Probe(obj1);
    String str1;
    c1->ToString(&str1);

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);

    AutoPtr<IEDIPartyName> name;
    CEDIPartyName::New(str0, str1, array, (IEDIPartyName**)&name);
    *object = TO_IINTERFACE(name);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CEDIPartyName::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CEDIPartyName* epn = (CEDIPartyName*)IEDIPartyName::Probe(object);
    values->Set(0, TO_IINTERFACE(CoreUtils::Convert(epn->mNameAssigner)));
    values->Set(1, TO_IINTERFACE(CoreUtils::Convert(epn->mPartyName)));
    return NOERROR;
}

AutoPtr<IASN1Sequence> CEDIPartyName::initASN1()
{
    AutoPtr<IASN1Type> explicit0;
    CASN1Explicit::New(0, IASN1Type::Probe(DirectoryString::ASN1), (IASN1Type**)&explicit0);
    AutoPtr<IASN1Type> explicit1;
    CASN1Explicit::New(1, IASN1Type::Probe(DirectoryString::ASN1), (IASN1Type**)&explicit1);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, explicit0);
    array->Set(1, explicit1);

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    tmp->SetOptional(0);
    return IASN1Sequence::Probe(tmp);
}

INIT_PROI_5 AutoPtr<IASN1Sequence> CEDIPartyName::ASN1 = initASN1();

CAR_OBJECT_IMPL(CEDIPartyName)

CAR_INTERFACE_IMPL(CEDIPartyName, Object, IEDIPartyName)

ECode CEDIPartyName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return E_NOT_IMPLEMENTED;
}

ECode CEDIPartyName::constructor(
    /* [in] */ const String& nameAssigner,
    /* [in] */ const String& partyName,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mNameAssigner = nameAssigner;
    mPartyName = partyName;
    mEncoding = encoding;
    return NOERROR;
}

ECode CEDIPartyName::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CEDIPartyName::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org