#include "elastos/droid/systemui/media/CNotificationPlayer.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/media/CMediaPlayer.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Media::CMediaPlayer;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

const Int32 CNotificationPlayer::PLAY = 1;
const Int32 CNotificationPlayer::STOP = 2;
const Boolean CNotificationPlayer::mDebug = FALSE;

//-------------------------------------------
//    CNotificationPlayer::Command
//-------------------------------------------

String CNotificationPlayer::Command::ToString()
{
    String info("{ code=");
    info.AppendFormat("{ code=%d looping=%d stream=%d }", mCode, mLooping, mStream);
    return info;
}

//-------------------------------------------
//    CNotificationPlayer::CreationAndCompletionThread
//-------------------------------------------

CNotificationPlayer::CreationAndCompletionThread::CreationAndCompletionThread(
    /* [in] */ Command* cmd,
    /* [in] */ CNotificationPlayer* host)
    : mCmd(cmd)
    , mHost(host)
{
    Init();
}

ECode CNotificationPlayer::CreationAndCompletionThread::Run()
{
    Looper::Prepare();
    mHost->mLooper = Looper::GetMyLooper();
    {
        AutoLock lock(mLock);
        AutoPtr<IAudioManager> audioManager;
        mCmd->mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioManager);
        // try {
            AutoPtr<IMediaPlayer> player;
            CMediaPlayer::New((IMediaPlayer**)&player);
            player->SetAudioStreamType(mCmd->mStream);
            player->SetDataSource(mCmd->mContext, mCmd->mUri);
            player->SetLooping(mCmd->mLooping);
            player->Prepare();
            if (mCmd->mUri != NULL) {
                String path;
                mCmd->mUri->GetEncodedPath(&path);
                 if (!path.IsNullOrEmpty()) {
                    Int32 result;
                    if (mCmd->mLooping) {
                        audioManager->RequestAudioFocus(NULL, mCmd->mStream,
                                IAudioManager::AUDIOFOCUS_GAIN, &result);
                    } else {
                        audioManager->RequestAudioFocus(NULL, mCmd->mStream,
                                IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK, &result);
                    }
                }
            }
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
        mHost->mAudioManager = audioManager;
        this->Notify();
    }
    Looper::Loop();
    return NOERROR;
}

//-------------------------------------------
//    CNotificationPlayer::CmdThread
//-------------------------------------------

CNotificationPlayer::CmdThread::CmdThread(
    /* [in] */ CNotificationPlayer* host)
    : mHost(host)
{
    Init(String("NotificationPlayer-") + mHost->mTag);
}

