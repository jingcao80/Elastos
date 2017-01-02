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

#include "elastos/droid/app/backup/CSharedPreferencesBackupHelper.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/app/QueuedWork.h"

using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::QueuedWork;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String CSharedPreferencesBackupHelper::TAG("CFileBackupHelper");
const Boolean CSharedPreferencesBackupHelper::DEBUG = TRUE;
ECode CSharedPreferencesBackupHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<String>* prefGroups)
{
    FileBackupHelperBase::Init(context);
    mContext = context;
    mPrefGroups = prefGroups;
    return NOERROR;
}

ECode CSharedPreferencesBackupHelper::PerformBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    AutoPtr<IContext> context = mContext;

    // If a SharedPreference has an outstanding write in flight,
    // wait for it to finish flushing to disk.
    QueuedWork::WaitToFinish();

    //make filenames for the prefGroups
    AutoPtr<ArrayOf<String> > prefGroups = mPrefGroups;
    Int32 N = prefGroups->GetLength();
    AutoPtr<ArrayOf<String> > files = ArrayOf<String>::Alloc(N);
    for ( Int32 i = 0; i < N; ++i ) {
        AutoPtr<IFile> file;
        context->GetSharedPrefsFile((*prefGroups)[i], (IFile**)&file);
        file->GetAbsolutePath(&((*files)[i]));
    }

    //go
    return PerformBackupChecked(oldState, data, newState, files, prefGroups);
}

ECode CSharedPreferencesBackupHelper::RestoreEntity(
    /* [in] */ IBackupDataInputStream* data)
{
    String key;
    data->GetKey(&key);
    if (DEBUG){
        Int32 size;
        data->GetSize(&size);
        Logger::D(TAG, "got entity %s size=%d", key.string(), size);
    }

    if (IsKeyInList(key, mPrefGroups)) {
        AutoPtr<IFile> file;
        mContext->GetSharedPrefsFile(key, (IFile**)&file);
        AutoPtr<IFile> absoluteFile;
        file->GetAbsoluteFile((IFile**)&absoluteFile);
        WriteFile(absoluteFile, data);
    }
    return NOERROR;
}

ECode CSharedPreferencesBackupHelper::WriteNewStateDescription(
    /* [in] */ IParcelFileDescriptor* fd)
{
    FileBackupHelperBase::WriteNewStateDescription(fd);
    return NOERROR;
}

}
}
}
}
