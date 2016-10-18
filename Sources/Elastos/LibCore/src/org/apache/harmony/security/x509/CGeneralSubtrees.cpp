
#include "org/apache/harmony/security/x509/CGeneralSubtrees.h"
#include "org/apache/harmony/security/x509/CGeneralSubtree.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/utility/CArrayList.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CGeneralSubtrees::MyASN1SequenceOf::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IGeneralSubtrees> tmp;
    CGeneralSubtrees::New(IList::Probe(con), (IGeneralSubtrees**)&tmp);
    *object = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CGeneralSubtrees::MyASN1SequenceOf::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);
    *values = NULL;

    CGeneralSubtrees* gss = (CGeneralSubtrees*)IGeneralSubtrees::Probe(object);

    if (gss->mGeneralSubtrees == NULL) {
        return CArrayList::New(values);
    }
    else {
        *values = ICollection::Probe(gss->mGeneralSubtrees);
        REFCOUNT_ADD(*values);
        return NOERROR;
    }
    return NOERROR;
}

AutoPtr<IASN1Type> CGeneralSubtrees::initASN1()
{
    AutoPtr<ASN1SequenceOf> tmp = new MyASN1SequenceOf();
    tmp->constructor(IASN1Type::Probe(CGeneralSubtree::ASN1));
    return IASN1Type::Probe(tmp);
}

AutoPtr<IASN1Type> CGeneralSubtrees::ASN1 = initASN1();

CAR_OBJECT_IMPL(CGeneralSubtrees)

CAR_INTERFACE_IMPL(CGeneralSubtrees, Object, IGeneralSubtrees)

ECode CGeneralSubtrees::GetSubtrees(
    /* [out] */ IList** ppSubtrees)
{
    VALIDATE_NOT_NULL(ppSubtrees);

    *ppSubtrees = mGeneralSubtrees;
    REFCOUNT_ADD(*ppSubtrees);
    return NOERROR;
}

ECode CGeneralSubtrees::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        ASN1->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CGeneralSubtrees::constructor(
    /* [in] */ IList* generalSubtrees)
{
    // TODO: the size should not be less than one
    mGeneralSubtrees = generalSubtrees;
    return NOERROR;
}

ECode CGeneralSubtrees::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CGeneralSubtrees::SetASN1(
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