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

#ifndef __ELASTOS_DROID_APP_CBACKUPAGENTBACKUPSERVICEBINDER_H__
#define __ELASTOS_DROID_APP_CBACKUPAGENTBACKUPSERVICEBINDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_App_CBackupAgentBackupServiceBinder.h"
#include "backup/BackupAgent.h"

using Elastos::Droid::App::Backup::BackupAgent;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CBackupAgentBackupServiceBinder)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()


    CARAPI constructor(
        /* [in] */ HANDLE host);

    CARAPI DoBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestore(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoFullBackup(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestoreFile(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int64 size,
        /* [in] */ Int32 type,
        /* [in] */ const String& domainName,
        /* [in] */ const String& path,
        /* [in] */ Int64 mode,
        /* [in] */ Int64 mtime,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

private:
    static const String TAG;
    static const Boolean DEBUG;
    BackupAgent* mHost;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CBACKUPAGENTBACKUPSERVICEBINDER_H__
