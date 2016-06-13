
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/CsimFileHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           CsimFileHandler
//=====================================================================

const String CsimFileHandler::LOGTAG("CsimFH");

CsimFileHandler::CsimFileHandler()
{
}

ECode CsimFileHandler::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ const String& aid,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // super(app, aid, ci);
    return NOERROR;
}

ECode CsimFileHandler::GetEFPath(
    /* [in] */ Int32 efid,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // switch(efid) {
    // case EF_SMS:
    // case EF_CST:
    // case EF_FDN:
    // case EF_MSISDN:
    // case EF_RUIM_SPN:
    // case EF_CSIM_LI:
    // case EF_CSIM_MDN:
    // case EF_CSIM_IMSIM:
    // case EF_CSIM_CDMAHOME:
    // case EF_CSIM_EPRL:
    // case EF_CSIM_MODEL:
    // case EF_MODEL:
    // case EF_CSIM_PRL:
    // case EF_RUIM_ID:
    //     return MF_SIM + DF_ADF;
    // case EF_CSIM_MSPL:
    // case EF_CSIM_MLPL:
    //     return MF_SIM + DF_TELECOM + DF_MMSS;
    // }
    // String path = getCommonIccEFPath(efid);
    // if (path == null) {
    //     // The EFids in UICC phone book entries are decided by the card manufacturer.
    //     // So if we don't match any of the cases above and if its a UICC return
    //     // the global 3g phone book path.
    //     return MF_SIM + DF_TELECOM + DF_PHONEBOOK;
    // }
    // return path;
    assert(0);
    return NOERROR;
}

ECode CsimFileHandler::Logd(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, msg);
    assert(0);
    return NOERROR;
}

ECode CsimFileHandler::Loge(
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
