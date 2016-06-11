#ifndef __ELASTOS_APPS_DIALER_DIALERBACKUPAGENT_H__
#define __ELASTOS_APPS_DIALER_DIALERBACKUPAGENT_H__

#include "elastos/droid/app/backup/BackupAgentHelper.h"

using Elastos::Droid::App::Backup::BackupAgentHelper;

namespace Elastos{
namespace Apps{
namespace Dialer{

/**
 * The Dialer backup agent backs up the shared preferences settings of the
 * Dialer App. Right now it backs up the whole shared preference file. This
 * can be modified in the future to accommodate partical backup.
 */
class DialerBackupAgent
    : public BackupAgentHelper
    , public IDialerBackupAgent
{
public:
    CAR_INTERFACE_DECL()

    //@Override
    CARAPI OnCreate();

private:
    static const String SHARED_KEY; // = "shared_pref";
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALERBACKUPAGENT_H__
