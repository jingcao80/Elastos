#include "elastos/droid/media/CAudioFormat.h"
#include "elastos/droid/media/CAudioTrack.h"
#include "elastos/droid/media/CJetPlayer.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <media/JetPlayer.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::Looper;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CJetPlayer::TAG("JetPlayer-J");

const Int32 CJetPlayer::MAXTRACKS                   = 32;

const Int32 CJetPlayer::JET_EVENT                   = 1;
const Int32 CJetPlayer::JET_USERID_UPDATE           = 2;
const Int32 CJetPlayer::JET_NUMQUEUEDSEGMENT_UPDATE = 3;
const Int32 CJetPlayer::JET_PAUSE_UPDATE            = 4;

const Int32 CJetPlayer::JET_EVENT_VAL_MASK          = 0x0000007f;                     // mask for value
const Int32 CJetPlayer::JET_EVENT_CTRL_MASK         = 0x00003f80;                     // mask for controller
const Int32 CJetPlayer::JET_EVENT_CHAN_MASK         = 0x0003c000;                     // mask for channel
const Int32 CJetPlayer::JET_EVENT_TRACK_MASK        = 0x00fc0000;                     // mask for track number
const Int32 CJetPlayer::JET_EVENT_SEG_MASK          = 0xff000000;                     // mask for segment ID

const Int32 CJetPlayer::JET_EVENT_CTRL_SHIFT        = 7;                              // shift to get controller number to bit 0
const Int32 CJetPlayer::JET_EVENT_CHAN_SHIFT        = 14;                             // shift to get MIDI channel to bit 0
const Int32 CJetPlayer::JET_EVENT_TRACK_SHIFT       = 18;                             // shift to get track ID to bit 0
const Int32 CJetPlayer::JET_EVENT_SEG_SHIFT         = 24;                             // shift to get segment ID to bit 0

const Int32 CJetPlayer::JET_OUTPUT_RATE             = 22050;                          // _SAMPLE_RATE_22050 in Android.mk
const Int32 CJetPlayer::JET_OUTPUT_CHANNEL_CONFIG   = IAudioFormat::CHANNEL_OUT_STEREO; // NUM_OUTPUT_CHANNELS=2 in Android.mk

AutoPtr<IJetPlayer> CJetPlayer::singletonRef;

//===========================================================================
//                  CJetPlayer::NativeEventHandler
//===========================================================================

CJetPlayer::NativeEventHandler::NativeEventHandler(
    /* [in] */ IWeakReference* jet,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mWeakHost(jet)
{
}

ECode CJetPlayer::NativeEventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    AutoPtr<IJetPlayer> jet;
    mWeakHost->Resolve(EIID_IJetPlayer, (IInterface**)&jet);
    if (jet == NULL) {
        return NOERROR;
    }

    CJetPlayer* jetPlayer = (CJetPlayer*)jet.Get();
    Object& lock = jetPlayer->mEventListenerLock;
    AutoPtr<IOnJetEventListener> listener;
    {    AutoLock syncLock(lock);
        listener = jetPlayer->mJetEventListener;
    }

    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    switch (what) {
        case CJetPlayer::JET_EVENT:
            if (listener != NULL) {
                // call the appropriate listener after decoding the event parameters
                // encoded in msg.arg1
                listener->OnJetEvent(
                    jet,
                    (Int16)((arg1 & CJetPlayer::JET_EVENT_SEG_MASK)   >> CJetPlayer::JET_EVENT_SEG_SHIFT),
                    (Byte) ((arg1 & CJetPlayer::JET_EVENT_TRACK_MASK) >> CJetPlayer::JET_EVENT_TRACK_SHIFT),
                    // JETCreator channel numbers start at 1, but the index starts at 0
                    // in the .jet files
                    (Byte)(((arg1 & CJetPlayer::JET_EVENT_CHAN_MASK)  >> CJetPlayer::JET_EVENT_CHAN_SHIFT) + 1),
                    (Byte) ((arg1 & CJetPlayer::JET_EVENT_CTRL_MASK)  >> CJetPlayer::JET_EVENT_CTRL_SHIFT),
                    (Byte) (arg1 & CJetPlayer::JET_EVENT_VAL_MASK));
            }
            return NOERROR;

        case CJetPlayer::JET_USERID_UPDATE: {
            Int32 arg2;
            msg->GetArg2(&arg2);
            if (listener != NULL) {
                listener->OnJetUserIdUpdate(jet, arg1, arg2);
            }
            return NOERROR;
        }
        case CJetPlayer::JET_NUMQUEUEDSEGMENT_UPDATE:
            if (listener != NULL) {
                listener->OnJetNumQueuedSegmentUpdate(jet, arg1);
            }
            return NOERROR;

        case CJetPlayer::JET_PAUSE_UPDATE:
            if (listener != NULL) {
                listener->OnJetPauseUpdate(jet, arg1);
            }
            return NOERROR;

        default:
            Logger::E("CJetPlayer", "Unknown message type %d", what);
            return NOERROR;
    }

    return NOERROR;
}

