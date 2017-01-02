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

#ifndef __ELASTOS_DROID_APP_BACKUP_CSHAREDPREFERENCESBACKUPHELPER_H__
#define __ELASTOS_DROID_APP_BACKUP_CSHAREDPREFERENCESBACKUPHELPER_H__

#include "_Elastos_Droid_App_Backup_CSharedPreferencesBackupHelper.h"
#include "elastos/droid/app/backup/FileBackupHelperBase.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CSharedPreferencesBackupHelper), public FileBackupHelperBase
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @hide */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<String>* prefGroups);

    CARAPI PerformBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState);

    CARAPI RestoreEntity(
        /* [in] */ IBackupDataInputStream* data);

    CARAPI WriteNewStateDescription(
        /* [in] */ IParcelFileDescriptor* fd);

private:
    static const String TAG;
    static const Boolean DEBUG;
    AutoPtr<IContext> mContext;
    AutoPtr<ArrayOf<String> > mPrefGroups;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_APP_BACKUP_CSHAREDPREFERENCESBACKUPHELPER_H__
