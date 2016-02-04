
#include "elastos/droid/media/CSoundPool.h"
#include "elastos/droid/os/CParcelFileDescriptorHelper.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/SystemProperties.h"
#include "Elastos.CoreLibrary.h"
#include "Elastos.CoreLibrary.IO.h"
#include <media/SoundPool.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::StringUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CSoundPool::TAG("SoundPool");
const Boolean CSoundPool::DEBUG = FALSE;

//----------------------------
//    CSoundPool::Builder
//----------------------------
CSoundPool::Builder::Builder(
    /* [in] */ CSoundPool* host)
    : mMaxStreams(1)
    , mHost(host)
{}

CSoundPool::Builder::~Builder()
{}

CAR_INTERFACE_IMPL(Builder, Object, ISoundPoolBuilder)

ECode CSoundPool::Builder::SetMaxStreams(
    /* [in] */ Int32 maxStreams,
    /* [out] */ ISoundPoolBuilder** result)
{
    VALIDATE_NOT_NULL(result);
    if (maxStreams <= 0) {
        Slogger::E("CSoundPool::Builder", "Strictly positive value required for the maximum number of streams");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMaxStreams = maxStreams;
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundPool::Builder::SetAudioAttributes(
    /* [in] */ IAudioAttributes* attributes)
{
    if (attributes == NULL) {
        Slogger::E("CSoundPool::Builder", "Invalid null AudioAttributes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAudioAttributes =(AudioAttributes*)attributes;
}

ECode CSoundPool::Builder::Build(
    /* [out] */ ISoundPool** result)
{
    VALIDATE_NOT_NULL(result);

    if (mAudioAttributes == NULL) {
        AutoPtr<IAudioAttributesBuilder> aab;
        CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aab);
        aab->SetUsage(IAudioAttributes::USAGE_MEDIA);
        AutoPtr<IAudioAttributes> attr;
        aab->Build((IAudioAttributes**)&attr);
        mAudioAttributes = (AudioAttributes*)attr;
    }
    mHost->constructor(mMaxStreams, mAudioAttributes);
    *result = mHost;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//----------------------------
//    CSoundPool::SoundPoolImpl
//----------------------------
ECode static initLoadLibrary()
{
    AutoPtr<ISystem> s;
    CSystem::AcquireSingleton((ISystem**)&s);
    s->LoadLibrary(String("soundpool"));
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::sStaticLoadLibrary = initLoadLibrary();

String CSoundPool::SoundPoolImpl::TAG(String("SoundPool"));
Boolean CSoundPool::SoundPoolImpl::DEBUG = FALSE;
Int32 CSoundPool::SoundPoolImpl::SAMPLE_LOADED = 1;

CSoundPool::SoundPoolImpl::SoundPoolImpl(
    /* [in] */ SoundPool* proxy,
    /* [in] */ Int32 maxStreams,
    /* [in] */ AudioAttributes* attr)
{
    // do native setup
    if (Native_setup(IWeakReference::Probe(this), maxStreams, IAudioAttributes::Probe(attr)) != 0) {
        Slogger::E(TAG, "Native setup failed");
        return E_RUNTIME_EXCEPTION;
    }
    mProxy = proxy;
    mAttributes = attr;
    AutoPtr<IInterface> inter = ServiceManager::GetService(IContext::APP_OPS_SERVICE);
    AutoPtr<IBinder> b = IBinder::Probe(inter);
    mAppOps = IIAppOpsService::Probe(b);
}

CSoundPool::SoundPoolImpl::~SoundPoolImpl()
{}

CAR_INTERFACE_IMPL()

ECode CSoundPool::SoundPoolImpl::Load(
    /* [in] */ const String& path,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // pass network streams to player
    if (path.StartWith(String("http:"))) {
        *result = _Load(path, priority);
        return NOERROR;
    }
    // try local path
    Int32 id = 0;
    // try {
        AutoPtr<IFile> f;
        CFile::New(path, (IFile**)&f);
        AutoPtr<IParcelFileDescriptor> fd;
        CParcelFileDescriptor::Open(f.Get(), IParcelFileDescriptor::MODE_READ_ONLY, (IParcelFileDescriptor**)&fd);
        if (fd != NULL) {
            AutoPtr<IFileDescriptor> des;
            fd->GetFileDescriptor((IFileDescriptor**)&des);
            Int64 len;
            f->GetLength(&len);
            id = _Load(des.Get(), 0, len, priority);
            ICloseable::Probe(fd)->Close();
        }
    // } catch (java.io.IOException e) {
        // Log.e(TAG, "error loading " + path);
    // }
    *result = id;
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::Load(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IAssetFileDescriptor> afd;
    res->OpenRawResourceFd(resId, (IAssetFileDescriptor**)&afd);
    Int32 id = 0;
    if (afd != NULL) {
        AutoPtr<IFileDescriptor> fd;
        afd->GetFileDescriptor((IFileDescriptor**)&fd);
        Int64 startOffset;
        afd->GetStartOffset(&startOffset);
        Int64 length;
        afd->GetLength(&length);
        id = _Load(fd.Get(), startOffset, length, priority);
        // try {
        ICloseable::Probe(afd)->Close();
        // } catch (java.io.IOException ex) {
            //Log.d(TAG, "close failed:", ex);
        // }
    }
    *result = id;
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::Load(
    /* [in] */ IAssetFileDescriptor* afd,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (afd != NULL) {
        Int64 len;
        afd->GetLength(&len);
        if (len < 0) {
            Slogger::E(TAG, "no length for fd");
            return E_ANDROID_RUNTIME_EXCEPTION;
        }
        AutoPtr<IFileDescriptor> fd;
        afd->GetFileDescriptor((IFileDescriptor**)&fd);
        Int64 startOffset;
        afd->GetStartOffset(&startOffset);
        *result = _load(fd.Get(), startOffset, len, priority);
        return NOERROR;
    } else {
        *result = 0;
        return NOERROR;
    }
}

ECode CSoundPool::SoundPoolImpl::Load(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length,
    /* [in] */ Int64 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = _Load(fd, offset, length, priority);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::UnLoad(
    /* [in] */ Int32 soundID,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    *result = ap->unload(soundID);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::Play(
    /* [in] */ Int32 soundID,
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume,
    /* [in] */ Int32 priority,
    /* [in] */ Int32 loop,
    /* [in] */ Float rate,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (IsRestricted()) {
        leftVolume = rightVolume = 0;
    }
    return _Play(soundID, leftVolume, rightVolume, priority, loop, rate);
}

ECode CSoundPool::SoundPoolImpl::_Play(
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

    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    *result = ap->play(soundID, leftVolume, rightVolume, priority, loop, rate);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::Pause(
    /* [in] */ Int32 streamID)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->pause(streamID);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::Resume(
    /* [in] */ Int32 streamID)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->resume(streamID);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::AutoPause()
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->autoPause();
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::AutoResume()
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->autoResume();
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::Stop(
    /* [in] */ Int32 streamID)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->stop(streamID);
}

ECode CSoundPool::SoundPoolImpl::SetVolume(
    /* [in] */ Int32 streamID,
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume)
{
    if (IsRestricted()) {
        return NOERROR;
    }
    _SetVolume(streamID, leftVolume, rightVolume);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::SetVolume(
    /* [in] */ Int32 streamID,
    /* [in] */ Float volume)
{
    return SetVolume(streamID, volume, volume);
}

ECode CSoundPool::SoundPoolImpl::SetPriority(
    /* [in] */ Int32 streamID,
    /* [in] */ Int32 priority)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->setPriority(streamID, priority);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::SetLoop(
    /* [in] */ Int32 streamID,
    /* [in] */ Int32 loop)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->setLoop(streamID, loop);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::SetRate(
    /* [in] */ Int32 streamID,
    /* [in] */ Float rate)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return NOERROR;
    }
    ap->setRate(streamID, rate);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::SetOnLoadCompleteListener(
    /* [in] */ ISoundPoolOnLoadCompleteListener* listener)
{
    synchronized(mLock) {
        if (listener != NULL) {
            // setup message handler
            AutoPtr<ILooper> looper;
            Looper::GetMyLooper((ILooper**)&looper);
            if (looper != NULL) {
                mEventHandler = new EventHandler(mProxy, looper);
            } else {
                looper = NULL;
                looper = Looper::GetMainLooper();
                if (looper != NULL) {
                    mEventHandler = new EventHandler(mProxy, looper);
                } else {
                    mEventHandler = NULL;
                }
            }
        } else {
            mEventHandler = NULL;
        }
        mOnLoadCompleteListener = listener;
    }
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::ReleaseSoundPoolImpl()
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
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

Int32 CSoundPool::SoundPoolImpl::_Load(
    /* [in] */ const String& uri,
    /* [in] */ Int32 priority)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    int id = ap->load(uri.string(), priority);
    return id;
}

Int32 CSoundPool::SoundPoolImpl::_Load(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length,
    /* [in] */ Int32 priority)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return 0;
    }
    Int32 ifd;
    fd->GetInt(&ifd);
    int ret = ap->load(ifd, offset, length, priority);
    return ret;
}

Boolean CSoundPool::SoundPoolImpl::IsRestricted()
{
    // try {
        Int32 usage;
        mAttributes->GetUsage(&usage);
        Int32 myUid;
        Process::MyUid(&myUid);
        String currentPackName = CActivityThread::GetCurrentPackageName()
        Int32 mode;
        mAppOps->CheckAudioOperation(IAppOpsManager::OP_PLAY_AUDIO,
                usage, myUid, currentPackName, &mode);
        return mode != IAppOpsManager::MODE_ALLOWED;
    // } catch (RemoteException e) {
        // return false;
    // }
}

void CSoundPool::SoundPoolImpl::_SetVolume(
    /* [in] */ Int32 streamID,
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume)
{
    android::SoundPool* ap = (android::SoundPool*)mNativeContext;
    if (ap == NULL) {
        return ;
    }
    ap->setVolume(streamID, leftVolume, rightVolume);
}

// post event from native code to message handler
void CSoundPool::SoundPoolImpl::PostEventFromNative(
    /* [in] */ IInterface* weakRef,
    /* [in] */ Int32 msg,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    AutoPtr<SoundPoolImpl> soundPoolImpl
        = (SoundPoolImpl*)ISoundPoolDelegate::Probe(weakRef);
    if (soundPoolImpl == NULL)
        return;

    if (soundPoolImpl->mEventHandler != NULL) {
        AutoPtr<EventHandler> eh = soundPoolImpl->mEventHandler;
        AutoPtr<IMessage> m;
        IHandler::Probe(eh)->ObtainMessage(msg, arg1, arg2, obj, (IMessage**)&m);
        Boolean flag = FALSE;
        soundPoolImpl->mEventHandler->SendMessage(m.Get(), &flag);
    }
}

Int32 CSoundPool::SoundPoolImpl::Native_setup(
    /* [in] */ IInterface* weakRef,
    /* [in] */ Int32 maxStreams,
    /* [in] */ IInterface* attributes)
{
    audio_attributes_t
    AutoPtr<AudioAttributes> aab = (AudioAttributes*)IAudioAttributes::Probe(attributes);
    AutoPtr<ISet> iset;
    aab->GetTags((ISet**)&iset);
    AutoPtr<IHashSet> tags = IHashSet::Probe(iset);
    Int32 usage;
    aab->GetUsage(&usage);
    Int32 contentType;
    aab->GetContentType(&contentType);
    Int32 flags;
    aab->GetFlags(&flags);
    audio_attributes_t* paa = NULL;
    assert(0 && "TODO");
}

void CSoundPool::SoundPoolImpl::Finalize()
{
    ReleaseSoundPoolImpl();
}

//----------------------------
//    CSoundPool::SoundPoolStub
//----------------------------
CSoundPool::SoundPoolStub::SoundPoolStub()
{}

CSoundPool::SoundPoolStub::~SoundPoolStub()
{}

CAR_INTERFACE_IMPL(CSoundPool::SoundPoolStub, Object, ISoundPoolDelegate)

ECode CSoundPool::SoundPoolStub::Load(
    /* [in] */ const String& path,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::Load(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::Load(
    /* [in] */ IAssetFileDescriptor* afd,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::Load(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::Unload(
    /* [in] */ Int32 soundID,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::Play(
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
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::Pause(
    /* [in] */ Int32 streamID)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::Resume(
    /* [in] */ Int32 streamID)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::AutoPause()
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::AutoResume()
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::Stop(
    /* [in] */ Int32 streamID)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::SetVolume(
    /* [in] */ Int32 streamID,
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::SetVolume(
    /* [in] */ Int32 streamID,
    /* [in] */ Float volume)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::SetPriority(
    /* [in] */ Int32 streamID,
    /* [in] */ Int32 priority)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::SetLoop(
    /* [in] */ Int32 streamID,
    /* [in] */ Int32 loop)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::SetRate(
    /* [in] */ Int32 streamID,
    /* [in] */ Float rate)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::SetOnLoadCompleteListener(
    /* [in] */ ISoundPoolOnLoadCompleteListener* listener)
{
    return NOERROR;
}

ECode CSoundPool::SoundPoolStub::ReleaseSoundPoolStub()
{
    return NOERROR;
}
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
    AutoPtr<IAudioAttributesBuilder> aab;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aab);
    aab->SetInternalLegacyStreamType(streamType);
    AutoPtr<IAudioAttributes> attr;
    return aab->Build((IAudioAttributes**)&attr);
}

ECode CSoundPool::constructor(
    /* [in] */ Int32 maxStreams,
    /* [in] */ IAudioAttributes* attributes)
{
    Boolean flag = FALSE;
    SystemProperties::GetBoolean(String("config.disable_media"), FALSE, &flag);
    if (flag) {
        mImpl = new SoundPoolStub();
    } else {
        mImpl = new SoundPoolImpl(this, maxStreams, attributes);
    }
}

ECode CSoundPool::Load(
    /* [in] */ const String& path,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mImpl->Load(path, priority, result);
}

ECode CSoundPool::Load(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    mImpl->Load(context, resId, priority, result);
}

ECode CSoundPool::Load(
    /* [in] */ IAssetFileDescriptor* afd,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mImpl->Load(afd, priority, result);
}

ECode CSoundPool::Load(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length,
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    return mImpl->Load(fd, offset, length, priority, result);
}

//-----------------------------------------
// native method in android_media_SoundPool
//-----------------------------------------

ECode CSoundPool::Unload(
    /* [in] */ Int32 soundID,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mImpl->Unload(soundID);
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
    return mImpl->Play(soundID, leftVolume, rightVolume,
        priority, loop, rate, result);
}

ECode CSoundPool::Pause(
    /* [in] */ Int32 streamID)
{
    return mImpl->Pause(streamID);
}

ECode CSoundPool::Resume(
    /* [in] */ Int32 streamID)
{
    mImpl->Resume(streamID);
}

ECode CSoundPool::AutoPause()
{
    return mImpl->AutoPause();
}

ECode CSoundPool::AutoResume()
{
    return mImpl->AutoResume();
}

ECode CSoundPool::Stop(
    /* [in] */ Int32 streamID)
{
    return mImpl->Stop(streamID);
}

ECode CSoundPool::SetVolume(
    /* [in] */ Int32 streamID,
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume)
{
    return mImpl->SetVolume(streamID, leftVolume, rightVolume);
}

ECode CSoundPool::SetVolume(
    /* [in] */ Int32 streamID,
    /* [in] */ Float volume)
{
    return SetVolume(streamID, volume, volume);
}

ECode CSoundPool::SetPriority(
    /* [in] */ Int32 streamID,
    /* [in] */ Int32 priority)
{
    return mImpl->SetPriority(streamID, priority);
}

ECode CSoundPool::SetLoop(
    /* [in] */ Int32 streamID,
    /* [in] */ Int32 loop)
{
    return mImpl->SetLoop(streamID, loop);
}

ECode CSoundPool::SetRate(
    /* [in] */ Int32 streamID,
    /* [in] */ Float rate)
{
    return mImpl->SetRate(streamID, rate);
}

//-----------------------------------------

ECode CSoundPool::SetOnLoadCompleteListener(
    /* [in] */ IOnLoadCompleteListener* listener)
{
    return mImpl->SetOnLoadCompleteListener(listener);
}

/*native method*/
ECode CSoundPool::ReleaseResources()
{
    return mImpl->ReleaseResources();
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
