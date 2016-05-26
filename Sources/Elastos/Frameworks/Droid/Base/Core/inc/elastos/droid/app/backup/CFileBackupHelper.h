#ifndef __ELASTOS_DROID_APP_BACKUP_CFILEBACKUPHELPER_H__
#define __ELASTOS_DROID_APP_BACKUP_CFILEBACKUPHELPER_H__

#include "_Elastos_Droid_App_Backup_CFileBackupHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/backup/FileBackupHelperBase.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CFileBackupHelper), public FileBackupHelperBase
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @hide */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<String>* files);

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
    AutoPtr<IFile> mFileDir;
    AutoPtr<ArrayOf<String> > mFiles;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_APP_BACKUP_CFILEBACKUPHELPER_H__
