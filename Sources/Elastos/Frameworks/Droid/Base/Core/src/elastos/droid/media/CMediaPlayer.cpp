#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/media/CAudioAttributes.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/media/CMediaFormat.h"
#include "elastos/droid/media/CMediaHTTPService.h"
#include "elastos/droid/media/CMediaPlayer.h"
#include "elastos/droid/media/CMediaPlayerTrackInfo.h"
// TODO: Need CMetadata
// #include "elastos/droid/media/CMetadata.h"
// TODO: Need CSRTRenderer
// #include "elastos/droid/media/CSRTRenderer.h"
// TODO: Need CTimedText
// #include "elastos/droid/media/CTimedText.h"
// TODO: Need CTrackInfoVendor
// #include "elastos/droid/media/CTrackInfoVendor.h"
// TODO: Need Media_Utils
// #include "elastos/droid/media/Media_Utils.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/droid/system/OsConstants.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <binder/Parcel.h>
#include <gui/Surface.h>
#include <utils/String8.h>
#include <media/IMediaHTTPService.h>
#include <media/mediaplayer.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::System::OsConstants;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IThread;
using Elastos::Core::StringBuilder;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Net::IInetAddress;
using Elastos::Utility::CScanner;
using Elastos::Utility::CVector;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IScanner;
using Elastos::Utility::IVector;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CIoBridge;
using Libcore::IO::CLibcore;
using Libcore::IO::IIoBridge;
using Libcore::IO::ILibcore;
using Libcore::IO::IOs;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaPlayer::TAG("CMediaPlayer");
const Boolean CMediaPlayer::LOGV = FALSE;

// Name of the remote interface for the media player. Must be kept
// in sync with the 2nd parameter of the IMPLEMENT_META_INTERFACE
// macro invocation in IMediaPlayer.cpp
const String CMediaPlayer::IMEDIA_PLAYER("android.media.IMediaPlayer");

const Int32 CMediaPlayer::INVOKE_ID_GET_TRACK_INFO = 1;
const Int32 CMediaPlayer::INVOKE_ID_ADD_EXTERNAL_SOURCE = 2;
const Int32 CMediaPlayer::INVOKE_ID_ADD_EXTERNAL_SOURCE_FD = 3;
const Int32 CMediaPlayer::INVOKE_ID_SELECT_TRACK = 4;
const Int32 CMediaPlayer::INVOKE_ID_DESELECT_TRACK = 5;
const Int32 CMediaPlayer::INVOKE_ID_SET_VIDEO_SCALE_MODE = 6;
const Int32 CMediaPlayer::INVOKE_ID_GET_SELECTED_TRACK = 7;

const Int32 CMediaPlayer::MEDIA_NOP = 0; // interface test message
const Int32 CMediaPlayer::MEDIA_PREPARED = 1;
const Int32 CMediaPlayer::MEDIA_PLAYBACK_COMPLETE = 2;
const Int32 CMediaPlayer::MEDIA_BUFFERING_UPDATE = 3;
const Int32 CMediaPlayer::MEDIA_SEEK_COMPLETE = 4;
const Int32 CMediaPlayer::MEDIA_SET_VIDEO_SIZE = 5;
const Int32 CMediaPlayer::MEDIA_STARTED = 6;
const Int32 CMediaPlayer::MEDIA_PAUSED = 7;
const Int32 CMediaPlayer::MEDIA_STOPPED = 8;
const Int32 CMediaPlayer::MEDIA_SKIPPED = 9;
const Int32 CMediaPlayer::MEDIA_TIMED_TEXT = 99;
const Int32 CMediaPlayer::MEDIA_ERROR = 100;
const Int32 CMediaPlayer::MEDIA_INFO = 200;
const Int32 CMediaPlayer::MEDIA_SUBTITLE_DATA = 201;

const Int32 CMediaPlayer::KEY_PARAMETER_AUDIO_ATTRIBUTES = 1400;

static Object sLock;

//===================================================================
//                  JNIMediaPlayerListener
//===================================================================

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class JNIMediaPlayerListener : public android::MediaPlayerListener
{
public:
    JNIMediaPlayerListener(IWeakReference* weak_thiz);

    ~JNIMediaPlayerListener();

    void notify(int msg, int ext1, int ext2, const android::Parcel* obj);

private:
    JNIMediaPlayerListener();
    AutoPtr<IWeakReference> mWeakObject;    // Weak ref to MediaPlayer Java object to call on
};

JNIMediaPlayerListener::JNIMediaPlayerListener(IWeakReference* weak_thiz)
{
    // We use a weak reference so the MediaPlayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mWeakObject  = weak_thiz;
}

JNIMediaPlayerListener::~JNIMediaPlayerListener()
{
}

void JNIMediaPlayerListener::notify(int msg, int ext1, int ext2, const android::Parcel* obj)
{
    CMediaPlayer::PostEventFromNative(mWeakObject, msg, ext1, ext2, 0);
}

static android::sp<android::MediaPlayer> getMediaPlayer(CMediaPlayer* thiz)
{
    AutoLock l(sLock);
    android::MediaPlayer* const p = (android::MediaPlayer*)thiz->mNativeContext;
    return android::sp<android::MediaPlayer>(p);
}

static android::sp<android::IGraphicBufferProducer> getVideoSurfaceTexture(CMediaPlayer* thiz)
{
    android::IGraphicBufferProducer * const p = reinterpret_cast<android::IGraphicBufferProducer *>(thiz->mNativeSurfaceTexture);
    return android::sp<android::IGraphicBufferProducer>(p);
}

static android::sp<android::MediaPlayer> setMediaPlayer(CMediaPlayer* thiz, const android::sp<android::MediaPlayer>& player)
{
    AutoLock l(sLock);
    android::sp<android::MediaPlayer> old = (android::MediaPlayer*)thiz->mNativeContext;
    if (player.get()) {
        player->incStrong(thiz);
    }
    if (old != 0) {
        old->decStrong(thiz);
    }
    thiz->mNativeContext = (Int32)player.get();
    return old;
}

