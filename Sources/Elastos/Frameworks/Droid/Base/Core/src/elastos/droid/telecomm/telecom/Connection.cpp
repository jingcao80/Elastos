
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/Connection.h"
#include "elastos/droid/telecomm/telecom/CVideoProviderBinder.h"
#include "elastos/droid/telecomm/telecom/CDisconnectCause.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::ISurface;
using Elastos::Core::IFloat;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// Connection::Listener::
//===============================================================
CAR_INTERFACE_IMPL(Connection::Listener, Object, IConnectionListener)

ECode Connection::Listener::OnStateChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnAddressChanged(
    /* [in] */ IConnection* c,
    /* [in] */ IUri* newAddress,
    /* [in] */ Int32 presentation)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnCallerDisplayNameChanged(
    /* [in] */ IConnection* c,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnVideoStateChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Int32 videoState)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnDisconnected(
    /* [in] */ IConnection* c,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnPostDialWait(
    /* [in] */ IConnection* c,
    /* [in] */ const String& remaining)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnRingbackRequested(
    /* [in] */ IConnection* c,
    /* [in] */ Boolean ringback)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnDestroyed(
    /* [in] */ IConnection* c)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnCallCapabilitiesChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Int32 callCapabilities)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnVideoProviderChanged(
    /* [in] */ IConnection* c,
    /* [in] */ IConnectionVideoProvider* videoProvider)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnAudioModeIsVoipChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Boolean isVoip)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnStatusHintsChanged(
    /* [in] */ IConnection* c,
    /* [in] */ IStatusHints* statusHints)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnConferenceableConnectionsChanged(
    /* [in] */ IConnection* c,
    /* [in] */ IList* conferenceableConnections)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::Listener::OnConferenceChanged(
    /* [in] */ IConnection* c,
    /* [in] */ IConference* conference)
{
    return E_NOT_IMPLEMENTED;
}

//===============================================================
// Connection::VideoProvider::VideoProviderHandler::
//===============================================================

Connection::VideoProvider::VideoProviderHandler::VideoProviderHandler(
    /* [in] */ VideoProvider* host)
    : mHost(host)
{}

ECode Connection::VideoProvider::VideoProviderHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case IConnectionVideoProvider::MSG_SET_VIDEO_CALLBACK:
            // mVideoCallback = IVideoCallback.Stub.asInterface((IBinder) msg.obj);
            break;
        case IConnectionVideoProvider::MSG_SET_CAMERA: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->OnSetCamera(strObj);
        }
            break;
        case IConnectionVideoProvider::MSG_SET_PREVIEW_SURFACE:
            mHost->OnSetPreviewSurface(ISurface::Probe(obj));
            break;
        case IConnectionVideoProvider::MSG_SET_DISPLAY_SURFACE:
            mHost->OnSetDisplaySurface(ISurface::Probe(obj));
            break;
        case IConnectionVideoProvider::MSG_SET_DEVICE_ORIENTATION: {
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            mHost->OnSetDeviceOrientation(arg1);
        }
            break;
        case IConnectionVideoProvider::MSG_SET_ZOOM: {
            AutoPtr<IFloat> pObj = IFloat::Probe(obj);
            Float val = 0;
            pObj->GetValue(&val);
            mHost->OnSetZoom(val);
        }
            break;
        case IConnectionVideoProvider::MSG_SEND_SESSION_MODIFY_REQUEST:
            mHost->OnSendSessionModifyRequest(IVideoProfile::Probe(obj));
            break;
        case IConnectionVideoProvider::MSG_SEND_SESSION_MODIFY_RESPONSE:
            mHost->OnSendSessionModifyResponse(IVideoProfile::Probe(obj));
            break;
        case IConnectionVideoProvider::MSG_REQUEST_CAMERA_CAPABILITIES:
            mHost->OnRequestCameraCapabilities();
            break;
        case IConnectionVideoProvider::MSG_REQUEST_CALL_DATA_USAGE:
            mHost->OnRequestCallDataUsage();
            break;
        case IConnectionVideoProvider::MSG_SET_PAUSE_IMAGE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->OnSetPauseImage(strObj);
        }
            break;
        default:
            break;
    }
    return NOERROR;
}

//===============================================================
// Connection::VideoProvider::
//===============================================================

CAR_INTERFACE_IMPL(Connection::VideoProvider, Object, IConnectionVideoProvider)

Connection::VideoProvider::VideoProvider()
{
    mMessageHandler = new VideoProviderHandler(this);
    mMessageHandler->constructor();
    CVideoProviderBinder::New(mMessageHandler, (IIVideoProvider**)&mBinder);
}

ECode Connection::VideoProvider::GetInterface(
    /* [out] */ IIVideoProvider** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBinder;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::VideoProvider::ReceiveSessionModifyRequest(
    /* [in] */ IVideoProfile* videoProfile)
{
    if (mVideoCallback != NULL) {
        mVideoCallback->ReceiveSessionModifyRequest(videoProfile);
    }
    return NOERROR;
}

ECode Connection::VideoProvider::ReceiveSessionModifyResponse(
    /* [in] */ Int32 status,
    /* [in] */ IVideoProfile* requestedProfile,
    /* [in] */ IVideoProfile* responseProfile)
{
    if (mVideoCallback != NULL) {
        mVideoCallback->ReceiveSessionModifyResponse(
                status, requestedProfile, responseProfile);
    }
    return NOERROR;
}

ECode Connection::VideoProvider::HandleCallSessionEvent(
    /* [in] */ Int32 event)
{
    if (mVideoCallback != NULL) {
        mVideoCallback->HandleCallSessionEvent(event);
    }
    return NOERROR;
}

ECode Connection::VideoProvider::ChangePeerDimensions(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mVideoCallback != NULL) {
        mVideoCallback->ChangePeerDimensions(width, height);
    }
    return NOERROR;
}

ECode Connection::VideoProvider::ChangeCallDataUsage(
    /* [in] */ Int32 dataUsage)
{
    if (mVideoCallback != NULL) {
        mVideoCallback->ChangeCallDataUsage(dataUsage);
    }
    return NOERROR;
}

ECode Connection::VideoProvider::ChangeCameraCapabilities(
    /* [in] */ ICameraCapabilities* cameraCapabilities)
{
    if (mVideoCallback != NULL) {
        mVideoCallback->ChangeCameraCapabilities(cameraCapabilities);
    }
    return NOERROR;
}

//===============================================================
// Connection::MyListener::
//===============================================================
Connection::MyListener::MyListener(
    /* [in] */ Connection* host)
    : mHost(host)
{}

ECode Connection::MyListener::OnDestroyed(
    /* [in] */ IConnection* c)
{
    Boolean bRm = FALSE;
    mHost->mConferenceableConnections->Remove(c, &bRm);
    if (bRm) {
        mHost->FireOnConferenceableConnectionsChanged();
    }
    return NOERROR;
}

//===============================================================
// Connection::
//===============================================================

Boolean Connection::PII_DEBUG = Logger::IsLoggable("Connection", Logger::___DEBUG);

CAR_INTERFACE_IMPL(Connection, Object, IConnection)

Connection::Connection()
    : mState(STATE_NEW)
    , mAddressPresentation(0)
    , mCallerDisplayNamePresentation(0)
    , mRingbackRequested(FALSE)
    , mCallCapabilities(0)
    , mAudioModeIsVoip(FALSE)
    , mVideoState(0)
{
    mConnectionDeathListener = new MyListener(this);

    AutoPtr<IMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->SetFromMap(map, (ISet**)&mListeners);
    CArrayList::New((IList**)&mConferenceableConnections);
    cls->UnmodifiableList(mConferenceableConnections, (IList**)&mUnmodifiableConferenceableConnections);
}

ECode Connection::GetAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::GetAddressPresentation(
    /* [out] */ Int32* presentation)
{
    VALIDATE_NOT_NULL(presentation)
    *presentation = mAddressPresentation;
    return NOERROR;
}

