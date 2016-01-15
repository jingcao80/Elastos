
#include "elastos/droid/media/CRemoteControlClient.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/media/CAudioSystemHelper.h"
#include "elastos/droid/os/CServiceManager.h"
#include <elastos/core/Math.h>
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRectF.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BitmapConfig_NONE;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;

namespace Elastos {
namespace Droid {
namespace Media {

static AutoPtr< ArrayOf<Int32> > InitMETADATA_KEYS_TYPE_STRING()
{
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(11);
    array->Set(0, IMediaMetadataRetriever::METADATA_KEY_ALBUM);
    array->Set(1, IMediaMetadataRetriever::METADATA_KEY_ALBUMARTIST);
    array->Set(2, IMediaMetadataRetriever::METADATA_KEY_TITLE);
    array->Set(3, IMediaMetadataRetriever::METADATA_KEY_ARTIST);
    array->Set(4, IMediaMetadataRetriever::METADATA_KEY_AUTHOR);
    array->Set(5, IMediaMetadataRetriever::METADATA_KEY_COMPILATION);
    array->Set(6, IMediaMetadataRetriever::METADATA_KEY_COMPOSER);
    array->Set(7, IMediaMetadataRetriever::METADATA_KEY_DATE);
    array->Set(8, IMediaMetadataRetriever::METADATA_KEY_GENRE);
    array->Set(9, IMediaMetadataRetriever::METADATA_KEY_TITLE);
    array->Set(10, IMediaMetadataRetriever::METADATA_KEY_WRITER);
    return array;
}

static AutoPtr< ArrayOf<Int32> > InitMETADATA_KEYS_TYPE_LONG()
{
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(3);

    array->Set(0, IMediaMetadataRetriever::METADATA_KEY_CD_TRACK_NUMBER);
    array->Set(1, IMediaMetadataRetriever::METADATA_KEY_DISC_NUMBER);
    array->Set(2, IMediaMetadataRetriever::METADATA_KEY_DURATION);
    return array;

}

AutoPtr< ArrayOf<Int32> > CRemoteControlClient::METADATA_KEYS_TYPE_STRING = ArrayOf<Int32>::Alloc(11);
AutoPtr< ArrayOf<Int32> > CRemoteControlClient::METADATA_KEYS_TYPE_LONG = ArrayOf<Int32>::Alloc(3);

const String CRemoteControlClient::TAG("RemoteControlClient");

const Int32 CRemoteControlClient::PLAYBACK_TYPE_MIN = IRemoteControlClient::PLAYBACK_TYPE_LOCAL;
const Int32 CRemoteControlClient::PLAYBACK_TYPE_MAX = IRemoteControlClient::PLAYBACK_TYPE_REMOTE;
AutoPtr<IIAudioService> CRemoteControlClient::sService;

const Int32 CRemoteControlClient::MSG_REQUEST_PLAYBACK_STATE = 1;
const Int32 CRemoteControlClient::MSG_REQUEST_METADATA = 2;
const Int32 CRemoteControlClient::MSG_REQUEST_TRANSPORTCONTROL = 3;
const Int32 CRemoteControlClient::MSG_REQUEST_ARTWORK = 4;
const Int32 CRemoteControlClient::MSG_NEW_INTERNAL_CLIENT_GEN = 5;
const Int32 CRemoteControlClient::MSG_NEW_CURRENT_CLIENT_GEN = 6;
const Int32 CRemoteControlClient::MSG_PLUG_DISPLAY = 7;
const Int32 CRemoteControlClient::MSG_UNPLUG_DISPLAY = 8;

//----------------------------------------
//    CRemoteControlClient::MetadataEditor
//----------------------------------------

CAR_INTERFACE_IMPL(CRemoteControlClient::MetadataEditor, IMetadataEditor);

CRemoteControlClient::MetadataEditor::MetadataEditor(
    /* [in] */ CRemoteControlClient* owner)
    : mApplied(FALSE)
    , mOwner(owner)
{}

ECode CRemoteControlClient::MetadataEditor::Clone( // throws CloneNotSupportedException
    /* [out] */ IMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);
    assert(0);
//    throw new CloneNotSupportedException();
    return E_NOT_IMPLEMENTED;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::PutString( // throws IllegalArgumentException
    /* [in] */ Int32 key,
    /* [in] */ const String& value,
    /* [out] */ IMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mThisLock);

