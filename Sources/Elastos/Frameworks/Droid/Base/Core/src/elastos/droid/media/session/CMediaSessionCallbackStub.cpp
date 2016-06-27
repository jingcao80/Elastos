#include "elastos/droid/media/session/CMediaSessionCallbackStub.h"
#include "elastos/droid/media/session/CMediaSession.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

const String CMediaSessionCallbackStub::TAG("CMediaSessionCallbackStub");

CAR_INTERFACE_IMPL_2(CMediaSessionCallbackStub, Object, IISessionCallback, IBinder)

CAR_OBJECT_IMPL(CMediaSessionCallbackStub)

CMediaSessionCallbackStub::CMediaSessionCallbackStub()
{
}

CMediaSessionCallbackStub::~CMediaSessionCallbackStub()
{
}

ECode CMediaSessionCallbackStub::constructor(
    /* [in] */ IMediaSession * session)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(session);
    wrs->GetWeakReference((IWeakReference**)&mMediaSession);
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle * args,
    /* [in] */ IResultReceiver * cb)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->PostCommand(command, args, cb);
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnMediaButton(
    /* [in] */ IIntent * mediaButtonIntent,
    /* [in] */ Int32 sequenceNumber,
    /* [in] */ IResultReceiver * cb)
{
    // try {
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchMediaButton(mediaButtonIntent);
    }
    // } finally {
    if (cb != NULL) {
        cb->Send(sequenceNumber, NULL);
    }
    // }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnPlay()
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchPlay();
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnPlayFromMediaId(
    /* [in] */ const String& mediaId,
    /* [in] */ IBundle * extras)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchPlayFromMediaId(mediaId, extras);
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnPlayFromSearch(
    /* [in] */ const String& query,
    /* [in] */ IBundle * extras)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchPlayFromSearch(query, extras);
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnSkipToTrack(
    /* [in] */ Int64 id)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchSkipToItem(id);
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnPause()
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchPause();
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnStop()
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchStop();
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnNext()
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchNext();
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnPrevious()
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchPrevious();
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnFastForward()
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchFastForward();
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnRewind()
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchRewind();
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnSeekTo(
    /* [in] */ Int64 pos)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchSeekTo(pos);
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnRate(
    /* [in] */ IRating * rating)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchRate(rating);
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::SetRemoteControlClientBrowsedPlayer()
{
    Logger::D(TAG, "setRemoteControlClientBrowsedPlayer in CallbackStub");
    AutoPtr<IInterface> strongObj;
    mMediaSession->Resolve(EIID_IInterface, (IInterface**)&strongObj);
    IObject* objTmp = IObject::Probe(strongObj);
    CMediaSession* session = (CMediaSession*)objTmp;
    if (session != NULL) {
        session->DispatchSetBrowsedPlayerCommand();
    }
    return NOERROR;
}

// @Override
ECode CMediaSessionCallbackStub::SetRemoteControlClientPlayItem(
    /* [in] */ Int64 uid,
    /* [in] */ Int32 scope)
{
    Logger::D(TAG, "setRemoteControlClientPlayItem in CallbackStub");
    AutoPtr<IInterface> strongObj;
    mMediaSession->Resolve(EIID_IInterface, (IInterface**)&strongObj);
    IObject* objTmp = IObject::Probe(strongObj);
    CMediaSession* session = (CMediaSession*)objTmp;
    if (session != NULL) {
        session->DispatchSetPlayItemCommand(uid, scope);
    }
    return NOERROR;
}

// @Override
ECode CMediaSessionCallbackStub::GetRemoteControlClientNowPlayingEntries()
{
    Logger::D(TAG, "getRemoteControlClientNowPlayingEntries in CallbackStub");
    AutoPtr<IInterface> strongObj;
    mMediaSession->Resolve(EIID_IInterface, (IInterface**)&strongObj);
    IObject* objTmp = IObject::Probe(strongObj);
    CMediaSession* session = (CMediaSession*)objTmp;
    if (session != NULL) {
        session->DispatchGetNowPlayingItemsCommand();
    }
    return NOERROR;
}


ECode CMediaSessionCallbackStub::OnCustomAction(
    /* [in] */ const String& action,
    /* [in] */ IBundle * args)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        ((CMediaSession*)ms.Get())->DispatchCustomAction(action, args);
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnAdjustVolume(
    /* [in] */ Int32 direction)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        if (((CMediaSession*)ms.Get())->mVolumeProvider != NULL) {
            ((CMediaSession*)ms.Get())->mVolumeProvider->OnAdjustVolume(direction);
        }
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::OnSetVolumeTo(
    /* [in] */ Int32 value)
{
    if (mMediaSession.Get() != NULL) {
        AutoPtr<IMediaSession> ms;
        mMediaSession->Resolve(EIID_IMediaSession, (IInterface**)&ms);
        if (((CMediaSession*)ms.Get())->mVolumeProvider != NULL) {
            ((CMediaSession*)ms.Get())->mVolumeProvider->OnSetVolumeTo(value);
        }
    }
    return NOERROR;
}

ECode CMediaSessionCallbackStub::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CMediaSessionCallbackStub";
    return NOERROR;
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