// If exception is NULL and opStatus is not OK, this method sends an error
// event to the client application; otherwise, if exception is not NULL and
// opStatus is not OK, this method throws the given exception to the client
// application.
static ECode process_media_player_call(CMediaPlayer* thiz, android::status_t opStatus,
    ECode ec, const char* msg = NULL)
{
    if (ec == NOERROR) {  // Don't throw exception. Instead, send an event.
        if (opStatus != (android::status_t) android::OK) {
            if (CMediaPlayer::LOGV) {
                Logger::E(CMediaPlayer::TAG, "process_media_player_call %d failed: %s", opStatus, msg);
            }
            android::sp<android::MediaPlayer> mp = getMediaPlayer(thiz);
            if (mp != 0) mp->notify(android::MEDIA_ERROR, opStatus, 0);
        }
    }
    else {  // Throw exception!
        if (opStatus == (android::status_t)android::INVALID_OPERATION ) {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        else if ( opStatus == (android::status_t) android::PERMISSION_DENIED ) {
            return E_SECURITY_EXCEPTION;
        }
        else if (opStatus != (android::status_t) android::OK) {
            if (msg != NULL) {
                Logger::E(CMediaPlayer::TAG, "process_media_player_call %d failed: %s", opStatus, msg);
            }
            return ec;
        }
    }

    return NOERROR;
}

static android::sp<android::Surface> android_view_Surface_getSurface(ISurface* surfaceObj)
{
    android::sp<android::Surface> sur;
// TODO: Need jni code
    // jobject lock = env->GetObjectField(surfaceObj, gSurfaceClassInfo.mLock);
    // if (env->MonitorEnter(lock) == JNI_OK) {
    //     sur = reinterpret_cast<android::Surface *>(env->GetLongField(surfaceObj, gSurfaceClassInfo.mNativeObject));
    //     env->MonitorExit(lock);
    // }
    // env->DeleteLocalRef(lock);
    return sur;
}

//===================================================================
//                  CMediaPlayer::TimeProvider
//===================================================================

CAR_INTERFACE_IMPL_2(CMediaPlayer::TimeProvider, Object,
        IMediaPlayerOnSeekCompleteListener, IMediaTimeProvider)

const String CMediaPlayer::TimeProvider::TAG("MTP");
const Int64 CMediaPlayer::TimeProvider::MAX_NS_WITHOUT_POSITION_CHECK = 5000000000L;
const Int64 CMediaPlayer::TimeProvider::MAX_EARLY_CALLBACK_US = 1000;
const Int64 CMediaPlayer::TimeProvider::TIME_ADJUSTMENT_RATE = 2;  /* meaning 1/2 */
const Int32 CMediaPlayer::TimeProvider::NOTIFY = 1;
const Int32 CMediaPlayer::TimeProvider::NOTIFY_TIME = 0;
const Int32 CMediaPlayer::TimeProvider::REFRESH_AND_NOTIFY_TIME = 1;
const Int32 CMediaPlayer::TimeProvider::NOTIFY_STOP = 2;
const Int32 CMediaPlayer::TimeProvider::NOTIFY_SEEK = 3;

CMediaPlayer::TimeProvider::TimeProvider(
    /* [in] */ CMediaPlayer* mp)
    : DEBUG(FALSE)
    , mLastTimeUs(0)
    , mPaused(TRUE)
    , mStopped(TRUE)
    , mLastReportedTime(0)
    , mTimeAdjustment(0)
    , mLastNanoTime(0)
    , mRefresh(FALSE)
    , mPausing(FALSE)
    , mSeeking(FALSE)
{
    mPlayer = mp;
    // try {
    Int64 result;
    GetCurrentTimeUs(TRUE, FALSE, &result);
    // } catch (IllegalStateException e) {
    //     // we assume starting position
    //     mRefresh = true;
    // }

    AutoPtr<ILooper> looper;
    if ((looper = Looper::GetMyLooper()) == NULL &&
        (looper = Looper::GetMainLooper()) == NULL) {
        // Create our own looper here in case MP was created without one
        CHandlerThread::New(String("MediaPlayerMTPEventThread"),
              IProcess::THREAD_PRIORITY_FOREGROUND,
              (IHandlerThread**)&mHandlerThread);
        IThread::Probe(mHandlerThread)->Start();
        mHandlerThread->GetLooper((ILooper**)&looper);
    }
    mEventHandler = new EventHandler(this, looper);

    mListeners = ArrayOf<IMediaTimeProviderOnMediaTimeListener*>::Alloc(0);
    mTimes = ArrayOf<Int64>::Alloc(0);
    mLastTimeUs = 0;
    mTimeAdjustment = 0;
}

CMediaPlayer::TimeProvider::~TimeProvider()
{
    if (mHandlerThread != NULL) {
        Boolean b;
        mHandlerThread->QuitSafely(&b);
    }
}

ECode CMediaPlayer::TimeProvider::Close()
{
    mEventHandler->RemoveMessages(NOTIFY);
    if (mHandlerThread != NULL) {
        Boolean b;
        mHandlerThread->QuitSafely(&b);
        mHandlerThread = NULL;
    }
    return NOERROR;
}

ECode CMediaPlayer::TimeProvider::OnPaused(
    /* [in] */ Boolean paused)
{
    synchronized(this) {
        if (DEBUG) Logger::D(TAG, "onPaused: %d", paused);
        if (mStopped) { // handle as seek if we were stopped
            mStopped = FALSE;
            mSeeking = TRUE;
            ScheduleNotification(NOTIFY_SEEK, 0 /* delay */);
        }
        else {
            mPausing = paused;  // special handling if player disappeared
            mSeeking = FALSE;
            ScheduleNotification(REFRESH_AND_NOTIFY_TIME, 0 /* delay */);
        }
    }
    return NOERROR;
}

ECode CMediaPlayer::TimeProvider::OnStopped()
{
    synchronized(this) {
        if (DEBUG) Logger::D(TAG, "onStopped");
        mPaused = TRUE;
        mStopped = TRUE;
        mSeeking = FALSE;
        ScheduleNotification(NOTIFY_STOP, 0 /* delay */);
    }
    return NOERROR;
}

ECode CMediaPlayer::TimeProvider::OnSeekComplete(
    /* [in] */ IMediaPlayer* mp)
{
    synchronized(this) {
        mStopped = FALSE;
        mSeeking = TRUE;
        ScheduleNotification(NOTIFY_SEEK, 0 /* delay */);
    }
    return NOERROR;
}

ECode CMediaPlayer::TimeProvider::OnNewPlayer()
{
    if (mRefresh) {
        synchronized(this) {
            mStopped = FALSE;
            mSeeking = TRUE;
            ScheduleNotification(NOTIFY_SEEK, 0 /* delay */);
        }
    }
    return NOERROR;
}

ECode CMediaPlayer::TimeProvider::NotifyAt(
    /* [in] */ Int64 timeUs,
    /* [in] */ IMediaTimeProviderOnMediaTimeListener* listener)
{
    synchronized(this) {
        if (DEBUG) Logger::D(TAG, "notifyAt %ld", timeUs);
        mTimes->Set(RegisterListener(listener), timeUs);
        ScheduleNotification(NOTIFY_TIME, 0 /* delay */);
    }
    return NOERROR;
}

ECode CMediaPlayer::TimeProvider::ScheduleUpdate(
    /* [in] */ IMediaTimeProviderOnMediaTimeListener* listener)
{
    synchronized(this) {
        if (DEBUG) Logger::D(TAG, "scheduleUpdate");
        Int32 i = RegisterListener(listener);

        if (!mStopped) {
            mTimes->Set(i, 0);
            ScheduleNotification(NOTIFY_TIME, 0 /* delay */);
        }
    }
    return NOERROR;
}

ECode CMediaPlayer::TimeProvider::CancelNotifications(
    /* [in] */ IMediaTimeProviderOnMediaTimeListener* listener)
{
    synchronized(this) {
        Int32 i = 0;
        for (; i < mListeners->GetLength(); i++) {
            if ((*mListeners)[i] == listener) {
                mListeners->Copy(i, mListeners, i + 1, mListeners->GetLength() - i - 1);
                // System.arraycopy(mListeners, i + 1,
                //         mListeners, i, mListeners->GetLength() - i - 1);
                mTimes->Copy(i, mTimes, i + 1, mTimes->GetLength() - i - 1);
                // System.arraycopy(mTimes, i + 1,
                //         mTimes, i, mTimes->GetLength() - i - 1);
                mListeners->Set(mListeners->GetLength() - 1, NULL);
                mTimes->Set(mTimes->GetLength() - 1, NO_TIME);
                break;
            }
            else if ((*mListeners)[i] == NULL) {
                break;
            }
        }

        ScheduleNotification(NOTIFY_TIME, 0 /* delay */);
    }
    return NOERROR;
}

ECode CMediaPlayer::TimeProvider::GetCurrentTimeUs(
    /* [in] */ Boolean refreshTime,
    /* [in] */ Boolean monotonic,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized(this) {
        // we always refresh the time when the paused-state changes, because
        // we expect to have received the pause-change event delayed.
        if (mPaused && !refreshTime) {
            *result = mLastReportedTime;
            return NOERROR;
        }

        AutoPtr<Elastos::Core::ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((Elastos::Core::ISystem**)&system);
        Int64 nanoTime;
        system->GetNanoTime(&nanoTime);

        if (refreshTime ||
                nanoTime >= mLastNanoTime + MAX_NS_WITHOUT_POSITION_CHECK) {
            // try {
            Int32 val;
            mPlayer->GetCurrentPosition(&val);
            mLastTimeUs = val * 1000L;
            Boolean b;
            mPlayer->IsPlaying(&b);
            mPaused = !b;
            // if (DEBUG) Logger::V(TAG, (mPaused ? "paused" : "playing") + " at " + mLastTimeUs);
            // } catch (IllegalStateException e) {
            //     if (mPausing) {
            //         // if we were pausing, get last estimated timestamp
            //         mPausing = false;
            //         getEstimatedTime(nanoTime, monotonic);
            //         mPaused = true;
            //         if (DEBUG) Log.d(TAG, "illegal state, but pausing: estimating at " + mLastReportedTime);
            //         return mLastReportedTime;
            //     }
            //     // TODO get time when prepared
            //     throw e;
            // }
            mLastNanoTime = nanoTime;
            if (monotonic && mLastTimeUs < mLastReportedTime) {
                /* have to adjust time */
                mTimeAdjustment = mLastReportedTime - mLastTimeUs;
                if (mTimeAdjustment > 1000000) {
                    // schedule seeked event if time jumped significantly
                    // TODO: do this properly by introducing an exception
                    mStopped = FALSE;
                    mSeeking = TRUE;
                    ScheduleNotification(NOTIFY_SEEK, 0 /* delay */);
                }
            }
            else {
                mTimeAdjustment = 0;
            }
        }

        *result = GetEstimatedTime(nanoTime, monotonic);
        return NOERROR;
    }
}

void CMediaPlayer::TimeProvider::ScheduleNotification(
    /* [in] */ Int32 type,
    /* [in] */ Int64 delayUs)
{
    // ignore time notifications until seek is handled
    if (mSeeking &&
            (type == NOTIFY_TIME || type == REFRESH_AND_NOTIFY_TIME)) {
        return;
    }

    if (DEBUG) Logger::V(TAG, "scheduleNotification %d in %ld", type, delayUs);
    mEventHandler->RemoveMessages(NOTIFY);
    AutoPtr<IMessage> msg;
    mEventHandler->ObtainMessage(NOTIFY, type, 0, (IMessage**)&msg);
    Boolean b;
    mEventHandler->SendMessageDelayed(msg, (Int32) (delayUs / 1000), &b);
}

/*synchronized*/
void CMediaPlayer::TimeProvider::NotifySeek()
{
    mSeeking = FALSE;
    // try {
    Int64 timeUs;
    GetCurrentTimeUs(TRUE, FALSE, &timeUs);
    if (DEBUG) Logger::D(TAG, "onSeekComplete at %ld", timeUs);

    for (Int32 i = 0; i < mListeners->GetLength(); i++) {
        AutoPtr<IMediaTimeProviderOnMediaTimeListener> listener = (*mListeners)[i];
        if (listener == NULL) {
            break;
        }
        listener->OnSeek(timeUs);
    }
    // } catch (IllegalStateException e) {
    //     // we should not be there, but at least signal pause
    //     if (DEBUG) Log.d(TAG, "onSeekComplete but no player");
    //     mPausing = TRUE;  // special handling if player disappeared
    //     notifyTimedEvent(FALSE /* refreshTime */);
    // }
}

/*synchronized*/
void CMediaPlayer::TimeProvider::NotifyStop()
{
    for (Int32 i = 0; i < mListeners->GetLength(); i++) {
        AutoPtr<IMediaTimeProviderOnMediaTimeListener> listener = (*mListeners)[i];
        if (listener == NULL) {
            break;
        }
        listener->OnStop();
    }
}

Int32 CMediaPlayer::TimeProvider::RegisterListener(
    /* [in] */ IMediaTimeProviderOnMediaTimeListener* listener)
{
    Int32 i = 0;
    for (; i < mListeners->GetLength(); i++) {
        if ((*mListeners)[i] == listener || (*mListeners)[i] == NULL) {
            break;
        }
    }

    // new listener
    if (i >= mListeners->GetLength()) {
        AutoPtr<ArrayOf<IMediaTimeProviderOnMediaTimeListener*> > newListeners =
            ArrayOf<IMediaTimeProviderOnMediaTimeListener*>::Alloc(i + 1);
        AutoPtr<ArrayOf<Int64> > newTimes = ArrayOf<Int64>::Alloc(i + 1);
        newListeners->Copy(0, mListeners, 0, mListeners->GetLength());
        // System.arraycopy(mListeners, 0, newListeners, 0, mListeners.length);
        newTimes->Copy(0, mTimes, 0, mTimes->GetLength());
        // System.arraycopy(mTimes, 0, newTimes, 0, mTimes.length);
        mListeners = newListeners;
        mTimes = newTimes;
    }

    if ((*mListeners)[i] == NULL) {
        mListeners->Set(i, listener);
        mTimes->Set(i, IMediaTimeProvider::NO_TIME);
    }
    return i;
}

/*synchronized*/
void CMediaPlayer::TimeProvider::NotifyTimedEvent(
    /* [in] */ Boolean refreshTime)
{
    // figure out next callback
    Int64 nowUs;
    // try {
    GetCurrentTimeUs(refreshTime, TRUE, &nowUs);
    // } catch (IllegalStateException e) {
    //     // assume we paused until new player arrives
    //     mRefresh = true;
    //     mPausing = true; // this ensures that call succeeds
    //     nowUs = getCurrentTimeUs(refreshTime, true);
    // }
    Int64 nextTimeUs = nowUs;

    if (mSeeking) {
        // skip timed-event notifications until seek is complete
        return;
    }

    if (DEBUG) {
        StringBuilder sb;
        sb.Append("notifyTimedEvent(");
        sb.Append(mLastTimeUs);
        sb.Append(" -> ");
        sb.Append(nowUs);
        sb.Append(") from {");

        Boolean first = TRUE;
        for (Int32 i = 0; i < mTimes->GetLength(); i++) {
            Int64 time = (*mTimes)[i];
            if (time == NO_TIME) {
                continue;
            }
            if (!first) sb.Append(", ");
            sb.Append(time);
            first = FALSE;
        }
        sb.Append("}");
        String str;
        sb.ToString(&str);
        Logger::D(TAG, str);
    }

    AutoPtr<IVector> activatedListeners;
    CVector::New((IVector**)&activatedListeners);
    // Vector<MediaTimeProvider.OnMediaTimeListener> activatedListeners =
    //     new Vector<MediaTimeProvider.OnMediaTimeListener>();
    for (Int32 ix = 0; ix < mTimes->GetLength(); ix++) {
        if ((*mListeners)[ix] == NULL) {
            break;
        }
        if ((*mTimes)[ix] <= NO_TIME) {
            // ignore, unless we were stopped
        }
        else if ((*mTimes)[ix] <= nowUs + MAX_EARLY_CALLBACK_US) {
            activatedListeners->Add((*mListeners)[ix]);
            if (DEBUG) Logger::D(TAG, "removed");
            mTimes->Set(ix, NO_TIME);
        }
        else if (nextTimeUs == nowUs || (*mTimes)[ix] < nextTimeUs) {
            nextTimeUs = (*mTimes)[ix];
        }
    }

    if (nextTimeUs > nowUs && !mPaused) {
        // schedule callback at nextTimeUs
        if (DEBUG) Logger::D(TAG, "scheduling for %ld and %ld", nextTimeUs, nowUs);
        ScheduleNotification(NOTIFY_TIME, nextTimeUs - nowUs);
    }
    else {
        mEventHandler->RemoveMessages(NOTIFY);
        // no more callbacks
    }

    Int32 size;
    activatedListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        activatedListeners->Get(i, (IInterface**)&obj);
        IMediaTimeProviderOnMediaTimeListener::Probe(obj)->OnTimedEvent(nowUs);
    }
}

Int64 CMediaPlayer::TimeProvider::GetEstimatedTime(
    /* [in] */ Int64 nanoTime,
    /* [in] */ Boolean monotonic)
{
    if (mPaused) {
        mLastReportedTime = mLastTimeUs + mTimeAdjustment;
    }
    else {
        Int64 timeSinceRead = (nanoTime - mLastNanoTime) / 1000;
        mLastReportedTime = mLastTimeUs + timeSinceRead;
        if (mTimeAdjustment > 0) {
            Int64 adjustment =
                mTimeAdjustment - timeSinceRead / TIME_ADJUSTMENT_RATE;
            if (adjustment <= 0) {
                mTimeAdjustment = 0;
            }
            else {
                mLastReportedTime += adjustment;
            }
        }
    }
    return mLastReportedTime;
}