/*
 * Public Implementation ===>
 *
 */
//===========================================================================
//                  CJetPlayer
//===========================================================================

CAR_INTERFACE_IMPL(CJetPlayer, Object, IJetPlayer)

CAR_OBJECT_IMPL(CJetPlayer)

CJetPlayer::CJetPlayer()
    : mNativePlayer(0)
{}

CJetPlayer::~CJetPlayer()
{
    NativeFinalize();
}

ECode CJetPlayer::constructor()
{
    // remember which looper is associated with the JetPlayer instanciation
    mInitializationLooper = Looper::GetMyLooper();
    if (mInitializationLooper == NULL) {
        mInitializationLooper = Looper::GetMainLooper();
    }

    assert(mInitializationLooper != NULL);

    Int32 buffSizeInBytes;
    CAudioTrack::GetMinBufferSize(JET_OUTPUT_RATE,
        JET_OUTPUT_CHANNEL_CONFIG, IAudioFormat::ENCODING_PCM_16BIT,
         &buffSizeInBytes);

    if (buffSizeInBytes == IAudioTrack::IAudioTrack::ERROR) {
        return NOERROR;
    }

    if (buffSizeInBytes == IAudioTrack::IAudioTrack::ERROR_BAD_VALUE) {
        return NOERROR;
    }

    /**
     * bytes to frame conversion: sample format is ENCODING_PCM_16BIT, 2 channels
     * 1200 == minimum buffer size in frames on generation 1 hardware
     */
    Int32 val;
    CAudioFormat::GetBytesPerSample(IAudioFormat::ENCODING_PCM_16BIT, &val);
    using Elastos::Core::Math;
    NativeSetup(CJetPlayer::GetMaxTracks(), Math::Max(1200, buffSizeInBytes /
            (val * 2 /*channels*/)));
    return NOERROR;
}

ECode CJetPlayer::Clone(
    /* [out] */ IJetPlayer** jetPlayer)
{
    VALIDATE_NOT_NULL(jetPlayer);
    *jetPlayer = NULL;
    /**
     * JetPlayer is a singleton class,
     * so you can't clone a JetPlayer instance
     */
    return E_NOT_IMPLEMENTED;
}

ECode CJetPlayer::ReleaseResources()
{
    NativeRelease();
    CJetPlayer::singletonRef = NULL;
    return NOERROR;
}

Int32 CJetPlayer::GetMaxTracks()
{
    return MAXTRACKS;
}

ECode CJetPlayer::LoadJetFile(
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    return NativeLoadJetFromFile(path, result);
}