ECode CNotificationPlayer::CmdThread::Run()
{
    while (TRUE) {
        AutoPtr<Command> cmd = NULL;
        List< AutoPtr<Command> >::Iterator it;
        {
            AutoLock lock(mHost->mCmdQueueLock);
            if (mDebug) Logger::D(mHost->mTag, "RemoveFirst");
            it = mHost->mCmdQueue.Begin();
            cmd = *it;
        }

        switch (cmd->mCode) {
        case PLAY:
            if (mDebug) Logger::D(mHost->mTag, "PLAY");
            mHost->StartSound(cmd);
            break;
        case STOP:
            if (mDebug) Logger::D(mHost->mTag, "STOP");
            if (mHost->mPlayer != NULL) {
                Int64 delay = SystemClock::GetUptimeMillis() - cmd->mRequestTime;
                if (delay > 1000) {
                    Logger::W(mHost->mTag, "Notification stop delayed by %ld msecs",delay);
                }
                mHost->mPlayer->Stop();
                mHost->mPlayer->Release();
                mHost->mPlayer = NULL;
                Int32 result;
                mHost->mAudioManager->AbandonAudioFocus(NULL, &result);
                mHost->mAudioManager = NULL;

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
        mHost->mCmdQueue.Remove(*it);

        {
            AutoLock l(mHost->mCmdQueueLock);
            if (mHost->mCmdQueue.IsEmpty()) {
                // nothing left to do, quit
                // doing this check after we're done prevents the case where they
                // added it during the operation from spawning two threads and
                // trying to do them in parallel.
                mHost->mThread = NULL;
                mHost->ReleaseWakeLock();
            }
        }
    }
    return NOERROR;
}

//-------------------------------------------
//    CNotificationPlayer
//-------------------------------------------

CNotificationPlayer::CNotificationPlayer()
    : mState(STOP)
{}

ECode CNotificationPlayer::constructor(
    /* [in] */ const String& tag)
{
    if (tag != NULL) {
        mTag = tag;
    } else {
        mTag = String("NotificationPlayer");
    }
    return NOERROR;
}

ECode CNotificationPlayer::OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
    if (mAudioManager != NULL) {
        Int32 result;
        mAudioManager->AbandonAudioFocus(NULL, &result);
    }
    // if there are no more sounds to play, end the Looper to listen for media completion
    {
        AutoLock lock(mCmdQueueLock);
        if (mCmdQueue.IsEmpty()) {
            {
                AutoLock l(mCompletionHandlingLock);
                if(mLooper != NULL) {
                    mLooper->Quit();
                }
                mCompletionThread = NULL;
            }
        }
    }
    return NOERROR;
}

ECode CNotificationPlayer::Play(
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
    cmd->mStream = stream;
    {
        AutoLock lock(mCmdQueueLock);
        EnqueueLocked(cmd);
        mState = PLAY;
    }
    return NOERROR;
}

ECode CNotificationPlayer::Stop()
{
    {
        AutoLock lock(mCmdQueueLock);
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

ECode CNotificationPlayer::SetUsesWakeLock(
    /* [in] */ IContext* context)
{
    if (mWakeLock != NULL || mThread != NULL) {
        // if either of these has happened, we've already played something.
        // and our releases will be out of sync.
        // throw new RuntimeException("assertion failed mWakeLock=" + mWakeLock
        //         + " mThread=" + mThread);
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IPowerManager> pm;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pm);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, mTag, (IPowerManagerWakeLock**)&mWakeLock);
    return NOERROR;
}

void CNotificationPlayer::EnqueueLocked(
    /* [in] */ Command* cmd)
{
    mCmdQueue.PushBack(cmd);
    if (mThread == NULL) {
        AcquireWakeLock();
        mThread = new CmdThread(this);
        mThread->Start();
    }
}

void CNotificationPlayer::AcquireWakeLock()
{
if (mWakeLock != NULL) {
        mWakeLock->AcquireLock();
    }
}

void CNotificationPlayer::ReleaseWakeLock()
{
    if (mWakeLock != NULL) {
        mWakeLock->ReleaseLock();
    }
}

void CNotificationPlayer::StartSound(
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
    {
        AutoLock lock(mCompletionHandlingLock);
        // if another sound was already playing, it doesn't matter we won't get notified
        // of the completion, since only the completion notification of the last sound
        // matters
        if(mLooper != NULL) {
            AutoPtr<IThread> td;
            mLooper->GetThread((IThread**)&td);
            ThreadState state;
            td->GetState(&state);
            if(state != ThreadState_TERMINATED) {
                mLooper->Quit();
            }
        }
        mCompletionThread = new CreationAndCompletionThread(cmd, this);
        {
            AutoLock l(mCompletionThreadLock);
            mCompletionThread->Start();
            mCompletionThread->Wait();
        }
    }
    //-----------------------------------

    Int64 delay = SystemClock::GetUptimeMillis() - cmd->mRequestTime;
    if (delay > 1000) {
        Logger::W(mTag, "Notification sound delayed by %ld msecs", delay);
    }
    // }
    // catch (Exception e) {
    //     Log.w(mTag, "error loading sound for " + cmd.uri, e);
    // }
}

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
