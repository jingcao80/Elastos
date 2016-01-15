#include "elastos/droid/app/backup/CAbsoluteFileBackupHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String CAbsoluteFileBackupHelper::TAG("CAbsoluteFileBackupHelper");
const Boolean CAbsoluteFileBackupHelper::DEBUG = TRUE;

ECode CAbsoluteFileBackupHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<String>* files)
{
    FileBackupHelperBase::Init(context);
    mContext = context;
    mFiles = files;
    return NOERROR;
}

ECode CAbsoluteFileBackupHelper::PerformBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    // use the file paths as the keys, too
    return PerformBackupChecked(oldState, data, newState, mFiles, mFiles);
}

ECode CAbsoluteFileBackupHelper::RestoreEntity(
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
        CFile::New(key, (IFile**)&file);
        WriteFile(file, data);
    }
    return NOERROR;
}

ECode CAbsoluteFileBackupHelper::WriteNewStateDescription(
    /* [in] */ IParcelFileDescriptor* fd)
{
    FileBackupHelperBase::WriteNewStateDescription(fd);
    return NOERROR;
}

}
}
}
}
