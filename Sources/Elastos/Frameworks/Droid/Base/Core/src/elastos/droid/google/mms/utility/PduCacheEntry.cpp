#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/utility/PduCacheEntry.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

//=====================================================================
//                            PduCacheEntry
//=====================================================================
CAR_INTERFACE_IMPL(PduCacheEntry, Object, IPduCacheEntry);

PduCacheEntry::PduCacheEntry()
{
}

ECode PduCacheEntry::constructor(
    /* [in] */ IGenericPdu* pdu,
    /* [in] */ Int32 msgBox,
    /* [in] */ Int64 threadId)
{
    // ==================before translated======================
    // mPdu = pdu;
    // mMessageBox = msgBox;
    // mThreadId = threadId;
    return NOERROR;
}

ECode PduCacheEntry::GetPdu(
    /* [out] */ IGenericPdu** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPdu;
    assert(0);
    return NOERROR;
}

ECode PduCacheEntry::GetMessageBox(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMessageBox;
    assert(0);
    return NOERROR;
}

ECode PduCacheEntry::GetThreadId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mThreadId;
    assert(0);
    return NOERROR;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
