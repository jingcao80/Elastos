
#include "CSystemBackupAgent.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/ServiceManager.h"
#include "CWallpaperManagerService.h"

using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::Backup::IBackupHelper;
using Elastos::Droid::App::Backup::IFullBackup;
using Elastos::Droid::App::Backup::CFullBackup;
using Elastos::Droid::App::Backup::IWallpaperBackupHelper;
using Elastos::Droid::App::Backup::CWallpaperBackupHelper;
using Elastos::Droid::App::Backup::IWallpaperBackupHelperHelper;
using Elastos::Droid::App::Backup::CWallpaperBackupHelperHelper;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;



namespace Elastos {
namespace Droid {
namespace Server {

const String CSystemBackupAgent::TAG("CSystemBackupAgent");
const String CSystemBackupAgent::WALLPAPER_IMAGE_FILENAME("wallpaper");
const String CSystemBackupAgent::WALLPAPER_INFO_FILENAME("wallpaper_info.xml");
String CSystemBackupAgent::WALLPAPER_IMAGE_DIR;
String CSystemBackupAgent::WALLPAPER_IMAGE;
String CSystemBackupAgent::WALLPAPER_INFO_DIR;
String CSystemBackupAgent::WALLPAPER_INFO;
const String CSystemBackupAgent::WALLPAPER_IMAGE_KEY = IWallpaperBackupHelper::WALLPAPER_IMAGE_KEY;
const String CSystemBackupAgent::WALLPAPER_INFO_KEY = IWallpaperBackupHelper::WALLPAPER_INFO_KEY;

ECode CSystemBackupAgent::OnBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    // We only back up the data under the current "wallpaper" schema with
    // metadata
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::WALLPAPER_SERVICE);
    AutoPtr<IIWallpaperManager> wallpaperservice = IIWallpaperManager::Probe(obj);
    AutoPtr<CWallpaperManagerService> wallpaper = (CWallpaperManagerService*)wallpaperservice.Get();

    AutoPtr<ArrayOf<String> > files = ArrayOf<String>::Alloc(2);
    files->Set(0, GetWALLPAPER_IMAGE());
    files->Set(1, GetWALLPAPER_INFO());

    AutoPtr<ArrayOf<String> > keys = ArrayOf<String>::Alloc(2);
    keys->Set(0, WALLPAPER_IMAGE_KEY);
    keys->Set(1, WALLPAPER_INFO_KEY);

    String name;
    if (wallpaper != NULL && (name = wallpaper->GetName(), !name.IsNullOrEmpty())) {
        // When the wallpaper has a name, back up the info by itself.
        // TODO: Don't rely on the innards of the service object like this!
        // TODO: Send a delete for any stored wallpaper image in this case?
        files = ArrayOf<String>::Alloc(1);
        files->Set(0, GetWALLPAPER_INFO());
        keys = ArrayOf<String>::Alloc(1);
        keys->Set(0, WALLPAPER_INFO_KEY);
    }

    for(Int32 i = 0; i < files->GetLength(); ++i) {
        Slogger::E(TAG, "backup files[%s] keys[%s]", ((*files)[i]).string(), ((*keys)[i]).string());
    }

    AutoPtr<IWallpaperBackupHelper> helper;
    CWallpaperBackupHelper::New(this, files, keys, (IWallpaperBackupHelper**)&helper);
    AddHelper(String("wallpaper"), IBackupHelper::Probe(helper));
    BackupAgentHelper::OnBackup(oldState, data, newState);
    return NOERROR;
}

ECode CSystemBackupAgent::OnRestore(
    /* [in] */ IBackupDataInput* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState)
{
    AutoPtr<ArrayOf<String> > files = ArrayOf<String>::Alloc(2);
    files->Set(0, GetWALLPAPER_IMAGE());
    files->Set(1, GetWALLPAPER_INFO());

    AutoPtr<ArrayOf<String> > keys = ArrayOf<String>::Alloc(2);
    keys->Set(0, WALLPAPER_IMAGE_KEY);
    keys->Set(1, WALLPAPER_INFO_KEY);

    AutoPtr<IWallpaperBackupHelper> helper;
    CWallpaperBackupHelper::New(this, files, keys, (IWallpaperBackupHelper**)&helper);
    AddHelper(String("wallpaper"), IBackupHelper::Probe(helper));

    helper = NULL;
    files = ArrayOf<String>::Alloc(1);
    files->Set(0, GetWALLPAPER_IMAGE());

    keys = ArrayOf<String>::Alloc(1);
    keys->Set(0, WALLPAPER_IMAGE_KEY);
    CWallpaperBackupHelper::New(this, files, keys, (IWallpaperBackupHelper**)&helper);
    AddHelper(String("system_files"), IBackupHelper::Probe(helper));

    //try{
    BackupAgentHelper::OnRestore(data, appVersionCode, newState);
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::WALLPAPER_SERVICE);
    AutoPtr<IIWallpaperManager> wallpaperservice = IIWallpaperManager::Probe(obj);
    AutoPtr<CWallpaperManagerService> wallpaper = (CWallpaperManagerService*)wallpaperservice.Get();
    wallpaper->SettingsRestored();
    //}
    //catch (IOException ex) {
    // If there was a failure, delete everything for the wallpaper, this is too
    // aggressive,  but this is hopefully a rare failure
    // Slog.d(TAG, "restore failed", ex);
    //  (new File(WALLPAPER_IMAGE)).delete();
    //  (new File(WALLPAPER_INFO)).delete();
    //}
    return NOERROR;
}

