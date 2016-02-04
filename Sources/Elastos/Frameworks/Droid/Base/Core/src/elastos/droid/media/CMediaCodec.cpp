#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/media/CMediaCodec.h"
#include "elastos/droid/media/CMediaCodecList.h"
#include "elastos/droid/media/CMediaFormat.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::IO::IBuffer;
using Elastos::IO::CNioUtils;
using Elastos::IO::INioUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Media {

Int32 const CMediaCodec::EVENT_CALLBACK = 1;
Int32 const CMediaCodec::EVENT_SET_CALLBACK = 2;
Int32 const CMediaCodec::CB_INPUT_AVAILABLE = 1;
Int32 const CMediaCodec::CB_OUTPUT_AVAILABLE = 2;
Int32 const CMediaCodec::CB_ERROR = 3;
Int32 const CMediaCodec::CB_OUTPUT_FORMAT_CHANGE = 4;

//==============================================================================
//  CMediaCodec::EventHandler
//==============================================================================

ECode CMediaCodec::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CMediaCodec::EVENT_CALLBACK:
        {
            HandleCallback(msg);
            break;
        }
        case CMediaCodec::EVENT_SET_CALLBACK:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mCodec->mCallback = IMediaCodecCallback::Probe(obj);
            break;
        }
        default:
        {
            break;
        }
    }
    return NOERROR;
}

void CMediaCodec::EventHandler::HandleCallback(
    /* [in] */ IMessage* msg)
{
    if (mCodec->mCallback == NULL) {
        return;
    }

    Int32 arg1;
    msg->GetArg1(&arg1);
    switch (arg1) {
        case CMediaCodec::CB_INPUT_AVAILABLE:
        {
            Int32 index;
            msg->GetArg2(&index);
            Object& lock = mCodec->mBufferLock;
            synchronized(lock) {
                mCodec->ValidateInputByteBuffer(mCodec->mCachedInputBuffers, index);
            }
            mCodec->mCallback->OnInputBufferAvailable(mCodec, index);
            break;
        }

        case CMediaCodec::CB_OUTPUT_AVAILABLE:
        {
            Int32 index;
            msg->GetArg2(&index);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IMediaCodecBufferInfo> info = IMediaCodecBufferInfo::Probe(obj);
            Object& lock = mCodec->mBufferLock;
            synchronized(lock) {
                mCodec->ValidateOutputByteBuffer(mCodec->mCachedOutputBuffers, index, info);
            }
            mCodec->mCallback->OnOutputBufferAvailable(
                    mCodec, index, info);
            break;
        }

        case CMediaCodec::CB_ERROR:
        {
            mCodec->mCallback->OnError(mCodec, E_MEDIACODEC_CODEC_EXCEPTION);
            break;
        }

        case CMediaCodec::CB_OUTPUT_FORMAT_CHANGE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IMap> map = IMap::Probe(obj);
            AutoPtr<IMediaFormat> mf;
            CMediaFormat::New(map, (IMediaFormat**)&mf);
            mCodec->mCallback->OnOutputFormatChanged(mCodec, mf);
            break;
        }

        default:
        {
            break;
        }
    }
}

//==============================================================================
//  CMediaCodec::BufferMap
//==============================================================================

ECode CMediaCodec::BufferMap::Remove(
    /* [in] */ Int32 index)
{
    AutoPtr<IInteger32> val;
    CInteger32::New(index, (IInteger32**)&val);
    AutoPtr<IInterface> obj;
    mMap->Get(val, (IInterface**)&obj);
    AutoPtr<CodecBuffer> buffer = (CodecBuffer*)IObject::Probe(obj.Get());

    if (buffer != NULL) {
        buffer->Free();
        mMap->Remove(val);
    }
    return NOERROR;
}

ECode CMediaCodec::BufferMap::Put(
    /* [in] */ Int32 index,
    /* [in] */ IByteBuffer* newBuffer)
{
    AutoPtr<IInteger32> val;
    CInteger32::New(index, (IInteger32**)&val);
    AutoPtr<IInterface> obj;
    mMap->Get(val, (IInterface**)&obj);
    AutoPtr<CodecBuffer> buffer = (CodecBuffer*)IObject::Probe(obj.Get());

    if (buffer == NULL) { // likely
        buffer = new CodecBuffer(this);
        mMap->Put(val, (IInterface*)(IObject*)buffer);
    }
    buffer->SetByteBuffer(newBuffer);
    return NOERROR;
}

ECode CMediaCodec::BufferMap::Put(
    /* [in] */ Int32 index,
    /* [in] */ IImage* newImage)
{
    AutoPtr<IInteger32> val;
    CInteger32::New(index, (IInteger32**)&val);
    AutoPtr<IInterface> obj;
    mMap->Get(val, (IInterface**)&obj);
    AutoPtr<CodecBuffer> buffer = (CodecBuffer*)IObject::Probe(obj.Get());

    if (buffer == NULL) { // likely
        buffer = new CodecBuffer(this);
        mMap->Put(val, (IInterface*)(IObject*)buffer);
    }
    buffer->SetImage(newImage);
    return NOERROR;
}

ECode CMediaCodec::BufferMap::Clear()
{
    AutoPtr<ICollection> collection;
    mMap->GetValues((ICollection**)&collection);
    AutoPtr<ArrayOf<IInterface*> > array;
    collection->ToArray((ArrayOf<IInterface*>**)&array);

    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<CodecBuffer> buffer = (CodecBuffer*)IObject::Probe((*array)[i]);
        buffer->Free();
    }
    mMap->Clear();
    return NOERROR;
}

