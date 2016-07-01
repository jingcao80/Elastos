#ifndef __ELASTOS_DROID_DIALER_DIALERBACKUPAGENT_H__
#define __ELASTOS_DROID_DIALER_DIALERBACKUPAGENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/app/backup/BackupAgentHelper.h"

using Elastos::Droid::App::Backup::BackupAgentHelper;

namespace Elastos {
namespace Droid {
namespace Dialer {

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
    CAR_INTERFACE_DECL();

    //@Override
    CARAPI OnCreate();

private:
    static const String SHARED_KEY; // = "shared_pref";
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALERBACKUPAGENT_H__
