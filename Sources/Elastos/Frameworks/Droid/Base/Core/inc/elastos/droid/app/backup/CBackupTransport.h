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

#ifndef __ELASTOS_DROID_APP_BACKUP_CBACKUPTRANSPORT_H__
#define __ELASTOS_DROID_APP_BACKUP_CBACKUPTRANSPORT_H__

#include "_Elastos_Droid_App_Backup_CBackupTransport.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CBackupTransport)
    , public Object
    , public IBackupTransport
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetBinder(
        /* [out] */ IBinder** binder);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetConfigurationIntent(
        /* [out] */ IIntent** intent);

    CARAPI GetCurrentDestinationString(
        /* [out] */ String* str);

    CARAPI GetDataManagementIntent(
        /* [out] */ IIntent** intent);

    CARAPI GetDataManagementLabel(
        /* [out] */ String* label);

    CARAPI GetTransportDirName(
        /* [out] */ String* name);

    CARAPI InitializeDevice(
        /* [out] */ Int32* result);

    CARAPI ClearBackupData(
        /* [in] */ IPackageInfo* packageInfo,
        /* [out] */ Int32* result);

    CARAPI FinishBackup(
        /* [out] */ Int32* result);

    CARAPI RequestBackupTime(
        /* [out] */ Int64* time);

    CARAPI PerformBackup(
        /* [in] */ IPackageInfo* packageInfo,
        /* [in] */ IParcelFileDescriptor* inFd,
        /* [out] */ Int32* result);

    CARAPI GetAvailableRestoreSets(
        /* [out, callee] */ ArrayOf<IRestoreSet*>** sets);

    CARAPI GetCurrentRestoreSet(
        /* [out] */ Int64* result);

    CARAPI StartRestore(
        /* [in] */ Int64 token,
        /* [in] */ ArrayOf<IPackageInfo*>* packages,
        /* [out] */ Int32* result);

    CARAPI GetNextRestorePackage(
        /* [out] */ IRestoreDescription** rd);

    CARAPI GetRestoreData(
        /* [in] */ IParcelFileDescriptor* outFd,
        /* [out] */ Int32* result);

    CARAPI FinishRestore();

    CARAPI RequestFullBackupTime(
        /* [out] */ Int64* result);

    CARAPI PerformFullBackup(
        /* [in] */ IPackageInfo* targetPackage,
        /* [in] */ IParcelFileDescriptor* socket,
        /* [out] */ Int32* result);

    CARAPI SendBackupData(
        /* [in] */ Int32 numBytes,
        /* [out] */ Int32* result);

    CARAPI CancelFullBackup();

    CARAPI GetNextFullRestoreDataChunk(
        /* [in] */ IParcelFileDescriptor* socket,
        /* [out] */ Int32* result);

    CARAPI AbortFullRestore(
        /* [out] */ Int32* result);
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_APP_BACKUP_CBACKUPTRANSPORT_H__
