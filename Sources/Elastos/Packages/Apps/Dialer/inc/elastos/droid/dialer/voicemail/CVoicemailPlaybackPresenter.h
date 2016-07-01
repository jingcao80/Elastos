#ifndef __ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKPRESENTER_H__
#define __ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKPRESENTER_H__

#include "_Elastos_Droid_Dialer_Voicemail_CVoicemailPlaybackPresenter.h"
#include "elastos/apps/dialer/voicemail/VoicemailPlaybackPresenter.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKPRESENTER_H__
