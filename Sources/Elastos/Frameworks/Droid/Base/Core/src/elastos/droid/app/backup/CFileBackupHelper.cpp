#include "elastos/droid/app/backup/CFileBackupHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String CFileBackupHelper::TAG("CFileBackupHelper");
const Boolean CFileBackupHelper::DEBUG = FALSE;
ECode CFileBackupHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<String>* files)
{
    FileBackupHelperBase::Init(context);
    mContext = context;
    context->GetFilesDir((IFile**)&mFileDir);
    mFiles = files;
    return NOERROR;
}

ECode CFileBackupHelper::PerformBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{

    AutoPtr<ArrayOf<String> > files = mFiles;
    AutoPtr<IFile> base;
    mContext->GetFilesDir((IFile**)&base);
    Int32 N = files->GetLength();
    AutoPtr<ArrayOf<String> > fullPaths = ArrayOf<String>::Alloc(N);
    for(Int32 i = 0; i < N; ++i) {
        AutoPtr<IFile> file;
        CFile::New(base, (*files)[i], (IFile**)&file);
        String filePath;
        file->GetAbsolutePath(&filePath);
        (*fullPaths)[i] = filePath;
        if (DEBUG) {
            Logger::D(TAG, "PerformBackup fullPaths:%s", filePath.string());
        }
    }

    //go
    return PerformBackupChecked(oldState, data, newState, fullPaths, files);
}

ECode CFileBackupHelper::RestoreEntity(
    /* [in] */ IBackupDataInputStream* data)
{
    String key;
    data->GetKey(&key);
    if (DEBUG){
        Int32 size;
        data->GetSize(&size);
        Logger::D(TAG, "got entity %s size=%d", key.string(), size);
    }

    if (IsKeyInList(key, mFiles)) {
        AutoPtr<IFile> file;
        CFile::New(mFileDir, key, (IFile**)&file);
        WriteFile(file, data);
    }
    return NOERROR;
}

ECode CFileBackupHelper::WriteNewStateDescription(
    /* [in] */ IParcelFileDescriptor* fd)
{
    FileBackupHelperBase::WriteNewStateDescription(fd);
    return NOERROR;
}

}
}
}
}