ECode Connection::GetCallerDisplayName(
    /* [out] */ String* disName)
{
    VALIDATE_NOT_NULL(disName)
    *disName = mCallerDisplayName;
    return NOERROR;
}

ECode Connection::GetCallerDisplayNamePresentation(
    /* [out] */ Int32* presentation)
{
    VALIDATE_NOT_NULL(presentation)
    *presentation = mCallerDisplayNamePresentation;
    return NOERROR;
}

ECode Connection::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = mState;
    return NOERROR;
}

ECode Connection::GetVideoState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = mVideoState;
    return NOERROR;
}

ECode Connection::GetAudioState(
    /* [out] */ IAudioState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAudioState;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::GetConference(
    /* [out] */ IConference** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConference;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::IsRingbackRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRingbackRequested;
    return NOERROR;
}

ECode Connection::GetAudioModeIsVoip(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAudioModeIsVoip;
    return NOERROR;
}

ECode Connection::GetStatusHints(
    /* [out] */ IStatusHints** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusHints;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::AddConnectionListener(
    /* [in] */ IConnectionListener* l)
{
    mListeners->Add(l);
    return NOERROR;
}

ECode Connection::RemoveConnectionListener(
    /* [in] */ IConnectionListener* l)
{
    if (l != NULL) {
        mListeners->Remove(l);
    }
    return NOERROR;
}

ECode Connection::GetDisconnectCause(
    /* [out] */ IDisconnectCause** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectCause;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::SetAudioState(
    /* [in] */ IAudioState* state)
{
    Logger::D("Connection", "setAudioState %p", state);
    mAudioState = state;
    OnAudioStateChanged(state);
    return NOERROR;
}

ECode Connection::StateToString(
    /* [in] */ Int32 state,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    switch (state) {
        case STATE_INITIALIZING:
            *result = String("STATE_INITIALIZING");
            return NOERROR;
        case STATE_NEW:
            *result = String("STATE_NEW");
            return NOERROR;
        case STATE_RINGING:
            *result = String("STATE_RINGING");
            return NOERROR;
        case STATE_DIALING:
            *result = String("STATE_DIALING");
            return NOERROR;
        case STATE_ACTIVE:
            *result = String("STATE_ACTIVE");
            return NOERROR;
        case STATE_HOLDING:
            *result = String("STATE_HOLDING");
            return NOERROR;
        case STATE_DISCONNECTED:
            *result = String("DISCONNECTED");
            return NOERROR;
        default:
            // Log.wtf(Connection.class, "Unknown state %d", state);
            *result = String("UNKNOWN");
            return NOERROR;
    }
    *result = String(NULL);
    return NOERROR;
}

ECode Connection::GetCallCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallCapabilities;
    return NOERROR;
}

ECode Connection::SetAddress(
    /* [in] */ IUri* address,
    /* [in] */ Int32 presentation)
{
    Logger::D("Connection", "setAddress %p", address);
    mAddress = address;
    mAddressPresentation = presentation;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnAddressChanged(this, address, presentation);
    }
    return NOERROR;
}

ECode Connection::SetCallerDisplayName(
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    Logger::D("Connection", "setCallerDisplayName %s", (const char*)callerDisplayName);
    mCallerDisplayName = callerDisplayName;
    mCallerDisplayNamePresentation = presentation;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> l = IConnectionListener::Probe(p);
        l->OnCallerDisplayNameChanged(this, callerDisplayName, presentation);
    }
    return NOERROR;
}

ECode Connection::SetVideoState(
    /* [in] */ Int32 videoState)
{
    Logger::D("Connection", "setVideoState %d", videoState);
    mVideoState = videoState;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnVideoStateChanged(this, mVideoState);
    }
    return NOERROR;
}

ECode Connection::SetActive()
{
    SetRingbackRequested(FALSE);
    SetState(STATE_ACTIVE);
    return NOERROR;
}

