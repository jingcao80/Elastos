
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/SIMFileHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                            SIMFileHandler
//=====================================================================
const String SIMFileHandler::LOGTAG("SIMFileHandler");

SIMFileHandler::SIMFileHandler()
{
}

ECode SIMFileHandler::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ const String& aid,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // super(app, aid, ci);
    return NOERROR;
}

ECode SIMFileHandler::GetEFPath(
    /* [in] */ Int32 efid,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // TODO(): DF_GSM can be 7F20 or 7F21 to handle backward compatibility.
    // // Implement this after discussion with OEMs.
    // switch(efid) {
    // case EF_SMS:
    //     return MF_SIM + DF_TELECOM;
    //
    // case EF_EXT6:
    // case EF_MWIS:
    // case EF_MBI:
    // case EF_SPN:
    // case EF_AD:
    // case EF_MBDN:
    // case EF_PNN:
    // case EF_SPDI:
    // case EF_SST:
    // case EF_CFIS:
    // case EF_GID1:
    //     return MF_SIM + DF_GSM;
    //
    // case EF_MAILBOX_CPHS:
    // case EF_VOICE_MAIL_INDICATOR_CPHS:
    // case EF_CFF_CPHS:
    // case EF_SPN_CPHS:
    // case EF_SPN_SHORT_CPHS:
    // case EF_INFO_CPHS:
    // case EF_CSP_CPHS:
    // case EF_PLMNWACT:
    //     return MF_SIM + DF_GSM;
    // }
    // String path = getCommonIccEFPath(efid);
    // if (path == null) {
    //     Rlog.e(LOGTAG, "Error: EF Path being returned in null");
    // }
    // return path;
    assert(0);
    return NOERROR;
}

ECode SIMFileHandler::Logd(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, msg);
    assert(0);
    return NOERROR;
}

ECode SIMFileHandler::Loge(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, msg);
    assert(0);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
