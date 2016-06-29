#ifndef __ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKPRESENTER_H__
#define __ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKPRESENTER_H__

#include "_Elastos_Apps_Dialer_Voicemail_CVoicemailPlaybackPresenter.h"
#include "elastos/apps/dialer/voicemail/VoicemailPlaybackPresenter.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Voicemail {

CarClass(CVoicemailPlaybackPresenter)
    , public VoicemailPlaybackPresenter
{
public:
    CAR_OBJECT_DECL();
};

} // Voicemail
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKPRESENTER_H__
