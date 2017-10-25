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

#ifndef __ELASTOS_DROID_SERVER_AM_BACKUPRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_BACKUPRECORD_H__

#include "elastos/droid/server/am/ProcessRecord.h"

using Elastos::Droid::Internal::Os::IBatteryStatsImplUidPkgServ;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class BackupRecord : public Object
{
public:
    BackupRecord(
        /* [in] */ IBatteryStatsImplUidPkgServ* agentStats,
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ Int32 backupMode)
        : mStats(agentStats)
        , mAppInfo(appInfo)
        , mBackupMode(backupMode)
    {}

    using Object::ToString;

    CARAPI_(String) ToString();

public:
    // backup/restore modes
    static const Int32 BACKUP_NORMAL = 0;
    static const Int32 BACKUP_FULL = 1;
    static const Int32 RESTORE = 2;
    static const Int32 RESTORE_FULL = 3;

    AutoPtr<IBatteryStatsImplUidPkgServ> mStats;
    String mStringName;                     // cached toString() output
    AutoPtr<IApplicationInfo> mAppInfo;     // information about BackupAgent's app
    Int32 mBackupMode;                      // full backup / incremental / restore
    AutoPtr<ProcessRecord> mApp;            // where this agent is running or null
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_BACKUPRECORD_H__
