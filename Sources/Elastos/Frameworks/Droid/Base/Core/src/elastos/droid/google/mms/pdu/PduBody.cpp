
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/PduBody.h"

#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CVector;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                               PduBody
//=====================================================================
CAR_INTERFACE_IMPL(PduBody, Object, IPduBody);

PduBody::PduBody()
{
    CVector::New((IVector**)&mParts);

    CHashMap::New((IMap**)&mPartMapByContentId);
    CHashMap::New((IMap**)&mPartMapByContentLocation);
    CHashMap::New((IMap**)&mPartMapByName);
    CHashMap::New((IMap**)&mPartMapByFileName);
}

ECode PduBody::AddPart(
    /* [in] */ IPduPart* part,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (NULL == part) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    PutPartToMaps(part);
    return mParts->Add(part, result);
}

ECode PduBody::AddPart(
    /* [in] */ Int32 index,
    /* [in] */ IPduPart* part)
{
    VALIDATE_NOT_NULL(part);
    if (NULL == part) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    PutPartToMaps(part);
    mParts->Add(index, part);
    return NOERROR;
}

ECode PduBody::RemovePart(
    /* [in] */ Int32 index,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mParts->Remove(index, (IInterface**)&p);
    *result = IPduPart::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduBody::RemoveAll()
{
    mParts->Clear();
    return NOERROR;
}

ECode PduBody::GetPart(
    /* [in] */ Int32 index,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mParts->Get(index, (IInterface**)&p);
    *result = IPduPart::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduBody::GetPartIndex(
    /* [in] */ IPduPart* part,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(part);
    VALIDATE_NOT_NULL(result);
    return mParts->IndexOf(part, result);
}

ECode PduBody::GetPartsNum(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mParts->GetSize(result);
}

ECode PduBody::GetPartByContentId(
    /* [in] */ const String& cid,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartMapByContentId->Get(CoreUtils::Convert(cid), (IInterface**)&p);
    *result = IPduPart::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduBody::GetPartByContentLocation(
    /* [in] */ const String& contentLocation,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartMapByContentLocation->Get(CoreUtils::Convert(contentLocation), (IInterface**)&p);
    *result = IPduPart::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduBody::GetPartByName(
    /* [in] */ const String& name,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartMapByContentId->Get(CoreUtils::Convert(name), (IInterface**)&p);
    *result = IPduPart::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduBody::GetPartByFileName(
    /* [in] */ const String& filename,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mPartMapByFileName->Get(CoreUtils::Convert(filename), (IInterface**)&p);
    *result = IPduPart::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void PduBody::PutPartToMaps(
    /* [in] */ IPduPart* part)
{
    // Put part to mPartMapByContentId.
    AutoPtr<ArrayOf<Byte> > contentId;
    part->GetContentId((ArrayOf<Byte>**)&contentId);
    if (NULL != contentId) {
        mPartMapByContentId->Put(CoreUtils::Convert(String(*contentId)), part);
    }

    // Put part to mPartMapByContentLocation.
    AutoPtr<ArrayOf<Byte> > contentLocation;
    part->GetContentLocation((ArrayOf<Byte>**)&contentLocation);
    if (NULL != contentLocation) {
        String clc(*contentLocation);
        mPartMapByContentLocation->Put(CoreUtils::Convert(clc), part);
    }

    // Put part to mPartMapByName.
    AutoPtr<ArrayOf<Byte> > name;
    part->GetName((ArrayOf<Byte>**)&name);
    if (NULL != name) {
        String clc(*name);
        mPartMapByName->Put(CoreUtils::Convert(clc), part);
    }

    // Put part to mPartMapByFileName.
    AutoPtr<ArrayOf<Byte> > fileName;
    part->GetFilename((ArrayOf<Byte>**)&fileName);
    if (NULL != fileName) {
        String clc(*fileName);
        mPartMapByFileName->Put(CoreUtils::Convert(clc), part);
    }
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
