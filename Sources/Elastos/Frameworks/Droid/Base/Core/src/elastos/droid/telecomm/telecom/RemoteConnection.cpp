
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/RemoteConnection.h"
#include "elastos/droid/telecomm/telecom/DisconnectCause.h"
#include "elastos/droid/telecomm/telecom/CVideoCallbackServant.h"
#include "elastos/droid/telecomm/telecom/CDisconnectCause.h"
#include "elastos/droid/telecomm/telecom/CRemoteConnection.h"

using Elastos::Core::CString;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::CConcurrentHashMap;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// RemoteConnection::VideoProvider::VideoCallback::
//===============================================================
RemoteConnection::VideoProvider::VideoCallback::VideoCallback(
    /* [in] */ VideoProvider* host)
    : mHost(host)
{}

ECode RemoteConnection::VideoProvider::VideoCallback::ReceiveSessionModifyRequest(
    /* [in] */ IVideoProfile* videoProfile)
{
    AutoPtr<IIterator> it;
    mHost->mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionListener> l = IRemoteConnectionListener::Probe(p);
        l->OnReceiveSessionModifyRequest(mHost, videoProfile);
    }
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::VideoCallback::ReceiveSessionModifyResponse(
    /* [in] */ Int32 status,
    /* [in] */ IVideoProfile* requestedProfile,
    /* [in] */ IVideoProfile* responseProfile)
{
    AutoPtr<IIterator> it;
    mHost->mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionListener> l = IRemoteConnectionListener::Probe(p);
        l->OnReceiveSessionModifyResponse(
                mHost,
                status,
                requestedProfile,
                responseProfile);
    }
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::VideoCallback::HandleCallSessionEvent(
    /* [in] */ Int32 event)
{
    AutoPtr<IIterator> it;
    mHost->mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionListener> l = IRemoteConnectionListener::Probe(p);
        l->OnHandleCallSessionEvent(mHost, event);
    }
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::VideoCallback::ChangePeerDimensions(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    AutoPtr<IIterator> it;
    mHost->mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionListener> l = IRemoteConnectionListener::Probe(p);
        l->OnPeerDimensionsChanged(mHost, width, height);
    }
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::VideoCallback::ChangeCallDataUsage(
    /* [in] */ Int32 dataUsage)
{
    AutoPtr<IIterator> it;
    mHost->mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionListener> l = IRemoteConnectionListener::Probe(p);
        l->OnCallDataUsageChanged(mHost, dataUsage);
    }
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::VideoCallback::ChangeCameraCapabilities(
    /* [in] */ ICameraCapabilities* cameraCapabilities)
{
    AutoPtr<IIterator> it;
    mHost->mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionListener> l = IRemoteConnectionListener::Probe(p);
        l->OnCameraCapabilitiesChanged(mHost, cameraCapabilities);
    }
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::VideoCallback::AsBinder(
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

//===============================================================
// RemoteConnection::VideoProvider::
//===============================================================
CAR_INTERFACE_IMPL(RemoteConnection::VideoProvider, Object, IRemoteConnectionVideoProvider)

RemoteConnection::VideoProvider::VideoProvider()
{
    CVideoCallbackServant::New(mVideoCallbackDelegate, (IVideoCallbackServant**)&mVideoCallbackServant);

    AutoPtr<IMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->SetFromMap(map, (ISet**)&mListeners);
}

RemoteConnection::VideoProvider::VideoProvider(
    /* [in] */ IIVideoProvider* videoProviderBinder)
{
    mVideoProviderBinder = videoProviderBinder;
    assert(0 && "TODO");
    //mVideoProviderBinder->SetVideoCallback(mVideoCallbackServant.getStub().asBinder());
}

ECode RemoteConnection::VideoProvider::AddListener(
    /* [in] */ IRemoteConnectionListener* l)
{
    mListeners->Add(l);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::RemoveListener(
    /* [in] */ IRemoteConnectionListener* l)
{
    mListeners->Remove(l);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::SetCamera(
    /* [in] */ const String& cameraId)
{
    mVideoProviderBinder->SetCamera(cameraId);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::SetPreviewSurface(
    /* [in] */ ISurface* surface)
{
    mVideoProviderBinder->SetPreviewSurface(surface);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::SetDisplaySurface(
    /* [in] */ ISurface* surface)
{
    mVideoProviderBinder->SetDisplaySurface(surface);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::SetDeviceOrientation(
    /* [in] */ Int32 rotation)
{
    mVideoProviderBinder->SetDeviceOrientation(rotation);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::SetZoom(
    /* [in] */ Float value)
{
    mVideoProviderBinder->SetZoom(value);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::SendSessionModifyRequest(
    /* [in] */ IVideoProfile* reqProfile)
{
    mVideoProviderBinder->SendSessionModifyRequest(reqProfile);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::SendSessionModifyResponse(
    /* [in] */ IVideoProfile* responseProfile)
{
    mVideoProviderBinder->SendSessionModifyResponse(responseProfile);
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::RequestCameraCapabilities()
{
    mVideoProviderBinder->RequestCameraCapabilities();
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::RequestCallDataUsage()
{
    mVideoProviderBinder->RequestCallDataUsage();
    return NOERROR;
}

ECode RemoteConnection::VideoProvider::SetPauseImage(
    /* [in] */ const String& uri)
{
    mVideoProviderBinder->SetPauseImage(uri);
    return NOERROR;
}

//===============================================================
// RemoteConnection::
//===============================================================
CAR_INTERFACE_IMPL(RemoteConnection, Object, IRemoteConnection)

RemoteConnection::RemoteConnection()
    : mState(IConnection::STATE_NEW)
    , mRingbackRequested(FALSE)
    , mConnected(FALSE)
    , mCallCapabilities(0)
    , mVideoState(0)
    , mIsVoipAudioMode(FALSE)
    , mAddressPresentation(0)
    , mCallerDisplayNamePresentation(0)
{
    AutoPtr<IMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->SetFromMap(map, (ISet**)&mCallbacks);

    CArrayList::New((IList**)&mConferenceableConnections);
    cls->UnmodifiableList(mConferenceableConnections, (IList**)&mUnmodifiableconferenceableConnections);
}

ECode RemoteConnection::constructor(
    /* [in] */ const String& id,
    /* [in] */ IIConnectionService* connectionService,
    /* [in] */ IConnectionRequest* request)
{
    mConnectionId = id;
    mConnectionService = connectionService;
    mConnected = TRUE;
    mState = IConnection::STATE_INITIALIZING;
    return NOERROR;
}

ECode RemoteConnection::constructor(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    constructor(String("NULL"), NULL, NULL);
    mConnected = FALSE;
    mState = IConnection::STATE_DISCONNECTED;
    mDisconnectCause = disconnectCause;
    return NOERROR;
}

ECode RemoteConnection::RegisterCallback(
    /* [in] */ IRemoteConnectionCallback* callback)
{
    mCallbacks->Add(callback);
    return NOERROR;
}

ECode RemoteConnection::UnregisterCallback(
    /* [in] */ IRemoteConnectionCallback* callback)
{
    if (callback != NULL) {
        mCallbacks->Remove(callback);
    }
    return NOERROR;
}

ECode RemoteConnection::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode RemoteConnection::GetDisconnectCause(
    /* [out] */ IDisconnectCause** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectCause;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConnection::GetCallCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallCapabilities;
    return NOERROR;
}

ECode RemoteConnection::IsVoipAudioMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsVoipAudioMode;
    return NOERROR;
}

ECode RemoteConnection::GetStatusHints(
    /* [out] */ IStatusHints** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusHints;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConnection::GetAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConnection::GetAddressPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddressPresentation;
    return NOERROR;
}

ECode RemoteConnection::GetCallerDisplayName(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> pDisName;
    CString::New(mCallerDisplayName, (ICharSequence**)&pDisName);
    *result = pDisName;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConnection::GetCallerDisplayNamePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayNamePresentation;
    return NOERROR;
}

ECode RemoteConnection::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoState;
    return NOERROR;
}

ECode RemoteConnection::GetVideoProvider(
    /* [out] */ IRemoteConnectionVideoProvider** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoProvider;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConnection::IsRingbackRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode RemoteConnection::Abort()
{
    if (mConnected) {
        mConnectionService->Abort(mConnectionId);
    }
    return NOERROR;
}

ECode RemoteConnection::Answer()
{
    if (mConnected) {
        mConnectionService->Answer(mConnectionId);
    }
    return NOERROR;
}

ECode RemoteConnection::Answer(
    /* [in] */ Int32 videoState)
{
    if (mConnected) {
        mConnectionService->AnswerVideo(mConnectionId, videoState);
    }
    return NOERROR;
}

ECode RemoteConnection::Reject()
{
    if (mConnected) {
        mConnectionService->Reject(mConnectionId);
    }
    return NOERROR;
}

ECode RemoteConnection::Hold()
{
    if (mConnected) {
        mConnectionService->Hold(mConnectionId);
    }
    return NOERROR;
}

ECode RemoteConnection::Unhold()
{
    if (mConnected) {
        mConnectionService->Unhold(mConnectionId);
    }
    return NOERROR;
}

ECode RemoteConnection::Disconnect()
{
    if (mConnected) {
        mConnectionService->Disconnect(mConnectionId);
    }
    return NOERROR;
}

ECode RemoteConnection::PlayDtmfTone(
    /* [in] */ Char32 digit)
{
    if (mConnected) {
        mConnectionService->PlayDtmfTone(mConnectionId, digit);
    }
    return NOERROR;
}

ECode RemoteConnection::StopDtmfTone()
{
    if (mConnected) {
        mConnectionService->StopDtmfTone(mConnectionId);
    }
    return NOERROR;
}

ECode RemoteConnection::PostDialContinue(
    /* [in] */ Boolean proceed)
{
    if (mConnected) {
        mConnectionService->OnPostDialContinue(mConnectionId, proceed);
    }
    return NOERROR;
}

ECode RemoteConnection::SetAudioState(
    /* [in] */ IAudioState* state)
{
    if (mConnected) {
        mConnectionService->OnAudioStateChanged(mConnectionId, state);
    }
    return NOERROR;
}

ECode RemoteConnection::GetConferenceableConnections(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUnmodifiableconferenceableConnections;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConnection::GetConference(
    /* [out] */ IRemoteConference** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConference;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConnection::GetId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectionId;
    return NOERROR;
}

ECode RemoteConnection::GetConnectionService(
    /* [out] */ IIConnectionService** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectionService;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConnection::SetState(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        mState = state;
        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
            c->OnStateChanged(this, state);
        }
    }
    return NOERROR;
}

ECode RemoteConnection::SetDisconnected(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    if (mState != IConnection::STATE_DISCONNECTED) {
        mState = IConnection::STATE_DISCONNECTED;
        mDisconnectCause = disconnectCause;

        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
            c->OnDisconnected(this, mDisconnectCause);
        }
    }
    return NOERROR;
}

ECode RemoteConnection::SetRingbackRequested(
    /* [in] */ Boolean ringback)
{
    if (mRingbackRequested != ringback) {
        mRingbackRequested = ringback;

        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
            c->OnRingbackRequested(this, ringback);
        }
    }
    return NOERROR;
}

ECode RemoteConnection::SetCallCapabilities(
    /* [in] */ Int32 callCapabilities)
{
    mCallCapabilities = callCapabilities;

    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnCallCapabilitiesChanged(this, callCapabilities);
    }
    return NOERROR;
}

ECode RemoteConnection::SetDestroyed()
{
    Boolean bEmp = FALSE;
    if (!(mCallbacks->IsEmpty(&bEmp), bEmp)) {
        // Make sure that the callbacks are notified that the call is destroyed first.
        if (mState != IConnection::STATE_DISCONNECTED) {
            AutoPtr<IDisconnectCause> p;
            CDisconnectCause::New(IDisconnectCause::ERROR, String("Connection destroyed."), (IDisconnectCause**)&p);
            SetDisconnected(p);
        }

        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
            c->OnDestroyed(this);
        }
        mCallbacks->Clear();

        mConnected = FALSE;
    }
    return NOERROR;
}

ECode RemoteConnection::SetPostDialWait(
    /* [in] */ const String& remainingDigits)
{
    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnPostDialWait(this, remainingDigits);
    }
    return NOERROR;
}

ECode RemoteConnection::SetVideoState(
    /* [in] */ Int32 videoState)
{
    mVideoState = videoState;
    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnVideoStateChanged(this, videoState);
    }
    return NOERROR;
}

ECode RemoteConnection::SetVideoProvider(
    /* [in] */ IRemoteConnectionVideoProvider* videoProvider)
{
    mVideoProvider = videoProvider;

    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnVideoProviderChanged(this, videoProvider);
    }
    return NOERROR;
}

ECode RemoteConnection::SetIsVoipAudioMode(
    /* [in] */ Boolean isVoip)
{
    mIsVoipAudioMode = isVoip;

    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnVoipAudioChanged(this, isVoip);
    }
    return NOERROR;
}

ECode RemoteConnection::SetStatusHints(
    /* [in] */ IStatusHints* statusHints)
{
    mStatusHints = statusHints;

    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnStatusHintsChanged(this, statusHints);
    }
    return NOERROR;
}

ECode RemoteConnection::SetAddress(
    /* [in] */ IUri* address,
    /* [in] */ Int32 presentation)
{
    mAddress = address;
    mAddressPresentation = presentation;

    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnAddressChanged(this, address, presentation);
    }
    return NOERROR;
}

ECode RemoteConnection::SetCallerDisplayName(
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    mCallerDisplayName = callerDisplayName;
    mCallerDisplayNamePresentation = presentation;

    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnCallerDisplayNameChanged(this, callerDisplayName, presentation);
    }
    return NOERROR;
}

ECode RemoteConnection::SetConferenceableConnections(
    /* [in] */ IList* conferenceableConnections)
{
    mConferenceableConnections->Clear();
    mConferenceableConnections->AddAll(ICollection::Probe(conferenceableConnections));

    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
        c->OnConferenceableConnectionsChanged(this, mUnmodifiableconferenceableConnections);
    }
    return NOERROR;
}

ECode RemoteConnection::SetConference(
    /* [in] */ IRemoteConference* conference)
{
    if (!Object::Equals(IInterface::Probe(mConference), IInterface::Probe(conference))) {
        mConference = conference;

        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConnectionCallback> c = IRemoteConnectionCallback::Probe(p);
            c->OnConferenceChanged(this, conference);
        }
    }
    return NOERROR;
}

ECode RemoteConnection::Failure(
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [out] */ IRemoteConnection** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IRemoteConnection> p;
    CRemoteConnection::New(disconnectCause, (IRemoteConnection**)&p);
    *result = p;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos