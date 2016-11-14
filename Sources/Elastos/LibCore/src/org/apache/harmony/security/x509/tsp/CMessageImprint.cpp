
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CASN1OctetString.h"
#include "org/apache/harmony/security/x509/tsp/CMessageImprint.h"
#include "org/apache/harmony/security/x509/AlgorithmIdentifier.h"
#include <elastos/core/CoreUtils.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::CASN1OctetString;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::CoreUtils;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

ECode CMessageImprint::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);
    AutoPtr<IInterface> obj1;
    values->Get(0, (IInterface**)&obj1);

    AutoPtr<IInterface> obj2;
    values->Get(1, (IInterface**)&obj2);
    AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(obj2);
    Int32 length;
    arrayof->GetLength(&length);
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> value;
        arrayof->Get(i, (IInterface**)&value);
        AutoPtr<IByte> b = IByte::Probe(value);
        Byte num;
        b->GetValue(&num);
        (*array)[i] = num;
    }

    AutoPtr<IMessageImprint> tmp;
    CMessageImprint::New(IAlgorithmIdentifier::Probe(obj1), array, (IMessageImprint**)&tmp);
    *object = tmp;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CMessageImprint::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CMessageImprint* mi = (CMessageImprint*)IMessageImprint::Probe(object);
    values->Set(0, TO_IINTERFACE(mi->mAlgId));

    AutoPtr<IArrayOf> array = CoreUtils::ConvertByteArray(mi->mHashedMessage);
    values->Set(1, TO_IINTERFACE(array));
    return NOERROR;
}

AutoPtr<IASN1Sequence> CMessageImprint::initASN1()
{
    AutoPtr<IASN1Sequence> aASN1;
    AlgorithmIdentifier::GetASN1((IASN1Sequence**)&aASN1);
    AutoPtr<IASN1OctetString> instance;
    CASN1OctetString::GetInstance((IASN1OctetString**)&instance);
    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, IASN1Type::Probe(aASN1));
    array->Set(1, IASN1Type::Probe(instance));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    return IASN1Sequence::Probe(tmp);
}

INIT_PROI_5 AutoPtr<IASN1Sequence> CMessageImprint::ASN1 = initASN1();

CAR_OBJECT_IMPL(CMessageImprint)

CAR_INTERFACE_IMPL(CMessageImprint, Object, IMessageImprint)

ECode CMessageImprint::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1 );
    return NOERROR;
}

ECode CMessageImprint::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

ECode CMessageImprint::constructor(
    /* [in] */ IAlgorithmIdentifier* pAlgId,
    /* [in] */ ArrayOf<Byte>* pHashedMessage)
{
    mAlgId = pAlgId;
    mHashedMessage = pHashedMessage;
    return NOERROR;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org