ECode Connection::SetRinging()
{
    SetState(STATE_RINGING);
    return NOERROR;
}

ECode Connection::SetInitializing()
{
    SetState(STATE_INITIALIZING);
    return NOERROR;
}

ECode Connection::SetInitialized()
{
    SetState(STATE_NEW);
    return NOERROR;
}

ECode Connection::SetDialing()
{
    SetState(STATE_DIALING);
    return NOERROR;
}

ECode Connection::SetOnHold()
{
    SetState(STATE_HOLDING);
    return NOERROR;
}

ECode Connection::SetVideoProvider(
    /* [in] */ IConnectionVideoProvider* videoProvider)
{
    mVideoProvider = videoProvider;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnVideoProviderChanged(this, videoProvider);
    }
    return NOERROR;
}

ECode Connection::GetVideoProvider(
    /* [out] */ IConnectionVideoProvider** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoProvider;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::SetDisconnected(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    mDisconnectCause = disconnectCause;
    SetState(STATE_DISCONNECTED);
    Logger::D("Connection", "Disconnected with cause %p", disconnectCause);
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnDisconnected(this, disconnectCause);
    }
    return NOERROR;
}

ECode Connection::SetPostDialWait(
    /* [in] */ const String& remaining)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnPostDialWait(this, remaining);
    }
    return NOERROR;
}

ECode Connection::SetRingbackRequested(
    /* [in] */ Boolean ringback)
{
    if (mRingbackRequested != ringback) {
        mRingbackRequested = ringback;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
            listener->OnRingbackRequested(this, ringback);
        }
    }
    return NOERROR;
}

ECode Connection::SetCallCapabilities(
    /* [in] */ Int32 callCapabilities)
{
    if (mCallCapabilities != callCapabilities) {
        mCallCapabilities = callCapabilities;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
            listener->OnCallCapabilitiesChanged(this, mCallCapabilities);
        }
    }
    return NOERROR;
}

ECode Connection::Destroy()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnDestroyed(this);
    }
    return NOERROR;
}

ECode Connection::SetAudioModeIsVoip(
    /* [in] */ Boolean isVoip)
{
    mAudioModeIsVoip = isVoip;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnAudioModeIsVoipChanged(this, isVoip);
    }
    return NOERROR;
}

ECode Connection::SetStatusHints(
    /* [in] */ IStatusHints* statusHints)
{
    mStatusHints = statusHints;
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnStatusHintsChanged(this, statusHints);
    }
    return NOERROR;
}

ECode Connection::SetConferenceableConnections(
    /* [in] */ IList* conferenceableConnections)
{
    ClearConferenceableList();
    AutoPtr<IIterator> it;
    conferenceableConnections->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnection> c = IConnection::Probe(p);
        // If statement checks for duplicates in input. It makes it N^2 but we're dealing with a
        // small amount of items here.
        Boolean bContains = FALSE;
        if (!(mConferenceableConnections->Contains(c, &bContains), bContains)) {
            c->AddConnectionListener(mConnectionDeathListener);
            mConferenceableConnections->Add(c);
        }
    }
    FireOnConferenceableConnectionsChanged();
    return NOERROR;
}

