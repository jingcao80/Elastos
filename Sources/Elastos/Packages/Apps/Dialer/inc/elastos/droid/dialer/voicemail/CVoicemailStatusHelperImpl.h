#ifndef __ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPL_H__
#define __ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPL_H__

#include "_Elastos_Droid_Dialer_Voicemail_CVoicemailStatusHelperImpl.h"
#include "elastos/apps/dialer/voicemail/VoicemailStatusHelperImpl.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPL_H__
