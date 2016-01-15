#include "elastos/droid/server/backup/SystemBackupAgent.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
// #include "elastos/droid/app/backup/CFullBackup.h"
// #include "elastos/droid/app/backup/CWallpaperBackupHelper.h"
// #include "elastos/droid/app/backup/CWallpaperBackupHelperHelper.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Slogger.h>

// using Elastos::Droid::App::Backup::CFullBackup;
// using Elastos::Droid::App::Backup::CWallpaperBackupHelper;
// using Elastos::Droid::App::Backup::CWallpaperBackupHelperHelper;
using Elastos::Droid::App::Backup::EIID_IBackupAgentHelper;
using Elastos::Droid::App::Backup::EIID_IBackupHelper;
using Elastos::Droid::App::Backup::IFullBackup;
using Elastos::Droid::App::Backup::IWallpaperBackupHelper;
using Elastos::Droid::App::Backup::IWallpaperBackupHelperHelper;
using Elastos::Droid::App::IIWallpaperManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Backup {

static String InitWALLPAPER_IMAGE_DIR()
{
    AutoPtr<IFile> f = Environment::GetUserSystemDirectory(IUserHandle::USER_OWNER);
    String path;
    f->GetAbsolutePath(&path);
    return path;
}

static String InitWALLPAPER_IMAGE()
{
    //TODO wait for CWallpaperBackupHelperHelper
    // AutoPtr<IWallpaperBackupHelperHelper> wbhh;
    // CWallpaperBackupHelperHelper::AcquireSingleton((IWallpaperBackupHelperHelper**)&wbhh);
    // String image;
    // wbhh->GetWALLPAPER_IMAGE(&image);
    // return image;
    return String(NULL);
}

static String InitWALLPAPER_INFO()
{
    //TODO wait for CWallpaperBackupHelperHelper
    // AutoPtr<IWallpaperBackupHelperHelper> wbhh;
    // CWallpaperBackupHelperHelper::AcquireSingleton((IWallpaperBackupHelperHelper**)&wbhh);
    // String info;
    // wbhh->GetWALLPAPER_INFO(&info);
    // return info;
    return String(NULL);
}

const String SystemBackupAgent::TAG("SystemBackupAgent");

const String SystemBackupAgent::WALLPAPER_IMAGE_FILENAME("wallpaper");
const String SystemBackupAgent::WALLPAPER_INFO_FILENAME("wallpaper_info.xml");

const String SystemBackupAgent::WALLPAPER_IMAGE_DIR = InitWALLPAPER_IMAGE_DIR();
const String SystemBackupAgent::WALLPAPER_IMAGE = InitWALLPAPER_IMAGE();

const String SystemBackupAgent::WALLPAPER_INFO_DIR = InitWALLPAPER_IMAGE_DIR();
const String SystemBackupAgent::WALLPAPER_INFO = InitWALLPAPER_INFO();

const String SystemBackupAgent::WALLPAPER_IMAGE_KEY = IWallpaperBackupHelper::WALLPAPER_IMAGE_KEY;
const String SystemBackupAgent::WALLPAPER_INFO_KEY = IWallpaperBackupHelper::WALLPAPER_INFO_KEY;

CAR_INTERFACE_IMPL(SystemBackupAgent, Object, IBackupAgentHelper)

SystemBackupAgent::SystemBackupAgent()
{
}

ECode SystemBackupAgent::constructor()
{
    return NOERROR;
}

ECode SystemBackupAgent::OnBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    // We only back up the data under the current "wallpaper" schema with metadata
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::WALLPAPER_SERVICE);
    AutoPtr<IIWallpaperManager> wallpaper = IIWallpaperManager::Probe(service);
    AutoPtr<ArrayOf<String> > files = ArrayOf<String>::Alloc(2);
    (*files)[0] = WALLPAPER_IMAGE;
    (*files)[1] = WALLPAPER_INFO;
    AutoPtr<ArrayOf<String> > keys = ArrayOf<String>::Alloc(2);
    (*keys)[0] = WALLPAPER_IMAGE_KEY;
    (*keys)[1] = WALLPAPER_INFO_KEY;

    if (wallpaper != NULL) {
        String name;
        ECode ec = wallpaper->GetName(&name);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Couldn't get wallpaper name\n");
            return E_REMOTE_EXCEPTION;
        }
        const String wallpaperName = name;
        if (!wallpaperName.IsNull() && wallpaperName.GetLength() > 0) {
            // When the wallpaper has a name, back up the info by itself.
            // TODO: Don't rely on the innards of the service object like this!
            // TODO: Send a delete for any stored wallpaper image in this case?
            files = ArrayOf<String>::Alloc(1);
            (*files)[0] = WALLPAPER_INFO;
            keys = ArrayOf<String>::Alloc(1);
            (*keys)[0] = WALLPAPER_INFO_KEY;
        }
    }
    //TODO wait for CWallpaperBackupHelper
    // AutoPtr<IBackupHelper> bh;
    // CWallpaperBackupHelper::New((IContext*)this, files.Get(), keys.Get(), (IBackupHelper**)&bh);
    // BackupAgentHelper::AddHelper(String("wallpaper"), bh.Get());
    // BackupAgentHelper::OnBackup(oldState, data, newState);
    return NOERROR;
}

