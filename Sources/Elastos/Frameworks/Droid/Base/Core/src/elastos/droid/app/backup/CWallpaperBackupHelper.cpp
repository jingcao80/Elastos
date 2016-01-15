#include "elastos/droid/app/backup/CWallpaperBackupHelper.h"
#include "elastos/droid/os/CEnvironment.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include <elastos/utility/logging/Slogger.h>


using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String CWallpaperBackupHelper::TAG("CWallpaperBackupHelper");
const Boolean CWallpaperBackupHelper::DEBUG = FALSE;
String CWallpaperBackupHelper::WALLPAPER_IMAGE;
String CWallpaperBackupHelper::WALLPAPER_INFO;
String CWallpaperBackupHelper::STAGE_FILE;

CWallpaperBackupHelper::CWallpaperBackupHelper()
    : mDesiredMinWidth(0.0)
    , mDesiredMinHeight(0.0)
{
}

CWallpaperBackupHelper::~CWallpaperBackupHelper()
{
}

ECode CWallpaperBackupHelper::constructor(
    /* [in] */ IContext *context,
    /* [in] */ ArrayOf<String> *files,
    /* [in] */ ArrayOf<String> *keys)
{
    FileBackupHelperBase::Init(context);
    mContext = context;
    mFiles = files;
    mKeys = keys;

    AutoPtr<IInterface> inter;
    context->GetSystemService(IContext::WALLPAPER_SERVICE, (IInterface**)&inter);
    AutoPtr<IWallpaperManager> wpm = IWallpaperManager::Probe(inter);
    Int32 width, height;
    wpm->GetDesiredMinimumWidth(&width);
    wpm->GetDesiredMinimumHeight(&height);
    mDesiredMinWidth = (Double)width;
    mDesiredMinHeight = (Double)height;

    if (mDesiredMinWidth <= 0 || mDesiredMinHeight <= 0) {
        inter = NULL;
        context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&inter);
        AutoPtr<IWindowManager> wm = IWindowManager::Probe(inter);
        AutoPtr<IDisplay> d;
        wm->GetDefaultDisplay((IDisplay**)&d);
        AutoPtr<IPoint> p;
        CPoint::New((IPoint**)&p);
        d->GetSize(p);
        p->GetX(&width);
        p->GetY(&height);
        mDesiredMinWidth = width;
        mDesiredMinHeight = height;
    }

    if (DEBUG) {
        Slogger::D(TAG, "dmW=%lu, dmH=%lu", mDesiredMinWidth, mDesiredMinHeight);
    }
    return NOERROR;
}

/**
 * Based on oldState, determine which of the files from the application's data directory
 * need to be backed up, write them to the data stream, and fill in newState with the
 * state as it exists now.
 */
ECode CWallpaperBackupHelper::PerformBackup(
    /* [in] */ IParcelFileDescriptor *oldState,
    /* [in] */ IBackupDataOutput *data,
    /* [in] */ IParcelFileDescriptor *newState)
{
    return PerformBackupChecked(oldState, data, newState, mFiles, mKeys);
}

/**
 * Restore one absolute file entity from the restore stream.  If we're restoring the
 * magic wallpaper file, take specific action to determine whether it is suitable for
 * the current device.
 */