//==============================================================================
//  CMediaCodec::BufferMap::CodecBuffer
//==============================================================================

ECode CMediaCodec::BufferMap::CodecBuffer::Free()
{
    if (mByteBuffer != NULL) {
        // all of our ByteBuffers are direct
        AutoPtr<INioUtils> utils;
        CNioUtils::AcquireSingleton((INioUtils**)&utils);
        utils->FreeDirectBuffer(mByteBuffer);
        mByteBuffer = NULL;
    }
    if (mImage != NULL) {
// TODO: Need IAutoCloseable
        // mImage->Close();
        mImage = NULL;
    }
    return NOERROR;
}

ECode CMediaCodec::BufferMap::CodecBuffer::SetImage(
    /* [in] */ IImage* image)
{
    Free();
    mImage = image;
    return NOERROR;
}

ECode CMediaCodec::BufferMap::CodecBuffer::SetByteBuffer(
    /* [in] */ IByteBuffer* buffer)
{
    Free();
    mByteBuffer = buffer;
    return NOERROR;
}

//==============================================================================
//  CMediaCodec
//==============================================================================

CAR_INTERFACE_IMPL(CMediaCodec, Object, IMediaCodec)

CAR_OBJECT_IMPL(CMediaCodec)

CMediaCodec::CMediaCodec()
    : mNativeContext(0)
{
    mDequeuedInputBuffers = new BufferMap(this);
    mDequeuedOutputBuffers = new BufferMap(this);
}

CMediaCodec::~CMediaCodec()
{
}

ECode CMediaCodec::constructor()
{
    return NOERROR;
}

ECode CMediaCodec::constructor(
    /* [in] */ const String& name,
    /* [in] */ Boolean nameIsType,
    /* [in] */ Boolean encoder)
{
    AutoPtr<ILooper> looper;
    if ((looper = Looper::GetMyLooper()) != NULL) {
        mEventHandler = new EventHandler(this, looper);
    }
    else if ((looper = Looper::GetMainLooper()) != NULL) {
        mEventHandler = new EventHandler(this, looper);
    }
    else {
        mEventHandler = NULL;
    }

    NativeSetup(name, nameIsType, encoder);
    return NOERROR;
}

ECode CMediaCodec::Reset()
{
    FreeAllTrackedBuffers(); // free buffers first
    NativeReset();
    return NOERROR;
}

ECode CMediaCodec::ReleaseResources()
{
    FreeAllTrackedBuffers(); // free buffers first
    NativeRelease();
    return NOERROR;
}

ECode CMediaCodec::Configure(
    /* [in] */ IMediaFormat* format,
    /* [in] */ ISurface* surface,
    /* [in] */ IMediaCrypto* crypto,
    /* [in] */ Int32 flags)
{
    AutoPtr<IMap> formatMap;
    format->GetMap((IMap**)&formatMap);

    AutoPtr<ArrayOf<String> > keys;
    AutoPtr<ArrayOf<IInterface*> > values;

    if (format != NULL) {
        Int32 size;
        formatMap->GetSize(&size);
        keys = ArrayOf<String>::Alloc(size);
        values = ArrayOf<IInterface*>::Alloc(size);

        AutoPtr<ISet> set;
        formatMap->GetEntrySet((ISet**)&set);
        AutoPtr<ArrayOf<IInterface*> > array;
        set->ToArray((ArrayOf<IInterface*>**)&array);

        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IMapEntry> entry = IMapEntry::Probe((*array)[i]);
            AutoPtr<IInterface> key;
            entry->GetKey((IInterface**)&key);
            String str;
            ICharSequence::Probe(key)->ToString(&str);

            if (str.Equals(IMediaFormat::KEY_AUDIO_SESSION_ID)) {
                // try {
                AutoPtr<IInterface> val;
                entry->GetValue((IInterface**)&val);
                Int32 sessionId;
                IInteger32::Probe(val)->GetValue(&sessionId);
                // }
                // catch (Exception e) {
                //     throw new IllegalArgumentException("Wrong Session ID Parameter!");
                // }
                keys->Set(i, String("audio-hw-sync"));
                Int32 value;
                AudioSystem::GetAudioHwSyncForSession(sessionId, &value);
                AutoPtr<IInteger32> iVal;
                CInteger32::New(value, (IInteger32**)&iVal);
                values->Set(i, iVal);
            }
            else {
                keys->Set(i, str);
                AutoPtr<IInterface> obj;
                entry->GetValue((IInterface**)&obj);
                values->Set(i, obj);
            }
        }
    }

    NativeConfigure(keys, values, surface, crypto, flags);
    return NOERROR;
}

// TODO: Need jni code
ECode CMediaCodec::CreateInputSurface(
    /* [out] */ ISurface** result)
{
    VALIDATE_NOT_NULL(result)
    // ALOGV("android_media_MediaCodec_createInputSurface");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);
    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return NULL;
    // }

    // // Tell the MediaCodec that we want to use a Surface as input.
    // sp<IGraphicBufferProducer> bufferProducer;
    // status_t err = codec->createInputSurface(&bufferProducer);
    // if (err != NO_ERROR) {
    //     throwExceptionAsNecessary(env, err);
    //     return NULL;
    // }

    // // Wrap the IGBP in a Java-language Surface.
    // return android_view_Surface_createFromIGraphicBufferProducer(env,
    //         bufferProducer);
    return NOERROR;
}