ECode SystemBackupAgent::OnFullBackup(
    /* [in] */ IFullBackupDataOutput* data)
{
    // At present we back up only the wallpaper
    return FullWallpaperBackup(data);
}

ECode SystemBackupAgent::FullWallpaperBackup(
    /* [in] */ IFullBackupDataOutput* output)
{
    // Back up the data files directly.  We do them in this specific order --
    // info file followed by image -- because then we need take no special
    // steps during restore; the restore will happen properly when the individual
    // files are restored piecemeal.

    // TODO wait extends BackupAgentHelper
    // String packageName;
    // (IContext*)this->GetPackageName(&packageName);
    AutoPtr<IBackupDataOutput> data;
    output->GetData((IBackupDataOutput**)&data);
    // Int32 result;
    // AutoPtr<IFullBackup> fb;
    //TODO wait CFullBackup
    // CFullBackup::AcquireSingleton((IFullBackup**)&fb);
    // fb->BackupToTar(packageName, IFullBackup::ROOT_TREE_TOKEN, String(NULL), WALLPAPER_INFO_DIR, WALLPAPER_INFO, data, &result);
    // fb->BackupToTar(packageName, IFullBackup::ROOT_TREE_TOKEN, String(NULL), WALLPAPER_IMAGE_DIR, WALLPAPER_IMAGE, data, &result);
    return NOERROR;
}