ECode CJetPlayer::LoadJetFile(
    /* [in] */ IAssetFileDescriptor* afd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(afd);

    Int64 len;
    afd->GetLength(&len);
    if (len < 0) {
        Logger::E(TAG, "no length for fd");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IFileDescriptor> fd;
    afd->GetFileDescriptor((IFileDescriptor**)&fd);
    Int64 offset;
    afd->GetStartOffset(&offset);
    return NativeLoadJetFromFileD(fd, offset, len, result);
}

ECode CJetPlayer::CloseJetFile(
    /* [out] */ Boolean* result)
{
    return NativeCloseJetFile(result);
}

ECode CJetPlayer::Play(
    /* [out] */ Boolean* result)
{
    return NativePlayJet(result);
}

ECode CJetPlayer::Pause(
    /* [out] */ Boolean* result)
{
    return NativePauseJet(result);
}

ECode CJetPlayer::QueueJetSegment(
    /* [in] */ Int32 segmentNum,
    /* [in] */ Int32 libNum,
    /* [in] */ Int32 repeatCount,
    /* [in] */ Int32 transpose,
    /* [in] */ Int32 muteFlags,
    /* [in] */ Byte userID,
    /* [out] */ Boolean* result)
{
    return NativeQueueJetSegment(segmentNum, libNum, repeatCount,
        transpose, muteFlags, userID, result);
}

ECode CJetPlayer::QueueJetSegmentMuteArray(
    /* [in] */ Int32 segmentNum,
    /* [in] */ Int32 libNum,
    /* [in] */ Int32 repeatCount,
    /* [in] */ Int32 transpose,
    /* [in] */ ArrayOf<Boolean>* muteArray,
    /* [in] */ Byte userID,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(muteArray);

    if (muteArray->GetLength() != CJetPlayer::GetMaxTracks()) {
        return NOERROR;
    }

    return NativeQueueJetSegmentMuteArray(segmentNum, libNum,
        repeatCount, transpose, muteArray, userID, result);
}

ECode CJetPlayer::SetMuteFlags(
    /* [in] */ Int32 muteFlags,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    return NativeSetMuteFlags(muteFlags, sync, result);
}

ECode CJetPlayer::SetMuteArray(
    /* [in] */ ArrayOf<Boolean>* muteArray,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    VALIDATE_NOT_NULL(muteArray);
    *result = FALSE;

    if (muteArray->GetLength() != CJetPlayer::GetMaxTracks()) {
        return NOERROR;
    }

    return NativeSetMuteArray(muteArray, sync, result);
}

ECode CJetPlayer::SetMuteFlag(
    /* [in] */ Int32 trackId,
    /* [in] */ Boolean muteFlag,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    return NativeSetMuteFlag(trackId, muteFlag, sync, result);
}

ECode CJetPlayer::TriggerClip(
    /* [in] */ Int32 clipId,
    /* [out] */ Boolean* result)
{
    return NativeTriggerClip(clipId, result);
}

ECode CJetPlayer::ClearQueue(
    /* [out] */ Boolean* result)
{
    return NativeClearQueue(result);
}

ECode CJetPlayer::SetEventListener(
    /* [in] */ IOnJetEventListener* listener)
{
    return SetEventListener(listener, NULL);
}

ECode CJetPlayer::SetEventListener(
    /* [in] */ IOnJetEventListener* listener,
    /* [in] */ IHandler* handler)
{
    {    AutoLock syncLock(mEventListenerLock);
        mJetEventListener = listener;

        if (listener == NULL) {
            mEventHandler = NULL;
            return NOERROR;
        }

        AutoPtr<IWeakReference> wr;
        GetWeakReference((IWeakReference**)&wr);
        if (handler != NULL) {
            AutoPtr<ILooper> looper;
            handler->GetLooper((ILooper**)&looper);
            mEventHandler = new NativeEventHandler(wr, looper);
        }
        else {
            mEventHandler = new NativeEventHandler(wr, mInitializationLooper);
        }
    }
    return NOERROR;
}

AutoPtr<IJetPlayer> CJetPlayer::GetJetPlayer()
{
    if (CJetPlayer::singletonRef == NULL) {
        AutoPtr<CJetPlayer> player;
        ASSERT_SUCCEEDED(CJetPlayer::NewByFriend((CJetPlayer**)&player));
        CJetPlayer::singletonRef = (IJetPlayer*)player.Get();
    }

    return CJetPlayer::singletonRef;
}

void CJetPlayer::PostEventFromNative(
    /* [in] */ IWeakReference* jetplayer_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    AutoPtr<IJetPlayer> jetPlayer;
    jetplayer_ref->Resolve(EIID_IJetPlayer, (IInterface**)&jetPlayer);

    //logd("Event posted from the native side: event="+ what + " args="+ arg1+" "+arg2);
    CJetPlayer* jet = (CJetPlayer*)jetPlayer.Get();
    if (jet == NULL) {
        return;
    }

    if (jet->mEventHandler == NULL) {
        return;
    }

    AutoPtr<IMessage> message;
    jet->mEventHandler->ObtainMessage(what, arg1, arg2, NULL, (IMessage**)&message);
    Boolean bval;
    jet->mEventHandler->SendMessage(message, &bval);
}

/*
 * This function is called from JetPlayer instance's render thread
 */
static void
jetPlayerEventCallback(int what, int arg1 = 0, int arg2 = 0, void* target = NULL)
{
    AutoPtr<IWeakReference> wr = (IWeakReference*)target;
    CJetPlayer::PostEventFromNative(wr, what, arg1, arg2);
}

Boolean CJetPlayer::NativeSetup(
    /* [in] */ Int32 maxTracks,
    /* [in] */ Int32 trackBufferSize)
{
    //LOGV("android_media_JetPlayer_setup(): entering.");
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    wr->AddRef();
    android::JetPlayer* lpJet = new android::JetPlayer(wr.Get(), maxTracks, trackBufferSize);

    EAS_RESULT result = lpJet->init();
    if (result == EAS_SUCCESS) {
        // save our newly created C++ JetPlayer in the "nativePlayerInJavaObj" field
        // of the Java object (in mNativePlayerInJavaObj)
        mNativePlayer = (Handle32)lpJet;
        return TRUE;
    }
    else {
        Logger::E(TAG, "android_media_JetPlayer_setup(): initialization failed with EAS error code %d", (int)result);
        delete lpJet;
        mNativePlayer = 0;
        return FALSE;
    }
}

void CJetPlayer::NativeFinalize()
{
    // LOGV("android_media_JetPlayer_finalize(): entering.");
    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;

    if (lpJet != NULL) {
        lpJet->release();
        delete lpJet;
    }

    // LOGV("android_media_JetPlayer_finalize(): exiting.");
}

void CJetPlayer::NativeRelease()
{
    NativeFinalize();
    mNativePlayer = 0;
    // LOGV("android_media_JetPlayer_release() done");
}

ECode CJetPlayer::NativeLoadJetFromFile(
    /* [in] */ const String& pathToJetFile,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for openFile()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // set up event callback function
    lpJet->setEventCallback(jetPlayerEventCallback);

    const char *pathStr = pathToJetFile.string();
    if (pathStr == NULL) {  // Out of memory
        Logger::E(TAG, "android_media_JetPlayer_openFile(): aborting, out of memory");
        return E_RUNTIME_EXCEPTION;
    }

    // LOGV("android_media_JetPlayer_openFile(): trying to open %s", pathStr );
    EAS_RESULT status = lpJet->loadFromFile(pathStr);
    if (status == EAS_SUCCESS) {
        // Logger::V(TAG, "android_media_JetPlayer_openFile(): file successfully opened");
        *result = TRUE;
    }
    else {
        Logger::E(TAG, "android_media_JetPlayer_openFile(): failed to open file with EAS error %d", status);
    }
    return NOERROR;
}

ECode CJetPlayer::NativeLoadJetFromFileD(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(fileDescriptor)

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for openFile()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // set up event callback function
    lpJet->setEventCallback(jetPlayerEventCallback);

    // LOGV("android_media_JetPlayer_openFileDescr(): trying to load JET file through its fd" );
    Int32 descriptor;
    fileDescriptor->GetDescriptor(&descriptor);
    EAS_RESULT status = lpJet->loadFromFD(descriptor,
        (long long)offset, (long long)length); // cast params to types used by EAS_FILE

    if (status == EAS_SUCCESS) {
        // Logger::V(TAG, "android_media_JetPlayer_openFileDescr(): file successfully opened");
        *result = TRUE;
    }
    else {
        Logger::E(TAG, "android_media_JetPlayer_openFileDescr(): failed to open file with EAS error %d",
            (int)status);
    }
    return NOERROR;
}

ECode CJetPlayer::NativeCloseJetFile(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result= FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for closeFile()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (lpJet->closeFile() == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_closeFile(): file successfully closed");
        *result = TRUE;
    }
    else {
        Logger::E(TAG, "android_media_JetPlayer_closeFile(): failed to close file");
    }
    return NOERROR;
}

ECode CJetPlayer::NativePlayJet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for play()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    EAS_RESULT status = lpJet->play();
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_play(): play successful");
        *result = TRUE;
    } else {
        Logger::E(TAG, "android_media_JetPlayer_play(): failed to play with EAS error code %lld",
            status);
    }
    return NOERROR;
}

ECode CJetPlayer::NativePauseJet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for pause()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    EAS_RESULT status = lpJet->pause();
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_pause(): pause successful");
        *result = TRUE;
    }
    else {
        if (status == EAS_ERROR_QUEUE_IS_EMPTY) {
            // LOGV("android_media_JetPlayer_pause(): paused with an empty queue");
            *result = TRUE;
        } else {
            Logger::E(TAG, "android_media_JetPlayer_pause(): failed to pause with EAS error code %lld",
                status);
        }
    }

    return NOERROR;
}

ECode CJetPlayer::NativeQueueJetSegment(
    /* [in] */ Int32 segmentNum,
    /* [in] */ Int32 libNum,
    /* [in] */ Int32 repeatCount,
    /* [in] */ Int32 transpose,
    /* [in] */ Int32 muteFlags,
    /* [in] */ Byte userID,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for queueSegment()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    EAS_RESULT status = lpJet->queueSegment(segmentNum, libNum, repeatCount, transpose, muteFlags, userID);
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_queueSegment(): segment successfully queued");
        *result = TRUE;
    } else {
        Logger::E(TAG, "android_media_JetPlayer_queueSegment(): failed with EAS error code %lld",
            status);
    }
    return NOERROR;
}

ECode CJetPlayer::NativeQueueJetSegmentMuteArray(
    /* [in] */ Int32 segmentNum,
    /* [in] */ Int32 libNum,
    /* [in] */ Int32 repeatCount,
    /* [in] */ Int32 transpose,
    /* [in] */ ArrayOf<Boolean>* muteArray,
    /* [in] */ Byte userID,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for queueSegmentMuteArray()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }


    Boolean* muteTracks = muteArray->GetPayload();
    if (muteTracks == NULL) {
        Logger::E(TAG, "android_media_JetPlayer_queueSegment(): failed to read track mute mask.");
        return FALSE;
    }

    EAS_U32 muteMask=0;
    int maxTracks = lpJet->getMaxTracks();

    for (Int32 trackIndex = 0; trackIndex < maxTracks; trackIndex++) {
        if (muteTracks[maxTracks - 1 - trackIndex] == TRUE) {
            muteMask = (muteMask << 1) | 0x00000001;
        } else {
            muteMask = muteMask << 1;
        }
    }
    //LOGV("android_media_JetPlayer_queueSegmentMuteArray(): FINAL mute mask =0x%08lX", mask);

    EAS_RESULT status = lpJet->queueSegment(segmentNum, libNum, repeatCount, transpose, muteMask, userID);
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_queueSegmentMuteArray(): segment successfully queued");
        *result = TRUE;
    }
    else {
        Logger::E(TAG, "android_media_JetPlayer_queueSegmentMuteArray(): failed with EAS error code %lld",
            status);
    }

    return NOERROR;
}

