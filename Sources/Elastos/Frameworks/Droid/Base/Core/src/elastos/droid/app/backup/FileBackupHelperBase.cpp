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

#include "elastos/droid/app/backup/FileBackupHelperBase.h"
#include "elastos/droid/app/backup/CBackupDataOutput.h"
#include <elastos/utility/logging/Logger.h>
#include <androidfw/BackupHelpers.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String FileBackupHelperBase::TAG("FileBackupHelperBase");

CAR_INTERFACE_IMPL(FileBackupHelperBase, Object, IFileBackupHelperBase)

FileBackupHelperBase::FileBackupHelperBase()
{
    mExceptionLogged = FALSE;
}

FileBackupHelperBase::~FileBackupHelperBase()
{
    NativeDtor();
}

ECode FileBackupHelperBase::Init(
    /* [in] */ IContext* constext) {

    mPtr = NativeCtor();
    mContext = constext;
    return NOERROR;
}

ECode FileBackupHelperBase::WriteNewStateDescription(
    /* [in] */ IParcelFileDescriptor* fd)
{
    AutoPtr<IFileDescriptor> fd1;
    fd->GetFileDescriptor((IFileDescriptor**)&fd1);
    int result = NativeWriteSnapshot(fd1);
    // TODO: Do something with the error.
    return NOERROR;
}

Boolean FileBackupHelperBase::IsKeyInList(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<String>* list)
{
    Int32 len = list->GetLength();
    for (Int32 i = 0; i < len; ++i) {
        if (key.Equals((*list)[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean FileBackupHelperBase::WriteFile(
    /* [in] */ IFile* f,
    /* [in] */ IBackupDataInputStream* in)
{
    Int32 result = -1;
    // Create the enclosing directory.
    AutoPtr<IFile> parent;
    f->GetParentFile((IFile**)&parent);
    Boolean successed;
    parent->Mkdirs(&successed);
    String path;
    f->GetAbsolutePath(&path);
    AutoPtr<IBackupDataInput> input;
    in->GetData((IBackupDataInput**)&input);
    Int32 backupReader;
    input->GetBackupReader(&backupReader);
    result = NativeWriteFile(path, backupReader);

    if (result != 0) {
        // Bail on this entity.  Only log one failure per helper object.
        if (!mExceptionLogged) {
            String str;
            f->ToString(&str);
            String packageName;
            mContext->GetPackageName(&packageName);
            Logger::E(TAG, "Failed restoring file '%s' for app '%s' , result[%p]",str.string(), packageName.string(), result );
            mExceptionLogged = TRUE;
        }
    }
    return (result == 0);
}

ECode FileBackupHelperBase::PerformBackupChecked(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState,
    /* [in] */ ArrayOf<String>* files,
    /* [in] */ ArrayOf<String>* keys)
{
    if (files->GetLength() == 0) {
        return NOERROR;
    }

    // files must be all absolute paths
    Int32 len = files->GetLength();
    for (Int32 i = 0; i < len; ++i) {
        if (((*files)[i]).GetChar(0) != '/') {
            Logger::E(TAG, "files must have all absolute paths: %s", ((*files)[i]).string());
            return E_RUNTIME_EXCEPTION;
        }
    }

    // the length of files and keys must be the same
    if (files->GetLength() != keys->GetLength()) {
        Logger::E(TAG, "file len = %d, key len = %d", files->GetLength(), keys->GetLength());
        return E_RUNTIME_EXCEPTION;
    }

    // oldStateFd can be null
    AutoPtr<IFileDescriptor> oldStateFd;
    if (oldState != NULL) {
        oldState->GetFileDescriptor((IFileDescriptor**)&oldStateFd);
    }

    AutoPtr<IFileDescriptor> newStateFd;
    newState->GetFileDescriptor((IFileDescriptor**)&newStateFd);
    if(newStateFd == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 backupdata = ((CBackupDataOutput*)data)->GetNativeBackupDataWriter();
    Int32 err = NativePerformBackup(oldStateFd, backupdata, newStateFd, files, keys);
    if (err != 0) {
        Logger::E(TAG, "Backup failed %p", err);
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

Int32 FileBackupHelperBase::NativeCtor()
{

    return (Int32)(new android::RestoreHelperBase());
}

void FileBackupHelperBase::NativeDtor()
{
    delete (android::RestoreHelperBase*)mPtr;
}

Int32 FileBackupHelperBase::NativePerformBackup(
    /* [in] */ IFileDescriptor* oldState,
    /* [in] */ Int32 data,
    /* [in] */ IFileDescriptor* newState,
    /* [in] */ ArrayOf<String>* files,
    /* [in] */ ArrayOf<String>* keys)
{
    android::BackupDataWriter* writer = (android::BackupDataWriter*)data;
    Int32 fileCount = files->GetLength();
    char const** filesUTF = (char const**)malloc(sizeof(char*)*fileCount);
    for(Int32 i = 0; i < fileCount; ++i) {
        filesUTF[i] = ((*files)[i]).string();
    }

    Int32 keyCount = keys->GetLength();
    char const** keyUTF = (char const**)malloc(sizeof(char*)*keyCount);
    for(Int32 i = 0; i < keyCount; ++i) {
        keyUTF[i] = ((*keys)[i]).string();
    }

    Int32 oldStateFd, newStateFd;
    oldState->GetDescriptor(&oldStateFd);
    newState->GetDescriptor(&newStateFd);
    int err = back_up_files(oldStateFd, writer, newStateFd, filesUTF, keyUTF, fileCount);

    free(keyUTF);
    free(filesUTF);
    return err;
}

Int32 FileBackupHelperBase::NativeWriteFile(
    /* [in] */ const String& fileName,
    /* [in] */ Int32 backupReader)
{
    android::RestoreHelperBase* restore = (android::RestoreHelperBase*)mPtr;
    android::BackupDataReader* reader = (android::BackupDataReader*)backupReader;
    char const* filename = fileName.string();
    int err = restore->WriteFile(android::String8(filename), reader);
    return err;
}

Int32 FileBackupHelperBase::NativeWriteSnapshot(
    /* [in] */ IFileDescriptor* fd)
{
    android::RestoreHelperBase* restore = (android::RestoreHelperBase*)mPtr;
    int tempFd;
    fd->GetDescriptor(&tempFd);
    int err = restore->WriteSnapshot(tempFd);

    return err;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