ECode CMediaCodec::Start()
{
    NativeStart();
    synchronized(mBufferLock) {
        CacheBuffers(TRUE /* input */);
        CacheBuffers(FALSE /* input */);
    }
    return NOERROR;
}

ECode CMediaCodec::Stop()
{
    NativeStop();
    FreeAllTrackedBuffers();

    if (mEventHandler != NULL) {
        mEventHandler->RemoveMessages(EVENT_CALLBACK);
        mEventHandler->RemoveMessages(EVENT_SET_CALLBACK);
    }
    return NOERROR;
}

ECode CMediaCodec::Flush()
{
    synchronized(mBufferLock) {
        InvalidateByteBuffers(mCachedInputBuffers);
        InvalidateByteBuffers(mCachedOutputBuffers);
        mDequeuedInputBuffers->Clear();
        mDequeuedOutputBuffers->Clear();
    }
    NativeFlush();
    return NOERROR;
}

ECode CMediaCodec::QueueInputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ Int64 presentationTimeUs,
    /* [in] */ Int32 flags)
{
    synchronized(mBufferLock) {
        InvalidateByteBuffer(mCachedInputBuffers, index);
        mDequeuedInputBuffers->Remove(index);
    }
    // try {
    NativeQueueInputBuffer(
            index, offset, size, presentationTimeUs, flags);
    // } catch (CryptoException | IllegalStateException e) {
    //     revalidateByteBuffer(mCachedInputBuffers, index);
    //     throw e;
    // }
    return NOERROR;
}

ECode CMediaCodec::QueueSecureInputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ IMediaCodecCryptoInfo* info,
    /* [in] */ Int64 presentationTimeUs,
    /* [in] */ Int32 flags)
{
    synchronized(mBufferLock) {
        InvalidateByteBuffer(mCachedInputBuffers, index);
        mDequeuedInputBuffers->Remove(index);
    }
    // try {
    NativeQueueSecureInputBuffer(
            index, offset, info, presentationTimeUs, flags);
    // } catch (CryptoException | IllegalStateException e) {
    //     revalidateByteBuffer(mCachedInputBuffers, index);
    //     throw e;
    // }
    return NOERROR;
}

ECode CMediaCodec::DequeueInputBuffer(
    /* [in] */ Int64 timeoutUs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 res = NativeDequeueInputBuffer(timeoutUs);
    if (res >= 0) {
        synchronized(mBufferLock) {
            ValidateInputByteBuffer(mCachedInputBuffers, res);
        }
    }
    *result = res;
    return NOERROR;
}

ECode CMediaCodec::DequeueOutputBuffer(
    /* [in] */ IMediaCodecBufferInfo* info,
    /* [in] */ Int64 timeoutUs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 res = NativeDequeueOutputBuffer(info, timeoutUs);
    synchronized(mBufferLock) {
        if (res == IMediaCodec::INFO_OUTPUT_BUFFERS_CHANGED) {
            CacheBuffers(FALSE /* input */);
        }
        else if (res >= 0) {
            ValidateOutputByteBuffer(mCachedOutputBuffers, res, info);
        }
    }
    *result = res;
    return NOERROR;
}

ECode CMediaCodec::ReleaseOutputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Boolean render)
{
    synchronized(mBufferLock) {
        InvalidateByteBuffer(mCachedOutputBuffers, index);
        mDequeuedOutputBuffers->Remove(index);
    }
    NativeReleaseOutputBuffer(index, render, FALSE /* updatePTS */, 0 /* dummy */);
    return NOERROR;
}

ECode CMediaCodec::ReleaseOutputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int64 renderTimestampNs)
{
    synchronized(mBufferLock) {
        InvalidateByteBuffer(mCachedOutputBuffers, index);
        mDequeuedOutputBuffers->Remove(index);
    }
    NativeReleaseOutputBuffer(
            index, TRUE /* render */, TRUE /* updatePTS */, renderTimestampNs);
    return NOERROR;
}

// TODO: Need jni code
ECode CMediaCodec::SignalEndOfInputStream()
{
    // ALOGV("android_media_MediaCodec_signalEndOfInputStream");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);
    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // status_t err = codec->signalEndOfInputStream();

    // throwExceptionAsNecessary(env, err);
    return NOERROR;
}

ECode CMediaCodec::GetOutputFormat(
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = NativeGetFormatNative(FALSE /* input */);
    return CMediaFormat::New(map, result);
    return NOERROR;
}

ECode CMediaCodec::GetInputFormat(
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = NativeGetFormatNative(TRUE /* input */);
    return CMediaFormat::New(map, result);
    return NOERROR;
}

ECode CMediaCodec::GetOutputFormat(
    /* [in] */ Int32 index,
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = NativeGetOutputFormatNative(index);
    return CMediaFormat::New(map, result);
    return NOERROR;
}