    if (mApplied) {
        Logger::E(TAG, "Can't edit a previously applied MetadataEditor");
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (!ValidTypeForKey(key, METADATA_KEYS_TYPE_STRING)) {
        Logger::E(TAG, "Invalid type 'String' for key %d", key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mEditorMetadata->PutString(StringUtils::Int32ToString(key), value);
    mMetadataChanged = TRUE;
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::PutInt64( // throws IllegalArgumentException
    /* [in] */ Int32 key,
    /* [in] */ Int64 value,
    /* [out] */ IMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mThisLock);

    if (mApplied) {
        Logger::E(TAG, "Can't edit a previously applied MetadataEditor");
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    if (!ValidTypeForKey(key, METADATA_KEYS_TYPE_LONG)) {
        //throw(new IllegalArgumentException("Invalid type 'long' for key "+ key));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mEditorMetadata->PutInt64(StringUtils::Int32ToString(key), value);
    mMetadataChanged = TRUE;
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::PutBitmap( // throws IllegalArgumentException
    /* [in] */ Int32 key,
    /* [in] */ IBitmap* bitmap,
    /* [out] */ IMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mThisLock);

    if (mApplied) {
        Logger::E(TAG, "Can't edit a previously applied MetadataEditor");
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (key != BITMAP_KEY_ARTWORK) {
        Logger::E(TAG, "Invalid type 'Bitmap' for key %d", key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((mOwner->mArtworkExpectedWidth > 0) && (mOwner->mArtworkExpectedHeight > 0)) {
        mEditorArtwork = mOwner->ScaleBitmapIfTooBig(bitmap,
                mOwner->mArtworkExpectedWidth, mOwner->mArtworkExpectedHeight);
    } else {
        // no valid resize dimensions, store as is
        mEditorArtwork = bitmap;
    }
    mArtworkChanged = TRUE;
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::Clear()
{
    AutoLock lock(mThisLock);

    if (mApplied) {
        Logger::E(TAG, "Can't clear a previously applied MetadataEditor");
        return NOERROR;
    }
    mEditorMetadata->Clear();
    mEditorArtwork = NULL;
    return NOERROR;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::Apply()
{
    AutoLock lock(mThisLock);

    if (mApplied) {
        Logger::E(TAG, "Can't apply a previously applied MetadataEditor");
        return NOERROR;
    }
    {
        AutoLock lock(mOwner->mCacheLock);
        // assign the edited data
        mOwner->mMetadata = NULL;
        CBundle::New(mEditorMetadata, (IBundle**)&mOwner->mMetadata);

        if ((mOwner->mArtwork != NULL) && (!(mOwner->mArtwork == mEditorArtwork))) {
            mOwner->mArtwork->Recycle();
        }
        mOwner->mArtwork = mEditorArtwork;
        mEditorArtwork = NULL;
        if (mMetadataChanged & mArtworkChanged) {
            // send to remote control display if conditions are met
            mOwner->SendMetadataWithArtwork_syncCacheLock();
        } else if (mMetadataChanged) {
            // send to remote control display if conditions are met
            mOwner->SendMetadata_syncCacheLock();
        } else if (mArtworkChanged) {
            // send to remote control display if conditions are met
            mOwner->SendArtwork_syncCacheLock();
        }
        mApplied = TRUE;
    }
    return NOERROR;
}

CRemoteControlClient::CRemoteControlClient()
    : mPlaybackType(PLAYBACK_TYPE_LOCAL)
    , mPlaybackVolumeMax(DEFAULT_PLAYBACK_VOLUME)
    , mPlaybackVolume(DEFAULT_PLAYBACK_VOLUME)
    , mPlaybackVolumeHandling(DEFAULT_PLAYBACK_VOLUME_HANDLING)
    , mPlaybackStream(IAudioManager::STREAM_MUSIC)
    , mPlaybackState(PLAYSTATE_NONE)
    , mPlaybackStateChangeTimeMs(0)
    , ARTWORK_DEFAULT_SIZE(256)
    , ARTWORK_INVALID_SIZE(-1)
    , mArtworkExpectedWidth(ARTWORK_DEFAULT_SIZE)
    , mArtworkExpectedHeight(ARTWORK_DEFAULT_SIZE)
    , mTransportControlFlags(FLAGS_KEY_MEDIA_NONE)
    , mCurrentClientGenId(-1)
    , mInternalClientGenId(-2)
    , mRcseId(RCSE_ID_UNREGISTERED)
{
    CBundle::New((IBundle**)&mMetadata);
    mIRCC = new MyRemoteControlClient(this);
}

ECode CRemoteControlClient::constructor(
    /* [in] */ IPendingIntent* mediaButtonIntent)
{
    mRcMediaIntent = mediaButtonIntent;

    AutoPtr<ILooper> looper;
    AutoPtr<ILooperHelper> looperHelper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
    if ((looperHelper->MyLooper((ILooper**)&looper), looper) != NULL) {
        mEventHandler = new EventHandler(this, looper, this);
    } else if ((looperHelper->GetMainLooper((ILooper**)&looper), looper) != NULL) {
        mEventHandler = new EventHandler(this, looper, this);
    } else {
        mEventHandler = NULL;
        Logger::E(TAG, "RemoteControlClient() couldn't find main application thread");
    }
    return NOERROR;
}

ECode CRemoteControlClient::constructor(
    /* [in] */ IPendingIntent* mediaButtonIntent,
    /* [in] */ ILooper* looper)
{
    mRcMediaIntent = mediaButtonIntent;

    mEventHandler = new EventHandler(this, looper, this);
    return NOERROR;
}

ECode CRemoteControlClient::EditMetadata(
    /* [in] */ Boolean startEmpty,
    /* [out] */ IMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<MetadataEditor> editor = new MetadataEditor(this);

    if (startEmpty) {
        CBundle::New((IBundle**)(&(editor->mEditorMetadata)));
        editor->mEditorArtwork = NULL;
        editor->mMetadataChanged = TRUE;
        editor->mArtworkChanged = TRUE;
    }
    else {
        CBundle::New(mMetadata, (IBundle**)(&(editor->mEditorMetadata)));
        editor->mEditorArtwork = mArtwork;
        editor->mMetadataChanged = FALSE;
        editor->mArtworkChanged = FALSE;
    }

    *result = editor;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRemoteControlClient::SetPlaybackState(
    /* [in] */ Int32 state)
{
    AutoLock lock(mCacheLock);

    if (mPlaybackState != state) {
        // store locally
        mPlaybackState = state;
        // keep track of when the state change occurred
        mPlaybackStateChangeTimeMs = SystemClock::GetElapsedRealtime();

        // send to remote control display if conditions are met
        SendPlaybackState_syncCacheLock();
        // update AudioService
        SendAudioServiceNewPlaybackInfo_syncCacheLock(PLAYBACKINFO_PLAYSTATE, state);
    }

    return NOERROR;
}

ECode CRemoteControlClient::SetTransportControlFlags(
    /* [in] */ Int32 transportControlFlags)
{

    AutoLock lock(mCacheLock);
    // store locally
    mTransportControlFlags = transportControlFlags;

    // send to remote control display if conditions are met
    SendTransportControlFlags_syncCacheLock();

    return NOERROR;
}

ECode CRemoteControlClient::SetPlaybackInformation(
    /* [in] */ Int32 what,
    /* [in] */ Int32 value)
{
    {
        AutoLock lock(mCacheLock);
        switch (what) {
            case PLAYBACKINFO_PLAYBACK_TYPE:
            {
                if ((value >= PLAYBACK_TYPE_MIN) && (value <= PLAYBACK_TYPE_MAX)) {
                    if (mPlaybackType != value) {
                        mPlaybackType = value;
                        SendAudioServiceNewPlaybackInfo_syncCacheLock(what, value);
                    }
                } else {
                    Logger::W(TAG, "using invalid value for PLAYBACKINFO_PLAYBACK_TYPE");
                }
                break;
            }
            case PLAYBACKINFO_VOLUME:
            {
                if ((value > -1) && (value <= mPlaybackVolumeMax)) {
                    if (mPlaybackVolume != value) {
                        mPlaybackVolume = value;
                        SendAudioServiceNewPlaybackInfo_syncCacheLock(what, value);
                    }
                } else {
                    Logger::W(TAG, "using invalid value for PLAYBACKINFO_VOLUME");
                }
                break;
            }
            case PLAYBACKINFO_VOLUME_MAX:
            {
                if (value > 0) {
                    if (mPlaybackVolumeMax != value) {
                        mPlaybackVolumeMax = value;
                        SendAudioServiceNewPlaybackInfo_syncCacheLock(what, value);
                    }
                } else {
                    Logger::W(TAG, "using invalid value for PLAYBACKINFO_VOLUME_MAX");
                }
                break;
            }
            case PLAYBACKINFO_USES_STREAM:
            {
                AutoPtr<IAudioSystemHelper> audioSystemHelper;
                CAudioSystemHelper::AcquireSingleton((IAudioSystemHelper**)&audioSystemHelper);
                Int32 tempValue;
                if ((value >= 0) && (value < (audioSystemHelper->GetNumStreamTypes(&tempValue), tempValue))) {
                    mPlaybackStream = value;
                } else {
                    Logger::W(TAG, "using invalid value for PLAYBACKINFO_USES_STREAM");
                }
                break;
            }
            case PLAYBACKINFO_VOLUME_HANDLING:
            {
                if ((value >= PLAYBACK_VOLUME_FIXED) && (value <= PLAYBACK_VOLUME_VARIABLE)) {
                    if (mPlaybackVolumeHandling != value) {
                        mPlaybackVolumeHandling = value;
                        SendAudioServiceNewPlaybackInfo_syncCacheLock(what, value);
                    }
                } else {
                    Logger::W(TAG, "using invalid value for PLAYBACKINFO_VOLUME_HANDLING");
                }
                break;
            }
            default:
            {
                // not throwing an exception or returning an error if more keys are to be
                // supported in the future
                Logger::W(TAG, String("setPlaybackInformation() ignoring unknown key ") + StringUtils::Int32ToString(what));
                break;
            }
        }
    }
    return NOERROR;
}

ECode CRemoteControlClient::GetInt32PlaybackInformation(
    /* [in] */ Int32 what,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    {
        AutoLock lock(mCacheLock);

        switch (what) {
            case PLAYBACKINFO_PLAYBACK_TYPE:
            {
                *result = mPlaybackType;
            }
            case PLAYBACKINFO_VOLUME:
            {
                *result = mPlaybackVolume;
            }
            case PLAYBACKINFO_VOLUME_MAX:
            {
                *result = mPlaybackVolumeMax;
            }
            case PLAYBACKINFO_USES_STREAM:
            {
                *result = mPlaybackStream;
            }
            case PLAYBACKINFO_VOLUME_HANDLING:
            {
                *result = mPlaybackVolumeHandling;
            }
            default:
            {
                Logger::E(TAG, String("getIntPlaybackInformation() unknown key ") + StringUtils::Int32ToString(what));
                *result = PLAYBACKINFO_INVALID_VALUE;
            }
        }
    }
    return NOERROR;
}

ECode CRemoteControlClient::GetRcMediaIntent(
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRcMediaIntent;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRemoteControlClient::GetIRemoteControlClient(
    /* [out] */ IIRemoteControlClient** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIRCC;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRemoteControlClient::SetRcseId(
    /* [in] */ Int32 id)
{
    mRcseId = id;
    return NOERROR;
}

ECode CRemoteControlClient::GetRcseId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRcseId;
    return NOERROR;
}

//-----------------------------------------------
//    CRemoteControlClient::MyRemoteControlClient
//-----------------------------------------------

CAR_INTERFACE_IMPL(CRemoteControlClient::MyRemoteControlClient, IIRemoteControlClient);

CRemoteControlClient::MyRemoteControlClient::MyRemoteControlClient(
    /* [in] */ CRemoteControlClient* owner)
    : mOwner(owner)
{}

ECode CRemoteControlClient::MyRemoteControlClient::OnInformationRequested(
    /* [in] */ Int32 clientGeneration,
    /* [in] */ Int32 infoFlags,
    /* [in] */ Int32 artWidth,
    /* [in] */ Int32 artHeight)
{
    // only post messages, we can't block here
    if (mOwner->mEventHandler != NULL) {
        // signal new client
        mOwner->mEventHandler->RemoveMessages(CRemoteControlClient::MSG_NEW_INTERNAL_CLIENT_GEN);

        AutoPtr<IMessage> msg;
        AutoPtr<IInteger32> iObj;
        CInteger32::New(clientGeneration, (IInteger32**)&iObj);
        mOwner->mEventHandler->ObtainMessage(
            CRemoteControlClient::MSG_NEW_INTERNAL_CLIENT_GEN,
            artWidth, artHeight, iObj, (IMessage**)&msg);
        mOwner->mEventHandler->DispatchMessage(msg);

        // send the information
        mOwner->mEventHandler->RemoveMessages(CRemoteControlClient::MSG_REQUEST_PLAYBACK_STATE);
        mOwner->mEventHandler->RemoveMessages(CRemoteControlClient::MSG_REQUEST_METADATA);
        mOwner->mEventHandler->RemoveMessages(CRemoteControlClient::MSG_REQUEST_TRANSPORTCONTROL);
        mOwner->mEventHandler->RemoveMessages(CRemoteControlClient::MSG_REQUEST_ARTWORK);

        msg = NULL;
        mOwner->mEventHandler->ObtainMessage(
            CRemoteControlClient::MSG_REQUEST_PLAYBACK_STATE, (IMessage**)&msg);
        mOwner->mEventHandler->DispatchMessage(msg);

        msg = NULL;
        mOwner->mEventHandler->ObtainMessage(
            CRemoteControlClient::MSG_REQUEST_TRANSPORTCONTROL, (IMessage**)&msg);
        mOwner->mEventHandler->DispatchMessage(msg);

        msg = NULL;
        mOwner->mEventHandler->ObtainMessage(
            CRemoteControlClient::MSG_REQUEST_METADATA, (IMessage**)&msg);
        mOwner->mEventHandler->DispatchMessage(msg);

        msg = NULL;
        mOwner->mEventHandler->ObtainMessage(
            CRemoteControlClient::MSG_REQUEST_ARTWORK, (IMessage**)&msg);
        mOwner->mEventHandler->DispatchMessage(msg);
    }
    return NOERROR;
}

ECode CRemoteControlClient::MyRemoteControlClient::SetCurrentClientGenerationId(
    /* [in] */ Int32 clientGeneration)
{
    // only post messages, we can't block here
    if (mOwner->mEventHandler != NULL) {
        mOwner->mEventHandler->RemoveMessages(MSG_NEW_CURRENT_CLIENT_GEN);

        AutoPtr<IMessage> msg;
        AutoPtr<IInteger32> iObj;
        CInteger32::New(clientGeneration, (IInteger32**)&iObj);
        mOwner->mEventHandler->ObtainMessage(
            CRemoteControlClient::MSG_NEW_CURRENT_CLIENT_GEN,
            clientGeneration, 0/*ignored*/, (IMessage**)&msg);
        mOwner->mEventHandler->DispatchMessage(msg);
    }
    return NOERROR;
}

ECode CRemoteControlClient::MyRemoteControlClient::PlugRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    // only post messages, we can't block here
    if (mOwner->mEventHandler != NULL) {
        AutoPtr<IMessage> msg;
        mOwner->mEventHandler->ObtainMessage(
            CRemoteControlClient::MSG_PLUG_DISPLAY, rcd, (IMessage**)&msg);
        mOwner->mEventHandler->DispatchMessage(msg);
    }
    return NOERROR;
}

ECode CRemoteControlClient::MyRemoteControlClient::UnplugRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    // only post messages, we can't block here
    if (mOwner->mEventHandler != NULL) {
        AutoPtr<IMessage> msg;
        mOwner->mEventHandler->ObtainMessage(
            CRemoteControlClient::MSG_UNPLUG_DISPLAY, rcd, (IMessage**)&msg);
        mOwner->mEventHandler->DispatchMessage(msg);
    }
    return NOERROR;
}

//--------------------------------------
//    CRemoteControlClient::EventHandler
//--------------------------------------

CRemoteControlClient::EventHandler::EventHandler(
    /* [in] */ IRemoteControlClient* rcc,
    /* [in] */ ILooper* looper,
    /* [in] */ CRemoteControlClient* owner)
    : HandlerBase(looper)
    , mOwner(owner)
{
}

ECode CRemoteControlClient::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch(what) {
        case MSG_REQUEST_PLAYBACK_STATE:
        {
            {
                AutoLock lock(mOwner->mCacheLock);
                mOwner->SendPlaybackState_syncCacheLock();
            }
            break;
        }
        case MSG_REQUEST_METADATA:
        {
            {
                AutoLock lock(mOwner->mCacheLock);
                mOwner->SendMetadata_syncCacheLock();
            }
            break;
        }
        case MSG_REQUEST_TRANSPORTCONTROL:
        {
            {
                AutoLock lock(mOwner->mCacheLock);
                mOwner->SendTransportControlFlags_syncCacheLock();
            }
            break;
        }
        case MSG_REQUEST_ARTWORK:
        {
            {
                AutoLock lock(mOwner->mCacheLock);
                mOwner->SendArtwork_syncCacheLock();
            }
            break;
        }
        case MSG_NEW_INTERNAL_CLIENT_GEN:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Int32 tempValue;
            IInteger32::Probe(obj)->GetValue(&tempValue);
            mOwner->OnNewInternalClientGen(tempValue, arg1, arg2);
            break;
        }
        case MSG_NEW_CURRENT_CLIENT_GEN:
        {
            mOwner->OnNewCurrentClientGen(arg1);
            break;
        }
        case MSG_PLUG_DISPLAY:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mOwner->OnPlugDisplay(IIRemoteControlDisplay::Probe(obj));
            break;
        }
        case MSG_UNPLUG_DISPLAY:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mOwner->OnUnplugDisplay(IIRemoteControlDisplay::Probe(obj));
            break;
        }
        default:
        {
            Logger::E(TAG, "Unknown event %d in RemoteControlClient handler", what);
            break;
        }
    }

    return NOERROR;
}

void CRemoteControlClient::DetachFromDisplay_syncCacheLock()
{
    mRcDisplay = NULL;
    mArtworkExpectedWidth = ARTWORK_INVALID_SIZE;
    mArtworkExpectedHeight = ARTWORK_INVALID_SIZE;
}

void CRemoteControlClient::SendPlaybackState_syncCacheLock()
{
    if ((mCurrentClientGenId == mInternalClientGenId) && (mRcDisplay != NULL)) {
        ECode ec;
//        try {
            ec = mRcDisplay->SetPlaybackState(mInternalClientGenId, mPlaybackState,
                    mPlaybackStateChangeTimeMs);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "Error in setPlaybackState(), dead display "/*+e*/);
                DetachFromDisplay_syncCacheLock();
            }
//        }
    }
}

void CRemoteControlClient::SendMetadata_syncCacheLock()
{
    if ((mCurrentClientGenId == mInternalClientGenId) && (mRcDisplay != NULL)) {
        ECode ec;
//        try {
            ec = mRcDisplay->SetMetadata(mInternalClientGenId, mMetadata);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "Error in sendPlaybackState(), dead display "/*+e*/);
                DetachFromDisplay_syncCacheLock();
            }
//        }
    }
}

void CRemoteControlClient::SendTransportControlFlags_syncCacheLock()
{
    if ((mCurrentClientGenId == mInternalClientGenId) && (mRcDisplay != NULL)) {
        ECode ec;
//        try {
            ec = mRcDisplay->SetTransportControlFlags(mInternalClientGenId,
                    mTransportControlFlags);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "Error in sendTransportControlFlags(), dead display "/*+e*/);
                DetachFromDisplay_syncCacheLock();
            }
//        }
    }
}

void CRemoteControlClient::SendArtwork_syncCacheLock()
{
    if ((mCurrentClientGenId == mInternalClientGenId) && (mRcDisplay != NULL)) {
        // even though we have already scaled in setArtwork(), when this client needs to
        // send the bitmap, there might be newer and smaller expected dimensions, so we have
        // to check again.
        mArtwork = ScaleBitmapIfTooBig(mArtwork, mArtworkExpectedWidth, mArtworkExpectedHeight);
        ECode ec;
//        try {
            ec = mRcDisplay->SetArtwork(mInternalClientGenId, mArtwork);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "Error in sendArtwork(), dead display "/*+e*/);
                DetachFromDisplay_syncCacheLock();
            }
//        }
    }
}

void CRemoteControlClient::SendMetadataWithArtwork_syncCacheLock()
{
    if ((mCurrentClientGenId == mInternalClientGenId) && (mRcDisplay != NULL)) {
        // even though we have already scaled in setArtwork(), when this client needs to
        // send the bitmap, there might be newer and smaller expected dimensions, so we have
        // to check again.
        mArtwork = ScaleBitmapIfTooBig(mArtwork, mArtworkExpectedWidth, mArtworkExpectedHeight);
        ECode ec;
//        try {
            ec = mRcDisplay->SetAllMetadata(mInternalClientGenId, mMetadata, mArtwork);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "Error in setAllMetadata(), dead display "/*+e*/);
                DetachFromDisplay_syncCacheLock();
            }
//        }
    }
}

/*static*/
AutoPtr<IIAudioService> CRemoteControlClient::GetService()
{
    if (sService != NULL) {
        return sService;
    }

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> obj;
    serviceManager->GetService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    sService = IIAudioService::Probe(obj.Get());

    return sService;
}

void CRemoteControlClient::SendAudioServiceNewPlaybackInfo_syncCacheLock(
    /* [in] */ Int32 what,
    /* [in] */ Int32 value)
{
    if (mRcseId == RCSE_ID_UNREGISTERED) {
        return;
    }
    //Logger::D(TAG, String("sending to AudioService key=") + StringUtils::Int32ToString(what) + String(", value=") + StringUtils::Int32ToString(value));
    AutoPtr<IIAudioService> service = GetService();
    ECode ec;
//    try {
        ec = service->SetPlaybackInfoForRcc(mRcseId, what, value);
//    } catch (RemoteException e) {
        if (FAILED(ec)) {
            Logger::E(TAG, "Dead object in sendAudioServiceNewPlaybackInfo_syncCacheLock"/*, e*/);
        }
//    }
}

