#ifndef __ELASTOS_APPS_DIALER_CDIALERBACKUPAGENT_H__
#define __ELASTOS_APPS_DIALER_CDIALERBACKUPAGENT_H__

#include "_Elastos_Apps_Dialer_CDialerBackupAgent.h"
#include "elastos/apps/dialer/DialerBackupAgent.h"

namespace Elastos {
namespace Apps {
namespace Dialer {

CarClass(CDialerBackupAgent)
    , public DialerBackupAgent
{
public:
    CAR_OBJECT_DECL();
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CDIALERBACKUPAGENT_H__
