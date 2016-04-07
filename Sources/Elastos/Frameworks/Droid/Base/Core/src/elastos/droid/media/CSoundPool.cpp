
#include "elastos/droid/media/CSoundPool.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"
#include "elastos/droid/os/CParcelFileDescriptorHelper.h"
#include "elastos/droid/os/CLooperHelper.h"
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/os/Process.h>
#include "Elastos.CoreLibrary.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <media/SoundPool.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::AutoLock;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

const String CSoundPool::TAG("SoundPool");
const Boolean CSoundPool::DEBUG = FALSE;

//----------------------------
//    CSoundPool::Builder
//----------------------------
CSoundPool::Builder::Builder()
    : mMaxStreams(1)
{}

CSoundPool::Builder::~Builder()
{}

CAR_INTERFACE_IMPL(CSoundPool::Builder, Object, ISoundPoolBuilder)

ECode CSoundPool::Builder::SetMaxStreams(
    /* [in] */ Int32 maxStreams)
{
    if (maxStreams <= 0) {
        Slogger::E("CSoundPool::Builder", "Strictly positive value required for the maximum number of streams");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMaxStreams = maxStreams;
    return NOERROR;
}

ECode CSoundPool::Builder::SetAudioAttributes(
    /* [in] */ IAudioAttributes* attributes)
{
    if (attributes == NULL) {
        Slogger::E(TAG, "Invalid null AudioAttributes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAudioAttributes = attributes;
    return NOERROR;
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
        mAudioAttributes = attr.Get();
    }
    AutoPtr<ISoundPool> sp;
    CSoundPool::New(mMaxStreams, mAudioAttributes.Get(), (ISoundPool**)&sp);
    *result = sp.Get();
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
    /* [in] */ CSoundPool* proxy,
    /* [in] */ Int32 maxStreams,
    /* [in] */ IAudioAttributes* attr)
{
    // do native setup
    if (Native_setup(ISoundPoolDelegate::Probe(this), maxStreams, IAudioAttributes::Probe(attr)) != 0) {
        Slogger::E(TAG, "Native setup failed");
    } else {
        mProxy = proxy;
        mAttributes = attr;
        AutoPtr<IInterface> inter = ServiceManager::GetService(IContext::APP_OPS_SERVICE);
        AutoPtr<IBinder> b = IBinder::Probe(inter);
        mAppOps = IIAppOpsService::Probe(b);
    }
}

CSoundPool::SoundPoolImpl::~SoundPoolImpl()
{}

CAR_INTERFACE_IMPL(CSoundPool::SoundPoolImpl, Object, ISoundPoolDelegate)

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
        *result = _Load(fd.Get(), startOffset, len, priority);
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
    /* [in] */ Int32 priority,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = _Load(fd, offset, length, priority);
    return NOERROR;
}

ECode CSoundPool::SoundPoolImpl::Unload(
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
    return _Play(soundID, leftVolume, rightVolume, priority, loop, rate, result);
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
    return NOERROR;
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
    synchronized(mLock){
        if (listener != NULL) {
            // setup message handler
            AutoPtr<ILooper> looper = Looper::GetMyLooper();
            if (looper != NULL) {
                mEventHandler = new EventHandler(mProxy, this, looper.Get());
            } else {
                looper = NULL;
                looper = Looper::GetMainLooper();
                if (looper != NULL) {
                    mEventHandler = new EventHandler(mProxy, this, looper.Get());
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
        Int32 myUid = Process::MyUid();
        String currentPackName = CActivityThread::GetCurrentPackageName();
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
        AutoPtr<EventHandler> eh;
        // eh = soundPoolImpl->mEventHandler;
        AutoPtr<IMessage> m;
        IHandler::Probe(eh)->ObtainMessage(msg, arg1, arg2, obj, (IMessage**)&m);
        Boolean flag = FALSE;
        ((IHandler*)(eh.Get()))->SendMessage(m.Get(), &flag);
    }
}

Int32 CSoundPool::SoundPoolImpl::Native_setup(
    /* [in] */ IInterface* weakRef,
    /* [in] */ Int32 maxStreams,
    /* [in] */ IInterface* attributes)
{
    audio_attributes_t *paa = NULL;
    AutoPtr<IAudioAttributes> aab = IAudioAttributes::Probe(attributes);
    //todo
    // const char* tags;
    // strncpy(paa->tags, tags, AUDIO_ATTRIBUTES_TAGS_MAX_SIZE - 1);
    Int32 usage;
    aab->GetUsage(&usage);
    paa->usage = (audio_usage_t)usage;
    Int32 ct;
    aab->GetContentType(&ct);
    // paa->content_type = ct;
    Int32 flags;
    aab->GetFlags(&flags);
    paa->flags = flags;

    ALOGV("android_media_SoundPool_SoundPoolImpl_native_setup");
    android::SoundPool *ap = new android::SoundPool(maxStreams, paa);
    if (ap == NULL) {
        return -1;
    }
    // ap->setCallback(android_media_callback, globalWeakRef);
}

void CSoundPool::SoundPoolImpl::Finalize()
{
    ReleaseSoundPoolImpl();
}

ECode CSoundPool::SoundPoolImpl::ReleaseSoundPoolDelegate()
{
    return ReleaseSoundPoolImpl();
}

//----------------------------
//    CSoundPool::SoundPoolImpl::EventHandler
//----------------------------

CSoundPool::SoundPoolImpl::EventHandler::EventHandler(
    /* [in] */ CSoundPool* soundPool,
    /* [in] */ SoundPoolImpl* host,
    /* [in] */ ILooper* looper)
    : mSoundPool(soundPool)
    , mHost(host)
{
}

CAR_INTERFACE_IMPL(CSoundPool::SoundPoolImpl::EventHandler, Object, IHandler)

ECode CSoundPool::SoundPoolImpl::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch(what) {
        case -1:
        {
            if (CSoundPool::DEBUG) {
                Logger::D(TAG, "Sample %d loaded", arg1);
            }

            {
                AutoLock lock(mSoundPool->mLock);
                if (mHost->mOnLoadCompleteListener != NULL) {
                    mHost->mOnLoadCompleteListener->OnLoadComplete(
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

ECode CSoundPool::SoundPoolStub::ReleaseSoundPoolDelegate()
{
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

CAR_OBJECT_IMPL(CSoundPool)

CAR_INTERFACE_IMPL(CSoundPool, Object, ISoundPool)

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
    return NOERROR;
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
    return mImpl->Unload(soundID, result);
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
    return mImpl->Resume(streamID);
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
    /* [in] */ ISoundPoolOnLoadCompleteListener* listener)
{
    return mImpl->SetOnLoadCompleteListener(listener);
}

/*native method*/
ECode CSoundPool::ReleaseResources()
{
    return mImpl->ReleaseSoundPoolDelegate();
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

} // namespace Media
} // namepsace Droid
} // namespace Elastos
