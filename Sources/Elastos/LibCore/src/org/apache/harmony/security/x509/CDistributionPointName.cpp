
#include "org/apache/harmony/security/x509/CDistributionPointName.h"
#include "org/apache/harmony/security/x509/CGeneralNames.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CASN1Explicit.h"
#include "org/apache/harmony/security/asn1/CASN1Implicit.h"
#include "org/apache/harmony/security/x501/CName.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "Elastos.CoreLibrary.Extensions.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::X501::CName;
using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CDistributionPointName::MyASN1Choice::GetIndex(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    CDistributionPointName* dpn = (CDistributionPointName*)IDistributionPointName::Probe(object);
    *index = (dpn->mFullName == NULL) ? 1 : 0;
    return NOERROR;
}

ECode CDistributionPointName::MyASN1Choice::GetObjectToEncode(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** outobject)
{
    VALIDATE_NOT_NULL(outobject)
    *outobject = NULL;

    CDistributionPointName* dpn = (CDistributionPointName*)IDistributionPointName::Probe(object);
    if (dpn->mFullName == NULL) {
        *outobject = dpn->mNameRelativeToCRLIssuer;
        REFCOUNT_ADD(*outobject);
        return NOERROR;
    }
    else {
        *outobject = dpn->mFullName;
        REFCOUNT_ADD(*outobject);
        return NOERROR;
    }
    return NOERROR;
}

ECode CDistributionPointName::MyASN1Choice::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<IDistributionPointName> result;
    Int32 choiceIndex;
    assert(0);
    //bis->GetChoiceIndex(&choiceIndex);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    if (choiceIndex == 0) {
        CDistributionPointName::New(IGeneralNames::Probe(con), (IDistributionPointName**)&result);
    }
    else {
        // note: ASN.1 decoder will report an error if index
        // is neither 0 or 1
        CDistributionPointName::New(IName::Probe(con), (IDistributionPointName**)&result);
    }
    *object = result;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

AutoPtr<IASN1Choice> CDistributionPointName::initASN1()
{
    AutoPtr<IASN1Explicit> explicit0;
    CASN1Explicit::New(0, CGeneralNames::ASN1, (IASN1Explicit**)&explicit0);
    AutoPtr<IASN1Implicit> implicit0;
    CASN1Implicit::New(1, IASN1Type::Probe(CName::ASN1_RDN), (IASN1Implicit**)&implicit0);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, IASN1Type::Probe(explicit0));
    array->Set(1, IASN1Type::Probe(implicit0));

    AutoPtr<ASN1Choice> tmp = new MyASN1Choice();
    tmp->constructor(array);
    return IASN1Choice::Probe(tmp);
}

INIT_PROI_8 AutoPtr<IASN1Choice> CDistributionPointName::ASN1 = initASN1();

CAR_OBJECT_IMPL(CDistributionPointName)

CAR_INTERFACE_IMPL(CDistributionPointName, Object, IDistributionPointName)

ECode CDistributionPointName::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("Distribution Point Name: [\n"));
    if (mFullName != NULL) {
        mFullName->DumpValue(sb, prefix + String("  "));
    }
    else {
        sb->Append(prefix);
        sb->Append(String("  "));
        String name;
        mNameRelativeToCRLIssuer->GetName(IX500Principal::RFC2253, &name);
        sb->Append(name);
    }
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CDistributionPointName::constructor(
    /* [in] */ IGeneralNames* fullName)
{
    mFullName = fullName;
    mNameRelativeToCRLIssuer = NULL;
    return NOERROR;
}

ECode CDistributionPointName::constructor(
    /* [in] */ IName* nameRelativeToCRLIssuer)
{
    mFullName = NULL;
    mNameRelativeToCRLIssuer = nameRelativeToCRLIssuer;
    return NOERROR;
}

ECode CDistributionPointName::GetASN1(
    /* [out] */ IASN1Choice** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CDistributionPointName::SetASN1(
    /* [in] */ IASN1Choice* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org