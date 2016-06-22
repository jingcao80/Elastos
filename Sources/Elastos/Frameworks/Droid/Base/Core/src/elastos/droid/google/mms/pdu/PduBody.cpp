#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/PduBody.h"

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
    // ==================before translated======================
    // mParts = new Vector<PduPart>();
    //
    // mPartMapByContentId = new HashMap<String, PduPart>();
    // mPartMapByContentLocation  = new HashMap<String, PduPart>();
    // mPartMapByName = new HashMap<String, PduPart>();
    // mPartMapByFileName = new HashMap<String, PduPart>();
}

ECode PduBody::AddPart(
    /* [in] */ IPduPart* part,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if(null == part) {
    //     throw new NullPointerException();
    // }
    //
    // putPartToMaps(part);
    // return mParts.add(part);
    assert(0);
    return NOERROR;
}

ECode PduBody::AddPart(
    /* [in] */ Int32 index,
    /* [in] */ IPduPart* part)
{
    VALIDATE_NOT_NULL(part);
    // ==================before translated======================
    // if(null == part) {
    //     throw new NullPointerException();
    // }
    //
    // putPartToMaps(part);
    // mParts.add(index, part);
    assert(0);
    return NOERROR;
}

ECode PduBody::RemovePart(
    /* [in] */ Int32 index,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParts.remove(index);
    assert(0);
    return NOERROR;
}

ECode PduBody::RemoveAll()
{
    // ==================before translated======================
    // mParts.clear();
    assert(0);
    return NOERROR;
}

ECode PduBody::GetPart(
    /* [in] */ Int32 index,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParts.get(index);
    assert(0);
    return NOERROR;
}

ECode PduBody::GetPartIndex(
    /* [in] */ IPduPart* part,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(part);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParts.indexOf(part);
    assert(0);
    return NOERROR;
}

ECode PduBody::GetPartsNum(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mParts.size();
    assert(0);
    return NOERROR;
}

ECode PduBody::GetPartByContentId(
    /* [in] */ const String& cid,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPartMapByContentId.get(cid);
    assert(0);
    return NOERROR;
}

ECode PduBody::GetPartByContentLocation(
    /* [in] */ const String& contentLocation,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPartMapByContentLocation.get(contentLocation);
    assert(0);
    return NOERROR;
}

ECode PduBody::GetPartByName(
    /* [in] */ const String& name,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPartMapByName.get(name);
    assert(0);
    return NOERROR;
}

ECode PduBody::GetPartByFileName(
    /* [in] */ const String& filename,
    /* [out] */ IPduPart** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPartMapByFileName.get(filename);
    assert(0);
    return NOERROR;
}

void PduBody::PutPartToMaps(
    /* [in] */ IPduPart* part)
{
    // ==================before translated======================
    // // Put part to mPartMapByContentId.
    // byte[] contentId = part.getContentId();
    // if(null != contentId) {
    //     mPartMapByContentId.put(new String(contentId), part);
    // }
    //
    // // Put part to mPartMapByContentLocation.
    // byte[] contentLocation = part.getContentLocation();
    // if(null != contentLocation) {
    //     String clc = new String(contentLocation);
    //     mPartMapByContentLocation.put(clc, part);
    // }
    //
    // // Put part to mPartMapByName.
    // byte[] name = part.getName();
    // if(null != name) {
    //     String clc = new String(name);
    //     mPartMapByName.put(clc, part);
    // }
    //
    // // Put part to mPartMapByFileName.
    // byte[] fileName = part.getFilename();
    // if(null != fileName) {
    //     String clc = new String(fileName);
    //     mPartMapByFileName.put(clc, part);
    // }
    assert(0);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
