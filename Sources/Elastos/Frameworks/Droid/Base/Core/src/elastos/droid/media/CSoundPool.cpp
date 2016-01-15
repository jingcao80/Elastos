
#include "elastos/droid/media/CSoundPool.h"
#include "elastos/droid/os/CParcelFileDescriptorHelper.h"
#include "elastos/droid/os/CLooperHelper.h"
#include <media/SoundPool.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Media {

const String CSoundPool::TAG("SoundPool");
const Boolean CSoundPool::DEBUG = FALSE;

//----------------------------
//    CSoundPool::EventHandler
//----------------------------

CSoundPool::EventHandler::EventHandler(
    /* [in] */ CSoundPool* soundPool,
    /* [in] */ ILooper* looper)
    : HandlerBase(looper)
    , mSoundPool(soundPool)
{
}

ECode CSoundPool::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch(what) {
        case SAMPLE_LOADED:
        {
            if (CSoundPool::DEBUG) {
                Logger::D(TAG, "Sample %d loaded", arg1);
            }

            {
                AutoLock lock(mSoundPool->mLock);
                if (mSoundPool->mOnLoadCompleteListener != NULL) {
                    mSoundPool->mOnLoadCompleteListener->OnLoadComplete(
                        (ISoundPool*)mSoundPool->Probe(EIID_ISoundPool), arg1, arg2);
                }
            }
            break;
        }

        default:
        {
            Logger::E(TAG, "Unknown message type %d", what);
            break;
        }
    }
    return NOERROR;
}

//----------------------------

CSoundPool::CSoundPool()
    : mNativeContext(0)
{}

CSoundPool::~CSoundPool()
{
    Finalize();
}

ECode CSoundPool::constructor(
    /* [in] */ Int32 maxStreams,
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 srcQuality)
{
    // do native setup
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    if (NativeSetup(wr.Get(), maxStreams, streamType, srcQuality) != 0) {
        Logger::E(TAG, "Native setup failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CSoundPool::Load(
    /* [in] */ const String& path,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) Logger::D(TAG, "Load priority %d, %s", priority, path.string());

    // pass network streams to player
    if (path.StartWith("http:")) {
        return NativeLoad(path, priority, result);
    }

    // try local path
    ECode ec = NOERROR;
    Boolean fileExists;
    AutoPtr<IParcelFileDescriptor> fd;
    AutoPtr<IFileDescriptor> fileDescriptor;
    Int64 tempValue;

    AutoPtr<IParcelFileDescriptorHelper> parcelFileDescriptorHelper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&parcelFileDescriptorHelper);

    AutoPtr<IFile> f;
    ec = CFile::New(path, (IFile**)&f);
    FAIL_GOTO(ec, _EXIT_);

    f->Exists(&fileExists);
    if (!fileExists) {
        Logger::E(TAG, "error loading %s, file does not exists.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    ec = parcelFileDescriptorHelper->Open(f, IParcelFileDescriptor::MODE_READ_ONLY, (IParcelFileDescriptor**)&fd);
    FAIL_GOTO(ec, _EXIT_);
    if (fd != NULL) {
        ec = fd->GetFileDescriptor((IFileDescriptor**)&fileDescriptor);
        FAIL_GOTO(ec, _EXIT_);

        f->GetLength(&tempValue);
        ec =NativeLoad(fileDescriptor, 0, tempValue, priority, result);
    }

_EXIT_:
    if (fd) {
        fd->Close();
        fd = NULL;
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "error loading %s", path.string());
    }
    return ec;
}

ECode CSoundPool::Load(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IAssetFileDescriptor> afd;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->OpenRawResourceFd(resId, (IAssetFileDescriptor**)&afd);
    Int32 id = 0;
    ECode ec = NOERROR;
    if (afd != NULL) {
        AutoPtr<IFileDescriptor> fileDescriptor;
        afd->GetFileDescriptor((IFileDescriptor**)&fileDescriptor);
        Int64 offset, length;
        afd->GetStartOffset(&offset);
        afd->GetLength(&length);
        ec = NativeLoad(fileDescriptor, offset, length, priority, &id);
        afd->Close();
    }

    *result = id;
    if (FAILED(ec)) {
        Logger::E(TAG, "error loading resId %08x", resId);
    }
    return ec;
}

ECode CSoundPool::Load(
    /* [in] */ IAssetFileDescriptor* afd,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (afd != NULL) {
        Int64 length, offset;
        afd->GetLength(&length);
        if (length < 0) {
            Logger::E(TAG, "no length for fd");
            return E_RUNTIME_EXCEPTION;
        }

        AutoPtr<IFileDescriptor> fileDescriptor;
        afd->GetFileDescriptor((IFileDescriptor**)&fileDescriptor);
        afd->GetStartOffset(&offset);

        return NativeLoad(fileDescriptor, offset, length, priority, result);
    }
    return NOERROR;
}

ECode CSoundPool::Load(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return NativeLoad(fd, offset, length, priority, result);
}

//-----------------------------------------
// native method in android_media_SoundPool
//-----------------------------------------

ECode CSoundPool::Unload(
    /* [in] */ Int32 soundID,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    *result = ap->unload(soundID);
    return NOERROR;
}

ECode CSoundPool::Play(
    /* [in] */ Int32 soundID,
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume,
    /* [in] */ Int32 priority,
    /* [in] */ Int32 loop,
    /* [in] */ Float rate,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }

    if (DEBUG) Logger::D(TAG, "play soundID %d, leftVolume %f, rightVolume %f, loop %d",
        soundID, leftVolume, rightVolume, loop);
    *result = ap->play(soundID, leftVolume, rightVolume, priority, loop, rate);
    return NOERROR;
}

