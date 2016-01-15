
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CGlobalsWallpaperManagerCallback.h"
#include "elastos/droid/app/CWallpaperManager.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace App {

const Int32 CGlobalsWallpaperManagerCallback::MSG_CLEAR_WALLPAPER = 1;

ECode CGlobalsWallpaperManagerCallback::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case CGlobalsWallpaperManagerCallback::MSG_CLEAR_WALLPAPER:
            mHost->HandleClearWallpaper();
            break;
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CGlobalsWallpaperManagerCallback, Object, IIWallpaperManagerCallback, IBinder)

CAR_OBJECT_IMPL(CGlobalsWallpaperManagerCallback)

ECode CGlobalsWallpaperManagerCallback::OnWallpaperChanged()
{
    /* The wallpaper has changed but we shouldn't eagerly load the
     * wallpaper as that would be inefficient. Reset the cached wallpaper
     * to null so if the user requests the wallpaper again then we'll
     * fetch it.
     */
    synchronized(this) {
        mWallpaper = NULL;
        mDefaultWallpaper = NULL;
    }
    return NOERROR;
}

AutoPtr<IBitmap> CGlobalsWallpaperManagerCallback::PeekWallpaperBitmap(
    /* [in] */ IContext* context,
    /* [in] */ Boolean returnDefault)
{
    AutoLock lock(this);
    if (mWallpaper != NULL) {
        return mWallpaper;
    }
    if (mDefaultWallpaper != NULL) {
        return mDefaultWallpaper;
    }
    mWallpaper = NULL;
    // try {
    mWallpaper = GetCurrentWallpaperLocked(context);
    // } catch (OutOfMemoryError e) {
    //     Log.w(TAG, "No memory load current wallpaper", e);
    // }
    if (returnDefault) {
        if (mWallpaper == NULL) {
            mDefaultWallpaper = GetDefaultWallpaperLocked(context);
            return mDefaultWallpaper;
        }
        else {
            mDefaultWallpaper = NULL;
        }
    }
    return mWallpaper;
}

void CGlobalsWallpaperManagerCallback::ForgetLoadedWallpaper()
{
    AutoLock lock(this);
    mWallpaper = NULL;
    mDefaultWallpaper = NULL;
}

AutoPtr<IBitmap> CGlobalsWallpaperManagerCallback::GetCurrentWallpaperLocked(
    /* [in] */ IContext* context)
{
    if (mService == NULL) {
        Logger::W("GlobalsWallpaperManagerCallback", "WallpaperService not running");
        return NULL;
    }

    // try {
    AutoPtr<IBundle> params;
    AutoPtr<IParcelFileDescriptor> fd;
    ASSERT_SUCCEEDED(mService->GetWallpaper((IIWallpaperManagerCallback*)this,
        (IBundle**)&params, (IParcelFileDescriptor**)&fd));
    if (fd != NULL) {

        // try {
        AutoPtr<IBitmapFactoryOptions> options;
        ASSERT_SUCCEEDED(CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options));
        AutoPtr<IBitmapFactory> factory;
        ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory));
        AutoPtr<IFileDescriptor> desc;
        ASSERT_SUCCEEDED(fd->GetFileDescriptor((IFileDescriptor**)&desc));
        AutoPtr<IBitmap> bm;
        ASSERT_SUCCEEDED(factory->DecodeFileDescriptor(desc, NULL, options, (IBitmap**)&bm));
        fd->Close();
        return bm;
        // } catch (OutOfMemoryError e) {
        //     Log.w(TAG, "Can't decode file", e);
        // } finally {
        //     try {
        //         fd.close();
        //     } catch (IOException e) {
        //         // Ignore
        //     }
        // }
    }
    // } catch (RemoteException e) {
    //     // Ignore
    // }
    return NULL;
}

AutoPtr<IBitmap> CGlobalsWallpaperManagerCallback::GetDefaultWallpaperLocked(
    /* [in] */ IContext* context)
{
    AutoPtr<IInputStream> is = CWallpaperManager::OpenDefaultWallpaper(context);
    if (is != NULL) {
        // try {
        AutoPtr<IBitmapFactoryOptions> options;
        ASSERT_SUCCEEDED(CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options));
        AutoPtr<IBitmapFactory> factory;
        ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory));
        AutoPtr<IBitmap> bm;
        ECode ec = factory->DecodeStream(is, NULL, options, (IBitmap**)&bm);
        if (ec == (ECode)E_OUT_OF_MEMORY_ERROR) {
            Logger::W("GlobalsWallpaperManagerCallback", "Can't decode stream");
        }

        ICloseable::Probe(is)->Close();
        return bm;
        // } catch (OutOfMemoryError e) {
        //     Log.w(TAG, "Can't decode stream", e);
        // } finally {
        //     try {
        //         is.close();
        //     } catch (IOException e) {
        //         // Ignore
        //     }
        // }
    }
    return NULL;
}

void CGlobalsWallpaperManagerCallback::HandleClearWallpaper()
{
    AutoLock lock(this);
    mWallpaper = NULL;
    mDefaultWallpaper = NULL;
}

ECode CGlobalsWallpaperManagerCallback::constructor(
    /* [in] */ ILooper* looper)
{
    mService = (IIWallpaperManager*)ServiceManager::GetService(IContext::WALLPAPER_SERVICE).Get();
    return NOERROR;
}

ECode CGlobalsWallpaperManagerCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
