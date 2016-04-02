#include "elastos/droid/systemui/media/NotificationPlayer.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Looper.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioAttributesHelper;
using Elastos::Droid::Media::CMediaPlayer;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesHelper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::StringBuilder;
using Elastos::Core::ThreadState;
using Elastos::Core::ThreadState_TERMINATED;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

const Int32 NotificationPlayer::PLAY = 1;
const Int32 NotificationPlayer::STOP = 2;
const Boolean NotificationPlayer::mDebug = FALSE;

//-------------------------------------------
//    NotificationPlayer::Command
//-------------------------------------------
NotificationPlayer::Command::Command()
    : mCode(0)
    , mLooping(FALSE)
    , mRequestTime(0)
{}

ECode NotificationPlayer::Command::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("{ code=");
    sb += mCode;
    sb += " looping=";
    sb += mLooping;
    sb += " attributes=";
    sb += mAttributes;
    sb += " uri=";
    sb += mUri;
    *str = sb.ToString();
    return NOERROR;
}

//-------------------------------------------
//    NotificationPlayer::CreationAndCompletionThread
//-------------------------------------------

NotificationPlayer::CreationAndCompletionThread::CreationAndCompletionThread(
    /* [in] */ Command* cmd,
    /* [in] */ NotificationPlayer* host)
    : Thread()
    , mCmd(cmd)
    , mHost(host)
{}

ECode NotificationPlayer::CreationAndCompletionThread::Run()
{
    Looper::Prepare();
    mHost->mLooper = Looper::GetMyLooper();
    synchronized(this) {
        AutoPtr<IInterface> amObj;
        mCmd->mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&amObj);
        AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(amObj);
        // try {
        AutoPtr<IMediaPlayer> player;
        CMediaPlayer::New((IMediaPlayer**)&player);
        player->SetAudioAttributes(mCmd->mAttributes);
        player->SetDataSource(mCmd->mContext, mCmd->mUri);
        player->SetLooping(mCmd->mLooping);
        player->Prepare();
        String ep;
        mCmd->mUri->GetEncodedPath(&ep);
        if ((mCmd->mUri != NULL) && (!ep.IsNull()) && (ep.GetLength() > 0)) {
            Boolean isMusicActiveRemotely;
            audioManager->IsMusicActiveRemotely(&isMusicActiveRemotely);
            if (!isMusicActiveRemotely) {
                synchronized(mHost->mQueueAudioFocusLock) {
                    if (mHost->mAudioManagerWithAudioFocus == NULL) {
                        if (mDebug) Logger::D(mHost->mTag, "requesting AudioFocus");
                        AutoPtr<IAudioAttributesHelper> aah;
                        CAudioAttributesHelper::AcquireSingleton((IAudioAttributesHelper**)&aah);
                        Int32 lstype;
                        aah->ToLegacyStreamType(mCmd->mAttributes, &lstype);
                        Int32 raf;
                        if (mCmd->mLooping) {
                            audioManager->RequestAudioFocus(NULL, lstype,
                                IAudioManager::AUDIOFOCUS_GAIN, &raf);
                        }
                        else {
                            audioManager->RequestAudioFocus(NULL, lstype,
                                IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK,
                                &raf);
                        }
                        mHost->mAudioManagerWithAudioFocus = audioManager;
                    }
                    else {
                        if (mDebug) Logger::D(mHost->mTag, "AudioFocus was previously requested");
                    }
                }
            }
        }
        // FIXME Having to start a new thread so we can receive completion callbacks
        //  is wrong, as we kill this thread whenever a new sound is to be played. This
        //  can lead to AudioFocus being released too early, before the second sound is
        //  done playing. This class should be modified to use a single thread, on which
        //  command are issued, and on which it receives the completion callbacks.
        player->SetOnCompletionListener(mHost);
        player->Start();
        if (mHost->mPlayer != NULL) {
            mHost->mPlayer->ReleaseResources();
        }
        mHost->mPlayer = player;
        // }
        // catch (Exception e) {
        //     Log.w(mTag, "error loading sound for " + mCmd->mUri, e);
        // }
        this->Notify();
    }
    Looper::Loop();
    return NOERROR;
}