ECode Connection::GetConferenceableConnections(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUnmodifiableConferenceableConnections;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::SetConnectionService(
    /* [in] */ IConnectionService* connectionService)
{
    if (mConnectionService != NULL) {
        // Logger::E("Connection", new Exception(), "Trying to set ConnectionService on a connection "
        //         "which is already associated with another ConnectionService.");
    }
    else {
        mConnectionService = connectionService;
    }
    return NOERROR;
}

ECode Connection::UnsetConnectionService(
    /* [in] */ IConnectionService* connectionService)
{
    if (!Object::Equals(mConnectionService, connectionService)) {
        // Logger::E("Connection", new Exception(), "Trying to remove ConnectionService from a Connection "
        //         "that does not belong to the ConnectionService.");
    }
    else {
        mConnectionService = NULL;
    }
    return NOERROR;
}

ECode Connection::GetConnectionService(
    /* [out] */ IConnectionService** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectionService;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::SetConference(
    /* [in] */ IConference* conference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // We check to see if it is already part of another conference.
    if (mConference == NULL) {
        mConference = conference;
        Boolean bContains = FALSE;
        if (mConnectionService != NULL &&
            (mConnectionService->ContainsConference(conference, &bContains), bContains)) {
            FireConferenceChanged();
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Connection::ResetConference()
{
    if (mConference != NULL) {
        Logger::D("Connection", "Conference reset");
        mConference = NULL;
        FireConferenceChanged();
    }
    return NOERROR;
}

ECode Connection::OnAudioStateChanged(
    /* [in] */ IAudioState* state)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnStateChanged(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnPlayDtmfTone(
    /* [in] */ Char32 c)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnStopDtmfTone()
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnDisconnect()
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnSeparate()
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnAbort()
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnHold()
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnUnhold()
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnAnswer(
    /* [in] */ Int32 videoState)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnAnswer()
{
    OnAnswer(IVideoProfileVideoState::AUDIO_ONLY);
    return NOERROR;
}

ECode Connection::OnReject()
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnPostDialContinue(
    /* [in] */ Boolean proceed)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::OnConferenceWith(
    /* [in] */ IConnection* otherConnection)
{
    return E_NOT_IMPLEMENTED;
}

ECode Connection::ToLogSafePhoneNumber(
    /* [in] */ const String& number,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // For unknown number, log empty string.
    if (number.IsNull()) {
        *result = "";
        return NOERROR;
    }

    if (PII_DEBUG) {
        // When PII_DEBUG is true we emit PII.
        *result = number;
        return NOERROR;
    }

    // Do exactly same thing as Uri#toSafeString() does, which will enable us to compare
    // sanitized phone numbers.
    StringBuilder builder;
    for (Int32 i = 0; i < number.GetLength(); i++) {
        Char32 c = number.GetChar(i);
        if (c == '-' || c == '@' || c == '.') {
            builder.AppendChar(c);
        }
        else {
            builder.AppendChar('x');
        }
    }
    *result = builder.ToString();
    return NOERROR;
}

void Connection::SetState(
    /* [in] */ Int32 state)
{
    if (mState == STATE_DISCONNECTED && mState != state) {
        Logger::D("Connection", "Connection already DISCONNECTED; cannot transition out of this state.");
        return;
    }
    if (mState != state) {
        String strState;
        StateToString(state, &strState);
        Logger::D("Connection", "setState: %s", (const char*)strState);
        mState = state;
        OnStateChanged(state);
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
            listener->OnStateChanged(this, state);
        }
    }
}

ECode Connection::CreateFailedConnection(
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<FailureSignalingConnection> p = new FailureSignalingConnection(disconnectCause);
    *result = IConnection::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Connection::CreateCanceledConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDisconnectCause> dc;
    CDisconnectCause::New(IDisconnectCause::CANCELED, (IDisconnectCause**)&dc);
    AutoPtr<FailureSignalingConnection> p = new FailureSignalingConnection(dc);
    *result = IConnection::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void Connection::FireOnConferenceableConnectionsChanged()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        AutoPtr<IList> l;
        GetConferenceableConnections((IList**)&l);
        listener->OnConferenceableConnectionsChanged(this, l);
    }
}

void Connection::FireConferenceChanged()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnectionListener> listener = IConnectionListener::Probe(p);
        listener->OnConferenceChanged(this, mConference);
    }
}

void Connection::ClearConferenceableList()
{
    AutoPtr<IIterator> it;
    mConferenceableConnections->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnection> listener = IConnection::Probe(p);
        listener->RemoveConnectionListener(mConnectionDeathListener);
    }
    mConferenceableConnections->Clear();
}

//===============================================================
// FailureSignalingConnection::
//===============================================================

FailureSignalingConnection::FailureSignalingConnection(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    SetDisconnected(disconnectCause);
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos
