
#include "org/apache/harmony/security/x509/CCertificateIssuer.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X501::IName;
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

ECode CCertificateIssuer::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);
    AutoPtr<IGeneralName> gname = IGeneralName::Probe(obj0);
    AutoPtr<IInterface> nameobj;
    gname->GetName((IInterface**)&nameobj);

    AutoPtr<IX500Principal> principal;
    IName::Probe(nameobj)->GetX500Principal((IX500Principal**)&principal);
    *object = TO_IINTERFACE(principal);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CCertificateIssuer::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    values->Set(0, object);
    return NOERROR;
}

AutoPtr<IASN1Type> CCertificateIssuer::initASN1()
{
    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(1);
    assert(0);
    //array->Set(0, IASN1Type::Probe(GeneralName::ASN1));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    return IASN1Type::Probe(tmp);
}

AutoPtr<IASN1Type> CCertificateIssuer::ASN1 = initASN1();

CAR_OBJECT_IMPL(CCertificateIssuer)

CAR_INTERFACE_IMPL(CCertificateIssuer, ExtensionValue, ICertificateIssuer)

ECode CCertificateIssuer::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("Certificate Issuer: "));
    if (mIssuer == NULL) {
        //try {
        ECode ec = GetIssuer((IX500Principal**)&mIssuer);
        //} catch (IOException e) {
        if (ec == (ECode)E_IO_EXCEPTION) {
            // incorrect extension value encoding
            sb->Append(String("Unparseable (incorrect!) extension value:\n"));
            ExtensionValue::DumpValue(sb);
        }
    }
    sb->Append(mIssuer);
    sb->Append('\n');
    return NOERROR;
}

ECode CCertificateIssuer::GetIssuer(
    /* [out] */ IX500Principal** ppX500Principal)
{
    VALIDATE_NOT_NULL(ppX500Principal);

    if (mIssuer == NULL) {
        AutoPtr<ArrayOf<Byte> > array;
        GetEncoded((ArrayOf<Byte>**)&array);
        AutoPtr<IInterface> obj;
        FAIL_RETURN(ASN1->Decode(array, (IInterface**)&obj));
        mIssuer = IX500Principal::Probe(obj);
    }
    *ppX500Principal = mIssuer;
    REFCOUNT_ADD(*ppX500Principal);
    return NOERROR;
}

ECode CCertificateIssuer::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    return ExtensionValue::constructor(encoding);
}

ECode CCertificateIssuer::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CCertificateIssuer::SetASN1(
    /* [in] */ IASN1Type* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org