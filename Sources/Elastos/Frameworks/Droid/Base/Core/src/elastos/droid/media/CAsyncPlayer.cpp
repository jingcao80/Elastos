
#include "elastos/droid/media/CAsyncPlayer.h"
#include "elastos/droid/media/CMediaPlayer.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const Int32 CAsyncPlayer::PLAY = 1;
const Int32 CAsyncPlayer::STOP = 2;
const Boolean CAsyncPlayer::mDebug = FALSE;

//================================================================================
//                      CAsyncPlayer::Command
//================================================================================
ECode CAsyncPlayer::Command::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("{ code=");
    sb += StringUtils::ToString(mCode);
    sb += " looping=";
    sb += StringUtils::BooleanToString(mLooping);
    sb += " stream=";
    sb += StringUtils::ToString(mStream);
    sb += " uri=";
    sb += TO_CSTR(mUri);
    sb += " }";
    return sb.ToString(result);
}

//================================================================================
//                      CAsyncPlayer::MyThread
//================================================================================
CAsyncPlayer::MyThread::MyThread(
    /* [in] */ CAsyncPlayer* owner)
    : mOwner(owner)
{
    constructor(String("AsyncPlayer-") + mOwner->mTag);
}

ECode CAsyncPlayer::MyThread::Run()
{
    while (TRUE) {
        AutoPtr<Command> cmd;
        AutoPtr<ILinkedList> lock = mOwner->mCmdQueue.Get();
        {    AutoLock syncLock(lock);
            if (mOwner->mDebug) Logger::D(mOwner->mTag, "RemoveFirst");
            AutoPtr<IInterface> obj;
            mOwner->mCmdQueue->RemoveFirst((IInterface**)&obj);
            cmd = (Command*)IObject::Probe(obj);
        }

        switch (cmd->mCode) {
            case CAsyncPlayer::PLAY:
            {
                if (mOwner->mDebug) Logger::D(mOwner->mTag, "PLAY");
                mOwner->StartSound(cmd);
                break;
            }
            case CAsyncPlayer::STOP:
            {
                if (mOwner->mDebug) Logger::D(mOwner->mTag, "STOP");
                if (mOwner->mPlayer != NULL) {
                    Int64 delay = SystemClock::GetUptimeMillis() - cmd->mRequestTime;
                    if (delay > 1000) {
                        Logger::W(mOwner->mTag, String("Notification stop delayed by ") + StringUtils::ToString(delay) + String("msecs"));
                    }
                    mOwner->mPlayer->Stop();
                    mOwner->mPlayer->ReleaseResources();
                    mOwner->mPlayer = NULL;
                }
                else {
                    Logger::W(mOwner->mTag, String("STOP command without a player"));
                }
                break;
            }
        }

        {    AutoLock syncLock(lock);
            Boolean b;
            mOwner->mCmdQueue->IsEmpty(&b);
            if (b) {
                // nothing left to do, quit
                // doing this check after we're done prevents the case where they
                // added it during the operation from spawning two threads and
                // trying to do them in parallel.
                mOwner->mThread = NULL;
                mOwner->ReleaseWakeLock();
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

//================================================================================
//                      CAsyncPlayer
//================================================================================

CAR_INTERFACE_IMPL(CAsyncPlayer, Object, IAsyncPlayer)

CAR_OBJECT_IMPL(CAsyncPlayer)

CAsyncPlayer::CAsyncPlayer()
    : mState(STOP)
{
}

CAsyncPlayer::~CAsyncPlayer()
{
}

ECode CAsyncPlayer::constructor(
    /* [in] */ const String& tag)
{
    if (!tag.IsNull()) {
        mTag = tag;
    }
    else {
        mTag = "AsyncPlayer";
    }
    return NOERROR;
}

void CAsyncPlayer::StartSound(
    /* [in] */ Command* cmd)
{
    // Preparing can be slow, so if there is something else
    // is playing, let it continue until we're done, so there
    // is less of a glitch.
    ECode ec = NOERROR;
    do {
        if (mDebug) Logger::D(mTag, "Starting playback");

        AutoPtr<IMediaPlayer> player;
        CMediaPlayer::New((IMediaPlayer**)&player);
        ec = player->SetAudioStreamType(cmd->mStream);
        if (FAILED(ec)) break;

        ec = player->SetDataSource(cmd->mContext, cmd->mUri);
        if (FAILED(ec)) break;

        ec = player->SetLooping(cmd->mLooping);
        if (FAILED(ec)) break;

        ec = player->Prepare();
        if (FAILED(ec)) break;

        ec = player->Start();
        if (FAILED(ec)) break;

        if (mPlayer != NULL) {
            ec = mPlayer->ReleaseResources();
            if (FAILED(ec)) break;
        }

        mPlayer = player;
        Int64 delay = SystemClock::GetUptimeMillis() - cmd->mRequestTime;
        if (delay > 1000) {
            Logger::W(mTag, String("Notification sound delayed by ") +
                          StringUtils::ToString(delay) + String("msecs"));
        }
    } while(0);

    if (FAILED(ec)) {
        // String tempText;
        // cmd->mUri->ToString(&tempText);
        // Logger::W(mTag, String("error loading sound for ") + tempText/*, e*/);
    }
}

ECode CAsyncPlayer::Play(
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

    {    AutoLock syncLock(mCmdQueue);
        EnqueueLocked(cmd);
        mState = PLAY;
    }
    return NOERROR;
}

ECode CAsyncPlayer::Stop()
{
    {    AutoLock syncLock(mCmdQueue);

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

void CAsyncPlayer::EnqueueLocked(
    /* [in] */ Command* cmd)
{
    mCmdQueue->Add((IInterface*)(IObject*)cmd);
    if (mThread == NULL) {
        AcquireWakeLock();
        mThread = new MyThread(this);
        mThread->Start();
    }
}

ECode CAsyncPlayer::SetUsesWakeLock(
    /* [in] */ IContext* context)
{
    if (mWakeLock != NULL || mThread != NULL) {
        // if either of these has happened, we've already played something.
        // and our releases will be out of sync.
        return E_RUNTIME_EXCEPTION;
       // throw new RuntimeException("assertion failed mWakeLock=" + mWakeLock
       //         + " mThread=" + mThread);
    }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj.Get());
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, mTag, (IPowerManagerWakeLock**)&mWakeLock);
    return NOERROR;
}

void CAsyncPlayer::AcquireWakeLock()
{
    if (mWakeLock != NULL) {
        mWakeLock->AcquireLock();
    }
}

void CAsyncPlayer::ReleaseWakeLock()
{
    if (mWakeLock != NULL) {
        mWakeLock->ReleaseLock();
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
