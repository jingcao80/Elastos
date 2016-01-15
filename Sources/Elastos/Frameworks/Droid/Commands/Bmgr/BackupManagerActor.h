#ifndef __BACKUPMANAGERDEMO_H__
#define __BACKUPMANAGERDEMO_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos.h>
#include <elautoptr.h>
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.Core.h>
#include <elastos/core/Object.h>
#include <bmgr.h>

using namespace Elastos;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::App::Backup::IIRestoreSession;
using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::App::Backup::IIRestoreObserver;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Bmgr {

class BackupManagerActor
    :public ElRefBase
{
public:
    BackupManagerActor();
    ~BackupManagerActor();

    void Init();

    //do Enabled
    void DoEnabled();
    void DoEnable(Boolean isEnable);
    void DoBackup(const String& pkg);
    void DoTransport(const String& transportName);
    void DoListTransport();
    void DoRun();
    void DoRestoreAll();

    void DoRestorePackage(String &pkgName);

private:
    AutoPtr<IIBackupManager> mBackupManager;
    AutoPtr<ArrayOf<String> > mTransports;
    AutoPtr<IIRestoreSession> mRestoreSession;
};

}
}
}
}
#endif
