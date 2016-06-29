#ifndef __ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPLHELPER_H__
#define __ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPLHELPER_H__

#include "_Elastos_Apps_Dialer_Voicemail_CVoicemailStatusHelperImplHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Voicemail {

CarClass(CVoicemailStatusHelperImplHelper)
    , public Singleton
    , public IVoicemailStatusHelperImplHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI GetPROJECTION(
        /* [out, callee] */ ArrayOf<String>** protection);
};

} // Voicemail
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_VOICEMAIL_CVOICEMAILSTATUSHELPERIMPLHELPER_H__
