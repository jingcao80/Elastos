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

#ifndef __ELASTOS_DROID_APP_BACKUP_CFULLBACKUPDATAOUTPUT_H__
#define __ELASTOS_DROID_APP_BACKUP_CFULLBACKUPDATAOUTPUT_H__

#include "_Elastos_Droid_App_Backup_CFullBackupDataOutput.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CFullBackupDataOutput)
    , public Object
    , public IFullBackupDataOutput
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IParcelFileDescriptor* fd);

    CARAPI GetData(
        /* [out] */ IBackupDataOutput** data);

private:
    // Currently a name-scoping shim around BackupDataOutput
    AutoPtr<IBackupDataOutput> mData;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_BACKUP_CFULLBACKUPDATAOUTPUT_H__
