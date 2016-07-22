
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/SIMFileHandler.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                            SIMFileHandler
//=====================================================================
CAR_INTERFACE_IMPL(SIMFileHandler, IccFileHandler, ISIMFileHandler);

const String SIMFileHandler::LOGTAG("SIMFileHandler");

SIMFileHandler::SIMFileHandler()
{
}

ECode SIMFileHandler::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ const String& aid,
    /* [in] */ ICommandsInterface* ci)
{
    return IccFileHandler::constructor(app, aid, ci);
}

String SIMFileHandler::GetEFPath(
    /* [in] */ Int32 efid)
{
    // TODO(): DF_GSM can be 7F20 or 7F21 to handle backward compatibility.
    // Implement this after discussion with OEMs.
    switch(efid) {
    case EF_SMS:
        return MF_SIM + DF_TELECOM;

    case EF_EXT6:
    case EF_MWIS:
    case EF_MBI:
    case EF_SPN:
    case EF_AD:
    case EF_MBDN:
    case EF_PNN:
    case EF_SPDI:
    case EF_SST:
    case EF_CFIS:
    case EF_GID1:
        return MF_SIM + DF_GSM;

    case EF_MAILBOX_CPHS:
    case EF_VOICE_MAIL_INDICATOR_CPHS:
    case EF_CFF_CPHS:
    case EF_SPN_CPHS:
    case EF_SPN_SHORT_CPHS:
    case EF_INFO_CPHS:
    case EF_CSP_CPHS:
    case EF_PLMNWACT:
        return MF_SIM + DF_GSM;
    }
    String path = GetCommonIccEFPath(efid);
    if (path.IsNull()) {
        Logger::E(LOGTAG, String("Error: EF Path being returned in null"));
    }
    return path;
}

ECode SIMFileHandler::Logd(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, msg);
    return NOERROR;
}

ECode SIMFileHandler::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(LOGTAG, msg);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