ECode CMediaCodec::GetInputBuffers(
    /* [out, callee] */ ArrayOf<IByteBuffer*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mCachedInputBuffers == NULL) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // FIXME: check codec status
    *result = mCachedInputBuffers;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodec::GetOutputBuffers(
    /* [out, callee] */ ArrayOf<IByteBuffer*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mCachedOutputBuffers == NULL) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // FIXME: check codec status
    *result = mCachedOutputBuffers;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodec::GetInputBuffer(
    /* [in] */ Int32 index,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IByteBuffer> newBuffer = NativeGetBuffer(TRUE /* input */, index);
    synchronized(mBufferLock) {
        InvalidateByteBuffer(mCachedInputBuffers, index);
        mDequeuedInputBuffers->Put(index, newBuffer);
    }
    *result = newBuffer;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodec::GetInputImage(
    /* [in] */ Int32 index,
    /* [out] */ IImage** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IImage> newImage = NativeGetImage(TRUE /* input */, index);
    synchronized(mBufferLock) {
        InvalidateByteBuffer(mCachedInputBuffers, index);
        mDequeuedInputBuffers->Put(index, newImage);
    }
    *result = newImage;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodec::GetOutputBuffer(
    /* [in] */ Int32 index,
    /* [out] */ IByteBuffer** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IByteBuffer> newBuffer = NativeGetBuffer(FALSE /* input */, index);
    synchronized(mBufferLock) {
        InvalidateByteBuffer(mCachedOutputBuffers, index);
        mDequeuedOutputBuffers->Put(index, newBuffer);
    }
    *result = newBuffer;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodec::GetOutputImage(
    /* [in] */ Int32 index,
    /* [out] */ IImage** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IImage> newImage = NativeGetImage(FALSE /* input */, index);
    synchronized(mBufferLock) {
        InvalidateByteBuffer(mCachedOutputBuffers, index);
        mDequeuedOutputBuffers->Put(index, newImage);
    }
    *result = newImage;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

// TODO: Need jni code
ECode CMediaCodec::SetVideoScalingMode(
    /* [in] */ Int32 mode)
{
    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // if (mode != NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW
    //         && mode != NATIVE_WINDOW_SCALING_MODE_SCALE_CROP) {
    //     jniThrowException(env, "java/lang/InvalidArgumentException", NULL);
    //     return;
    // }

    // codec->setVideoScalingMode(mode);
    return NOERROR;
}

// TODO: Need jni code
ECode CMediaCodec::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // ALOGV("android_media_MediaCodec_getName");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return NULL;
    // }

    // jstring name;
    // status_t err = codec->getName(env, &name);

    // if (err == OK) {
    //     return name;
    // }

    // throwExceptionAsNecessary(env, err);

    // return NULL;
    return NOERROR;
}

ECode CMediaCodec::SetParameters(
    /* [in] */ IBundle* params)
{
    if (params == NULL) {
        return NOERROR;
    }

    Int32 size;
    params->GetSize(&size);
    AutoPtr<ArrayOf<String> > keys = ArrayOf<String>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > values = ArrayOf<IInterface*>::Alloc(size);

    AutoPtr<ISet> set;
    params->GetKeySet((ISet**)&set);
    AutoPtr<ArrayOf<IInterface*> > array;
    set->ToArray((ArrayOf<IInterface*>**)&array);

    for (Int32 i = 0; i < array->GetLength(); i++) {
        String key;
        ICharSequence::Probe((*array)[i])->ToString(&key);
        keys->Set(i, key);
        AutoPtr<IInterface> obj;
        params->Get(key, (IInterface**)&obj);
        values->Set(i, obj);
    }

    NativeSetParameters(keys, values);
    return NOERROR;
}

ECode CMediaCodec::SetCallback(
    /* [in] */ IMediaCodecCallback* cb)
{
    if (mEventHandler != NULL) {
        // set java callback on handler
        AutoPtr<IMessage> msg;
        mEventHandler->ObtainMessage(EVENT_SET_CALLBACK, 0, 0, cb, (IMessage**)&msg);
        Boolean b;
        mEventHandler->SendMessage(msg, &b);

        // set native handler here, don't post to handler because
        // it may cause the callback to be delayed and set in a wrong state,
        // and MediaCodec is already doing it on looper.
        NativeSetCallback(cb);
    }
    return NOERROR;
}

ECode CMediaCodec::GetCodecInfo(
    /* [out] */ IMediaCodecInfo** result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    GetName(&str);
    return CMediaCodecList::GetInfoFor(str, result);
}

ECode CMediaCodec::CreateDecoderByType(
    /* [in] */ const String& type,
    /* [out] */ IMediaCodec** result)
{
    VALIDATE_NOT_NULL(result)
    return CMediaCodec::New(type, TRUE /* nameIsType */,
            FALSE /* encoder */, result);
}

ECode CMediaCodec::CreateEncoderByType(
    /* [in] */ const String& type,
    /* [out] */ IMediaCodec** result)
{
    VALIDATE_NOT_NULL(result)
    return CMediaCodec::New(type, TRUE /* nameIsType */,
            TRUE /* encoder */, result);
}

ECode CMediaCodec::CreateByCodecName(
    /* [in] */ const String& name,
    /* [out] */ IMediaCodec** result)
{
    VALIDATE_NOT_NULL(result)
    return CMediaCodec::New(name, FALSE /* nameIsType */,
            FALSE /* unused */, result);
}

// TODO: Need jni code
void CMediaCodec::NativeReset()
{
    // ALOGV("android_media_MediaCodec_reset");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // status_t err = codec->reset();
    // if (err != OK) {
    //     // treat all errors as fatal for now, though resource not available
    //     // errors could be treated as transient.
    //     // we also should avoid sending INVALID_OPERATION here due to
    //     // the transitory nature of reset(), it should not inadvertently
    //     // trigger an IllegalStateException.
    //     err = UNKNOWN_ERROR;
    // }
    // throwExceptionAsNecessary(env, err);
}

void CMediaCodec::NativeRelease()
{
    // setMediaCodec(env, thiz, NULL);
}

void CMediaCodec::NativeSetCallback(
    /* [in] */ IMediaCodecCallback* cb)
{
    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // status_t err = codec->setCallback(cb);

    // throwExceptionAsNecessary(env, err);
}

void CMediaCodec::NativeConfigure(
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [in] */ ISurface* surface,
    /* [in] */ IMediaCrypto* crypto,
    /* [in] */ Int32 flags)
{
    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // sp<AMessage> format;
    // status_t err = ConvertKeyValueArraysToMessage(env, keys, values, &format);

    // if (err != OK) {
    //     jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
    //     return;
    // }

    // sp<IGraphicBufferProducer> bufferProducer;
    // if (jsurface != NULL) {
    //     sp<Surface> surface(android_view_Surface_getSurface(env, jsurface));
    //     if (surface != NULL) {
    //         bufferProducer = surface->getIGraphicBufferProducer();
    //     } else {
    //         jniThrowException(
    //                 env,
    //                 "java/lang/IllegalArgumentException",
    //                 "The surface has been released");
    //         return;
    //     }
    // }

    // sp<ICrypto> crypto;
    // if (jcrypto != NULL) {
    //     crypto = JCrypto::GetCrypto(env, jcrypto);
    // }

    // err = codec->configure(format, bufferProducer, crypto, flags);

    // throwExceptionAsNecessary(env, err);
}

void CMediaCodec::NativeStart()
{
    // ALOGV("android_media_MediaCodec_start");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // status_t err = codec->start();

    // throwExceptionAsNecessary(env, err, ACTION_CODE_FATAL, "start failed");
}

void CMediaCodec::NativeStop()
{
    // ALOGV("android_media_MediaCodec_stop");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // status_t err = codec->stop();

    // throwExceptionAsNecessary(env, err);
}

void CMediaCodec::NativeFlush()
{
    // ALOGV("android_media_MediaCodec_flush");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // status_t err = codec->flush();

    // throwExceptionAsNecessary(env, err);
}

void CMediaCodec::NativeQueueInputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ Int64 presentationTimeUs,
    /* [in] */ Int32 flags)
{
    // ALOGV("android_media_MediaCodec_queueInputBuffer");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // AString errorDetailMsg;

    // status_t err = codec->queueInputBuffer(
    //         index, offset, size, timestampUs, flags, &errorDetailMsg);

    // throwExceptionAsNecessary(
    //         env, err, ACTION_CODE_FATAL, errorDetailMsg.empty() ? NULL : errorDetailMsg.c_str());
}

void CMediaCodec::NativeQueueSecureInputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ IMediaCodecCryptoInfo* info,
    /* [in] */ Int64 presentationTimeUs,
    /* [in] */ Int32 flags)
{
    // ALOGV("android_media_MediaCodec_queueSecureInputBuffer");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // jint numSubSamples =
    //     env->GetIntField(cryptoInfoObj, gFields.cryptoInfoNumSubSamplesID);

    // jintArray numBytesOfClearDataObj =
    //     (jintArray)env->GetObjectField(
    //             cryptoInfoObj, gFields.cryptoInfoNumBytesOfClearDataID);

    // jintArray numBytesOfEncryptedDataObj =
    //     (jintArray)env->GetObjectField(
    //             cryptoInfoObj, gFields.cryptoInfoNumBytesOfEncryptedDataID);

    // jbyteArray keyObj =
    //     (jbyteArray)env->GetObjectField(cryptoInfoObj, gFields.cryptoInfoKeyID);

    // jbyteArray ivObj =
    //     (jbyteArray)env->GetObjectField(cryptoInfoObj, gFields.cryptoInfoIVID);

    // jint mode = env->GetIntField(cryptoInfoObj, gFields.cryptoInfoModeID);

    // status_t err = OK;

    // CryptoPlugin::SubSample *subSamples = NULL;
    // jbyte *key = NULL;
    // jbyte *iv = NULL;

    // if (numSubSamples <= 0) {
    //     err = -EINVAL;
    // } else if (numBytesOfClearDataObj == NULL
    //         && numBytesOfEncryptedDataObj == NULL) {
    //     err = -EINVAL;
    // } else if (numBytesOfEncryptedDataObj != NULL
    //         && env->GetArrayLength(numBytesOfEncryptedDataObj) < numSubSamples) {
    //     err = -ERANGE;
    // } else if (numBytesOfClearDataObj != NULL
    //         && env->GetArrayLength(numBytesOfClearDataObj) < numSubSamples) {
    //     err = -ERANGE;
    // // subSamples array may silently overflow if number of samples are too large.  Use
    // // INT32_MAX as maximum allocation size may be less than SIZE_MAX on some platforms
    // } else if ( CC_UNLIKELY(numSubSamples >= (signed)(INT32_MAX / sizeof(*subSamples))) ) {
    //     err = -EINVAL;
    // } else {
    //     jboolean isCopy;

    //     jint *numBytesOfClearData =
    //         (numBytesOfClearDataObj == NULL)
    //             ? NULL
    //             : env->GetIntArrayElements(numBytesOfClearDataObj, &isCopy);

    //     jint *numBytesOfEncryptedData =
    //         (numBytesOfEncryptedDataObj == NULL)
    //             ? NULL
    //             : env->GetIntArrayElements(numBytesOfEncryptedDataObj, &isCopy);

    //     subSamples = new CryptoPlugin::SubSample[numSubSamples];

    //     for (jint i = 0; i < numSubSamples; ++i) {
    //         subSamples[i].mNumBytesOfClearData =
    //             (numBytesOfClearData == NULL) ? 0 : numBytesOfClearData[i];

    //         subSamples[i].mNumBytesOfEncryptedData =
    //             (numBytesOfEncryptedData == NULL)
    //                 ? 0 : numBytesOfEncryptedData[i];
    //     }

    //     if (numBytesOfEncryptedData != NULL) {
    //         env->ReleaseIntArrayElements(
    //                 numBytesOfEncryptedDataObj, numBytesOfEncryptedData, 0);
    //         numBytesOfEncryptedData = NULL;
    //     }

    //     if (numBytesOfClearData != NULL) {
    //         env->ReleaseIntArrayElements(
    //                 numBytesOfClearDataObj, numBytesOfClearData, 0);
    //         numBytesOfClearData = NULL;
    //     }
    // }

    // if (err == OK && keyObj != NULL) {
    //     if (env->GetArrayLength(keyObj) != 16) {
    //         err = -EINVAL;
    //     } else {
    //         jboolean isCopy;
    //         key = env->GetByteArrayElements(keyObj, &isCopy);
    //     }
    // }

    // if (err == OK && ivObj != NULL) {
    //     if (env->GetArrayLength(ivObj) != 16) {
    //         err = -EINVAL;
    //     } else {
    //         jboolean isCopy;
    //         iv = env->GetByteArrayElements(ivObj, &isCopy);
    //     }
    // }

    // AString errorDetailMsg;

    // if (err == OK) {
    //     err = codec->queueSecureInputBuffer(
    //             index, offset,
    //             subSamples, numSubSamples,
    //             (const uint8_t *)key, (const uint8_t *)iv,
    //             (CryptoPlugin::Mode)mode,
    //             timestampUs,
    //             flags,
    //             &errorDetailMsg);
    // }

    // if (iv != NULL) {
    //     env->ReleaseByteArrayElements(ivObj, iv, 0);
    //     iv = NULL;
    // }

    // if (key != NULL) {
    //     env->ReleaseByteArrayElements(keyObj, key, 0);
    //     key = NULL;
    // }

    // delete[] subSamples;
    // subSamples = NULL;

    // throwExceptionAsNecessary(
    //         env, err, ACTION_CODE_FATAL, errorDetailMsg.empty() ? NULL : errorDetailMsg.c_str());
}

Int32 CMediaCodec::NativeDequeueInputBuffer(
    /* [in] */ Int64 timeoutUs)
{
    // ALOGV("android_media_MediaCodec_dequeueInputBuffer");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return -1;
    // }

    // size_t index;
    // status_t err = codec->dequeueInputBuffer(&index, timeoutUs);

    // if (err == OK) {
    //     return (jint) index;
    // }

    // return throwExceptionAsNecessary(env, err);
    return 0;
}

Int32 CMediaCodec::NativeDequeueOutputBuffer(
    /* [in] */ IMediaCodecBufferInfo* info,
    /* [in] */ Int64 timeoutUs)
{
    // ALOGV("android_media_MediaCodec_dequeueOutputBuffer");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return 0;
    // }

    // size_t index;
    // status_t err = codec->dequeueOutputBuffer(
    //         env, bufferInfo, &index, timeoutUs);

    // if (err == OK) {
    //     return (jint) index;
    // }

    // return throwExceptionAsNecessary(env, err);
    return 0;
}

void CMediaCodec::NativeReleaseOutputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Boolean render,
    /* [in] */ Boolean updatePTS,
    /* [in] */ Int64 timeNs)
{
    // ALOGV("android_media_MediaCodec_renderOutputBufferAndRelease");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // status_t err = codec->releaseOutputBuffer(index, render, updatePTS, timestampNs);

    // throwExceptionAsNecessary(env, err);
}