ECode CWallpaperBackupHelper::RestoreEntity(
    /* [in] */ IBackupDataInputStream *data)
{
    String key;
    data->GetKey(&key);
    if (IsKeyInList(key, mKeys)) {
        if (key.Equals(WALLPAPER_IMAGE_KEY)) {
            // restore the file to the stage for inspection
            AutoPtr<IFile> f;
            CFile::New(GetSTAGE_FILE(), (IFile**)&f);
            if (WriteFile(f, data)) {
                // Preflight the restored image's dimensions without loading it
                AutoPtr<IBitmapFactoryOptions> options;
                ASSERT_SUCCEEDED(CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options));
                options->SetInJustDecodeBounds(TRUE);
                AutoPtr<IBitmapFactory> factory;
                ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory));
                AutoPtr<IBitmap> bm;
                ASSERT_SUCCEEDED(factory->DecodeFile(GetSTAGE_FILE(), options, (IBitmap**)&bm));
                Int32 w, h;
                options->GetOutWidth(&w);
                options->GetOutHeight(&h);
                if (DEBUG) {
                    Slogger::D(TAG, "Restoring wallpaper image w=%d, h=%d",w, h);
                }

                // How much does the image differ from our preference?  The threshold
                // here is set to accept any image larger than our target, because
                // scaling down is acceptable; but to reject images that are deemed
                // "too small" to scale up attractively.  The value 1.33 is just barely
                // too low to pass Nexus 1 or Droid wallpapers for use on a Xoom, but
                // will pass anything relatively larger.
                Double widthRatio = mDesiredMinWidth / w;
                Double heightRatio = mDesiredMinHeight / h;
                if (widthRatio > 0 && widthRatio < 1.33
                    && heightRatio > 0 && heightRatio < 1.33) {
                    // sufficiently close to our resolution; go ahead and use it
                    if (DEBUG) Slogger::D(TAG, "wallpaper dimension match; using");
                    AutoPtr<IFile> file;
                    CFile::New(GetWALLPAPER_IMAGE(), (IFile**)&file);
                    Boolean b;
                    f->RenameTo(file, &b);
                    // TODO: spin a service to copy the restored image to sd/usb storage,
                    // since it does not exist anywhere other than the private wallpaper
                    // file.
                } else {
                    if (DEBUG) {
                        Slogger::D(TAG, "dimensions too far off: wr=%lu, hr=%lu", widthRatio, heightRatio);
                        Boolean b;
                        f->Delete(&b);
                    }
                }

            }

        } else if (key.Equals(WALLPAPER_INFO_KEY)) {
            // XML file containing wallpaper info
            AutoPtr<IFile> file;
            CFile::New(GetWALLPAPER_INFO(), (IFile**)&file);
            WriteFile(file, data);
        }
    }
    return NOERROR;
}

ECode CWallpaperBackupHelper::WriteNewStateDescription(
    /* [in] */ IParcelFileDescriptor* fd)
{
    FileBackupHelperBase::WriteNewStateDescription(fd);
    return NOERROR;
}

String CWallpaperBackupHelper::GetWALLPAPER_IMAGE()
{
    if (WALLPAPER_IMAGE.IsNull()) {
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> file;
        env->GetUserSystemDirectory(IUserHandle::USER_OWNER, (IFile**)&file);
        AutoPtr<IFile> infoFile;
        CFile::New(file, String("wallpaper"), (IFile**)&infoFile);
        infoFile->GetAbsolutePath(&WALLPAPER_IMAGE);
    }
    return WALLPAPER_IMAGE;
}

String CWallpaperBackupHelper::GetWALLPAPER_INFO()
{
    if (WALLPAPER_INFO.IsNull()) {
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> file;
        env->GetUserSystemDirectory(IUserHandle::USER_OWNER, (IFile**)&file);
        AutoPtr<IFile> infoFile;
        CFile::New(file, String("wallpaper_info.xml"), (IFile**)&infoFile);
        infoFile->GetAbsolutePath(&WALLPAPER_INFO);
    }
    return WALLPAPER_INFO;
}

String CWallpaperBackupHelper::GetSTAGE_FILE()
{
    if (STAGE_FILE.IsNull()) {
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> file;
        env->GetUserSystemDirectory(IUserHandle::USER_OWNER, (IFile**)&file);
        AutoPtr<IFile> infoFile;
        CFile::New(file, String("wallpaper-tmp"), (IFile**)&infoFile);
        infoFile->GetAbsolutePath(&STAGE_FILE);
    }
    return STAGE_FILE;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