void CRemoteControlClient::OnNewInternalClientGen(
    /* [in] */ Int32 clientGeneration,
    /* [in] */ Int32 artWidth,
    /* [in] */ Int32 artHeight)
{
    {
        AutoLock lock(mCacheLock);

        // this remote control client is told it is the "focused" one:
        // it implies that now (mCurrentClientGenId == mInternalClientGenId) is true
        mInternalClientGenId = clientGeneration;
        if (artWidth > 0) {
            mArtworkExpectedWidth = artWidth;
            mArtworkExpectedHeight = artHeight;
        }
    }
}

void CRemoteControlClient::OnNewCurrentClientGen(
    /* [in] */ Int32 clientGeneration)
{
    {
        AutoLock lock(mCacheLock);
        mCurrentClientGenId = clientGeneration;
    }
}

void CRemoteControlClient::OnPlugDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    {
        AutoLock lock(mCacheLock);
        mRcDisplay = rcd;
    }
}

void CRemoteControlClient::OnUnplugDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    {
        AutoLock lock(mCacheLock);

        if ((mRcDisplay != NULL) && (rcd == mRcDisplay)) {
            mRcDisplay = NULL;
            mArtworkExpectedWidth = ARTWORK_DEFAULT_SIZE;
            mArtworkExpectedHeight = ARTWORK_DEFAULT_SIZE;
        }
    }
}