ECode CJetPlayer::NativeSetMuteFlags(
    /* [in] */ Int32 muteFlags,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for setMuteFlags()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    EAS_RESULT status = lpJet->setMuteFlags(muteFlags, sync== TRUE ? true : false);
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_setMuteFlags(): mute flags successfully updated");
        *result = TRUE;
    } else {
        Logger::E(TAG, "android_media_JetPlayer_setMuteFlags(): failed with EAS error code %lld", status);
    }
    return NOERROR;
}

ECode CJetPlayer::NativeSetMuteArray(
    /* [in] */ ArrayOf<Boolean>* muteArray,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for setMuteArray()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Boolean* muteTracks = muteArray->GetPayload();
    if (muteTracks == NULL) {
        Logger::E(TAG, "android_media_JetPlayer_setMuteArray(): failed to read track mute mask.");
        return NOERROR;
    }

    EAS_U32 muteMask=0;
    int maxTracks = lpJet->getMaxTracks();

    for (Int32 trackIndex = 0; trackIndex < maxTracks; trackIndex++) {
        if (muteTracks[maxTracks - 1 - trackIndex] == TRUE) {
            muteMask = (muteMask << 1) | 0x00000001;
        } else {
            muteMask = muteMask << 1;
        }
    }
    //LOGV("android_media_JetPlayer_setMuteArray(): FINAL mute mask =0x%08lX", muteMask);

    EAS_RESULT status = lpJet->setMuteFlags(muteMask, sync == TRUE ? true : false);
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_setMuteArray(): mute flags successfully updated");
        *result = TRUE;
    } else {
        Logger::E(TAG, "android_media_JetPlayer_setMuteArray(): \
            failed to update mute flags with EAS error code %lld", status);
    }
    return NOERROR;
}