AutoPtr<IMap> CMediaCodec::NativeGetFormatNative(
    /* [in] */ Boolean input)
{
    // ALOGV("android_media_MediaCodec_getFormatNative");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return NULL;
    // }

    // jobject format;
    // status_t err = codec->getFormat(env, input, &format);

    // if (err == OK) {
    //     return format;
    // }

    // throwExceptionAsNecessary(env, err);

    // return NULL;
    return NULL;
}

AutoPtr<IMap> CMediaCodec::NativeGetOutputFormatNative(
    /* [in] */ Int32 index)
{
    // ALOGV("android_media_MediaCodec_getOutputFormatForIndexNative");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return NULL;
    // }

    // jobject format;
    // status_t err = codec->getOutputFormat(env, index, &format);

    // if (err == OK) {
    //     return format;
    // }

    // throwExceptionAsNecessary(env, err);

    // return NULL;
    return NULL;
}

void CMediaCodec::InvalidateByteBuffer(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 index)
{
    if (buffers != NULL && index >= 0 && index < buffers->GetLength()) {
        AutoPtr<IByteBuffer> buffer = (*buffers)[index];
        if (buffer != NULL) {
            buffer->SetAccessible(FALSE);
        }
    }
}

void CMediaCodec::ValidateInputByteBuffer(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 index)
{
    if (buffers != NULL && index >= 0 && index < buffers->GetLength()) {
        AutoPtr<IByteBuffer> buffer = (*buffers)[index];
        if (buffer != NULL) {
            buffer->SetAccessible(TRUE);
            IBuffer::Probe(buffer)->Clear();
        }
    }
}