//===================================================================
//                  CMediaPlayer::TimeProvider::EventHandler
//===================================================================

ECode CMediaPlayer::TimeProvider::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    if (what == NOTIFY) {
        Int32 arg1;
        msg->GetArg1(&arg1);
        switch (arg1) {
        case CMediaPlayer::TimeProvider::NOTIFY_TIME:
            mHost->NotifyTimedEvent(FALSE /* refreshTime */);
            break;
        case CMediaPlayer::TimeProvider::REFRESH_AND_NOTIFY_TIME:
            mHost->NotifyTimedEvent(TRUE /* refreshTime */);
            break;
        case CMediaPlayer::TimeProvider::NOTIFY_STOP:
            mHost->NotifyStop();
            break;
        case CMediaPlayer::TimeProvider::NOTIFY_SEEK:
            mHost->NotifySeek();
            break;
        }
    }
    return NOERROR;
}

//===================================================================
//                  CMediaPlayer::EventHandler
//===================================================================

CMediaPlayer::EventHandler::EventHandler(
    /* [in] */ CMediaPlayer* host,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mMediaPlayer(host)
{
}

ECode CMediaPlayer::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    if (mMediaPlayer->mNativeContext == 0) {
        Logger::W(CMediaPlayer::TAG, "mediaplayer went away with unhandled events");
        return NOERROR;
    }

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case MEDIA_PREPARED:
            mMediaPlayer->ScanInternalSubtitleTracks();
            if (mMediaPlayer->mOnPreparedListener != NULL)
                mMediaPlayer->mOnPreparedListener->OnPrepared(mMediaPlayer);
            return NOERROR;

        case MEDIA_PLAYBACK_COMPLETE:
            if (mMediaPlayer->mOnCompletionListener != NULL)
                mMediaPlayer->mOnCompletionListener->OnCompletion(mMediaPlayer);
            mMediaPlayer->StayAwake(FALSE);
            return NOERROR;

        case MEDIA_STOPPED:
            if (mMediaPlayer->mTimeProvider != NULL) {
                mMediaPlayer->mTimeProvider->OnStopped();
            }
            break;

        case MEDIA_STARTED:
        case MEDIA_PAUSED:
            if (mMediaPlayer->mTimeProvider != NULL) {
                mMediaPlayer->mTimeProvider->OnPaused(what == MEDIA_PAUSED);
            }
            break;

        case MEDIA_BUFFERING_UPDATE:
            if (mMediaPlayer->mOnBufferingUpdateListener != NULL)
                mMediaPlayer->mOnBufferingUpdateListener->OnBufferingUpdate(mMediaPlayer, arg1);
            return NOERROR;

        case MEDIA_SEEK_COMPLETE:
          if (mMediaPlayer->mOnSeekCompleteListener != NULL) {
              mMediaPlayer->mOnSeekCompleteListener->OnSeekComplete(mMediaPlayer);
          }
          // fall through

        case MEDIA_SKIPPED:
          if (mMediaPlayer->mTimeProvider != NULL) {
              mMediaPlayer->mTimeProvider->OnSeekComplete(mMediaPlayer);
          }
          return NOERROR;

        case MEDIA_SET_VIDEO_SIZE:
          if (mMediaPlayer->mOnVideoSizeChangedListener != NULL)
              mMediaPlayer->mOnVideoSizeChangedListener->OnVideoSizeChanged(mMediaPlayer, arg1, arg2);
          return NOERROR;

        case MEDIA_ERROR: {
            Logger::E(CMediaPlayer::TAG, "Error (%d, %d)", arg1, arg2);
            Boolean error_was_handled = FALSE;
            if (mMediaPlayer->mOnErrorListener != NULL) {
                Boolean b;
                error_was_handled = mMediaPlayer->mOnErrorListener->OnError(mMediaPlayer, arg1, arg2, &b);
            }
            if (mMediaPlayer->mOnCompletionListener != NULL && ! error_was_handled) {
                mMediaPlayer->mOnCompletionListener->OnCompletion(mMediaPlayer);
            }
            mMediaPlayer->StayAwake(FALSE);
            return NOERROR;
        }
        case MEDIA_INFO: {
            switch (arg1) {
            case MEDIA_INFO_VIDEO_TRACK_LAGGING:
                Logger::I(CMediaPlayer::TAG, "Info (%d, %d)", arg1, arg2);
                break;
            case MEDIA_INFO_METADATA_UPDATE:
                mMediaPlayer->ScanInternalSubtitleTracks();
                // fall through

            case MEDIA_INFO_EXTERNAL_METADATA_UPDATE:
                arg1 = MEDIA_INFO_METADATA_UPDATE;
                // update default track selection
                if (mMediaPlayer->mSubtitleController != NULL) {
                    mMediaPlayer->mSubtitleController->SelectDefaultTrack();
                }
                break;
            }

            if (mMediaPlayer->mOnInfoListener != NULL) {
                Boolean b;
                mMediaPlayer->mOnInfoListener->OnInfo(mMediaPlayer, arg1, arg2, &b);
            }
            // No real default action so far.
            return NOERROR;
        }
        case MEDIA_TIMED_TEXT: {
            if (mMediaPlayer->mOnTimedTextListener == NULL)
                return NOERROR;
            if (obj == NULL) {
                mMediaPlayer->mOnTimedTextListener->OnTimedText(mMediaPlayer, NULL);
            }
            else {
                if (IParcel::Probe(obj) != NULL) {
                    AutoPtr<ITimedText> text;
                    assert(0);
                    // AutoPtr<IParcel> parcel = IParcel::Probe(obj);
                    // CTimedText::New(parcel, (ITimedText**)&text);
                    // parcel->Recycle();
                    mMediaPlayer->mOnTimedTextListener->OnTimedText(mMediaPlayer, text);
                }
            }
            return NOERROR;
        }
        case MEDIA_SUBTITLE_DATA: {
            if (mMediaPlayer->mOnSubtitleDataListener == NULL) {
                return NOERROR;
            }
            if (IParcel::Probe(obj) != NULL) {
                AutoPtr<ISubtitleData> data;
                assert(0);
                // AutoPtr<IParcel> parcel = IParcel::Probe(obj);
                // CSubtitleData::New(parcel, (ISubtitleData**)&data);
                // parcel->Recycle();
                mMediaPlayer->mOnSubtitleDataListener->OnSubtitleData(mMediaPlayer, data);
            }
            return NOERROR;
        }
        case MEDIA_NOP: // interface test message - ignore
            break;

        default:
            Logger::E(CMediaPlayer::TAG, "Unknown message type %d", what);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

//===================================================================
//                  CMediaPlayer::MediaPlayerOnSubtitleDataListener
//===================================================================

CAR_INTERFACE_IMPL(CMediaPlayer::MediaPlayerOnSubtitleDataListener,
        Object, IMediaPlayerOnSubtitleDataListener)

ECode CMediaPlayer::MediaPlayerOnSubtitleDataListener::OnSubtitleData(
    /* [in] */ IMediaPlayer* mp,
    /* [in] */ ISubtitleData* data)
{
    Int32 index;
    data->GetTrackIndex(&index);
    if (index >= mHost->mInbandSubtitleTracks->GetLength()) {
        return NOERROR;
    }
    AutoPtr<ISubtitleTrack> track = (*mHost->mInbandSubtitleTracks)[index];
    if (track != NULL) {
        track->OnData(data);
    }
    return NOERROR;
}

//===================================================================
//                  CMediaPlayer::HandlerRunnable
//===================================================================

ECode CMediaPlayer::HandlerRunnable::Run()
{
    Int32 res = AddTrack();
    if (mHost->mEventHandler != NULL) {
        AutoPtr<IMessage> m;
        mHost->mEventHandler->ObtainMessage(MEDIA_INFO, res, 0, NULL);
        Boolean b;
        mHost->mEventHandler->SendMessage(m, &b);
    }
    AutoPtr<ILooper> looper;
    mThread->GetLooper((ILooper**)&looper);
    looper->QuitSafely();
    return NOERROR;
}

Int32 CMediaPlayer::HandlerRunnable::AddTrack()
{
    if (mIs == NULL || mHost->mSubtitleController == NULL) {
        return MEDIA_INFO_UNSUPPORTED_SUBTITLE;
    }

    AutoPtr<ISubtitleTrack> track;
    mHost->mSubtitleController->AddTrack(mFormat, (ISubtitleTrack**)&track);
    if (track == NULL) {
        return MEDIA_INFO_UNSUPPORTED_SUBTITLE;
    }

    // TODO: do the conversion in the subtitle track
    AutoPtr<IScanner> scanner;
    CScanner::New(mIs, String("UTF-8"), (IScanner**)&scanner);
    AutoPtr<IScanner> tmp;
    scanner->UseDelimiter(String("\\A"), (IScanner**)&tmp);
    String contents;
    tmp->Next(&contents);
    AutoPtr<IVector> lock = mHost->mOpenSubtitleSources;
    synchronized(lock) {
        mHost->mOpenSubtitleSources->Remove(mIs);
    }
    scanner->Close();
    mHost->mOutOfBandSubtitleTracks->Add(track);
    track->OnData(contents.GetBytes(), true /* eos */, ~0 /* runID: keep forever */);
    return MEDIA_INFO_EXTERNAL_METADATA_UPDATE;
}

//===================================================================
//                  CMediaPlayer::MyAnchor
//===================================================================

CAR_INTERFACE_IMPL(CMediaPlayer::MyAnchor, Object, ISubtitleControllerAnchor)

ECode CMediaPlayer::MyAnchor::SetSubtitleWidget(
    /* [in] */ ISubtitleTrackRenderingWidget* subtitleWidget)
{
    return NOERROR;
}

ECode CMediaPlayer::MyAnchor::GetSubtitleLooper(
    /* [out] */ ILooper** result)
{
    *result = Looper::GetMainLooper();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===================================================================
//                  CMediaPlayer::MyRunnable
//===================================================================

ECode CMediaPlayer::MyRunnable::Run()
{
    Int32 res = AddTrack();
    if (mHost->mEventHandler != NULL) {
        AutoPtr<IMessage> m;
        mHost->mEventHandler->ObtainMessage(MEDIA_INFO, res, 0, NULL, (IMessage**)&m);
        Boolean b;
        mHost->mEventHandler->SendMessage(m, &b);
    }
    AutoPtr<ILooper> looper;
    mThread->GetLooper((ILooper**)&looper);
    looper->QuitSafely();
    return NOERROR;
}

Int32 CMediaPlayer::MyRunnable::AddTrack()
{
    AutoPtr<IInputStream> is;
    AutoPtr<IByteArrayOutputStream> bos;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&bos);
    // try {
    AutoPtr<ILibcore> lc;
    CLibcore::AcquireSingleton((ILibcore**)&lc);
    AutoPtr<IOs> os;
    lc->GetOs((IOs**)&os);
    Int64 size = 0;
    os->Lseek(mFd, mOffset, OsConstants::_SEEK_SET, &size);

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(4096);
    for (Int64 total = 0; total < mLength;) {
        Int32 bytesToRead = (Int32) Elastos::Core::Math::Min((Int64)buffer->GetLength(), mLength - total);
        AutoPtr<IIoBridge> ib;
        CIoBridge::AcquireSingleton((IIoBridge**)&ib);
        Int32 bytes;
        ib->Read(mFd, buffer, 0, bytesToRead, &bytes);
        if (bytes < 0) {
            break;
        }
        else {
            IOutputStream::Probe(bos)->Write(buffer, 0, bytes);
            total += bytes;
        }
    }
    AutoPtr<ArrayOf<Byte> > bytes;
    bos->ToByteArray((ArrayOf<Byte>**)&bytes);
    mTrack->OnData(bytes, TRUE /* eos */, ~0 /* runID: keep forever */);
    // } catch (Exception e) {
    //     Log.e(TAG, e.getMessage(), e);
    //     return MEDIA_INFO_TIMED_TEXT_ERROR;
    // } finally {
    if (is != NULL) {
        // try {
        is->Close();
        // } catch (IOException e) {
        //     Log.e(TAG, e.getMessage(), e);
        // }
    }
    // }
    return MEDIA_INFO_EXTERNAL_METADATA_UPDATE;
}

//===================================================================
//                  CMediaPlayer
//===================================================================

CAR_INTERFACE_IMPL(CMediaPlayer, Object, IMediaPlayer)

CAR_OBJECT_IMPL(CMediaPlayer)

CMediaPlayer::CMediaPlayer()
    : mNativeContext(0)
    , mNativeSurfaceTexture(0)
    , mListenerContext(0)
    , mScreenOnWhilePlaying(FALSE)
    , mStayAwake(FALSE)
    , mStreamType(IAudioManager::USE_DEFAULT_STREAM_TYPE)
    , mUsage(-1)
    , mSelectedSubtitleTrackIndex(-1)
{
    mSubtitleDataListener = new MediaPlayerOnSubtitleDataListener(this);
}

CMediaPlayer::~CMediaPlayer()
{
    NativeFinalize();
}

ECode CMediaPlayer::constructor()
{
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    if (looper != NULL) {
        mEventHandler = new EventHandler(this, looper);
    }
    else if ((looper = Looper::GetMainLooper()) != NULL) {
        mEventHandler = new EventHandler(this, looper);
    }
    else {
        mEventHandler = NULL;
    }

    mTimeProvider = new TimeProvider(this);
    CVector::New((IVector**)&mOutOfBandSubtitleTracks);
    CVector::New((IVector**)&mOpenSubtitleSources);
    mInbandSubtitleTracks = ArrayOf<ISubtitleTrack*>::Alloc(0);

    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::APP_OPS_SERVICE);
    mAppOps = IIAppOpsService::Probe(service);

    /* Native setup requires a weak reference to our object.
    * It's easier to create it here than in C++.
    */
    ECode ec = NativeSetup();
    ASSERT_SUCCEEDED(ec);
    return ec;
}

