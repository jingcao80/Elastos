#ifndef __ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKFRAGMENT_H__

#include "_Elastos_Apps_Dialer_Voicemail_CVoicemailPlaybackFragment.h"
#include "VoicemailPlaybackFragment.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Voicemail {

CarClass(CVoicemailPlaybackFragment)
    , public VoicemailPlaybackFragment
{
public:
    CAR_OBJECT_DECL()
};

} // Voicemail
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILPLAYBACKFRAGMENT_H__
