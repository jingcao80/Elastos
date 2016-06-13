
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IsimFileHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           IsimFileHandler
//=====================================================================
const String IsimFileHandler::LOGTAG("IsimFH");

IsimFileHandler::IsimFileHandler()
{
}

ECode IsimFileHandler::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ const String& aid,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // super(app, aid, ci);
    return NOERROR;
}

ECode IsimFileHandler::GetEFPath(
    /* [in] */ Int32 efid,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // switch(efid) {
    // case EF_IMPI:
    // case EF_IMPU:
    // case EF_DOMAIN:
    // case EF_IST:
    // case EF_PCSCF:
    //     return MF_SIM + DF_ADF;
    // }
    // String path = getCommonIccEFPath(efid);
    // return path;
    assert(0);
    return NOERROR;
}

ECode IsimFileHandler::Logd(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, msg);
    assert(0);
    return NOERROR;
}

ECode IsimFileHandler::Loge(
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