ECode CMediaPlayer::DecVideoSurfaceRef()
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL) {
        return NOERROR;
    }

    android::sp<android::IGraphicBufferProducer> old_st = getVideoSurfaceTexture(this);
    if (old_st != NULL) {
        old_st->decStrong(this);
    }
    return NOERROR;
}

ECode CMediaPlayer::SetVideoSurface(
    /* [in] */ ISurface* insurface,
    /* [in] */ Boolean mediaPlayerMustBeAlive)
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if ( mp == NULL ) {
        if (mediaPlayerMustBeAlive) {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        return NOERROR;
    }

    DecVideoSurfaceRef();

    android::sp<android::IGraphicBufferProducer> new_st;
    if (insurface) {
        android::sp<android::Surface> surface(android_view_Surface_getSurface(insurface));
        if (surface != NULL) {
            new_st = surface->getIGraphicBufferProducer();
            if (new_st == NULL) {
                Logger::E(TAG, "The surface does not have a binding SurfaceTexture!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            new_st->incStrong(this);
        }
        else {
            Logger::E(TAG, "The surface has been released!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    // env->SetIntField(thiz, fields.surface_texture, (int)new_st.get());
    mNativeSurfaceTexture = reinterpret_cast<Handle32>(new_st.get());
    // This will fail if the media player has not been initialized yet. This
    // can be the case if setDisplay() on MediaPlayer.java has been called
    // before setDataSource(). The redundant call to setVideoSurfaceTexture()
    // in prepare/prepareAsync covers for this case.
    mp->setVideoSurfaceTexture(new_st);
    return NOERROR;
}

ECode CMediaPlayer::NativeSetVideoSurface(
    /* [in] */ ISurface* surface )
{
    return SetVideoSurface(surface, TRUE /* mediaPlayerMustBeAlive */);
}

ECode CMediaPlayer::NewRequest(
    /* [out] */ IParcel ** parcel)
{
    VALIDATE_NOT_NULL(parcel);

    AutoPtr<IParcel> params;
    CCallbackParcel::New((IParcel**)&params);
    assert(0 && "TODO");
    // Parcel parcel = Parcel.obtain();
    // parcel.writeInterfaceToken(IMEDIA_PLAYER);
    // return parcel;
    return E_NOT_IMPLEMENTED;
}

ECode CMediaPlayer::Invoke(
    /* [in] */ IParcel* request,
    /* [in] */ IParcel* reply)
{
    Int32 retcode;
    FAIL_RETURN(NativeInvoke(request, reply, &retcode));
    reply->SetDataPosition(0);

    if (retcode != 0) {
        Logger::E(TAG, "failure code: %d", retcode);
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode CMediaPlayer::SetDisplay(
    /* [in] */ ISurfaceHolder* sh)
{
    mSurfaceHolder = sh;
    AutoPtr<ISurface> surface;
    if (sh != NULL) {
        sh->GetSurface((ISurface**)&surface);
    }

    FAIL_RETURN(NativeSetVideoSurface(surface));
    return UpdateSurfaceScreenOn();
}

ECode CMediaPlayer::SetSurface(
    /* [in] */ ISurface* surface)
{
    if (mScreenOnWhilePlaying && surface != NULL) {
        Logger::W(TAG, "setScreenOnWhilePlaying(true) is ineffective for Surface");
    }

    mSurfaceHolder = NULL;
    FAIL_RETURN(NativeSetVideoSurface(surface));
    return UpdateSurfaceScreenOn();
}

ECode CMediaPlayer::SetVideoScalingMode(
    /* [in] */ Int32 mode)
{
    if (!IsVideoScalingModeSupported(mode)) {
        Logger::E(TAG, "Scaling mode %d  is not supported.", mode);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    assert(0 && "TODO");
    // AutoPtr<IParcel> request = Parcel->Obtain();
    // AutoPtr<IParcel> reply = Parcel->Obtain();
    // //try {
    //     request->WriteInterfaceToken(IMEDIA_PLAYER);
    //     request->WriteInt(INVOKE_ID_SET_VIDEO_SCALE_MODE);
    //     request->WriteInt(mode);
    //     Invoke(request, reply);
    // //} finally {
    //     request->Recycle();
    //     reply->Recycle();
    // //}

    return NOERROR;
}

ECode CMediaPlayer::Create(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [out] */ IMediaPlayer** player)
{
    return Create(context, uri, NULL, player);
}

ECode CMediaPlayer::Create(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ ISurfaceHolder* holder,
    /* [out] */ IMediaPlayer** player)
{
    Int32 s;
// TODO: Need CAudioSystem
    // CAudioSystem::NewAudioSessionId(&s);
    return Create(context, uri, holder, NULL, s > 0 ? s : 0, player);
}

ECode CMediaPlayer::Create(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ IAudioAttributes* audioAttributes,
    /* [in] */ Int32 audioSessionId,
    /* [out] */ IMediaPlayer** player)
{
    VALIDATE_NOT_NULL(player);

    // try {
    AutoPtr<IMediaPlayer> mp;
    CMediaPlayer::New((IMediaPlayer**)&mp);
    AutoPtr<IAudioAttributes> aa;
    if (audioAttributes != NULL) {
        aa = audioAttributes;
    }
    else {
        AutoPtr<IAudioAttributesBuilder> aabb;
        CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aabb);
        aabb->Build((IAudioAttributes**)&aa);
    }

    mp->SetAudioAttributes(aa);
    mp->SetAudioSessionId(audioSessionId);
    mp->SetDataSource(context, uri);
    if (holder != NULL) {
        mp->SetDisplay(holder);
    }
    mp->Prepare();
    *player = mp;
    REFCOUNT_ADD(*player)
    return NOERROR;
    // } catch (IOException ex) {
    //     Log.d(TAG, "create failed:", ex);
    //     // fall through
    // } catch (IllegalArgumentException ex) {
    //     Log.d(TAG, "create failed:", ex);
    //     // fall through
    // } catch (SecurityException ex) {
    //     Log.d(TAG, "create failed:", ex);
    //     // fall through
    // }
}

ECode CMediaPlayer::Create(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resid,
    /* [out] */ IMediaPlayer** player)
{
    Int32 s;
// TODO: Need CAudioSystem
    // CAudioSystem::NewAudioSessionId(&s);
    return Create(context, resid, NULL, s > 0 ? s : 0, player);
}

ECode CMediaPlayer::Create(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resid,
    /* [in] */ IAudioAttributes* audioAttributes,
    /* [in] */ Int32 audioSessionId,
    /* [out] */ IMediaPlayer** player)
{
    VALIDATE_NOT_NULL(player);
    *player = NULL;
    VALIDATE_NOT_NULL(context);

    //try {
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IAssetFileDescriptor> afd;
    res->OpenRawResourceFd(resid, (IAssetFileDescriptor**)&afd);
    if (afd == NULL) {
        return NOERROR;
    }

    AutoPtr<IFileDescriptor> fd;
    Int64 offset, length;

    AutoPtr<IMediaPlayer> mp;
    ECode ec = CMediaPlayer::New((IMediaPlayer**)&mp);

    AutoPtr<IAudioAttributes> aa;
    if (audioAttributes != NULL) {
        aa = audioAttributes;
    }
    else {
        AutoPtr<IAudioAttributesBuilder> aabb;
        CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aabb);
        aabb->Build((IAudioAttributes**)&aa);
    }
    mp->SetAudioAttributes(aa);
    mp->SetAudioSessionId(audioSessionId);

    afd->GetFileDescriptor((IFileDescriptor**)&fd);
    afd->GetStartOffset(&offset);
    afd->GetLength(&length);

    ec = mp->SetDataSource(fd, offset, length);
    FAIL_GOTO(ec, _EXIT_);

    if (afd != NULL) {
        afd->Close();
        afd = NULL;
    }

    ec = mp->Prepare();
    FAIL_GOTO(ec, _EXIT_);

    *player = mp ;
    REFCOUNT_ADD(*player);
    return NOERROR;

_EXIT_:
    if (afd != NULL) {
        afd->Close();
        afd = NULL;
    }

    Logger::D(TAG, "create MediaPlayer failed: %08x", ec);
    return ec;
}

ECode CMediaPlayer::SetDataSource(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    return SetDataSource(context, uri, NULL);
}

ECode CMediaPlayer::SetDataSource(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ IMap* headers)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(uri);

    String scheme;
    uri->GetScheme(&scheme);
    if(scheme.IsNull() || scheme.Equals("file")) {
        String path;
        uri->GetPath(&path);
        return SetDataSource(path);
    }

    AutoPtr<IAssetFileDescriptor> afd;
    // try {
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->OpenAssetFileDescriptor(uri, String("r"), (IAssetFileDescriptor**)&afd);
    if (afd == NULL) {
       return NOERROR;
    }

    // Note: using getDeclaredLength so that our behavior is the same
    // as previous versions when the content provider is returning
    // a full file.
    ECode ec = NOERROR;
    String uriStr;
    Int64 length, offset;
    AutoPtr<IFileDescriptor> fd;
    ec = afd->GetStartOffset(&offset);
    FAIL_GOTO(ec, _EXIT_)

    afd->GetDeclaredLength(&length);
    FAIL_GOTO(ec, _EXIT_)

    afd->GetFileDescriptor((IFileDescriptor**)&fd);
    FAIL_GOTO(ec, _EXIT_)

    if (length < 0) {
       ec = SetDataSource(fd);
    }
    else {
       ec = SetDataSource(fd, offset, length);
    }

_EXIT_:
    if (afd) {
        afd->Close();
        afd = NULL;
    }

    if (ec != E_SECURITY_EXCEPTION && ec != E_IO_EXCEPTION) {
        return ec;
    }

    Logger::D(TAG, "Couldn't open file on client side, trying server side");
    IObject::Probe(uri)->ToString(&uriStr);
    return SetDataSource(uriStr, headers);
}

ECode CMediaPlayer::SetDataSource(
    /* [in] */ const String& path)
{
    ECode ec = SetDataSource(path, NULL, NULL);
    return ec;
}

ECode CMediaPlayer::SetDataSource(
    /* [in] */ const String& path,
    /* [in] */ IMap* headers)
{
    AutoPtr<ArrayOf<String> > keys;
    AutoPtr<ArrayOf<String> > values;

    if (headers != NULL) {
        Int32 size;
        headers->GetSize(&size);
        keys = ArrayOf<String>::Alloc(size);
        values = ArrayOf<String>::Alloc(size);

        AutoPtr<ISet> entrySet;
        headers->GetEntrySet((ISet**)&entrySet);
        AutoPtr<ArrayOf<IInterface*> > array;
        entrySet->ToArray((ArrayOf<IInterface*>**)&array);

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IMapEntry> entry;
            entry = IMapEntry::Probe((*array)[i]);
            AutoPtr<IInterface> iKey;
            entry->GetKey((IInterface**)&iKey);
            String key;
            ICharSequence::Probe(iKey)->ToString(&key);
            keys->Set(i, key);
            AutoPtr<IInterface> iValue;
            entry->GetValue((IInterface**)&iValue);
            String value;
            ICharSequence::Probe(iKey)->ToString(&value);
            values->Set(i, value);
        }
    }

    return SetDataSource(path, keys, values);
}

ECode CMediaPlayer::SetDataSource(
    /* [in] */ const String& _path,
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<String>* values)
{
    String path = _path;
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->Parse(path, (IUri**)&uri);

    String scheme;
    uri->GetScheme(&scheme);
    if (scheme.Equals("file")) {
        uri->GetPath(&path);
    }
    else if (scheme != NULL) {
        // handle non-file sources
        AutoPtr<IBinder> binder;
        CMediaHTTPService::CreateHttpServiceBinderIfNecessary(path, (IBinder**)&binder);
        return NativeSetDataSource(
            binder,
            path,
            keys,
            values);
    }

    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);

    Boolean b;
    if (file->Exists(&b), b) {
    // if (file != NULL && (file->Exists(&bol), bol)) {
        AutoPtr<IFileInputStream> is;
        CFileInputStream::New(file, (IFileInputStream**)&is);

        AutoPtr<IFileDescriptor> fd;
        is->GetFD((IFileDescriptor**)&fd);
        ECode ec = SetDataSource(fd);
        ICloseable::Probe(is)->Close();
        return ec;
    }
    else {
        // throw new IOException("setDataSource failed.");
        return E_IO_EXCEPTION;
    }
}

ECode CMediaPlayer::NativeSetDataSource(
    /* [in] */ IBinder* httpServiceBinderObj,
    /* [in] */ const String& path,
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<String>* values)
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (path.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (LOGV) Logger::V(TAG, "NativeSetDataSource: path %s", path.string());

    android::String8 pathStr(path.string());

    // headers is a Map<String, String>.
    // We build a similar KeyedVector out of it.
    android::KeyedVector<android::String8, android::String8> headersVector;
// TODO: Need Media_Utils
    // FAIL_RETURN(Media_Utils::ConvertKeyValueArraysToKeyedVector(
    //     keys, values, &headersVector));

    android::sp<android::IMediaHTTPService> httpService;
    if (httpServiceBinderObj != NULL) {
// TODO: Need jni code
        // android::sp<IBinder> binder = ibinderForJavaObject(env, httpServiceBinderObj);
        // httpService = interface_cast<android::IMediaHTTPService>(binder);
    }

    android::status_t opStatus = mp->setDataSource(
        httpService, pathStr, headersVector.size() > 0? &headersVector : NULL);
    return process_media_player_call(this, opStatus, E_IO_EXCEPTION, "NativeSetDataSource failed.");
}

ECode CMediaPlayer::SetDataSource(
    /* [in] */ IFileDescriptor* fd)
{
    // intentionally less than LONG_MAX
    return SetDataSource(fd, 0, 0x7ffffffffffffffll);
}

ECode CMediaPlayer::SetDataSource(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    return NativeSetDataSource(fd, offset, length);
}

ECode CMediaPlayer::NativeSetDataSource(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (fd == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 num;
    fd->GetDescriptor(&num);

    if (LOGV) Logger::V(TAG, "SetDataSourceFD %d, offset %d, length %d", num, offset, length);

    return process_media_player_call(this,
        mp->setDataSource(num, offset, length),
        E_IO_EXCEPTION, "setDataSourceFD failed.");
}

ECode CMediaPlayer::Prepare()
{
    NativePrepare();
    ScanInternalSubtitleTracks();
    return NOERROR;
}

ECode CMediaPlayer::NativePrepare()
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::sp<android::IGraphicBufferProducer> st = getVideoSurfaceTexture(this);
    mp->setVideoSurfaceTexture(st);

    return process_media_player_call(this, mp->prepare(),
        E_IO_EXCEPTION, "Prepare failed.");
}

ECode CMediaPlayer::PrepareAsync()
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::sp<android::IGraphicBufferProducer> st = getVideoSurfaceTexture(this);
    mp->setVideoSurfaceTexture(st);

    return process_media_player_call(this, mp->prepareAsync(),
        E_IO_EXCEPTION, "Prepare Async failed.");
}

ECode CMediaPlayer::Start()
{
    if (IsRestricted()) {
        NativeSetVolume(0, 0);
    }

    StayAwake(TRUE);
    return NativeStart();
}

ECode CMediaPlayer::NativeStart()
{
    if (LOGV) Logger::V(TAG, " > start");
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return process_media_player_call(this, mp->start(), NOERROR, "NativeStart");
}

Boolean CMediaPlayer::IsRestricted()
{
    // try {
    Int32 usage;
    if (mUsage != -1) {
        usage = mUsage;
    }
    else {
        CAudioAttributes::UsageForLegacyStreamType(GetAudioStreamType(), &usage);
    }
    Int32 mode;
    mAppOps->CheckAudioOperation(IAppOpsManager::OP_PLAY_AUDIO, usage,
            Process::MyUid(), CActivityThread::GetCurrentPackageName(), &mode);
    return mode != IAppOpsManager::MODE_ALLOWED;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

Int32 CMediaPlayer::GetAudioStreamType()
{
    if (mStreamType == IAudioManager::USE_DEFAULT_STREAM_TYPE) {
        mStreamType = NativeGetAudioStreamType();
    }
    return mStreamType;
}

Int32 CMediaPlayer::NativeGetAudioStreamType()
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    audio_stream_type_t streamtype;
    process_media_player_call(this, mp->getAudioStreamType(&streamtype), NOERROR);
    ALOGV("getAudioStreamType: %d (streamtype)", streamtype);
    return (Int32)streamtype;
}

ECode CMediaPlayer::Stop()
{
    StayAwake(FALSE);
    return NativeStop();
}

ECode CMediaPlayer::NativeStop()
{
    if (LOGV) Logger::V(TAG, " > stop");
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return process_media_player_call(this, mp->stop(), NOERROR, "NativeStop");
}

ECode CMediaPlayer::Pause()
{
    StayAwake(FALSE);
    return NativePause();
}

ECode CMediaPlayer::NativePause()
{
    if (LOGV) Logger::V(TAG, " > pause");
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return process_media_player_call(this, mp->pause(), NOERROR, "NativePause");
}

ECode CMediaPlayer::SetWakeMode(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode)
{
    Boolean washeld = FALSE;
    if (mWakeLock != NULL) {
        Boolean bval;
        mWakeLock->IsHeld(&bval);
        if (bval) {
            washeld = TRUE;
            mWakeLock->ReleaseLock();
        }
        mWakeLock = NULL;
    }

    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(tmpObj.Get());
    if (pm != NULL) {
        pm->NewWakeLock(mode | IPowerManager::ON_AFTER_RELEASE, String("CMediaPlayer"),
            (IPowerManagerWakeLock**)&mWakeLock);
        mWakeLock->SetReferenceCounted(FALSE);
        if (washeld) {
            mWakeLock->AcquireLock();
        }
    }

    return NOERROR;
}

ECode CMediaPlayer::SetScreenOnWhilePlaying(
    /* [in] */ Boolean screenOn)
{
    if (mScreenOnWhilePlaying != screenOn) {
        if (screenOn && mSurfaceHolder == NULL) {
            Logger::W(TAG, "setScreenOnWhilePlaying(true) is ineffective without a SurfaceHolder");
        }
        mScreenOnWhilePlaying = screenOn;
        return UpdateSurfaceScreenOn();
    }
    return NOERROR;
}

ECode CMediaPlayer::StayAwake(
    /* [in] */ Boolean awake)
{
    if (mWakeLock != NULL) {
        Boolean bval;
        mWakeLock->IsHeld(&bval);
        if (awake && !bval) {
            mWakeLock->AcquireLock();
        }
        else if (!awake && bval) {
            mWakeLock->ReleaseLock();
        }
    }

    mStayAwake = awake;
    return UpdateSurfaceScreenOn();
}

ECode CMediaPlayer::UpdateSurfaceScreenOn()
{
    if (mSurfaceHolder != NULL) {
        return mSurfaceHolder->SetKeepScreenOn(mScreenOnWhilePlaying && mStayAwake);
    }
    return NOERROR;
}

/**
 * Returns the width of the video.
 *
 * @return the width of the video, or 0 if there is no video,
 * no display surface was set, or the width has not been determined
 * yet. The OnVideoSizeChangedListener can be registered via
 * {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}
 * to provide a notification when the width is available.
 */
ECode CMediaPlayer::GetVideoWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    int w;
    if (0 != mp->getVideoWidth(&w)) {
        Logger::E(TAG, "getVideoWidth failed");
        w = 0;
    }

    if (LOGV) Logger::V(TAG, "getVideoWidth: %d", w);
    *width = w;
    return NOERROR;
}

ECode CMediaPlayer::GetVideoHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    int h;
    if (0 != mp->getVideoHeight(&h)) {
        Logger::E(TAG, "getVideoHeight failed");
        h = 0;
    }

    if (LOGV) Logger::V(TAG, "getVideoHeight: %d", h);
    *height = h;
    return NOERROR;
}

ECode CMediaPlayer::IsPlaying(
    /* [out] */ Boolean* isPlaying)
{
    VALIDATE_NOT_NULL(isPlaying);
    *isPlaying = FALSE;

    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *isPlaying = mp->isPlaying();

    // if (LOGV) Logger::V(TAG, "isPlaying: %d", *isPlaying);
    return NOERROR;
}

ECode CMediaPlayer::SeekTo(
    /* [in] */ Int32 msec)
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (LOGV) Logger::V(TAG, "seekTo: %d(msec)", msec);
    return process_media_player_call(this, mp->seekTo(msec), NOERROR, "SeekTo");
}

ECode CMediaPlayer::GetCurrentPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);

    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        *position = 0;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    int msec;
    ECode ec = process_media_player_call(this, mp->getCurrentPosition(&msec), NOERROR, "GetCurrentPosition");
    if (LOGV) Logger::V(TAG, "getCurrentPosition: %d (msec) ec: %08x", msec, ec);
    *position = msec;
    return ec;
}

ECode CMediaPlayer::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = 0;

    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    int msec;
    ECode ec = process_media_player_call(this, mp->getDuration(&msec), NOERROR);
    if (LOGV) Logger::V(TAG, "getDuration: %d (msec), ec=%08x", msec, ec);
    *duration = msec;
    return ec;
}

