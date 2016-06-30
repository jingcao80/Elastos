
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/tv/CTvContentRating.h"
#include "elastos/droid/media/tv/CTvInputManager.h"
#include "elastos/droid/media/tv/CTvInputManagerHardware.h"
#include "elastos/droid/media/tv/CTvInputManagerSession.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

//==============================================================================
// CTvInputManager::PostSessionCreatedRunnable
//==============================================================================

ECode CTvInputManager::PostSessionCreatedRunnable::Run()
{
    return mHost->mSessionCallback->OnSessionCreated(mSession);
}

//==============================================================================
// CTvInputManager::PostSessionReleasedRunnable
//==============================================================================

ECode CTvInputManager::PostSessionReleasedRunnable::Run()
{
    return mHost->mSessionCallback->OnSessionReleased(mHost->mSession);
}

//==============================================================================
// CTvInputManager::PostChannelRetunedRunnable
//==============================================================================

ECode CTvInputManager::PostChannelRetunedRunnable::Run()
{
    return mHost->mSessionCallback->OnChannelRetuned(mHost->mSession, mChannelUri);
}

//==============================================================================
// CTvInputManager::PostTracksChangedRunnable
//==============================================================================

ECode CTvInputManager::PostTracksChangedRunnable::Run()
{
    AutoPtr<CTvInputManagerSession> session = (CTvInputManagerSession*)mHost->mSession.Get();
    session->mAudioTracks->Clear();
    session->mVideoTracks->Clear();
    session->mSubtitleTracks->Clear();

    AutoPtr<ArrayOf<IInterface *> > array;
    mTracks->ToArray((ArrayOf<IInterface *>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<ITvTrackInfo> track = ITvTrackInfo::Probe((*array)[i]);
        Int32 type;
        track->GetType(&type);
        if (type == ITvTrackInfo::TYPE_AUDIO) {
            session->mAudioTracks->Add(track);
        } else if (type == ITvTrackInfo::TYPE_VIDEO) {
            session->mVideoTracks->Add(track);
        } else if (type == ITvTrackInfo::TYPE_SUBTITLE) {
            session->mSubtitleTracks->Add(track);
        } else {
            // Silently ignore.
        }
    }
    return mHost->mSessionCallback->OnTracksChanged(mHost->mSession, mTracks);
}

//==============================================================================
// CTvInputManager::PostTrackSelectedRunnable
//==============================================================================

ECode CTvInputManager::PostTrackSelectedRunnable::Run()
{
    AutoPtr<CTvInputManagerSession> session = (CTvInputManagerSession*)mHost->mSession.Get();
    if (mType == ITvTrackInfo::TYPE_AUDIO) {
        session->mSelectedAudioTrackId = mTrackId;
    } else if (mType == ITvTrackInfo::TYPE_VIDEO) {
        session->mSelectedVideoTrackId = mTrackId;
    } else if (mType == ITvTrackInfo::TYPE_SUBTITLE) {
        session->mSelectedSubtitleTrackId = mTrackId;
    } else {
        // Silently ignore.
        return NOERROR;
    }
    return mHost->mSessionCallback->OnTrackSelected(mHost->mSession, mType, mTrackId);
}

//==============================================================================
// CTvInputManager::PostVideoAvailableRunnable
//==============================================================================

ECode CTvInputManager::PostVideoAvailableRunnable::Run()
{
    return mHost->mSessionCallback->OnVideoAvailable(mHost->mSession);
}

//==============================================================================
// CTvInputManager::PostVideoUnavailableRunnable
//==============================================================================

ECode CTvInputManager::PostVideoUnavailableRunnable::Run()
{
    return mHost->mSessionCallback->OnVideoUnavailable(mHost->mSession, mReason);
}

//==============================================================================
// CTvInputManager::PostContentAllowedRunnable
//==============================================================================

ECode CTvInputManager::PostContentAllowedRunnable::Run()
{
    return mHost->mSessionCallback->OnContentAllowed(mHost->mSession);
}

//==============================================================================
// CTvInputManager::PostContentBlockedRunnable
//==============================================================================

ECode CTvInputManager::PostContentBlockedRunnable::Run()
{
    return mHost->mSessionCallback->OnContentBlocked(mHost->mSession, mRating);
}

//==============================================================================
// CTvInputManager::PostLayoutSurfaceRunnable
//==============================================================================

ECode CTvInputManager::PostLayoutSurfaceRunnable::Run()
{
    return mHost->mSessionCallback->OnLayoutSurface(mHost->mSession, mLeft, mTop, mRight, mBottom);
}

//==============================================================================
// CTvInputManager::PostSessionEventRunnable
//==============================================================================

ECode CTvInputManager::PostSessionEventRunnable::Run()
{
    return mHost->mSessionCallback->OnSessionEvent(mHost->mSession, mEventType, mEventArgs);
}

//==============================================================================
// CTvInputManager::PostInputStateChangedRunnable
//==============================================================================

ECode CTvInputManager::PostInputStateChangedRunnable::Run()
{
    return mHost->mCallback->OnInputStateChanged(mInputId, mState);
}

//==============================================================================
// CTvInputManager::PostInputAddedRunnable
//==============================================================================

ECode CTvInputManager::PostInputAddedRunnable::Run()
{
    return mHost->mCallback->OnInputAdded(mInputId);
}

//==============================================================================
// CTvInputManager::PostInputRemovedRunnable
//==============================================================================

ECode CTvInputManager::PostInputRemovedRunnable::Run()
{
    return mHost->mCallback->OnInputRemoved(mInputId);
}

//==============================================================================
// CTvInputManager::PostInputUpdatedRunnable
//==============================================================================

ECode CTvInputManager::PostInputUpdatedRunnable::Run()
{
    return mHost->mCallback->OnInputUpdated(mInputId);
}

//==============================================================================
// CTvInputManager::TvInputHardwareCallback
//==============================================================================

CAR_INTERFACE_IMPL_2(CTvInputManager::TvInputClient, Object, IITvInputClient, IBinder)

ECode CTvInputManager::TvInputClient::OnSessionCreated(
    /* [in] */ const String& inputId,
    /* [in] */ IBinder* token,
    /* [in] */ IInputChannel* channel,
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            String str;
            token->ToString(&str);
            Logger::E(TAG, String("Callback not found for ") + str);
            return NOERROR;
        }
        AutoPtr<ITvInputManagerSession> session;
        if (token != NULL) {
            CTvInputManagerSession::New(token, channel, mHost->mService, mHost->mUserId, seq,
                    mHost->mSessionCallbackRecordMap, (ITvInputManagerSession**)&session);
        }
        record->PostSessionCreated(session);
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnSessionReleased(
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        mHost->mSessionCallbackRecordMap->Delete(seq);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        ((CTvInputManagerSession*)record->mSession.Get())->ReleaseInternal();
        record->PostSessionReleased();
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnSessionEvent(
    /* [in] */ const String& eventType,
    /* [in] */ IBundle* eventArgs,
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        record->PostSessionEvent(eventType, eventArgs);
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnChannelRetuned(
    /* [in] */ IUri* channelUri,
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        record->PostChannelRetuned(channelUri);
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnTracksChanged(
    /* [in] */ IList* tracks,
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        record->PostTracksChanged(tracks);
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnTrackSelected(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId,
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        record->PostTrackSelected(type, trackId);
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnVideoAvailable(
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        record->PostVideoAvailable();
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnVideoUnavailable(
    /* [in] */ Int32 reason,
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        record->PostVideoUnavailable(reason);
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnContentAllowed(
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        record->PostContentAllowed();
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnContentBlocked(
    /* [in] */ const String& rating,
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        AutoPtr<ITvContentRating> r;
        CTvContentRating::UnflattenFromString(rating, (ITvContentRating**)&r);
        record->PostContentBlocked(r);
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::OnLayoutSurface(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 seq)
{
    AutoPtr<ISparseArray> lock = mHost->mSessionCallbackRecordMap;
    {    AutoLock syncLock(lock);
        AutoPtr<IInterface> obj;
        mHost->mSessionCallbackRecordMap->Get(seq, (IInterface**)&obj);
        AutoPtr<SessionCallbackRecord> record = (SessionCallbackRecord*)IObject::Probe(obj);
        if (record == NULL) {
            Logger::E(TAG, "Callback not found for seq %d", seq);
            return NOERROR;
        }
        record->PostLayoutSurface(left, top, right, bottom);
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputClient::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = "CTvInputManager::TvInputClient";
    return NOERROR;
}

//==============================================================================
// CTvInputManager::TvInputManagerCallback
//==============================================================================

CAR_INTERFACE_IMPL_2(CTvInputManager::TvInputManagerCallback, Object, IITvInputManagerCallback, IBinder)

ECode CTvInputManager::TvInputManagerCallback::OnInputStateChanged(
    /* [in] */ const String& inputId,
    /* [in] */ Int32 state)
{
    Object& lock = mHost->mLock;
    {    AutoLock syncLock(lock);
        AutoPtr<ICharSequence> csq;
        CString::New(inputId, (ICharSequence**)&csq);
        AutoPtr<IInteger32> i;
        CInteger32::New(state, (IInteger32**)&i);
        mHost->mStateMap->Put(csq, i);

        AutoPtr<ArrayOf<IInterface *> > array;
        mHost->mCallbackRecords->ToArray((ArrayOf<IInterface *>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<TvInputCallbackRecord> record = (TvInputCallbackRecord*)(IObject*)((*array)[i]);
            record->PostInputStateChanged(inputId, state);
        }
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputManagerCallback::OnInputAdded(
    /* [in] */ const String& inputId)
{
    Object& lock = mHost->mLock;
    {    AutoLock syncLock(lock);
        AutoPtr<ICharSequence> csq;
        CString::New(inputId, (ICharSequence**)&csq);
        AutoPtr<IInteger32> i;
        CInteger32::New(ITvInputManager::INPUT_STATE_CONNECTED, (IInteger32**)&i);
        mHost->mStateMap->Put(csq, i);

        AutoPtr<ArrayOf<IInterface *> > array;
        mHost->mCallbackRecords->ToArray((ArrayOf<IInterface *>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<TvInputCallbackRecord> record = (TvInputCallbackRecord*)(IObject*)((*array)[i]);
            record->PostInputAdded(inputId);
        }
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputManagerCallback::OnInputRemoved(
    /* [in] */ const String& inputId)
{
    Object& lock = mHost->mLock;
    {    AutoLock syncLock(lock);
        AutoPtr<ICharSequence> csq;
        CString::New(inputId, (ICharSequence**)&csq);
        mHost->mStateMap->Remove(csq);

        AutoPtr<ArrayOf<IInterface *> > array;
        mHost->mCallbackRecords->ToArray((ArrayOf<IInterface *>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<TvInputCallbackRecord> record = (TvInputCallbackRecord*)(IObject*)((*array)[i]);
            record->PostInputRemoved(inputId);
        }
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputManagerCallback::OnInputUpdated(
    /* [in] */ const String& inputId)
{
    Object& lock = mHost->mLock;
    {    AutoLock syncLock(lock);
        AutoPtr<ArrayOf<IInterface *> > array;
        mHost->mCallbackRecords->ToArray((ArrayOf<IInterface *>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<TvInputCallbackRecord> record = (TvInputCallbackRecord*)(IObject*)((*array)[i]);
            record->PostInputUpdated(inputId);
        }
    }
    return NOERROR;
}

ECode CTvInputManager::TvInputManagerCallback::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = "CTvInputManager::TvInputManagerCallback";
    return NOERROR;
}

//==============================================================================
// CTvInputManager::TvInputHardwareCallback
//==============================================================================

CAR_INTERFACE_IMPL_2(CTvInputManager::TvInputHardwareCallback, Object, IITvInputHardwareCallback, IBinder)

ECode CTvInputManager::TvInputHardwareCallback::OnReleased()
{
    return mCallback->OnReleased();
}

ECode CTvInputManager::TvInputHardwareCallback::OnStreamConfigChanged(
    /* [in] */ ArrayOf<ITvStreamConfig*>* configs)
{
    return mCallback->OnStreamConfigChanged(configs);
}

ECode CTvInputManager::TvInputHardwareCallback::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = "CTvInputManager::TvInputHardwareCallback";
    return NOERROR;
}

//==============================================================================
// CTvInputManager::SessionCallbackRecord
//==============================================================================

CTvInputManager::SessionCallbackRecord::SessionCallbackRecord(
    /* [in] */ ITvInputManagerSessionCallback* sessionCallback,
    /* [in] */ IHandler* handler,
    /* [in] */ CTvInputManager* host)
    : mSessionCallback(sessionCallback)
    , mHandler(handler)
    , mHost(host)
{
}

ECode CTvInputManager::SessionCallbackRecord::PostSessionCreated(
    /* [in] */ ITvInputManagerSession* session)
{
    mSession = session;
    AutoPtr<PostSessionCreatedRunnable> run = new PostSessionCreatedRunnable(this, session);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostSessionReleased()
{
    AutoPtr<PostSessionReleasedRunnable> run = new PostSessionReleasedRunnable(this);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostChannelRetuned(
    /* [in] */ IUri* channelUri)
{
    AutoPtr<PostChannelRetunedRunnable> run = new PostChannelRetunedRunnable(this, channelUri);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostTracksChanged(
    /* [in] */ IList* tracks)
{
    AutoPtr<PostTracksChangedRunnable> run = new PostTracksChangedRunnable(this, tracks);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostTrackSelected(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    AutoPtr<PostTrackSelectedRunnable> run = new PostTrackSelectedRunnable(this, type, trackId);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostVideoAvailable()
{
    AutoPtr<PostVideoAvailableRunnable> run = new PostVideoAvailableRunnable(this);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostVideoUnavailable(
    /* [in] */ Int32 reason)
{
    AutoPtr<PostVideoUnavailableRunnable> run = new PostVideoUnavailableRunnable(this, reason);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostContentAllowed()
{
    AutoPtr<PostContentAllowedRunnable> run = new PostContentAllowedRunnable(this);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostContentBlocked(
    /* [in] */ ITvContentRating* rating)
{
    AutoPtr<PostContentBlockedRunnable> run = new PostContentBlockedRunnable(this, rating);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostLayoutSurface(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<PostLayoutSurfaceRunnable> run = new PostLayoutSurfaceRunnable(this, left, top, right, bottom);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::SessionCallbackRecord::PostSessionEvent(
    /* [in] */ const String& eventType,
    /* [in] */ IBundle* eventArgs)
{
    AutoPtr<PostSessionEventRunnable> run = new PostSessionEventRunnable(this, eventType, eventArgs);
    Boolean b;
    return mHandler->Post(run, &b);
}

//==============================================================================
// CTvInputManager::TvInputCallbackRecord
//==============================================================================

CTvInputManager::TvInputCallbackRecord::TvInputCallbackRecord(
    /* [in] */ ITvInputManagerTvInputCallback* callback,
    /* [in] */ IHandler* handler,
    /* [in] */ CTvInputManager* host)
    : mCallback(callback)
    , mHandler(handler)
    , mHost(host)
{
}

ECode CTvInputManager::TvInputCallbackRecord::GetCallback(
    /* [out] */ ITvInputManagerTvInputCallback** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallback;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTvInputManager::TvInputCallbackRecord::PostInputStateChanged(
    /* [in] */ const String& inputId,
    /* [in] */ Int32 state)
{
    AutoPtr<PostInputStateChangedRunnable> run = new PostInputStateChangedRunnable(this, inputId, state);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::TvInputCallbackRecord::PostInputAdded(
    /* [in] */ const String& inputId)
{
    AutoPtr<PostInputAddedRunnable> run = new PostInputAddedRunnable(this, inputId);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::TvInputCallbackRecord::PostInputRemoved(
    /* [in] */ const String& inputId)
{
    AutoPtr<PostInputRemovedRunnable> run = new PostInputRemovedRunnable(this, inputId);
    Boolean b;
    return mHandler->Post(run, &b);
}

ECode CTvInputManager::TvInputCallbackRecord::PostInputUpdated(
    /* [in] */ const String& inputId)
{
    AutoPtr<PostInputUpdatedRunnable> run = new PostInputUpdatedRunnable(this, inputId);
    Boolean b;
    return mHandler->Post(run, &b);
}

//==============================================================================
// CTvInputManager
//==============================================================================

String CTvInputManager::TAG("TvInputManager");

CAR_INTERFACE_IMPL(CTvInputManager, Object, ITvInputManager)

CAR_OBJECT_IMPL(CTvInputManager)

CTvInputManager::CTvInputManager()
{
    CLinkedList::New((ILinkedList**)&mCallbackRecords);
    CArrayMap::New((IArrayMap**)&mStateMap);
    CSparseArray::New((ISparseArray**)&mSessionCallbackRecordMap);
}

CTvInputManager::~CTvInputManager()
{
}

ECode CTvInputManager::constructor(
    /* [in] */ IITvInputManager* service,
    /* [in] */ Int32 userId)
{
    mService = service;
    mUserId = userId;
    mClient = new TvInputClient(this);
    mManagerCallback = new TvInputManagerCallback(this);

    // try {
    if (mService != NULL) {
        mService->RegisterCallback(mManagerCallback, mUserId);
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "mService.registerCallback failed: " + e);
    // }
    return NOERROR;
}

ECode CTvInputManager::GetTvInputList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mService->GetTvInputList(mUserId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::GetTvInputInfo(
    /* [in] */ const String& inputId,
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (inputId == NULL) {
        // throw new IllegalArgumentException("inputId cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    return mService->GetTvInputInfo(inputId, mUserId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::GetInputState(
    /* [in] */ const String& inputId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (inputId == NULL) {
        // throw new IllegalArgumentException("inputId cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {    AutoLock syncLock(mLock);
        AutoPtr<ICharSequence> csq;
        CString::New(inputId, (ICharSequence**)&csq);
        AutoPtr<IInterface> state;
        mStateMap->Get(csq, (IInterface**)&state);
        if (state == NULL) {
            // throw new IllegalArgumentException("Unrecognized input ID: " + inputId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return IInteger32::Probe(state)->GetValue(result);
    }
    return NOERROR;
}

ECode CTvInputManager::RegisterCallback(
    /* [in] */ ITvInputManagerTvInputCallback* callback,
    /* [in] */ IHandler* handler)
{
    if (callback == NULL) {
        // throw new IllegalArgumentException("callback cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (handler == NULL) {
        // throw new IllegalArgumentException("handler cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {    AutoLock syncLock(mLock);
        AutoPtr<TvInputCallbackRecord> r = new TvInputCallbackRecord(callback, handler, this);
        mCallbackRecords->Add((IInterface*)(IObject*)r);
    }
    return NOERROR;
}

ECode CTvInputManager::UnregisterCallback(
    /* [in] */ ITvInputManagerTvInputCallback* callback)
{
    if (callback == NULL) {
        // throw new IllegalArgumentException("callback cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {    AutoLock syncLock(mLock);
        AutoPtr<ArrayOf<IInterface *> > array;
        mCallbackRecords->ToArray((ArrayOf<IInterface *>**)&array);
        for (Int32 i = 0 ; i < array->GetLength(); i++) {
            AutoPtr<TvInputCallbackRecord> record = (TvInputCallbackRecord*)(IObject*)(*array)[i];
            AutoPtr<ITvInputManagerTvInputCallback> cb;
            record->GetCallback((ITvInputManagerTvInputCallback**)&cb);
            if (cb.Get() == callback) {
                mCallbackRecords->Remove((IInterface*)(IObject*)record);
                break;
            }
        }
    }
    return NOERROR;
}

ECode CTvInputManager::IsParentalControlsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mService->IsParentalControlsEnabled(mUserId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::SetParentalControlsEnabled(
    /* [in] */ Boolean enabled)
{
    // try {
    return mService->SetParentalControlsEnabled(enabled, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::IsRatingBlocked(
    /* [in] */ ITvContentRating* rating,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (rating == NULL) {
        // throw new IllegalArgumentException("rating cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    String str;
    rating->FlattenToString(&str);
    return mService->IsRatingBlocked(str, mUserId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::GetBlockedRatings(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IArrayList> ratings;
    CArrayList::New((IArrayList**)&ratings);

    AutoPtr<IList> list;
    mService->GetBlockedRatings(mUserId, (IList**)&list);
    AutoPtr<ArrayOf<IInterface *> > array;
    list->ToArray((ArrayOf<IInterface *>**)&array);
    for (Int32 i = 0 ; i < array->GetLength(); i++) {
        String rating;
        ICharSequence::Probe((*array)[i])->ToString(&rating);
        AutoPtr<ITvContentRating> r;
        CTvContentRating::UnflattenFromString(rating, (ITvContentRating**)&r);
        ratings->Add(r);
    }
    *result = IList::Probe(ratings);
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::AddBlockedRating(
    /* [in] */ ITvContentRating* rating)
{
    if (rating == NULL) {
        // throw new IllegalArgumentException("rating cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    String str;
    rating->FlattenToString(&str);
    return mService->AddBlockedRating(str, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::RemoveBlockedRating(
    /* [in] */ ITvContentRating* rating)
{
    if (rating == NULL) {
        // throw new IllegalArgumentException("rating cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    String str;
    rating->FlattenToString(&str);
    return mService->RemoveBlockedRating(str, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::GetTvContentRatingSystemList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mService->GetTvContentRatingSystemList(mUserId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::CreateSession(
    /* [in] */ const String& inputId,
    /* [in] */ ITvInputManagerSessionCallback* callback,
    /* [in] */ IHandler* handler)
{
    if (inputId == NULL) {
        // throw new IllegalArgumentException("id cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (callback == NULL) {
        // throw new IllegalArgumentException("callback cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (handler == NULL) {
        // throw new IllegalArgumentException("handler cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<SessionCallbackRecord> record = new SessionCallbackRecord(callback, handler, this);
    {    AutoLock syncLock(mSessionCallbackRecordMap);
        Int32 seq = mNextSeq++;
        mSessionCallbackRecordMap->Put(seq, (IInterface*)(IObject*)record);
        // try {
        mService->CreateSession(mClient, inputId, seq, mUserId);
        // } catch (RemoteException e) {
        //     throw new RuntimeException(e);
        // }
    }
    return NOERROR;
}

ECode CTvInputManager::GetAvailableTvStreamConfigList(
    /* [in] */ const String& inputId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mService->GetAvailableTvStreamConfigList(inputId, mUserId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::CaptureFrame(
    /* [in] */ const String& inputId,
    /* [in] */ ISurface* surface,
    /* [in] */ ITvStreamConfig* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mService->CaptureFrame(inputId, surface, config, mUserId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::IsSingleSessionActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mService->IsSingleSessionActive(mUserId, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
   // return NOERROR;
}

ECode CTvInputManager::GetHardwareList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mService->GetHardwareList(result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::AcquireTvInputHardware(
    /* [in] */ Int32 deviceId,
    /* [in] */ ITvInputManagerHardwareCallback* callback,
    /* [in] */ ITvInputInfo* info,
    /* [out] */ ITvInputManagerHardware** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<TvInputHardwareCallback> cb = new TvInputHardwareCallback(this, callback);
    AutoPtr<IITvInputHardware> hw;
    mService->AcquireTvInputHardware(deviceId, cb,  info, mUserId, (IITvInputHardware**)&hw);
    return CTvInputManagerHardware::New(hw, result);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManager::ReleaseTvInputHardware(
    /* [in] */ Int32 deviceId,
    /* [in] */ ITvInputManagerHardware* hardware)
{
    // try {
    AutoPtr<CTvInputManagerHardware> h = (CTvInputManagerHardware*)hardware;
    return mService->ReleaseTvInputHardware(deviceId, h->GetInterface(), mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
