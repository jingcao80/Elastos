#include "elastos/apps/dialer/DialerBackupAgent.h"
#include "Elastos.Droid.App.h"

using Elastos::Droid::App::Backup::ISharedPreferencesBackupHelper;
using Elastos::Droid::App::Backup::CSharedPreferencesBackupHelper;

namespace Elastos {
namespace Apps {
namespace Dialer {

const String DialerBackupAgent::SHARED_KEY("shared_pref");

ECode DialerBackupAgent::OnCreate()
{
    AutoPtr<ISharedPreferencesBackupHelper> helper;
    CSharedPreferencesBackupHelper::New(this,
            IDialtactsActivity::SHARED_PREFS_NAME, (ISharedPreferencesBackupHelper**)&helper);
    AddHelper(SHARED_KEY, helper);
    return NOERROR;
}

} // Dialer
} // Apps
} // Elastos
