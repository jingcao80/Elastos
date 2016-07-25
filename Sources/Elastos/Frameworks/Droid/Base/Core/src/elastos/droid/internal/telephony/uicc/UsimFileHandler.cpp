
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UsimFileHandler.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           UsimFileHandler
//=====================================================================
CAR_INTERFACE_IMPL(UsimFileHandler, IccFileHandler, IUsimFileHandler);

const String UsimFileHandler::LOGTAG("UsimFH");

UsimFileHandler::UsimFileHandler()
{
}

ECode UsimFileHandler::constructor(
    /* [in] */ IUiccCardApplication* app,
    /* [in] */ const String& aid,
    /* [in] */ ICommandsInterface* ci)
{
    IccFileHandler::constructor(app, aid, ci);
    return NOERROR;
}

String UsimFileHandler::GetEFPath(
    /* [in] */ Int32 efid)
{
    switch(efid) {
        case EF_SMS:
        case EF_EXT6:
        case EF_EXT5:
        case EF_MWIS:
        case EF_MBI:
        case EF_SPN:
        case EF_AD:
        case EF_MBDN:
        case EF_PNN:
        case EF_OPL:
        case EF_SPDI:
        case EF_SST:
        case EF_CFIS:
        case EF_MAILBOX_CPHS:
        case EF_VOICE_MAIL_INDICATOR_CPHS:
        case EF_CFF_CPHS:
        case EF_SPN_CPHS:
        case EF_SPN_SHORT_CPHS:
        case EF_FDN:
        case EF_MSISDN:
        case EF_EXT2:
        case EF_INFO_CPHS:
        case EF_CSP_CPHS:
        case EF_GID1:
        case EF_LI:
        case EF_PLMNWACT: {
            return MF_SIM + DF_ADF;
        }

        case EF_PBR: {
            // we only support global phonebook.
            return MF_SIM + DF_TELECOM + DF_PHONEBOOK;
        }
    }
    String path = GetCommonIccEFPath(efid);
    if (path == NULL) {
        // The EFids in USIM phone book entries are decided by the card manufacturer.
        // So if we don't match any of the cases above and if its a USIM return
        // the phone book path.
        return MF_SIM + DF_TELECOM + DF_PHONEBOOK;
    }
    return path;
}

ECode UsimFileHandler::Logd(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, msg);
    return NOERROR;
}

ECode UsimFileHandler::Loge(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, msg);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
