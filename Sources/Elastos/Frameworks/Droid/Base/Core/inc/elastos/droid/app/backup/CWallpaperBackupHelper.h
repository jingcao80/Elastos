#ifndef __ELASTOS_DROID_APP_BACKUP_CWALLPAPERBACKUPHELPER_H__
#define __ELASTOS_DROID_APP_BACKUP_CWALLPAPERBACKUPHELPER_H__

#include "_Elastos_Droid_App_Backup_CWallpaperBackupHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/backup/FileBackupHelperBase.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CWallpaperBackupHelper), public FileBackupHelperBase
{
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWallpaperBackupHelper();

    ~CWallpaperBackupHelper();

    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ ArrayOf<String>* files,
        /* [in] */ ArrayOf<String>* keys);

        /**
         * Based on oldState, determine which of the files from the application's data directory
         * need to be backed up, write them to the data stream, and fill in newState with the
         * state as it exists now.
         */
    CARAPI PerformBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState);

        /**
         * Restore one absolute file entity from the restore stream.  If we're restoring the
         * magic wallpaper file, take specific action to determine whether it is suitable for
         * the current device.
         */
    CARAPI RestoreEntity(
        /* [in] */ IBackupDataInputStream* data);

    CARAPI WriteNewStateDescription(
        /* [in] */ IParcelFileDescriptor* fd);

public:
    static CARAPI_(String) GetWALLPAPER_IMAGE();
    static CARAPI_(String) GetWALLPAPER_INFO();
    static CARAPI_(String) GetSTAGE_FILE();

private:
    // This path must match what the WallpaperManagerService uses
    // TODO: Will need to change if backing up non-primary user's wallpaper
    static String WALLPAPER_IMAGE;
    static String WALLPAPER_INFO;

    static const String TAG;
    static const Boolean DEBUG;
    static String STAGE_FILE;

    AutoPtr<IContext> mContext;
    AutoPtr<ArrayOf<String> > mFiles;
    AutoPtr<ArrayOf<String> > mKeys;
    Double mDesiredMinWidth;
    Double mDesiredMinHeight;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_BACKUP_CWALLPAPERBACKUPHELPER_H__
