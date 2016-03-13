
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/server/media/projection/CMediaProjection.h"
#include "elastos/droid/server/media/projection/MediaProjectionManagerService.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Media::Projection::IMediaProjectionManager;
using Elastos::Droid::Media::Projection::CMediaProjectionInfo;
using Elastos::Droid::Media::Projection::EIID_IIMediaProjection;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {
namespace Projection {

//==============================================================================
//                  CMediaProjection::DeathEater
//==============================================================================

CAR_INTERFACE_IMPL(CMediaProjection::DeathEater, Object, IProxyDeathRecipient)

CMediaProjection::DeathEater::ProxyDied()
{
    mHost->mHost->mCallbackDelegate->Remove(mCallback);
    return mHost->Stop();
}


//==============================================================================
//                  CMediaProjection
//==============================================================================

CAR_INTERFACE_IMPL_2(CMediaProjection, Object, IIMediaProjection, IBinder)

CAR_OBJECT_IMPL(CMediaProjection)

CMediaProjection::CMediaProjection()
    : mUid(0)
    , mType(0)
{}

ECode CMediaProjection::constructor(
    /* [in] */ Int32 type,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ Handle64 host)
{
    mType = type;
    mUid = uid;
    mPackageName = packageName;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetUserId(uid, &id);
    CUserHandle::New(id, (IUserHandle**)&mUserHandle);
    mHost = (MediaProjectionManagerService*)host;
    return NOERROR;
}

ECode CMediaProjection::CanProjectVideo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType == IMediaProjectionManager::TYPE_MIRRORING ||
            mType == IMediaProjectionManager::TYPE_SCREEN_CAPTURE;
    return NOERROR;
}

ECode CMediaProjection::CanProjectSecureVideo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CMediaProjection::CanProjectAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType == IMediaProjectionManager::TYPE_MIRRORING ||
            mType == IMediaProjectionManager::TYPE_PRESENTATION;
    return NOERROR;
}

ECode CMediaProjection::ApplyVirtualDisplayFlags(
    /* [in] */ Int32 flags,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mType == IMediaProjectionManager::TYPE_SCREEN_CAPTURE) {
        flags &= ~IDisplayManager::VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY;
        flags |= IDisplayManager::VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR
                | IDisplayManager::VIRTUAL_DISPLAY_FLAG_PRESENTATION;
        *result = flags;
        return NOERROR;
    }
    else if (mType == IMediaProjectionManager::TYPE_MIRRORING) {
        flags &= ~(IDisplayManager::VIRTUAL_DISPLAY_FLAG_PUBLIC |
                IDisplayManager::VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR);
        flags |= IDisplayManager::VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY |
                IDisplayManager::VIRTUAL_DISPLAY_FLAG_PRESENTATION;
        *result = flags;
        return NOERROR;
    }
    else if (mType == IMediaProjectionManager::TYPE_PRESENTATION) {
        flags &= ~IDisplayManager::VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY;
        flags |= IDisplayManager::VIRTUAL_DISPLAY_FLAG_PUBLIC |
                IDisplayManager::VIRTUAL_DISPLAY_FLAG_PRESENTATION |
                IDisplayManager::VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR;
        *result = flags;
        return NOERROR;
    }
    else  {
        *result = -1;
        Slogger::E("CMediaProjection", "Unknown MediaProjection type");
        return E_RUNTIME_EXCEPTION;
    }
}

ECode CMediaProjection::Start(
    /* [in] */ IIMediaProjectionCallback* cb)
{
    if (cb == NULL) {
        Slogger::E("CMediaProjection", "callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoLock lock(mHost->mLock);
    if (mHost->IsValidMediaProjection(IBinder::Probe(this))) {
        Slogger::E("CMediaProjection", "Cannot start already started MediaProjection");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    RegisterCallback(cb);
    // try {
    mToken = IBinder::Probe(cb);
    mDeathEater = (IProxyDeathRecipient*)new DeathEater(cb, this);
    AutoPtr<IProxy> proxy = (IProxy*)mToken->Probe(EIID_IProxy);
    if (proxy != NULL) {
        ECode ec = proxy->LinkToDeath(mDeathEater, 0);
        if (FAILED(ec)) {
            Slogger::W("CMediaProjection", "MediaProjectionCallbacks must be valid, aborting MediaProjection");
            return ec;
        }
    }
    // } catch (RemoteException e) {
    //     Slog.w(TAG,
    //             , e);
    //     return;
    // }
    mHost->StartProjectionLocked(this);
    return NOERROR;
}

ECode CMediaProjection::Stop()
{
    AutoLock lock(mHost->mLock);
    if (!mHost->IsValidMediaProjection(IBinder::Probe(this))) {
        Slogger::W("CMediaProjection", "Attempted to stop inactive MediaProjection (uid=%d, pid=%d)",
                Binder::GetCallingUid(), Binder::GetCallingPid());
        return NOERROR;
    }
    AutoPtr<IProxy> proxy = (IProxy*)mToken->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean result;
        proxy->UnlinkToDeath(mDeathEater, 0, &result);
    }
    mHost->StopProjectionLocked(this);
    return NOERROR;
}

ECode CMediaProjection::RegisterCallback(
    /* [in] */ IIMediaProjectionCallback* cb)
{
    if (cb == NULL) {
        Slogger::E("CMediaProjection", "callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHost->mCallbackDelegate->Add(cb);
    return NOERROR;
}

ECode CMediaProjection::UnregisterCallback(
    /* [in] */ IIMediaProjectionCallback* cb)
{
    if (cb == NULL) {
        Slogger::E("CMediaProjection", "callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHost->mCallbackDelegate->Remove(cb);
    return NOERROR;
}

AutoPtr<IMediaProjectionInfo> CMediaProjection::GetProjectionInfo()
{
    AutoPtr<IMediaProjectionInfo> info;
    CMediaProjectionInfo::New(mPackageName, mUserHandle, (IMediaProjectionInfo**)&info);
    return info;
}

ECode CMediaProjection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Projection
} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