ECode CMediaPlayer::GetMetadata(
    /* [in] */ Boolean update_only,
    /* [in] */ Boolean apply_filter,
    /* [out] */ IMetadata** metadata)
{
    VALIDATE_NOT_NULL(metadata);
    *metadata = NULL;

    AutoPtr<IParcel> reply;
    CCallbackParcel::New((IParcel**)&reply);

    AutoPtr<IMetadata> data;
// TODO: Need CMetadata
    // FAIL_RETURN(CMetadata::New((IMetadata**)&data));
    Boolean succeeded;
    FAIL_RETURN(NativeGetMetadata(update_only, apply_filter, reply, &succeeded));
    if (!succeeded) {
        return NOERROR;
    }

    // Metadata takes over the parcel, don't recycle it unless
    // there is an error.
    Boolean res;
    FAIL_RETURN(data->Parse(reply, &res));
    if (!res) {
        // reply->Recycle();
        return NOERROR;
    }
    *metadata = data;
    REFCOUNT_ADD(*metadata);
    return NOERROR;
}

ECode CMediaPlayer::SetMetadataFilter(
    /* [in] */ ISet* allow,
    /* [in] */ ISet* block,
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    // Do our serialization manually instead of calling
    // Parcel.writeArray since the sets are made of the same type
    // we avoid paying the price of calling writeValue (used by
    // writeArray) which burns an extra Int32 per element to encode
    // the type.
    AutoPtr<IParcel> request;
    FAIL_RETURN(NewRequest((IParcel**)&request));

    // The parcel starts already with an interface token. There
    // are 2 filters. Each one starts with a 4bytes number to
    // store the len followed by a number of Int32 (4 bytes as well)
    // representing the metadata type.
    Int32 capacity;
    //request->DataSize(&capacity);
    //capacity += 4 * (1 + allow->Size() + 1 + block->Size());

    Int32 cap;
    //request->DataCapacity(&cap);
    if (cap < capacity) {
        //request->SetDataCapacity(capacity);
    }

    //request->WriteInt32(allow->size());
    /*for(Integer t: allow) {
        request->WriteInt32(t);
    }*/
    //request->WriteInt32(block->Size());
    /*for(Integer t: block) {
        request->WriteInt32(t);
    }*/
    return NativeSetMetadataFilter(request);
}

ECode CMediaPlayer::SetNextMediaPlayer(
    /* [in] */ IMediaPlayer* next)
{
    if (LOGV) Logger::V(TAG, "setNextMediaPlayer");
    android::sp<android::MediaPlayer> thisplayer = getMediaPlayer(this);
    if (thisplayer == NULL) {
        Logger::E(TAG, "This player not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    android::sp<android::MediaPlayer> nextplayer = NULL;
    if (next != NULL) {
        nextplayer = getMediaPlayer((CMediaPlayer*)next);
    }

    if (nextplayer == NULL && next != NULL) {
        Logger::E(TAG, "Next player not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (nextplayer == thisplayer) {
        Logger::E(TAG, "Next player can't be self");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // tie the two players together
    return process_media_player_call(this, thisplayer->setNextMediaPlayer(nextplayer),
        E_ILLEGAL_ARGUMENT_EXCEPTION, "setNextMediaPlayer failed.");
}

ECode CMediaPlayer::ReleaseResources()
{
    StayAwake(FALSE);
    UpdateSurfaceScreenOn();
    mOnPreparedListener = NULL;
    mOnBufferingUpdateListener = NULL;
    mOnCompletionListener = NULL;
    mOnSeekCompleteListener = NULL;
    mOnErrorListener = NULL;
    mOnInfoListener = NULL;
    mOnVideoSizeChangedListener = NULL;
    mOnTimedTextListener = NULL;
    if (mTimeProvider != NULL) {
        mTimeProvider->Close();
        mTimeProvider = NULL;
    }
    mOnSubtitleDataListener = NULL;
    NativeRelease();

    return NOERROR;
}

ECode CMediaPlayer::NativeRelease()
{
    if (LOGV) Logger::V(TAG, "release");
    DecVideoSurfaceRef();
    android::sp<android::MediaPlayer> mp = setMediaPlayer(this, NULL);
    if (mp != NULL) {
        // this prevents native callbacks after the object is released
        mp->setListener(0);
        mp->disconnect();
    }
    return NOERROR;
}

ECode CMediaPlayer::Reset()
{
    mSelectedSubtitleTrackIndex = -1;
    synchronized(mOpenSubtitleSources) {
        Int32 size;
        mOpenSubtitleSources->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mOpenSubtitleSources->Get(i, (IInterface**)&obj);
            // try {
            IInputStream::Probe(obj)->Close();
            // } catch (IOException e) {
            // }
        }
        mOpenSubtitleSources->Clear();
    }
    mOutOfBandSubtitleTracks->Clear();
    mInbandSubtitleTracks = ArrayOf<ISubtitleTrack*>::Alloc(0);
    if (mSubtitleController != NULL) {
        mSubtitleController->Reset();
    }
    if (mTimeProvider != NULL) {
        mTimeProvider->Close();
        mTimeProvider = NULL;
    }

    StayAwake(FALSE);
    FAIL_RETURN(NativeReset());

    // make sure none of the listeners get called anymore
    if (mEventHandler != NULL) {
        mEventHandler->RemoveCallbacksAndMessages(NULL);
    }
    return NOERROR;
}

ECode CMediaPlayer::NativeReset()
{
    if (LOGV) Logger::V(TAG, "reset");
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return process_media_player_call(this, mp->reset(), NOERROR, "NativeReset");
}

ECode CMediaPlayer::SetAudioStreamType(
    /* [in] */ Int32 streamtype)
{
    NativeSetAudioStreamType(streamtype);
    mStreamType = streamtype;
    return NOERROR;
}

ECode CMediaPlayer::NativeSetAudioStreamType(
    /* [in] */ Int32 streamtype)
{
    if (LOGV) Logger::V(TAG, "setAudioStreamType: %d", streamtype);
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return process_media_player_call(this,
        mp->setAudioStreamType((audio_stream_type_t)streamtype), NOERROR, "SetAudioStreamType");
}

Boolean CMediaPlayer::NativeSetParameter(
    /* [in] */ Int32 key,
    /* [in] */ IParcel* value)
{
    ALOGV("setParameter: key %d", key);
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

// TODO: Need jni code
    android::Parcel *request;
    // request = parcelForJavaObject(env, java_request);
    android::status_t err = mp->setParameter(key, *request);
    if (err == android::OK) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

ECode CMediaPlayer::SetAudioAttributes(
    /* [in] */ IAudioAttributes* attributes)
{
    if (attributes == NULL) {
        String msg("Cannot set AudioAttributes to null");
        // throw new IllegalArgumentException(msg);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    attributes->GetUsage(&mUsage);
    assert(0);
    // AutoPtr<IParcel> pattributes; // = Parcel.obtain();
    // CParcel::New((IParcel**)&pattributes);
    // IParcelable::Probe(attributes)->WriteToParcel(pattributes, IAudioAttributes::FLATTEN_TAGS);
    // NativeSetParameter(KEY_PARAMETER_AUDIO_ATTRIBUTES, pattributes);
    // pattributes->Recycle();
    return NOERROR;
}

ECode CMediaPlayer::SetLooping(
    /* [in] */ Boolean looping)
{
    if (LOGV) Logger::V(TAG, "setLooping: %d", looping);
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return process_media_player_call(this, mp->setLooping(looping), NOERROR, "SetLooping");
}

ECode CMediaPlayer::IsLooping(
    /* [out] */ Boolean* isLooping)
{
    VALIDATE_NOT_NULL(isLooping);
    *isLooping = FALSE;

    if (LOGV) Logger::V(TAG, "isLooping");
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *isLooping = mp->isLooping();
    return NOERROR;
}

ECode CMediaPlayer::SetVolume(
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume)
{
    if (IsRestricted()) {
        return NOERROR;
    }
    NativeSetVolume(leftVolume, rightVolume);
}

ECode CMediaPlayer::NativeSetVolume(
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume)
{
    if (LOGV) Logger::V(TAG, "setVolume: left %f  right %f", leftVolume, rightVolume);
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return process_media_player_call(this, mp->setVolume(leftVolume, rightVolume),
        NOERROR, NULL);
}

ECode CMediaPlayer::SetVolume(
    /* [in] */ Float volume)
{
    return SetVolume(volume, volume);
}

ECode CMediaPlayer::SetAudioSessionId(
    /* [in] */ Int32 sessionId)
{
    if (LOGV) Logger::V(TAG, "set_session_id(): %d", sessionId);
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return process_media_player_call(this, mp->setAudioSessionId(sessionId), NOERROR);
}

ECode CMediaPlayer::GetAudioSessionId(
    /* [out] */ Int32* sessionID)
{
    VALIDATE_NOT_NULL(sessionID);
    *sessionID = 0;

    if (LOGV) Logger::V(TAG, "get_session_id()");
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *sessionID = mp->getAudioSessionId();
    return NOERROR;
}

ECode CMediaPlayer::AttachAuxEffect(
    /* [in] */ Int32 effectId)
{
    if (LOGV) Logger::V(TAG, "attachAuxEffect(): %d", effectId);
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return process_media_player_call(this, mp->attachAuxEffect(effectId), NOERROR);
}

ECode CMediaPlayer::SetAuxEffectSendLevel(
    /* [in] */ Float level)
{
    if (IsRestricted()) {
        return NOERROR;
    }
    NativeSetAuxEffectSendLevel(level);
}

ECode CMediaPlayer::NativeSetAuxEffectSendLevel(
    /* [in] */ Float level)
{
    if (LOGV) Logger::V(TAG, "setAuxEffectSendLevel: level %f", level);
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return process_media_player_call(this, mp->setAuxEffectSendLevel(level), NOERROR);
}

ECode CMediaPlayer::NativeInvoke(
    /* [in] */ IParcel* request,
    /* [in] */ IParcel* reply,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    android::sp<android::MediaPlayer> media_player = getMediaPlayer(this);
    if (media_player == NULL ) {
        *result = android::UNKNOWN_ERROR;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::Parcel* _request;
    request->GetElementPayload((Handle32*)&_request);
    android::Parcel* _reply;
    reply->GetElementPayload((Handle32*)&_reply);

    // Don't use process_media_player_call which use the async loop to
    // report errors, instead returns the status.
    *result = media_player->invoke(*_request, _reply);
    return NOERROR;
}

ECode CMediaPlayer::NativeGetMetadata(
    /* [in] */ Boolean update_only,
    /* [in] */ Boolean apply_filter,
    /* [in] */ IParcel* reply,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = FALSE;

    android::sp<android::MediaPlayer> media_player = getMediaPlayer(this);
    if (media_player == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::Parcel* metadata;
    reply->GetElementPayload((Handle32*)&metadata);
    if (metadata == NULL ) {
        Logger::E(TAG, "Reply parcel is NULL");
        return E_FAIL;
    }

    metadata->freeData();
    // On return metadata is positioned at the beginning of the
    // metadata. Note however that the parcel actually starts with the
    // return code so you should not rewind the parcel using
    // setDataPosition(0).
    *succeeded = media_player->getMetadata(update_only, apply_filter, metadata) == 0;
    return NOERROR;
}

ECode CMediaPlayer::NativeSetMetadataFilter(
    /* [in] */ IParcel* request)
{
    android::sp<android::MediaPlayer> media_player = getMediaPlayer(this);
    if (media_player == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::Parcel* filter;
    request->GetElementPayload((Handle32*)&filter);

    if (filter == NULL ) {
        Logger::E(TAG, "Filter is NULL.");
        return E_FAIL;
    }

    android::status_t err = media_player->setMetadataFilter(*filter);
    return (err == android::OK) ? NOERROR : E_FAIL;
}

ECode CMediaPlayer::NativeSetup()
{
    if (LOGV) Logger::V(TAG, "NativeSetup");
    android::sp<android::MediaPlayer> mp = new android::MediaPlayer();
    if (mp == NULL) {
        Logger::E(TAG, "Out of memory");
        return E_RUNTIME_EXCEPTION;
    }

    // create new listener and give it to MediaPlayer
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    android::sp<JNIMediaPlayerListener> listener = new JNIMediaPlayerListener(wr);
    mp->setListener(listener);

    // Stow our new C++ MediaPlayer in an opaque field in the Java object.
    setMediaPlayer(this, mp);
    return NOERROR;
}

ECode CMediaPlayer::GetTrackInfo(
    /* [out, callee] */ ArrayOf<IMediaPlayerTrackInfo*>** result)
{
    AutoPtr<ArrayOf<IMediaPlayerTrackInfo*> > trackInfo;
    GetInbandTrackInfo((ArrayOf<IMediaPlayerTrackInfo*>**)&trackInfo);

    // add out-of-band tracks
    Int32 size;
    mOutOfBandSubtitleTracks->GetSize(&size);
    AutoPtr<ArrayOf<IMediaPlayerTrackInfo*> > allTrackInfo =
    ArrayOf<IMediaPlayerTrackInfo*>::Alloc(trackInfo->GetLength() + size);

    // System.arraycopy(trackInfo, 0, allTrackInfo, 0, trackInfo.length);
    allTrackInfo->Copy(0, trackInfo, 0, trackInfo->GetLength());

    Int32 i = trackInfo->GetLength();
    for (Int32 j = 0; j < size; j++) {
        AutoPtr<IInterface> obj;
        mOutOfBandSubtitleTracks->Get(j, (IInterface**)&obj);
        AutoPtr<ISubtitleTrack> track = ISubtitleTrack::Probe(obj);
        Boolean b;
        track->IsTimedText(&b);
        Int32 type;
        if (b) {
            type = IMediaPlayerTrackInfo::MEDIA_TRACK_TYPE_TIMEDTEXT;
        }
        else {
            type = IMediaPlayerTrackInfo::MEDIA_TRACK_TYPE_SUBTITLE;
        }
        AutoPtr<IMediaFormat> mf;
        track->GetFormat((IMediaFormat**)&mf);
        AutoPtr<IMediaPlayerTrackInfo> ti;

        CMediaPlayerTrackInfo::New(type, mf, (IMediaPlayerTrackInfo**)&ti);
        allTrackInfo->Set(i, ti);
        ++i;
    }
    *result = allTrackInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaPlayer::GetInbandTrackInfo(
    /* [out, callee] */ ArrayOf<IMediaPlayerTrackInfo*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    assert(0 && "TODO");
    /*
    AutoPtr<IParcel> request;
    CCallbackParcel::New((IParcel**)&request);
    AutoPtr<IParcel> reply;
    CCallbackParcel::New((IParcel**)&reply);
    //try {
        request->WriteInterfaceToken(IMEDIA_PLAYER);
        request->WriteInt32(INVOKE_ID_GET_TRACK_INFO);
        Invoke(request, reply);
        AutoPtr<ArrayOf<IMediaPlayerTrackInfo> > trackInfo = reply->CreateTypedArray(TrackInfo_CREATOR);
        *result = trackInfo;
        REFCOUNT_ADD(*result);
    //} finally {
    //    request->Recycle();
    //    reply->Recycle();
    //}
    */
    return NOERROR;
}

Boolean CMediaPlayer::AvailableMimeTypeForExternalSource(
    /* [in] */ const String& mimeType)
{
    return (mimeType.Equals(MEDIA_MIMETYPE_TEXT_SUBRIP));
}

ECode CMediaPlayer::SetSubtitleAnchor(
    /* [in] */ ISubtitleController* controller,
    /* [in] */ ISubtitleControllerAnchor* anchor)
{
    // TODO: create SubtitleController in MediaPlayer
    mSubtitleController = controller;
    mSubtitleController->SetAnchor(anchor);
    return NOERROR;
}

ECode CMediaPlayer::OnSubtitleTrackSelected(
    /* [in] */ ISubtitleTrack* track)
{
    if (mSelectedSubtitleTrackIndex >= 0) {
        // try {
        SelectOrDeselectInbandTrack(mSelectedSubtitleTrackIndex, FALSE);
        // } catch (IllegalStateException e) {
        // }
        mSelectedSubtitleTrackIndex = -1;
    }
    SetOnSubtitleDataListener(NULL);
    if (track == NULL) {
        return NOERROR;
    }

    for (Int32 i = 0; i < mInbandSubtitleTracks->GetLength(); i++) {
        if ((*mInbandSubtitleTracks)[i] == track) {
            Logger::V(TAG, "Selecting subtitle track %d", i);
            mSelectedSubtitleTrackIndex = i;
            // try {
            SelectOrDeselectInbandTrack(mSelectedSubtitleTrackIndex, TRUE);
            // } catch (IllegalStateException e) {
            // }
            SetOnSubtitleDataListener(mSubtitleDataListener);
            break;
        }
    }
    // no need to select out-of-band tracks
    return NOERROR;
}

ECode CMediaPlayer::AddSubtitleSource(
    /* [in] */ IInputStream* is,
    /* [in] */ IMediaFormat* format)
{
    AutoPtr<IInputStream> fIs = is;
    AutoPtr<IMediaFormat> fFormat = format;

    // Ensure all input streams are closed.  It is also a handy
    // way to implement timeouts in the future.
    synchronized(mOpenSubtitleSources) {
        mOpenSubtitleSources->Add(is);
    }

    // process each subtitle in its own thread
    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(String("SubtitleReadThread"),
            IProcess::THREAD_PRIORITY_BACKGROUND + IProcess::THREAD_PRIORITY_MORE_FAVORABLE,
            (IHandlerThread**)&thread);
    IThread::Probe(thread)->Start();
    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);
    AutoPtr<IHandler> handler;
    CHandler::New(looper, (IHandler**)&handler);
    AutoPtr<HandlerRunnable> run = new HandlerRunnable(this, thread, fIs, fFormat);
    Boolean b;
    return handler->Post(run, &b);
}

void CMediaPlayer::ScanInternalSubtitleTracks()
{
    if (mSubtitleController == NULL) {
        Logger::E(TAG, "Should have subtitle controller already set");
        return;
    }

    AutoPtr<ArrayOf<IMediaPlayerTrackInfo*> > tracks;
    GetInbandTrackInfo((ArrayOf<IMediaPlayerTrackInfo*>**)&tracks);
    synchronized (mInbandSubtitleLock) {
        AutoPtr<ArrayOf<ISubtitleTrack*> > inbandTracks = ArrayOf<ISubtitleTrack*>::Alloc(tracks->GetLength());
        for (Int32 i = 0; i < tracks->GetLength(); i++) {
            Int32 type;
            (*tracks)[i]->GetTrackType(&type);
            if (type == IMediaPlayerTrackInfo::MEDIA_TRACK_TYPE_SUBTITLE) {
                if (i < mInbandSubtitleTracks->GetLength()) {
                    inbandTracks->Set(i, (*mInbandSubtitleTracks)[i]);
                }
                else {
                    AutoPtr<IMediaFormat> mf;
                    (*tracks)[i]->GetFormat((IMediaFormat**)&mf);
                    AutoPtr<ISubtitleTrack> track;
                    mSubtitleController->AddTrack(mf, (ISubtitleTrack**)&track);
                    inbandTracks->Set(i, track);
                }
            }
        }
        mInbandSubtitleTracks = inbandTracks;
    }
    mSubtitleController->SelectDefaultTrack();
}

ECode CMediaPlayer::AddTimedTextSource(
    /* [in] */ const String& path,
    /* [in] */ const String& mimeType)
{
    if (!AvailableMimeTypeForExternalSource(mimeType)) {
        Logger::E(TAG, "Illegal mimeType for timed text source: %s", mimeType.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IFile> file;
    CFile::New(path, (IFile**)&file);
    Boolean exists;
    if (file->Exists(&exists), exists) {
        AutoPtr<IFileInputStream> is;
        CFileInputStream::New(file, (IFileInputStream**)&is);

        AutoPtr<IFileDescriptor> fd;
        is->GetFD((IFileDescriptor**)&fd);
        ECode ec = AddTimedTextSource(fd, mimeType);
        ICloseable::Probe(is)->Close();
        return ec;
    }
    else {
        // We do not support the case where the path is not a file.
        return E_IO_EXCEPTION;
    }
}

ECode CMediaPlayer::AddTimedTextSource(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType)
{
    VALIDATE_NOT_NULL(uri);

    String scheme;
    uri->GetScheme(&scheme);
    if(scheme.IsNull() || scheme.Equals("file")) {
        String path;
        uri->GetPath(&path);
        return AddTimedTextSource(path, mimeType);
    }

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IAssetFileDescriptor> fd;
    resolver->OpenAssetFileDescriptor(uri, String("r"), (IAssetFileDescriptor**)&fd);
    if (fd == NULL) {
        return NOERROR;
    }

    AutoPtr<IFileDescriptor> des;
    fd->GetFileDescriptor((IFileDescriptor**)&des);
    ECode ec = AddTimedTextSource(des, mimeType);
    fd->Close();
    return ec;
    //} catch (SecurityException ex) {
    //} catch (IOException ex) {
    //} finally {
    //    if (fd != NULL) {
    //        fd->Close();
    //    }
    //}
}

ECode CMediaPlayer::AddTimedTextSource(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const String& mimeType)
{
    // intentionally less than LONG_MAX
    return AddTimedTextSource(fd, 0, 0x7ffffffffffffffL, mimeType);
}

ECode CMediaPlayer::AddTimedTextSource(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length,
    /* [in] */ const String& mime)
{
    if (!AvailableMimeTypeForExternalSource(mime)) {
        Logger::E(TAG, "Illegal mimeType for timed text source: %s", mime.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IFileDescriptor> fd2;
    // try {
    AutoPtr<ILibcore> lc;
    CLibcore::AcquireSingleton((ILibcore**)&lc);
    AutoPtr<IOs> os;
    lc->GetOs((IOs**)&os);
    Int64 size = 0;
    os->Dup(fd, (IFileDescriptor**)&fd2);

    // } catch (ErrnoException ex) {
    //     Log.e(TAG, ex.getMessage(), ex);
    //     throw new RuntimeException(ex);
    // }

    AutoPtr<IMediaFormat> fFormat;
    CMediaFormat::New((IMediaFormat**)&fFormat);
    fFormat->SetString(IMediaFormat::KEY_MIME, mime);
    fFormat->SetInt32(IMediaFormat::KEY_IS_TIMED_TEXT, 1);

    AutoPtr<IApplication> app = CActivityThread::GetCurrentApplication();
    AutoPtr<IContext> context = IContext::Probe(app);
    // A MediaPlayer created by a VideoView should already have its mSubtitleController set.
    if (mSubtitleController == NULL) {
// TODO: Need CSubtitleController
        // CSubtitleController::New(context, mTimeProvider,
        //         THIS_PROBE(ISubtitleControllerListener),
        //         (ISubtitleController**)&mSubtitleController);

        AutoPtr<MyAnchor> anchor = new MyAnchor(this);
        mSubtitleController->SetAnchor(anchor);
    }

    Boolean b;
    mSubtitleController->HasRendererFor(fFormat, &b);
    if (!b) {
        // test and add not atomic
        AutoPtr<ISRTRenderer> srtr;
// TODO: Need CSRTRenderer
        // CSRTRenderer::New(context, mEventHandler, (ISRTRenderer**)&srtr);
        mSubtitleController->RegisterRenderer(ISubtitleControllerRenderer::Probe(srtr));
    }
    AutoPtr<ISubtitleTrack> track;
    mSubtitleController->AddTrack(fFormat, (ISubtitleTrack**)&track);
    mOutOfBandSubtitleTracks->Add(track);

    AutoPtr<IFileDescriptor> fd3 = fd2;
    Int64 offset2 = offset;
    Int64 length2 = length;
    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(String("TimedTextReadThread"),
            IProcess::THREAD_PRIORITY_BACKGROUND + IProcess::THREAD_PRIORITY_MORE_FAVORABLE,
            (IHandlerThread**)&thread);
    IThread::Probe(thread)->Start();
    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);
    AutoPtr<IHandler> handler;
    CHandler::New(looper, (IHandler**)&handler);
    AutoPtr<MyRunnable> run = new MyRunnable(this, thread, fd3, offset2, length2, track);
    return handler->Post(run, &b);
}

ECode CMediaPlayer::GetSelectedTrack(
    /* [in] */ Int32 trackType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (trackType == IMediaPlayerTrackInfo::MEDIA_TRACK_TYPE_SUBTITLE && mSubtitleController != NULL) {
        AutoPtr<ISubtitleTrack> subtitleTrack;
        mSubtitleController->GetSelectedTrack((ISubtitleTrack**)&subtitleTrack);
        if (subtitleTrack != NULL) {
            Int32 index;
            mOutOfBandSubtitleTracks->IndexOf(subtitleTrack, &index);
            if (index >= 0) {
                *result = mInbandSubtitleTracks->GetLength() + index;
                return NOERROR;
            }
        }
    }

    AutoPtr<IParcel> request;
    CCallbackParcel::New((IParcel**)&request);
    AutoPtr<IParcel> reply;
    CCallbackParcel::New((IParcel**)&reply);

    assert(0 && "TODO");
    //try {
    // request->WriteInterfaceToken(IMEDIA_PLAYER);
    // request->WriteInt32(INVOKE_ID_GET_SELECTED_TRACK);
    // request->WriteInt32(trackType);
    // Invoke(request, reply);
    // Int32 selectedTrack;
    // reply->ReadInt(&selectedTrack);
    // *result = selectedTrack;
    //} finally {
        // request->Recycle();
        // reply->Recycle();
    //}
    return NOERROR;
}

ECode CMediaPlayer::SelectTrack(
    /* [in] */ Int32 index)
{
    return SelectOrDeselectTrack(index, TRUE /* select */);
}

ECode CMediaPlayer::DeselectTrack(
    /* [in] */ Int32 index)
{
    return SelectOrDeselectTrack(index, FALSE /* select */);
}

ECode CMediaPlayer::SelectOrDeselectTrack(
    /* [in] */ Int32 index,
    /* [in] */ Boolean select)
{
    // handle subtitle track through subtitle controller
    AutoPtr<ISubtitleTrack> track;
    synchronized(mInbandSubtitleLock) {
        if (mInbandSubtitleTracks->GetLength() == 0) {
            AutoPtr<ArrayOf<IMediaPlayerTrackInfo*> > tracks;
            GetInbandTrackInfo((ArrayOf<IMediaPlayerTrackInfo*>**)&tracks);
            mInbandSubtitleTracks = ArrayOf<ISubtitleTrack*>::Alloc(tracks->GetLength());
            for (Int32 i=0; i < tracks->GetLength(); i++) {
                Int32 type;
                (*tracks)[i]->GetTrackType(&type);
                if (type == IMediaPlayerTrackInfo::MEDIA_TRACK_TYPE_SUBTITLE) {
                    AutoPtr<IMediaFormat> mf;
                    (*tracks)[i]->GetFormat((IMediaFormat**)&mf);
                    AutoPtr<ISubtitleTrack> st;
                    mSubtitleController->AddTrack(mf, (ISubtitleTrack**)&st);
                    mInbandSubtitleTracks->Set(i, st);
                }
            }
        }
    }

    Int32 size;
    mOutOfBandSubtitleTracks->GetSize(&size);
    if (index < mInbandSubtitleTracks->GetLength()) {
        track = (*mInbandSubtitleTracks)[index];
    }
    else if (index < mInbandSubtitleTracks->GetLength() + size) {
        AutoPtr<IInterface> obj;
        mOutOfBandSubtitleTracks->Get(index - mInbandSubtitleTracks->GetLength(), (IInterface**)&obj);
        track = ISubtitleTrack::Probe(obj);
    }

    if (mSubtitleController != NULL && track != NULL) {
        AutoPtr<ISubtitleTrack> st;
        mSubtitleController->GetSelectedTrack((ISubtitleTrack**)&st);
        if (select) {
            Boolean b;
            if (track->IsTimedText(&b), b) {
                Int32 ttIndex;
                GetSelectedTrack(IMediaPlayerTrackInfo::MEDIA_TRACK_TYPE_TIMEDTEXT, &ttIndex);
                if (ttIndex >= 0 && ttIndex < mInbandSubtitleTracks->GetLength()) {
                    // deselect inband counterpart
                    SelectOrDeselectInbandTrack(ttIndex, FALSE);
                }
            }
            mSubtitleController->SelectTrack(track, &b);
        }
        else if (st == track) {
            Boolean b;
            mSubtitleController->SelectTrack(NULL, &b);
        }
        else {
            Logger::W(TAG, "trying to deselect track that was not selected");
        }
        return NOERROR;
    }

    return SelectOrDeselectInbandTrack(index, select);
}

ECode CMediaPlayer::SelectOrDeselectInbandTrack(
    /* [in] */ Int32 index,
    /* [in] */ Boolean select)
{
    assert(0 && "TODO");
    /*
    AutoPtr<IParcel> request = Parcel->Obtain();
    AutoPtr<IParcel> reply = Parcel->Obtain();
    //try {
        request->WriteInterfaceToken(IMEDIA_PLAYER);
        request->WriteInt(select? INVOKE_ID_SELECT_TRACK: INVOKE_ID_DESELECT_TRACK);
        request->WriteInt(index);
        Invoke(request, reply);
    //} finally {
        request->Recycle();
        reply->Recycle();
    //}
    */
   return NOERROR;
}

ECode CMediaPlayer::NativePullBatteryData(
    /* [in] */ IParcel* reply,
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    /*
    android::sp<IBinder> binder = defaultServiceManager()->getService(String16("media.player"));
    android::sp<IMediaPlayerService> service = interface_cast<IMediaPlayerService>(binder);
    if (service->Get() == NULL) {
        //jniThrowException(env, "java/lang/RuntimeException", "cannot get MediaPlayerService");
        return UNKNOWN_ERROR;
    }

    AutoPtr<IParcel> mReply = reply;
    */
    // return service->pullBatteryData(mReply);
    return E_NOT_IMPLEMENTED;
}

ECode CMediaPlayer::SetRetransmitEndpoint(
    /* [in] */ IInetSocketAddress* endpoint)
{
    String addrString;
    Int32 port = 0;
    if (NULL != endpoint) {
        AutoPtr<IInetAddress> address;
        endpoint->GetAddress((IInetAddress**)&address);
        address->GetHostAddress(&addrString);
        endpoint->GetPort(&port);
    }

    return NativeSetRetransmitEndpoint(addrString, port);
}

ECode CMediaPlayer::NativeSetRetransmitEndpoint(
    /* [in] */ const String& addrString,
    /* [in] */ Int32 port)
{
    android::sp<android::MediaPlayer> mp = getMediaPlayer(this);
    if (mp == NULL ) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    const char *cAddrString = NULL;

    if (!addrString.IsNull()) {
        cAddrString = addrString.string();
    }

    if (LOGV) Logger::V(TAG, "setRetransmitEndpoint: %s:%d",
            cAddrString ? cAddrString : "(NULL)", port);

    android::status_t ret;
    if (cAddrString && (port > 0xFFFF)) {
        ret = android::BAD_VALUE;
    }
    else {
        ret = mp->setRetransmitEndpoint(cAddrString,
                static_cast<uint16_t>(port));
    }

    return (ret == android::OK) ? NOERROR : E_FAIL;
}

ECode CMediaPlayer::GetMediaTimeProvider(
    /* [out] */ IMediaTimeProvider** result)
{
    VALIDATE_NOT_NULL(result)
    if (mTimeProvider == NULL) {
        mTimeProvider = new TimeProvider(this);
    }
    *result = IMediaTimeProvider::Probe(mTimeProvider);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaPlayer::NativeFinalize()
{
    if (LOGV) Logger::V(TAG, "native_finalize");
    return NativeRelease();
}

ECode CMediaPlayer::PostEventFromNative(
    /* [in] */ IWeakReference* mediaplayer_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    if (LOGV) {
        Logger::V(TAG, "PostEventFromNative: what=%d, arg1=%d arg2=%d.", what, arg1, arg2);
    }

    AutoPtr<IMediaPlayer> strObj;
    mediaplayer_ref->Resolve(EIID_IMediaPlayer, (IInterface**)&strObj);
    if (strObj == NULL) {
        if (LOGV) {
            Logger::V(TAG, "PostEventFromNative: CMediaPlayer already destroyed.");
        }
        return NOERROR;
    }

    CMediaPlayer* mp = (CMediaPlayer*)strObj.Get();
    if (what == MEDIA_INFO && arg1 == MEDIA_INFO_STARTED_AS_NEXT) {
        // this acquires the wakelock if needed, and sets the client side state
        mp->Start();
    }

    if (mp->mEventHandler != NULL) {
        AutoPtr<IMessage> message;
        mp->mEventHandler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&message);
        Boolean bval;
        return mp->mEventHandler->SendMessage(message, &bval);
    }

    return NOERROR;
}

ECode CMediaPlayer::SetOnPreparedListener(
    /* [in] */ IMediaPlayerOnPreparedListener* listener)
{
    mOnPreparedListener = listener;
    return NOERROR;
}

ECode CMediaPlayer::SetOnCompletionListener(
    /* [in] */ IMediaPlayerOnCompletionListener* listener)
{
    mOnCompletionListener = listener;
    return NOERROR;
}

ECode CMediaPlayer::SetOnBufferingUpdateListener(
    /* [in] */ IMediaPlayerOnBufferingUpdateListener* listener)
{
    mOnBufferingUpdateListener = listener;
    return NOERROR;
}

ECode CMediaPlayer::SetOnSeekCompleteListener(
    /* [in] */ IMediaPlayerOnSeekCompleteListener* listener)
{
    mOnSeekCompleteListener = listener;
    return NOERROR;
}

ECode CMediaPlayer::SetOnVideoSizeChangedListener(
    /* [in] */ IMediaPlayerOnVideoSizeChangedListener* listener)
{
    mOnVideoSizeChangedListener = listener;
    return NOERROR;
}

ECode CMediaPlayer::SetOnTimedTextListener(
    /* [in] */ IMediaPlayerOnTimedTextListener* listener)
{
    mOnTimedTextListener = listener;
    return NOERROR;
}

ECode CMediaPlayer::SetOnSubtitleDataListener(
    /* [in] */ IMediaPlayerOnSubtitleDataListener* listener)
{
    mOnSubtitleDataListener = listener;
    return NOERROR;
}

ECode CMediaPlayer::SetOnErrorListener(
    /* [in] */ IMediaPlayerOnErrorListener* listener)
{
    mOnErrorListener = listener;
    return NOERROR;
}

ECode CMediaPlayer::SetOnInfoListener(
    /* [in] */ IMediaPlayerOnInfoListener* listener)
{
    mOnInfoListener = listener;
    return NOERROR;
}

Boolean CMediaPlayer::IsVideoScalingModeSupported(
    /* [in] */ Int32 mode)
{
    return (mode == VIDEO_SCALING_MODE_SCALE_TO_FIT ||
            mode == VIDEO_SCALING_MODE_SCALE_TO_FIT_WITH_CROPPING);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