ECode CSoundPool::Pause(
    /* [in] */ Int32 streamID)
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->pause(streamID);
    return NOERROR;
}

ECode CSoundPool::Resume(
    /* [in] */ Int32 streamID)
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->resume(streamID);
    return NOERROR;
}

ECode CSoundPool::AutoPause()
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->autoPause();
    return NOERROR;
}

ECode CSoundPool::AutoResume()
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->autoResume();
    return NOERROR;
}

ECode CSoundPool::Stop(
    /* [in] */ Int32 streamID)
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->stop(streamID);
    return NOERROR;
}

ECode CSoundPool::SetVolume(
    /* [in] */ Int32 streamID,
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume)
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->setVolume(streamID, leftVolume, rightVolume);
    return NOERROR;
}

ECode CSoundPool::SetPriority(
    /* [in] */ Int32 streamID,
    /* [in] */ Int32 priority)
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->setPriority(streamID, priority);
    return NOERROR;
}

ECode CSoundPool::SetLoop(
    /* [in] */ Int32 streamID,
    /* [in] */ Int32 loop)
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->setLoop(streamID, loop);
    return NOERROR;
}

ECode CSoundPool::SetRate(
    /* [in] */ Int32 streamID,
    /* [in] */ Float rate)
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->setRate(streamID, rate);
    return NOERROR;
}

//-----------------------------------------

ECode CSoundPool::SetOnLoadCompleteListener(
    /* [in] */ IOnLoadCompleteListener* listener)
{
    AutoLock lock(mLock);

    mEventHandler = NULL;
    if (listener != NULL) {
        // setup message handler
        AutoPtr<ILooper> looper;
        AutoPtr<ILooperHelper> looperHelper;
        CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
        looperHelper->MyLooper((ILooper**)&looper);
        if (looper != NULL) {
            mEventHandler = new EventHandler(this, looper);
        }
        else if ((looperHelper->GetMainLooper((ILooper**)&looper), looper) != NULL) {
            mEventHandler = new EventHandler(this, looper);
        }
    }

    mOnLoadCompleteListener = listener;

    return NOERROR;
}

/*native method*/
ECode CSoundPool::ReleaseResources()
{
    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap != NULL) {
        // release weak reference
        IWeakReference* weakRef = (IWeakReference*)ap->getUserData();
        weakRef->Release();

        // clear callback and native context
        ap->setCallback(NULL, NULL);
        delete ap;
        mNativeContext = 0;
    }
    return NOERROR;
}

void CSoundPool::Finalize()
{
    ReleaseResources();
}

/*native method*/
ECode CSoundPool::NativeLoad(
    /* [in] */ const String& path,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;

    if (path.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }

    *id = ap->load(path.string(), priority);
    return NOERROR;
}

/*native method*/
ECode CSoundPool::NativeLoad(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;

    android::SoundPool *ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }

    Int32 tempValue;
    fd->GetDescriptor(&tempValue);
    *id = ap->load(tempValue, offset, length, priority);
    return NOERROR;
}

/*static*/
ECode CSoundPool::PostEventFromNative(
    /* [in] */ IWeakReference* weakRef,
    /* [in] */ Int32 msg,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<ISoundPool> pool;
    weakRef->Resolve(EIID_ISoundPool, (IInterface**)&pool);
    if (pool == NULL) {
        return NOERROR;
    }

    CSoundPool* soundPool = (CSoundPool*)pool.Get();
    if (soundPool->mEventHandler != NULL) {
        AutoPtr<IMessage> message;
        soundPool->mEventHandler->ObtainMessage(msg, arg1, arg2, obj, (IMessage**)&message);
        Boolean bval;
        soundPool->mEventHandler->SendMessage(message, &bval);
    }

    return NOERROR;
}

//-------------------------------------
static void android_media_callback(
    android::SoundPoolEvent event,
    android::SoundPool* soundPool,
    void* user)
{
    if (CSoundPool::DEBUG) {
        Logger::D("SoundPool::media_callback", "callback: (%d, %d, %d, %p, %p)",
            event.mMsg, event.mArg1, event.mArg2, soundPool, user);
    }

    CSoundPool::PostEventFromNative((IWeakReference*)user, event.mMsg, event.mArg1, event.mArg2, NULL);
}

/*native method*/
Int32 CSoundPool::NativeSetup(
    /* [in] */ IWeakReference* weakRef,
    /* [in] */ Int32 maxStreams,
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 srcQuality)
{
    android::SoundPool *ap = new android::SoundPool(maxStreams, (audio_stream_type_t) streamType, srcQuality);
    if (ap == NULL) {
        return -1;
    }

    mNativeContext = (Handle32)ap;
    weakRef->AddRef();
    ap->setCallback(android_media_callback, (void*)weakRef);
    return 0;
}

//-------------------------------------

} // namespace Media
} // namepsace Droid
} // namespace Elastos