void CMediaCodec::RevalidateByteBuffer(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 index)
{
    synchronized(mBufferLock) {
        if (buffers != NULL && index >= 0 && index < buffers->GetLength()) {
            AutoPtr<IByteBuffer> buffer = (*buffers)[index];
            if (buffer != NULL) {
                buffer->SetAccessible(TRUE);
            }
        }
    }
}

void CMediaCodec::ValidateOutputByteBuffer(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 index,
    /* [in] */ IMediaCodecBufferInfo* info)
{
    if (buffers != NULL && index >= 0 && index < buffers->GetLength()) {
        AutoPtr<IByteBuffer> buffer = (*buffers)[index];
        if (buffer != NULL) {
            buffer->SetAccessible(TRUE);
            Int32 offset, size;
            info->GetOffset(&offset);
            info->GetSize(&size);
            IBuffer::Probe(buffer)->SetLimit(offset + size);
            IBuffer::Probe(buffer)->SetPosition(offset);
        }
    }
}

void CMediaCodec::InvalidateByteBuffers(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers)
{
    if (buffers != NULL) {
        for (Int32 i = 0; i < buffers->GetLength(); i++) {
            AutoPtr<IByteBuffer> buffer = (*buffers)[i];
            if (buffer != NULL) {
                buffer->SetAccessible(FALSE);
            }
        }
    }
}

