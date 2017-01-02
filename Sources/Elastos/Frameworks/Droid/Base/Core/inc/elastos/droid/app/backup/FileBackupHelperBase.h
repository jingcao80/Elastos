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

#ifndef __ELASTOS_DROID_APP_BACKUP_FILEBACKUPHELPERBASE_H__
#define __ELASTOS_DROID_APP_BACKUP_FILEBACKUPHELPERBASE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

class FileBackupHelperBase
    : public Object
    , public IFileBackupHelperBase
{
public:
    CAR_INTERFACE_DECL()

    FileBackupHelperBase();

    virtual ~FileBackupHelperBase();

    CARAPI Init(
    /* [in] */ IContext* constext);

    CARAPI WriteNewStateDescription(
        /* [in] */ IParcelFileDescriptor* fd);

protected:
    CARAPI_(Boolean) IsKeyInList(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<String>* list);

    CARAPI_(Boolean) WriteFile(
        /* [in] */ IFile* f,
        /* [in] */ IBackupDataInputStream* in);

    static CARAPI PerformBackupChecked(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ ArrayOf<String>* files,
        /* [in] */ ArrayOf<String>* keys);

    CARAPI_(Int32) NativeCtor();

    CARAPI_(void) NativeDtor();

    static CARAPI_(Int32) NativePerformBackup(
        /* [in] */ IFileDescriptor* oldState,
        /* [in] */ Int32 data,
        /* [in] */ IFileDescriptor* newState,
        /* [in] */ ArrayOf<String>* files,
        /* [in] */ ArrayOf<String>* keys);

    CARAPI_(Int32) NativeWriteFile(
        /* [in] */ const String& fileName,
        /* [in] */ Int32 backupReader);

    CARAPI_(Int32) NativeWriteSnapshot(
        /* [in] */ IFileDescriptor* fd);

private:
    static const String TAG;
    Int32 mPtr;
    AutoPtr<IContext> mContext;
    Boolean mExceptionLogged;
};
} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
#endif
