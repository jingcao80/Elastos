#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/RuimPhoneBookInterfaceManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                    RuimPhoneBookInterfaceManager
//=====================================================================
CAR_INTERFACE_IMPL(RuimPhoneBookInterfaceManager, /*TODO IccPhoneBookInterfaceManager*/Object, IRuimPhoneBookInterfaceManager);
const String RuimPhoneBookInterfaceManager::LOGTAG("RuimPhoneBookIM");

RuimPhoneBookInterfaceManager::RuimPhoneBookInterfaceManager(
    /* [in] */ ICDMAPhone* phone)
{
    // ==================before translated======================
    // super(phone);
    // //NOTE service "simphonebook" added by IccSmsInterfaceManagerProxy
}

ECode RuimPhoneBookInterfaceManager::Dispose()
{
    // ==================before translated======================
    // super.dispose();
    assert(0);
    return NOERROR;
}

ECode RuimPhoneBookInterfaceManager::GetAdnRecordsSize(
    /* [in] */ Int32 efid,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // if (DBG) logd("getAdnRecordsSize: efid=" + efid);
    // synchronized(mLock) {
    //     checkThread();
    //     mRecordSize = new int[3];
    //
    //     //Using mBaseHandler, no difference in EVENT_GET_SIZE_DONE handling
    //     AtomicBoolean status = new AtomicBoolean(false);
    //     Message response = mBaseHandler.obtainMessage(EVENT_GET_SIZE_DONE, status);
    //
    //     IccFileHandler fh = mPhone.getIccFileHandler();
    //     //IccFileHandler can be null if there is no icc card present.
    //     if (fh != null) {
    //         fh.getEFLinearRecordSize(efid, response);
    //         waitForResult(status);
    //     }
    // }
    //
    // return mRecordSize;
    assert(0);
    return NOERROR;
}

void RuimPhoneBookInterfaceManager::Finalize()
{
    // ==================before translated======================
    // try {
    //     super.finalize();
    // } catch (Throwable throwable) {
    //     Rlog.e(LOGTAG, "Error while finalizing:", throwable);
    // }
    // if(DBG) Rlog.d(LOGTAG, "RuimPhoneBookInterfaceManager finalized");
    assert(0);
}

ECode RuimPhoneBookInterfaceManager::Logd(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "[RuimPbInterfaceManager] " + msg);
    assert(0);
    return NOERROR;
}

ECode RuimPhoneBookInterfaceManager::Loge(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[RuimPbInterfaceManager] " + msg);
    assert(0);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