void CMediaCodec::FreeByteBuffer(
    /* [in] */ IByteBuffer* buffer)
{
    if (buffer != NULL /* && buffer->IsDirect() */) {
        // all of our ByteBuffers are direct
        AutoPtr<INioUtils> utils;
        CNioUtils::AcquireSingleton((INioUtils**)&utils);
        utils->FreeDirectBuffer(buffer);
    }
}

void CMediaCodec::FreeByteBuffers(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers)
{
    if (buffers != NULL) {
        for (Int32 i = 0; i < buffers->GetLength(); i++) {
            AutoPtr<IByteBuffer> buffer = (*buffers)[i];
            FreeByteBuffer(buffer);
        }
    }
}

void CMediaCodec::FreeAllTrackedBuffers()
{
    synchronized(mBufferLock) {
        FreeByteBuffers(mCachedInputBuffers);
        FreeByteBuffers(mCachedOutputBuffers);
        mCachedInputBuffers = NULL;
        mCachedOutputBuffers = NULL;
        mDequeuedInputBuffers->Clear();
        mDequeuedOutputBuffers->Clear();
    }
}

void CMediaCodec::CacheBuffers(
    /* [in] */ Boolean input)
{
    AutoPtr<ArrayOf<IByteBuffer*> > buffers;
    // try {
        buffers = NativeGetBuffers(input);
        InvalidateByteBuffers(buffers);
    // } catch (IllegalStateException e) {
    //     // we don't get buffers in async mode
    // }
    if (input) {
        mCachedInputBuffers = buffers;
    }
    else {
        mCachedOutputBuffers = buffers;
    }
}

void CMediaCodec::PostEventFromNative(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    if (mEventHandler != NULL) {
        AutoPtr<IMessage> msg;
        mEventHandler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&msg);
        Boolean b;
        mEventHandler->SendMessage(msg, &b);
    }
}

// TODO: Need jni code
void CMediaCodec::NativeSetParameters(
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    // ALOGV("android_media_MediaCodec_setParameters");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return;
    // }

    // sp<AMessage> params;
    // status_t err = ConvertKeyValueArraysToMessage(env, keys, vals, &params);

    // if (err == OK) {
    //     err = codec->setParameters(params);
    // }

    // throwExceptionAsNecessary(env, err);
}

AutoPtr<ArrayOf<IByteBuffer*> > CMediaCodec::NativeGetBuffers(
    /* [in] */ Boolean input)
{
    // ALOGV("android_media_MediaCodec_getBuffers");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return NULL;
    // }

    // jobjectArray buffers;
    // status_t err = codec->getBuffers(env, input, &buffers);

    // if (err == OK) {
    //     return buffers;
    // }

    // // if we're out of memory, an exception was already thrown
    // if (err != NO_MEMORY) {
    //     throwExceptionAsNecessary(env, err);
    // }

    // return NULL;
    return NULL;
}

AutoPtr<IByteBuffer> CMediaCodec::NativeGetBuffer(
    /* [in] */ Boolean input,
    /* [in] */ Int32 index)
{
    // ALOGV("android_media_MediaCodec_getBuffer");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return NULL;
    // }

    // jobject buffer;
    // status_t err = codec->getBuffer(env, input, index, &buffer);

    // if (err == OK) {
    //     return buffer;
    // }

    // // if we're out of memory, an exception was already thrown
    // if (err != NO_MEMORY) {
    //     throwExceptionAsNecessary(env, err);
    // }

    // return NULL;
    return NULL;
}

AutoPtr<IImage> CMediaCodec::NativeGetImage(
    /* [in] */ Boolean input,
    /* [in] */ Int32 index)
{
    // ALOGV("android_media_MediaCodec_getImage");

    // sp<JMediaCodec> codec = getMediaCodec(env, thiz);

    // if (codec == NULL) {
    //     throwExceptionAsNecessary(env, INVALID_OPERATION);
    //     return NULL;
    // }

    // jobject image;
    // status_t err = codec->getImage(env, input, index, &image);

    // if (err == OK) {
    //     return image;
    // }

    // // if we're out of memory, an exception was already thrown
    // if (err != NO_MEMORY) {
    //     throwExceptionAsNecessary(env, err);
    // }

    // return NULL;
    return NULL;
}