//-------------------------------------------
//    NotificationPlayer::CmdThread
//-------------------------------------------

NotificationPlayer::CmdThread::CmdThread(
    /* [in] */ NotificationPlayer* host)
    : mHost(host)
{
    Thread::constructor(String("NotificationPlayer-") + mHost->mTag);
}

ECode NotificationPlayer::CmdThread::Run()
{
    while (TRUE) {
        AutoPtr<Command> cmd;
        synchronized(mHost->mCmdQueue) {
            if (mHost->mDebug) Logger::D(mHost->mTag, "RemoveFirst");
            AutoPtr<IInterface> obj;
            mHost->mCmdQueue->RemoveFirst((IInterface**)&obj);
            cmd = (Command*)(IObject::Probe(obj));
        }

        switch (cmd->mCode) {
        case PLAY:
            if (mHost->mDebug) Logger::D(mHost->mTag, "PLAY");
            mHost->StartSound(cmd);
            break;
        case STOP:
            if (mHost->mDebug) Logger::D(mHost->mTag, "STOP");
            if (mHost->mPlayer != NULL) {
                Int64 delay = SystemClock::GetUptimeMillis() - cmd->mRequestTime;
                if (delay > 1000) {
                    Logger::W(mHost->mTag, "Notification stop delayed by %ldmsecs", delay);
                }
                mHost->mPlayer->Stop();
                mHost->mPlayer->Release();
                mHost->mPlayer = NULL;
                synchronized(mHost->mQueueAudioFocusLock) {
                    if (mHost->mAudioManagerWithAudioFocus != NULL) {
                        Int32 result;
                        mHost->mAudioManagerWithAudioFocus->AbandonAudioFocus(NULL, &result);
                        mHost->mAudioManagerWithAudioFocus = NULL;
                    }
                }
                if(mHost->mLooper != NULL) {
                    AutoPtr<IThread> td;
                    mHost->mLooper->GetThread((IThread**)&td);
                    ThreadState state;
                    td->GetState(&state);
                    if(state != ThreadState_TERMINATED) {
                        mHost->mLooper->Quit();
                    }
                }
            } else {
                Logger::W(mHost->mTag, "STOP command without a player");
            }
            break;
        }
        synchronized (mHost->mCmdQueue) {
            Int32 size;
            mHost->mCmdQueue->GetSize(&size);
            if (size == 0) {
                // nothing left to do, quit
                // doing this check after we're done prevents the case where they
                // added it during the operation from spawning two threads and
                // trying to do them in parallel.
                mHost->mThread = NULL;
                mHost->ReleaseWakeLock();
                return E_NULL_POINTER_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

//-------------------------------------------
//    NotificationPlayer
//-------------------------------------------

CAR_INTERFACE_IMPL(NotificationPlayer, Object, IMediaPlayerOnCompletionListener)

NotificationPlayer::NotificationPlayer(
    /* [in] */ const String& tag)
    : mState(STOP)
{
    CLinkedList::New((ILinkedList**)&mCmdQueue);
    if (tag != NULL) {
        mTag = tag;
    } else {
        mTag = String("NotificationPlayer");
    }
}

void NotificationPlayer::StartSound(
    /* [in] */ Command* cmd)
{
    // Preparing can be slow, so if there is something else
    // is playing, let it continue until we're done, so there
    // is less of a glitch.
    // try {
    if (mDebug) Logger::D(mTag, "Starting playback");
    //-----------------------------------
    // This is were we deviate from the AsyncPlayer implementation and create the
    // MediaPlayer in a new thread with which we're synchronized
    synchronized(mCompletionHandlingLock) {
        // if another sound was already playing, it doesn't matter we won't get notified
        // of the completion, since only the completion notification of the last sound
        // matters
        AutoPtr<IThread> thread;
        mLooper->GetThread((IThread**)&thread);
        ThreadState state;
        thread->GetState(&state);

        if((mLooper != NULL) && state != ThreadState_TERMINATED) {
            mLooper->Quit();
        }
        mCompletionThread = new CreationAndCompletionThread(cmd, this);
        synchronized(mCompletionThread) {
            mCompletionThread->Start();
            mCompletionThread->Wait();
        }
    }
    //-----------------------------------

    Int64 delay = SystemClock::GetUptimeMillis() - cmd->mRequestTime;
    if (delay > 1000) {
        Logger::W(mTag, "Notification sound delayed by %ldmsecs", delay);
    }
    // }
    // catch (Exception e) {
        // Log.w(mTag, "error loading sound for " + cmd.uri, e);
    // }
}

ECode NotificationPlayer::OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
    synchronized(mQueueAudioFocusLock) {
        if (mAudioManagerWithAudioFocus != NULL) {
            if (mDebug) Logger::D(mTag, "onCompletion() abandonning AudioFocus");
            Int32 aaf;
            mAudioManagerWithAudioFocus->AbandonAudioFocus(NULL, &aaf);
            mAudioManagerWithAudioFocus = NULL;
        }
        else {
            if (mDebug) Logger::D(mTag, "onCompletion() no need to abandon AudioFocus");
        }
    }
    // if there are no more sounds to play, end the Looper to listen for media completion
    synchronized (mCmdQueue) {
        Int32 size;
        mCmdQueue->GetSize(&size);
        if (size == 0) {
            synchronized(mCompletionHandlingLock) {
                if(mLooper != NULL) {
                    mLooper->Quit();
                }
                mCompletionThread = NULL;
            }
        }
    }
    return NOERROR;
}

ECode NotificationPlayer::Play(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ Boolean looping,
    /* [in] */ Int32 stream)
{
    AutoPtr<Command> cmd = new Command();
    cmd->mRequestTime = SystemClock::GetUptimeMillis();
    cmd->mCode = PLAY;
    cmd->mContext = context;
    cmd->mUri = uri;
    cmd->mLooping = looping;
    AutoPtr<IAudioAttributesBuilder> aab;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aab);
    aab->SetInternalLegacyStreamType(stream);
    AutoPtr<IAudioAttributes> aa;
    aab->Build((IAudioAttributes**)&aa);
    cmd->mAttributes = aa;
    synchronized(mCmdQueue) {
        EnqueueLocked(cmd);
        mState = PLAY;
    }
    return NOERROR;
}

ECode NotificationPlayer::Play(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ Boolean looping,
    /* [in] */ IAudioAttributes* attributes)
{
    AutoPtr<Command> cmd = new Command();
    cmd->mRequestTime = SystemClock::GetUptimeMillis();
    cmd->mCode = PLAY;
    cmd->mContext = context;
    cmd->mUri = uri;
    cmd->mLooping = looping;
    cmd->mAttributes = attributes;
    synchronized (mCmdQueue) {
        EnqueueLocked(cmd);
        mState = PLAY;
    }
    return NOERROR;
}

ECode NotificationPlayer::Stop()
{
    synchronized (mCmdQueue) {
        // This check allows stop to be called multiple times without starting
        // a thread that ends up doing nothing.
        if (mState != STOP) {
            AutoPtr<Command> cmd = new Command();
            cmd->mRequestTime = SystemClock::GetUptimeMillis();
            cmd->mCode = STOP;
            EnqueueLocked(cmd);
            mState = STOP;
        }
    }
    return NOERROR;
}

void NotificationPlayer::EnqueueLocked(
    /* [in] */ Command* cmd)
{
    mCmdQueue->Add((IObject*)cmd);
    if (mThread == NULL) {
        AcquireWakeLock();
        mThread = new CmdThread(this);
        mThread->Start();
    }
}

ECode NotificationPlayer::SetUsesWakeLock(
    /* [in] */ IContext* context)
{
    if (mWakeLock != NULL || mThread != NULL) {
        // if either of these has happened, we've already played something.
        // and our releases will be out of sync.
        // throw new RuntimeException("assertion failed mWakeLock=" + mWakeLock
        //         + " mThread=" + mThread);
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IInterface> pmObj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pmObj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(pmObj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, mTag, (IPowerManagerWakeLock**)&mWakeLock);
    return NOERROR;
}

void NotificationPlayer::AcquireWakeLock()
{
if (mWakeLock != NULL) {
        mWakeLock->AcquireLock();
    }
}

void NotificationPlayer::ReleaseWakeLock()
{
    if (mWakeLock != NULL) {
        mWakeLock->ReleaseLock();
    }
}

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos