
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CGlobalsWallpaperManagerCallback.h"
#include "elastos/droid/app/CWallpaperManager.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/BitmapFactory.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::BitmapFactory;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::AutoLock;
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

ECode CGlobalsWallpaperManagerCallback::constructor(
    /* [in] */ ILooper* looper)
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::WALLPAPER_SERVICE);
    mService = IIWallpaperManager::Probe(obj);
    assert(mService != NULL);
    return NOERROR;
}

ECode CGlobalsWallpaperManagerCallback::OnWallpaperChanged()
{
    /* The wallpaper has changed but we shouldn't eagerly load the
     * wallpaper as that would be inefficient. Reset the cached wallpaper
     * to null so if the user requests the wallpaper again then we'll
     * fetch it.
     */
    AutoLock syncLock(this);
    mWallpaper = NULL;
    mDefaultWallpaper = NULL;
    return NOERROR;
}

ECode CGlobalsWallpaperManagerCallback::OnKeyguardWallpaperChanged()
{
    AutoLock syncLock(this);
    mKeyguardWallpaper = NULL;
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

AutoPtr<IBitmap> CGlobalsWallpaperManagerCallback::PeekKeyguardWallpaperBitmap(
    /* [in] */ IContext* context)
{
    AutoLock syncLock(this);
    if (mKeyguardWallpaper != NULL) {
        return mKeyguardWallpaper;
    }
    // try {
    mKeyguardWallpaper = GetCurrentKeyguardWallpaperLocked(context);
    // } catch (OutOfMemoryError e) {
    //     Log.w(TAG, "No memory load current keyguard wallpaper", e);
    // }
    return mKeyguardWallpaper;
}

void CGlobalsWallpaperManagerCallback::ForgetLoadedWallpaper()
{
    AutoLock lock(this);
    mWallpaper = NULL;
    mDefaultWallpaper = NULL;
}

void CGlobalsWallpaperManagerCallback::ForgetLoadedKeyguardWallpaper()
{
    AutoLock syncLock(this);
    mKeyguardWallpaper = NULL;
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
    ECode ec = mService->GetWallpaper(this,
            (IBundle**)&params, (IParcelFileDescriptor**)&fd);
    if (SUCCEEDED(ec) && fd != NULL) {
        // try {
        AutoPtr<IBitmapFactoryOptions> options;
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
        AutoPtr<IFileDescriptor> desc;
        fd->GetFileDescriptor((IFileDescriptor**)&desc);
        AutoPtr<IBitmap> bm;
        ec = BitmapFactory::DecodeFileDescriptor(desc, NULL, options, (IBitmap**)&bm);
        fd->Close();
        if (SUCCEEDED(ec)) return bm;
        Logger::W("GlobalsWallpaperManagerCallback", "Can't decode file");
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

AutoPtr<IBitmap> CGlobalsWallpaperManagerCallback::GetCurrentKeyguardWallpaperLocked(
    /* [in] */ IContext* context)
{
    // try {
    AutoPtr<IBundle> params;
    AutoPtr<IParcelFileDescriptor> fd;
    ECode ec = mService->GetKeyguardWallpaper(this, (IBundle**)&params, (IParcelFileDescriptor**)&fd);
    if (SUCCEEDED(ec) && fd != NULL) {
        // try {
        AutoPtr<IBitmapFactoryOptions> options;
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
        AutoPtr<IFileDescriptor> desc;
        fd->GetFileDescriptor((IFileDescriptor**)&desc);
        AutoPtr<IBitmap> bm;
        ECode ec = BitmapFactory::DecodeFileDescriptor(desc, NULL, options, (IBitmap**)&bm);
        fd->Close();
        if (SUCCEEDED(ec)) return bm;
        Logger::W("GlobalsWallpaperManagerCallback", "Can't decode file");
        // } catch (OutOfMemoryError e) {
        //     Log.w("GlobalsWallpaperManagerCallback", "Can't decode file", e);
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
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
        AutoPtr<IBitmap> bm;
        ECode ec = BitmapFactory::DecodeStream(is, NULL, options, (IBitmap**)&bm);
        ICloseable::Probe(is)->Close();
        if (SUCCEEDED(ec)) return bm;
        Logger::W("GlobalsWallpaperManagerCallback", "Can't decode stream");
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

ECode CGlobalsWallpaperManagerCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

void CGlobalsWallpaperManagerCallback::ClearKeyguardWallpaper()
{
    AutoLock syncLock(this);
    // try {
    mService->ClearKeyguardWallpaper();
    // } catch (RemoteException e) {
    //     // ignore
    // }
    mKeyguardWallpaper = NULL;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
