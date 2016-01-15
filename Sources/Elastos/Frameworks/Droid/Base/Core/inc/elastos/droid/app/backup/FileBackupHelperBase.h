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