AutoPtr<IBitmap> CRemoteControlClient::ScaleBitmapIfTooBig(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 maxWidth,
    /* [in] */ Int32 maxHeight)
{
    using Elastos::Core::Math;
    if (bitmap != NULL) {
        Int32 width;
        bitmap->GetWidth(&width);
        Int32 height;
        bitmap->GetHeight(&height);
        if (width > maxWidth || height > maxHeight) {
            Float scale = Elastos::Core::Math::Min((Float) maxWidth / width, (Float) maxHeight / height);
            Int32 newWidth = Elastos::Core::Math::Round(scale * width);
            Int32 newHeight = Elastos::Core::Math::Round(scale * height);
            BitmapConfig newConfig;
            bitmap->GetConfig(&newConfig);
            if (newConfig == BitmapConfig_NONE) {
                newConfig = BitmapConfig_ARGB_8888;
            }
            AutoPtr<IBitmap> outBitmap;
            CBitmap::CreateBitmap(newWidth, newHeight, newConfig, (IBitmap**)&outBitmap);

            AutoPtr<ICanvas> canvas;
            CCanvas::New(outBitmap,(ICanvas**)&canvas);

            AutoPtr<IPaint> paint;
            CPaint::New((IPaint**)&paint);
            paint->SetAntiAlias(TRUE);
            paint->SetFilterBitmap(TRUE);

            AutoPtr<IRectF> rectF;
            Int32 tempValue1, tempValue2;
            outBitmap->GetWidth(&tempValue1);
            outBitmap->GetHeight(&tempValue2);
            CRectF::New(0, 0, tempValue1, tempValue2, (IRectF**)&rectF);
            canvas->DrawBitmap(bitmap, NULL, rectF, paint);
            bitmap = outBitmap;
        }
    }
    return bitmap;
}

/*static*/
Boolean CRemoteControlClient::ValidTypeForKey(
    /* [in] */ Int32 key,
    /* [in] */ ArrayOf<Int32>* validKeys)
{
//    try {
        for (Int32 i = 0; i != validKeys->GetLength(); i++) {
            if (key == (*validKeys)[i]) {
                return TRUE;
            }
        }
        return FALSE;
//    } catch (ArrayIndexOutOfBoundsException e) {
//        return FALSE;
//    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
