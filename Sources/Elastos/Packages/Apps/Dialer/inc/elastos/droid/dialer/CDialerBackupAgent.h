#ifndef __ELASTOS_DROID_DIALER_CDIALERBACKUPAGENT_H__
#define __ELASTOS_DROID_DIALER_CDIALERBACKUPAGENT_H__

#include "_Elastos_Droid_Dialer_CDialerBackupAgent.h"
#include "elastos/apps/dialer/DialerBackupAgent.h"

namespace Elastos {
namespace Droid {
namespace Dialer {

CarClass(CDialerBackupAgent)
    , public DialerBackupAgent
{
public:
    CAR_OBJECT_DECL();
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CDIALERBACKUPAGENT_H__