ECode SystemBackupAgent::OnRestore(
    /* [in] */ IBackupDataInput* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState)
{
    // On restore, we also support a previous data schema "system_files"
    AutoPtr<ArrayOf<String> > files = ArrayOf<String>::Alloc(2);
    (*files)[0] = WALLPAPER_IMAGE;
    (*files)[1] = WALLPAPER_INFO;
    AutoPtr<ArrayOf<String> > keys = ArrayOf<String>::Alloc(2);
    (*keys)[0] = WALLPAPER_IMAGE_KEY;
    (*keys)[1] = WALLPAPER_INFO_KEY;
    //TODO
    // AutoPtr<IBackupHelper> bh;
    // CWallpaperBackupHelper::New((IContext*)this, files.Get(), keys.Get(), (IBackupHelper**)&bh);
    // BackupAgentHelper::AddHelper(String("wallpaper"), bh.Get());

    AutoPtr<ArrayOf<String> > files2 = ArrayOf<String>::Alloc(1);
    (*files2)[0] = WALLPAPER_IMAGE;
    AutoPtr<ArrayOf<String> > keys2 = ArrayOf<String>::Alloc(1);
    (*keys2)[0] = WALLPAPER_IMAGE_KEY;
    //TODO
    // AutoPtr<IBackupHelper> bh2;
    // CWallpaperBackupHelper::New((IContext*)this, files2.Get(), keys2.Get(), (IBackupHelper**)&bh2);
    // BackupAgentHelper::AddHelper(String("system_files"), bh2.Get());

    //TODO wait BackupAgentHelper
    // ECode ec = BackupAgentHelper::OnRestore(data, appVersionCode, newState);
    // if (FAILED(ec)) {
    //     // If there was a failure, delete everything for the wallpaper, this is too aggressive,
    //     // but this is hopefully a rare failure.
    //     Slogger::D(TAG, "restore failed");
    //     AutoPtr<IFile> f1, f2;
    //     CFile::New(WALLPAPER_IMAGE, (IFile**)&f1);
    //     f1->Delete();
    //     CFile::New(WALLPAPER_INFO, (IFile**)&f2);
    //     f2->Delete();
    //     return E_IO_EXCEPTION;
    // }
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::WALLPAPER_SERVICE);
    AutoPtr<IIWallpaperManager> wallpaper = IIWallpaperManager::Probe(service);
    if (wallpaper != NULL) {
        ECode ec2 = wallpaper->SettingsRestored();
        if (FAILED(ec2)) {
            Slogger::E(TAG, "Couldn't restore settings\n");
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode SystemBackupAgent::OnRestoreFile(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int64 size,
    /* [in] */ Int32 type,
    /* [in] */ const String& domain,
    /* [in] */ const String& path,
    /* [in] */ Int64 mode,
    /* [in] */ Int64 mtime)
{
    Slogger::I(TAG, "Restoring file domain=%s path=%s", domain.string(), path.string());

    // Bits to indicate postprocessing we may need to perform
    Boolean restoredWallpaper = FALSE;

    AutoPtr<IFile> outFile;
    // Various domain+files we understand a priori
    if (domain.Equals(IFullBackup::ROOT_TREE_TOKEN)) {
        if (path.Equals(WALLPAPER_INFO_FILENAME)) {
            CFile::New(WALLPAPER_INFO, (IFile**)&outFile);
            restoredWallpaper = TRUE;
        }
        else if (path.Equals(WALLPAPER_IMAGE_FILENAME)) {
            CFile::New(WALLPAPER_IMAGE, (IFile**)&outFile);
            restoredWallpaper = TRUE;
        }
    }

    if (outFile == NULL) {
        Slogger::W(TAG, "Skipping unrecognized system file: [ %s :  %s]", domain.string(), path.string());
    }
    //TODO wait CFullBackup
    // AutoPtr<IFullBackup> fb;
    // CFullBackup::AcquireSingleton((IFullBackup**)&fb);
    // ECode ec = fb->RestoreFile(data, size, type, mode, mtime, outFile.Get());
    // if (FAILED(ec)) {
    //     if (restoredWallpaper) {
    //         // Make sure we wind up in a good state
    //         AutoPtr<IFile> f1, f2;
    //         CFile::New(WALLPAPER_IMAGE, (IFile**)&f1);
    //         f1->Delete();
    //         CFile::New(WALLPAPER_INFO, (IFile**)&f2);
    //         f2->Delete();
    //         return E_IO_EXCEPTION;
    //     }
    // }
    if (restoredWallpaper) {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::WALLPAPER_SERVICE);
        AutoPtr<IIWallpaperManager> wallpaper = IIWallpaperManager::Probe(service);
        if (wallpaper != NULL) {
            ECode ec2 = wallpaper->SettingsRestored();
            if (FAILED(ec2)) {
                Slogger::E(TAG, "Couldn't restore settings\n");
                return E_REMOTE_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

} // namespace Backup
} // namespace Server
} // namespace Droid
} // namespace Elastos