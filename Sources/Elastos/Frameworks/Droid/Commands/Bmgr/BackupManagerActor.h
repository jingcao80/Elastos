//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