ECode CSystemBackupAgent::OnFullBackup(
    /* [in] */ IFullBackupDataOutput* data)
{
    // At present we back up only the wallpaper
    FullWallpaperBackup(data);
    return NOERROR;
}

ECode CSystemBackupAgent::OnRestoreFile(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int64 size,
    /* [in] */ Int32 type,
    /* [in] */ const String& domain,
    /* [in] */ const String& path,
    /* [in] */ Int64 mode,
    /* [in] */ Int64 mtime)
{
    Slogger::I(TAG, "Restoring file domain= %s path= %s", domain.string(), path.string());

    // Bits to indicate postprocessing we may need to perform
    Boolean restoreWallpaper = FALSE;
    AutoPtr<IFile> outFile;
    // Various domain+files we understand a priori
    if (domain.Equals(IFullBackup::ROOT_TREE_TOKEN)) {
        if (path.Equals(WALLPAPER_INFO_FILENAME)) {
            CFile::New(GetWALLPAPER_INFO(), (IFile**)&outFile);
            restoreWallpaper = TRUE;
        } else if (path.Equals(WALLPAPER_IMAGE_FILENAME)) {
            CFile::New(GetWALLPAPER_IMAGE(), (IFile**)&outFile);
            restoreWallpaper = TRUE;
        }
    }

    //try{
    if (outFile == NULL) {
        Slogger::W(TAG, "Skipping unrecognized system file: [ %s: %s ]", domain.string(), path.string());
    }
    AutoPtr<IFullBackup> backup;
    CFullBackup::AcquireSingleton((IFullBackup**)&backup);
    ECode ec = backup->RestoreFile(data, size, type, mode, mtime, outFile);
    if (FAILED(ec)) {
        Slogger::E(TAG, "restoreFile has error:%p", ec);
        AutoPtr<IFile> tempFile;
        CFile::New(GetWALLPAPER_IMAGE(), (IFile**)&tempFile);
        Boolean bTemp;
        tempFile->Delete(&bTemp);

        tempFile = NULL;
        CFile::New(GetWALLPAPER_INFO(), (IFile**)&tempFile);
        tempFile->Delete(&bTemp);
    }
    if (restoreWallpaper) {
        AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::WALLPAPER_SERVICE);
        AutoPtr<IIWallpaperManager> wallpaperservice = IIWallpaperManager::Probe(obj);
        AutoPtr<CWallpaperManagerService> wallpaper = (CWallpaperManagerService*)wallpaperservice.Get();
        wallpaper->SettingsRestored();
    }
    //} catch (IOException e) {
    // Make sure we wind up in a good state
    // (new File(WALLPAPER_IMAGE)).delete();
    // (new File(WALLPAPER_INFO)).delete();
    //}
    return NOERROR;
}

void CSystemBackupAgent::FullWallpaperBackup(
    /* [in] */ IFullBackupDataOutput* output)
{
    // Back up the data files directly.  We do them in this specific order --
    // info file followed by image -- because then we need take no special
    // steps during restore; the restore will happen properly when the
    // individual files are restored piecemeal.
    AutoPtr<IFullBackup> backup;
    CFullBackup::AcquireSingleton((IFullBackup**)&backup);
    String pkgName;
    GetPackageName(&pkgName);
    AutoPtr<IBackupDataOutput> outData;
    output->GetData((IBackupDataOutput**)&outData);
    Int32 iTemp;
    backup->BackupToTar(pkgName, IFullBackup::ROOT_TREE_TOKEN, String(NULL), GetWALLPAPER_INFO_DIR(), GetWALLPAPER_INFO(), outData, &iTemp);
    backup->BackupToTar(pkgName, IFullBackup::ROOT_TREE_TOKEN, String(NULL), GetWALLPAPER_IMAGE_DIR(), GetWALLPAPER_IMAGE(), outData, &iTemp);
}


String CSystemBackupAgent::GetWALLPAPER_IMAGE_DIR()
{
    if (WALLPAPER_IMAGE_DIR.IsNull()) {
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> file;
        env->GetUserSystemDirectory(IUserHandle::USER_OWNER, (IFile**)&file);
        file->GetAbsolutePath(&WALLPAPER_IMAGE_DIR);
    }
    return WALLPAPER_IMAGE_DIR;
}

String CSystemBackupAgent::GetWALLPAPER_INFO_DIR()
{
    if (WALLPAPER_INFO_DIR.IsNull()) {
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> file;
        env->GetUserSystemDirectory(IUserHandle::USER_OWNER, (IFile**)&file);
        file->GetAbsolutePath(&WALLPAPER_INFO_DIR);
    }
    return WALLPAPER_INFO_DIR;
}

String CSystemBackupAgent::GetWALLPAPER_IMAGE()
{
    if (WALLPAPER_IMAGE.IsNull()) {
        AutoPtr<IWallpaperBackupHelperHelper> helper;
        CWallpaperBackupHelperHelper::AcquireSingleton((IWallpaperBackupHelperHelper**)&helper);
        helper->GetWALLPAPER_IMAGE(&WALLPAPER_IMAGE);
    }
    return WALLPAPER_IMAGE;
}

String CSystemBackupAgent::GetWALLPAPER_INFO()
{
    if (WALLPAPER_INFO.IsNull()) {
        AutoPtr<IWallpaperBackupHelperHelper> helper;
        CWallpaperBackupHelperHelper::AcquireSingleton((IWallpaperBackupHelperHelper**)&helper);
        helper->GetWALLPAPER_INFO(&WALLPAPER_INFO);
    }
    return WALLPAPER_INFO;
}

}// namespace Server
}// namespace Droid
}// namespace Elastos