ECode CJetPlayer::NativeSetMuteFlag(
    /* [in] */ Int32 trackId,
    /* [in] */ Boolean muteFlag,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for setMuteFlag()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    EAS_RESULT status = lpJet->setMuteFlag(trackId, muteFlag == TRUE ? true : false, sync == TRUE ? true : false);
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_setMuteFlag(): mute flag successfully updated for track %d", trackId);
        *result = TRUE;
    } else {
        Logger::E(TAG, "android_media_JetPlayer_setMuteFlag(): failed to update mute flag for track %d with EAS error code %lld",
            trackId, status);
    }
    return NOERROR;
}

ECode CJetPlayer::NativeTriggerClip(
    /* [in] */ Int32 clipId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for triggerClip()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    EAS_RESULT status = lpJet->triggerClip(clipId);
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_triggerClip(): triggerClip successful for clip %d", clipId);
        *result = TRUE;
    } else {
        Logger::E(TAG, "android_media_JetPlayer_triggerClip(): triggerClip for clip %d failed with EAS error code %lld",
            clipId, status);
    }
    return NOERROR;
}

ECode CJetPlayer::NativeClearQueue(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    android::JetPlayer* lpJet = (android::JetPlayer*)mNativePlayer;
    if (lpJet == NULL ) {
        Logger::E(TAG, "Unable to retrieve JetPlayer pointer for clearQueue()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    EAS_RESULT status = lpJet->clearQueue();
    if (status == EAS_SUCCESS) {
        //LOGV("android_media_JetPlayer_clearQueue(): clearQueue successful");
        *result = TRUE;
    } else {
        Logger::E(TAG, "android_media_JetPlayer_clearQueue(): clearQueue failed with EAS error code %lld",
            status);
    }
    return NOERROR;
}

/*
 * Private Static Implementation ===>
 *
 */
void CJetPlayer::Logd(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, "[ android.media.JetPlayer ] %s", msg.string());
}

void CJetPlayer::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(TAG, "[ android.media.JetPlayer ] %s", msg.string());
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