void CMediaCodec::NativeInit()
{
    // ScopedLocalRef<jclass> clazz(
    //         env, env->FindClass("android/media/MediaCodec"));
    // CHECK(clazz.get() != NULL);

    // gFields.context = env->GetFieldID(clazz.get(), "mNativeContext", "J");
    // CHECK(gFields.context != NULL);

    // gFields.postEventFromNativeID =
    //     env->GetMethodID(
    //             clazz.get(), "postEventFromNative", "(IIILjava/lang/Object;)V");

    // CHECK(gFields.postEventFromNativeID != NULL);

    // clazz.reset(env->FindClass("android/media/MediaCodec$CryptoInfo"));
    // CHECK(clazz.get() != NULL);

    // gFields.cryptoInfoNumSubSamplesID =
    //     env->GetFieldID(clazz.get(), "numSubSamples", "I");
    // CHECK(gFields.cryptoInfoNumSubSamplesID != NULL);

    // gFields.cryptoInfoNumBytesOfClearDataID =
    //     env->GetFieldID(clazz.get(), "numBytesOfClearData", "[I");
    // CHECK(gFields.cryptoInfoNumBytesOfClearDataID != NULL);

    // gFields.cryptoInfoNumBytesOfEncryptedDataID =
    //     env->GetFieldID(clazz.get(), "numBytesOfEncryptedData", "[I");
    // CHECK(gFields.cryptoInfoNumBytesOfEncryptedDataID != NULL);

    // gFields.cryptoInfoKeyID = env->GetFieldID(clazz.get(), "key", "[B");
    // CHECK(gFields.cryptoInfoKeyID != NULL);

    // gFields.cryptoInfoIVID = env->GetFieldID(clazz.get(), "iv", "[B");
    // CHECK(gFields.cryptoInfoIVID != NULL);

    // gFields.cryptoInfoModeID = env->GetFieldID(clazz.get(), "mode", "I");
    // CHECK(gFields.cryptoInfoModeID != NULL);

    // clazz.reset(env->FindClass("android/media/MediaCodec$CryptoException"));
    // CHECK(clazz.get() != NULL);

    // jfieldID field;
    // field = env->GetStaticFieldID(clazz.get(), "ERROR_NO_KEY", "I");
    // CHECK(field != NULL);
    // gCryptoErrorCodes.cryptoErrorNoKey =
    //     env->GetStaticIntField(clazz.get(), field);

    // field = env->GetStaticFieldID(clazz.get(), "ERROR_KEY_EXPIRED", "I");
    // CHECK(field != NULL);
    // gCryptoErrorCodes.cryptoErrorKeyExpired =
    //     env->GetStaticIntField(clazz.get(), field);

    // field = env->GetStaticFieldID(clazz.get(), "ERROR_RESOURCE_BUSY", "I");
    // CHECK(field != NULL);
    // gCryptoErrorCodes.cryptoErrorResourceBusy =
    //     env->GetStaticIntField(clazz.get(), field);

    // field = env->GetStaticFieldID(clazz.get(), "ERROR_INSUFFICIENT_OUTPUT_PROTECTION", "I");
    // CHECK(field != NULL);
    // gCryptoErrorCodes.cryptoErrorInsufficientOutputProtection =
    //     env->GetStaticIntField(clazz.get(), field);

    // clazz.reset(env->FindClass("android/media/MediaCodec$CodecException"));
    // CHECK(clazz.get() != NULL);
    // field = env->GetStaticFieldID(clazz.get(), "ACTION_TRANSIENT", "I");
    // CHECK(field != NULL);
    // gCodecActionCodes.codecActionTransient =
    //     env->GetStaticIntField(clazz.get(), field);

    // field = env->GetStaticFieldID(clazz.get(), "ACTION_RECOVERABLE", "I");
    // CHECK(field != NULL);
    // gCodecActionCodes.codecActionRecoverable =
    //     env->GetStaticIntField(clazz.get(), field);
}

void CMediaCodec::NativeSetup(
    /* [in] */ const String& name,
    /* [in] */ Boolean nameIsType,
    /* [in] */ Boolean encoder)
{
    // if (name == NULL) {
    //     jniThrowException(env, "java/lang/NullPointerException", NULL);
    //     return;
    // }

    // const char *tmp = env->GetStringUTFChars(name, NULL);

    // if (tmp == NULL) {
    //     return;
    // }

    // sp<JMediaCodec> codec = new JMediaCodec(env, thiz, tmp, nameIsType, encoder);

    // const status_t err = codec->initCheck();
    // if (err == NAME_NOT_FOUND) {
    //     // fail and do not try again.
    //     jniThrowException(env, "java/lang/IllegalArgumentException",
    //             String8::format("Failed to initialize %s, error %#x", tmp, err));
    //     env->ReleaseStringUTFChars(name, tmp);
    //     return;
    // } else if (err != OK) {
    //     // believed possible to try again
    //     jniThrowException(env, "java/io/IOException",
    //             String8::format("Failed to find matching codec %s, error %#x", tmp, err));
    //     env->ReleaseStringUTFChars(name, tmp);
    //     return;
    // }

    // env->ReleaseStringUTFChars(name, tmp);

    // codec->registerSelf();

    // setMediaCodec(env,thiz, codec);
}

void CMediaCodec::NativeFinalize()
{
    // android_media_MediaCodec_release(env, thiz);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos