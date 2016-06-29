#ifndef __ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPL_H__
#define __ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPL_H__

#include "_Elastos_Apps_Dialer_Voicemail_CVoicemailStatusHelperImpl.h"
#include "elastos/apps/dialer/voicemail/VoicemailStatusHelperImpl.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Voicemail {

CarClass(CVoicemailStatusHelperImpl)
    , public VoicemailStatusHelperImpl
{
public:
    CAR_OBJECT_DECL();
};

} // Voicemail
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPL_H__
