#ifndef __ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKFRAGMENT_H__

#include "_Elastos_Droid_Dialer_Voicemail_CVoicemailPlaybackFragment.h"
#include "elastos/apps/dialer/voicemail/VoicemailPlaybackFragment.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Voicemail {

CarClass(CVoicemailPlaybackFragment)
    , public VoicemailPlaybackFragment
{
public:
    CAR_OBJECT_DECL();
};

} // Voicemail
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKFRAGMENT_H__
