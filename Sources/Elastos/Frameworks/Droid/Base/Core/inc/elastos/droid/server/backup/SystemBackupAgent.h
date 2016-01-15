#ifndef __ELASTOS_DROID_SERVER_BACKUP_SYSTEMBACKUPAGENT_H__
#define __ELASTOS_DROID_SERVER_BACKUP_SYSTEMBACKUPAGENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
// #include "elastos/droid/app/backup/BackupAgentHelper.h"
#include <elastos/core/Object.h>

// using Elastos::Droid::App::Backup::BackupAgentHelper;
using Elastos::Droid::App::Backup::IBackupAgentHelper;
using Elastos::Droid::App::Backup::IBackupDataInput;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::App::Backup::IBackupHelper;
using Elastos::Droid::App::Backup::IBackupHelperDispatcher;
using Elastos::Droid::App::Backup::IFullBackupDataOutput;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Backup {

/**
 * Backup agent for various system-managed data, currently just the system wallpaper
 */
class SystemBackupAgent
    // : public BackupAgentHelper
    : public Object
    , public IBackupAgentHelper
{
public:
    CAR_INTERFACE_DECL()

    SystemBackupAgent();

    CARAPI constructor();

    // @Override
    CARAPI OnBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState);

    // @Override
    CARAPI OnFullBackup(
        /* [in] */ IFullBackupDataOutput* data);

    // @Override
    CARAPI OnRestore(
        /* [in] */ IBackupDataInput* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState);

    // @Override
    CARAPI OnRestoreFile(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int64 size,
        /* [in] */ Int32 type,
        /* [in] */ const String& domain,
        /* [in] */ const String& path,
        /* [in] */ Int64 mode,
        /* [in] */ Int64 mtime);

/////////////////TODO////////////////////////////////after BackupAgentHelper get support, remove these.
    CARAPI GetDispatcher(
            /* [out] */ IBackupHelperDispatcher** dispatcher)
    {
        return NOERROR;
    }

    CARAPI AddHelper(
        /* [in] */ const String& keyPrefix,
        /* [in] */ IBackupHelper* helper)
    {
        return NOERROR;
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    CARAPI FullWallpaperBackup(
        /* [in] */ IFullBackupDataOutput* output);

private:
    const static String TAG;

    // These paths must match what the WallpaperManagerService uses.  The leaf *_FILENAME
    // are also used in the full-backup file format, so must not change unless steps are
    // taken to support the legacy backed-up datasets.
    const static String WALLPAPER_IMAGE_FILENAME;
    const static String WALLPAPER_INFO_FILENAME;

    // TODO: Will need to change if backing up non-primary user's wallpaper
    const static String WALLPAPER_IMAGE_DIR;
    const static String WALLPAPER_IMAGE;

    // TODO: Will need to change if backing up non-primary user's wallpaper
    const static String WALLPAPER_INFO_DIR;
    const static String WALLPAPER_INFO;
    // Use old keys to keep legacy data compatibility and avoid writing two wallpapers
    const static String WALLPAPER_IMAGE_KEY;
    const static String WALLPAPER_INFO_KEY;

};

} // namespace Backup
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_BACKUP_SYSTEMBACKUPAGENT_H__