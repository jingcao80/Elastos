
#include "CWMSWallpaperConnection.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

ECode CWMSWallpaperConnection::constructor(
    /* [in] */ IWallpaperInfo* info,
    /* [in] */ Handle32 wallpaper,
    /* [in] */ Handle32 owner)
{
    mInfo = info;
    mWallpaper = (CWallpaperManagerService::WallpaperData*)wallpaper;
    mOwner = (CWallpaperManagerService*)owner;
    return CBinder::New((IBinder**)&mToken);
}

ECode CWMSWallpaperConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoLock lock(mOwner->mLock);
    if (mWallpaper->mConnection == this) {
        mWallpaper->mLastDiedTime = SystemClock::GetUptimeMillis();
        mService = IIWallpaperService::Probe(service);
        mOwner->AttachServiceLocked(this, mWallpaper);
        // XXX should probably do saveSettingsLocked() later
        // when we have an engine, but I'm not sure about
        // locking there and anyway we always need to be able to
        // recover if there is something wrong.
        mOwner->SaveSettingsLocked(mWallpaper);
    }

    return NOERROR;
}

ECode CWMSWallpaperConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoLock lock(mOwner->mLock);
    mService = NULL;
    mEngine = NULL;
    if (mWallpaper->mConnection == this) {
        String str;
        mWallpaper->mWallpaperComponent->ToString(&str);
        Slogger::W(CWallpaperManagerService::TAG, "Wallpaper service gone: %s", str.string());
        if (!mWallpaper->mWallpaperUpdating
            && (mWallpaper->mLastDiedTime + CWallpaperManagerService::MIN_WALLPAPER_CRASH_TIME)
                > SystemClock::GetUptimeMillis()
            && mWallpaper->mUserId == mOwner->mCurrentUserId) {
            Slogger::W(CWallpaperManagerService::TAG, "Reverting to built-in wallpaper!");
            mOwner->ClearWallpaperLocked(TRUE, mWallpaper->mUserId, NULL);
        }
    }

    return NOERROR;
}

ECode CWMSWallpaperConnection::AttachEngine(
    /* [in] */ IWallpaperEngine* engine)
{
    AutoLock lock(mOwner->mLock);
    mEngine = engine;

    return NOERROR;
}

ECode CWMSWallpaperConnection::EngineShown(
    /* [in] */ IWallpaperEngine* engine)
{
    AutoLock lock(mOwner->mLock);
    if (mReply != NULL) {
        Int64 ident = Binder::ClearCallingIdentity();
        if (FAILED(mReply->SendResult(NULL)))
            Binder::RestoreCallingIdentity(ident);
        mReply = NULL;
    }

    return NOERROR;
}

ECode CWMSWallpaperConnection::SetWallpaper(
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoLock lock(mOwner->mLock);
    if (mWallpaper->mConnection == this) {
        AutoPtr<IParcelFileDescriptor> temp =
            mOwner->UpdateWallpaperBitmapLocked(name, mWallpaper);
        *result = temp;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    return NOERROR;
}

ECode CWMSWallpaperConnection::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

} // Server
} // Droid
} // Elastos
