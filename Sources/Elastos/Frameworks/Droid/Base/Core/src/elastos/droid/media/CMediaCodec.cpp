//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/media/CMediaCodec.h"
#include "elastos/droid/media/CMediaCodecList.h"
#include "elastos/droid/media/CMediaCodecBufferInfo.h"
#include "elastos/droid/media/CMediaCodecMediaImage.h"
#include "elastos/droid/media/CMediaFormat.h"
#include "elastos/droid/media/CMediaCrypto.h"
#include "elastos/droid/media/Media_Utils.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/view/CSurface.h"
#include <elastos/core/AutoLock.h>

#include <gui/Surface.h>
#include <media/ICrypto.h>
#include <media/MediaCodecBuffer.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/MediaErrors.h>
#include <system/window.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::View::CSurface;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::IO::ByteOrder;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CNioUtils;
using Elastos::IO::INioUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;

using android::ABuffer;
using android::ALooper;
using android::AMessage;
using android::AString;
using android::IGraphicBufferProducer;
using android::MediaCodec;
using android::sp;
using android::status_t;
using android::Surface;

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
//  JMediaCodec
//==============================================================================

#define CC_UNLIKELY( exp )  (__builtin_expect( !!(exp), false ))

struct JMediaCodec : public android::AHandler {
    JMediaCodec(
            CMediaCodec* object,
            const char *name, bool nameIsType, bool encoder);

    status_t initCheck() const;

    void registerSelf();
    void release();

    status_t setCallback(IMediaCodecCallback* cb);

    status_t configure(
            const sp<AMessage> &format,
            const sp<IGraphicBufferProducer> &bufferProducer,
            const sp<android::ICrypto> &crypto,
            int flags);

    status_t createInputSurface(sp<IGraphicBufferProducer>* bufferProducer);

    status_t start();
    status_t stop();
    status_t reset();

    status_t flush();

    status_t queueInputBuffer(
            size_t index,
            size_t offset, size_t size, int64_t timeUs, uint32_t flags,
            AString *errorDetailMsg);

    status_t queueSecureInputBuffer(
            size_t index,
            size_t offset,
            const android::CryptoPlugin::SubSample *subSamples,
            size_t numSubSamples,
            const uint8_t key[16],
            const uint8_t iv[16],
            android::CryptoPlugin::Mode mode,
            const android::CryptoPlugin::Pattern &pattern,
            int64_t presentationTimeUs,
            uint32_t flags,
            AString *errorDetailMsg);

    status_t dequeueInputBuffer(size_t *index, int64_t timeoutUs);

    status_t dequeueOutputBuffer(
            IMediaCodecBufferInfo* bufferInfo, size_t *index, int64_t timeoutUs);

    status_t releaseOutputBuffer(
            size_t index, bool render, bool updatePTS, int64_t timestampNs);

    status_t signalEndOfInputStream();

    status_t getFormat(bool input, IMap **format) const;

    status_t getOutputFormat(size_t index, IMap **format) const;

    status_t getBuffers(
            bool input, ArrayOf<IByteBuffer*> **bufArray) const;

    status_t getBuffer(
            bool input, size_t index, IByteBuffer **buf) const;

    status_t getImage(
            bool input, size_t index, IImage **image) const;

    status_t getName(String *name) const;

    status_t setParameters(const sp<AMessage> &params);

    void setVideoScalingMode(int mode);

protected:
    virtual ~JMediaCodec();

    virtual void onMessageReceived(const sp<AMessage> &msg);
    ECode handleCallback(const sp<AMessage> &msg);

private:
    enum {
        kWhatCallbackNotify,
    };

    CMediaCodec* mObject;
    sp<Surface> mSurfaceTextureClient;

    ByteOrder mNativeByteOrderObj;

    sp<ALooper> mLooper;
    sp<MediaCodec> mCodec;

    sp<AMessage> mCallbackNotification;

    status_t mInitStatus;

    template <typename T>
    status_t createByteBufferFromABuffer(
            bool readOnly, bool clearBuffer, const sp<T> &buffer,
            IByteBuffer **buf) const;

    DISALLOW_EVIL_CONSTRUCTORS(JMediaCodec);
};

JMediaCodec::JMediaCodec(
        CMediaCodec* object,
        const char *name, bool nameIsType, bool encoder)
    : mObject(object)
{
    mLooper = new ALooper;
    mLooper->setName("MediaCodec_looper");

    mLooper->start(
            false,      // runOnCallingThread
            true,       // canCallJava
            android::PRIORITY_FOREGROUND);

    if (nameIsType) {
        mCodec = android::MediaCodec::CreateByType(mLooper, name, encoder, &mInitStatus);
    }
    else {
        mCodec = android::MediaCodec::CreateByComponentName(mLooper, name, &mInitStatus);
    }
    CHECK((mCodec != NULL) != (mInitStatus != android::OK));
}

status_t JMediaCodec::initCheck() const {
    return mInitStatus;
}

void JMediaCodec::registerSelf() {
    mLooper->registerHandler(this);
}

void JMediaCodec::release() {
    if (mCodec != NULL) {
        mCodec->release();
        mCodec.clear();
        mInitStatus = android::NO_INIT;
    }

    if (mLooper != NULL) {
        mLooper->unregisterHandler(id());
        mLooper->stop();
        mLooper.clear();
    }
}

JMediaCodec::~JMediaCodec() {
    if (mCodec != NULL || mLooper != NULL) {
        /* MediaCodec and looper should have been released explicitly already
         * in setMediaCodec() (see comments in setMediaCodec()).
         *
         * Otherwise JMediaCodec::~JMediaCodec() might be called from within the
         * message handler, doing release() there risks deadlock as MediaCodec::
         * release() post synchronous message to the same looper.
         *
         * Print a warning and try to proceed with releasing.
         */
        ALOGW("try to release MediaCodec from JMediaCodec::~JMediaCodec()...");
        release();
        ALOGW("done releasing MediaCodec from JMediaCodec::~JMediaCodec().");
    }
}

status_t JMediaCodec::setCallback(IMediaCodecCallback* cb) {
    if (cb != NULL) {
        if (mCallbackNotification == NULL) {
            mCallbackNotification = new AMessage(kWhatCallbackNotify, this);
        }
    }
    else {
        mCallbackNotification.clear();
    }

    return mCodec->setCallback(mCallbackNotification);
}

status_t JMediaCodec::configure(
    const sp<AMessage> &format,
    const sp<IGraphicBufferProducer> &bufferProducer,
    const sp<android::ICrypto> &crypto,
    int flags)
{
    sp<Surface> client;
    if (bufferProducer != NULL) {
        mSurfaceTextureClient =
            new Surface(bufferProducer, true /* controlledByApp */);
    }
    else {
        mSurfaceTextureClient.clear();
    }

    return mCodec->configure(format, mSurfaceTextureClient, crypto, flags);
}

status_t JMediaCodec::createInputSurface(
    sp<IGraphicBufferProducer>* bufferProducer)
{
    return mCodec->createInputSurface(bufferProducer);
}

status_t JMediaCodec::start()
{
    return mCodec->start();
}

status_t JMediaCodec::stop()
{
    mSurfaceTextureClient.clear();

    return mCodec->stop();
}

status_t JMediaCodec::flush() {
    return mCodec->flush();
}

status_t JMediaCodec::reset() {
    return mCodec->reset();
}

status_t JMediaCodec::queueInputBuffer(
    size_t index,
    size_t offset, size_t size, int64_t timeUs, uint32_t flags,
    AString *errorDetailMsg)
{
    return mCodec->queueInputBuffer(
            index, offset, size, timeUs, flags, errorDetailMsg);
}

status_t JMediaCodec::queueSecureInputBuffer(
    size_t index,
    size_t offset,
    const android::CryptoPlugin::SubSample *subSamples,
    size_t numSubSamples,
    const uint8_t key[16],
    const uint8_t iv[16],
    android::CryptoPlugin::Mode mode,
    const android::CryptoPlugin::Pattern &pattern,
    int64_t presentationTimeUs,
    uint32_t flags,
    AString *errorDetailMsg)
{
    return mCodec->queueSecureInputBuffer(
            index, offset, subSamples, numSubSamples, key, iv, mode, pattern,
            presentationTimeUs, flags, errorDetailMsg);
}

status_t JMediaCodec::dequeueInputBuffer(size_t *index, int64_t timeoutUs)
{
    return mCodec->dequeueInputBuffer(index, timeoutUs);
}

status_t JMediaCodec::dequeueOutputBuffer(
    IMediaCodecBufferInfo* bufferInfo, size_t *index, int64_t timeoutUs)
{
    size_t size, offset;
    int64_t timeUs;
    uint32_t flags;
    status_t err = mCodec->dequeueOutputBuffer(
            index, &offset, &size, &timeUs, &flags, timeoutUs);

    if (err != android::OK) {
        return err;
    }

    bufferInfo->Set((Int32)offset, (Int32)size, timeUs, (Int32)flags);

    return android::OK;
}

status_t JMediaCodec::releaseOutputBuffer(
    size_t index, bool render, bool updatePTS, int64_t timestampNs)
{
    if (updatePTS) {
        return mCodec->renderOutputBufferAndRelease(index, timestampNs);
    }
    return render
        ? mCodec->renderOutputBufferAndRelease(index)
        : mCodec->releaseOutputBuffer(index);
}

status_t JMediaCodec::signalEndOfInputStream()
{
    return mCodec->signalEndOfInputStream();
}

status_t JMediaCodec::getFormat(bool input, IMap **format) const
{
    VALIDATE_NOT_NULL(format)
    *format = NULL;

    sp<AMessage> msg;
    status_t err;
    err = input ? mCodec->getInputFormat(&msg) : mCodec->getOutputFormat(&msg);
    if (err != android::OK) {
        return err;
    }

    return Media_Utils::ConvertMessageToMap(msg, format);
}

status_t JMediaCodec::getOutputFormat(size_t index, IMap **format) const
{
    VALIDATE_NOT_NULL(format)
    *format = NULL;

    sp<AMessage> msg;
    status_t err;
    if ((err = mCodec->getOutputFormat(index, &msg)) != android::OK) {
        return err;
    }

    return Media_Utils::ConvertMessageToMap(msg, format);
}

status_t JMediaCodec::getBuffers(
    bool input, ArrayOf<IByteBuffer*> **bufArray) const
{
    VALIDATE_NOT_NULL(bufArray)
    *bufArray = NULL;

    android::Vector<sp<android::MediaCodecBuffer> > buffers;

    status_t err =
        input
            ? mCodec->getInputBuffers(&buffers)
            : mCodec->getOutputBuffers(&buffers);

    if (err != android::OK) {
        return err;
    }

    AutoPtr<ArrayOf<IByteBuffer*> > array = ArrayOf<IByteBuffer*>::Alloc(buffers.size());
    if (array == NULL) {
        return android::NO_MEMORY;
    }

    for (size_t i = 0; i < buffers.size(); ++i) {
        const sp<android::MediaCodecBuffer> &buffer = buffers.itemAt(i);

        AutoPtr<IByteBuffer> byteBuffer;
        err = createByteBufferFromABuffer(
                !input /* readOnly */, true /* clearBuffer */, buffer, (IByteBuffer**)&byteBuffer);
        if (err != android::OK) {
            return err;
        }
        if (byteBuffer != NULL) {
            array->Set(i, byteBuffer);
        }
    }

    *bufArray = array;
    REFCOUNT_ADD(*bufArray)
    return android::OK;
}

// static
template <typename T>
status_t JMediaCodec::createByteBufferFromABuffer(
    bool readOnly, bool clearBuffer, const sp<T> &buffer,
    IByteBuffer **buf) const
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    // if this is an ABuffer that doesn't actually hold any accessible memory,
    // use a null ByteBuffer
    *buf = NULL;
    if (buffer->base() == NULL) {
        return android::OK;
    }

    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> byteBuffer;
    helper->AllocateDirect(buffer->capacity(), (IByteBuffer**)&byteBuffer);

    if (readOnly && byteBuffer != NULL) {
        AutoPtr<IByteBuffer> readOnlyBuffer;
        byteBuffer->AsReadOnlyBuffer((IByteBuffer**)&readOnlyBuffer);
        byteBuffer = NULL;
        byteBuffer = readOnlyBuffer;
    }
    if (byteBuffer == NULL) {
        return android::NO_MEMORY;
    }
    byteBuffer->SetOrder(mNativeByteOrderObj);
    IBuffer::Probe(byteBuffer)->SetLimit(
            clearBuffer ? buffer->capacity() : (buffer->offset() + buffer->size()));
    IBuffer::Probe(byteBuffer)->SetPosition(clearBuffer ? 0 : buffer->offset());

    *buf = byteBuffer;
    REFCOUNT_ADD(*buf)
    return android::OK;
}

status_t JMediaCodec::getBuffer(
    bool input, size_t index, IByteBuffer **buf) const
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    sp<android::MediaCodecBuffer> buffer;

    status_t err =
        input
            ? mCodec->getInputBuffer(index, &buffer)
            : mCodec->getOutputBuffer(index, &buffer);

    if (err != android::OK) {
        return err;
    }

    return createByteBufferFromABuffer(
            !input /* readOnly */, input /* clearBuffer */, buffer, buf);
}

status_t JMediaCodec::getImage(
    bool input, size_t index, IImage **buf) const
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    sp<android::MediaCodecBuffer> buffer;

    status_t err =
        input
            ? mCodec->getInputBuffer(index, &buffer)
            : mCodec->getOutputBuffer(index, &buffer);

    if (err != android::OK) {
        return err;
    }

    // if this is an ABuffer that doesn't actually hold any accessible memory,
    // use a null ByteBuffer
    *buf = NULL;
    if (buffer->base() == NULL) {
        return android::OK;
    }

    // check if buffer is an image
    sp<ABuffer> imageData;
    if (!buffer->meta()->findBuffer("image-data", &imageData)) {
        return android::OK;
    }

    int64_t timestamp = 0;
    if (!input && buffer->meta()->findInt64("timeUs", &timestamp)) {
        timestamp *= 1000; // adjust to ns
    }

    AutoPtr<IByteBuffer> byteBuffer;
    err = createByteBufferFromABuffer(
            !input /* readOnly */, input /* clearBuffer */, buffer,
            (IByteBuffer**)&byteBuffer);
    if (err != android::OK) {
        return android::OK;
    }

    AutoPtr<IByteBuffer> infoBuffer;
    err = createByteBufferFromABuffer(
            true /* readOnly */, true /* clearBuffer */, imageData,
            (IByteBuffer**)&infoBuffer);
    if (err != android::OK) {
        return android::OK;
    }

    AutoPtr<IRect> cropRect;
    int32_t left, top, right, bottom;
    if (buffer->meta()->findRect("crop-rect", &left, &top, &right, &bottom)) {
        CRect::New(left, top, right + 1, bottom + 1, (IRect**)&cropRect);
    }

    AutoPtr<IMediaCodecMediaImage> image;
    CMediaCodecMediaImage::New(
            byteBuffer, infoBuffer,
            (Boolean)!input /* readOnly */,
            (Int64)timestamp,
            (Int32)0 /* xOffset */, (Int32)0 /* yOffset */, cropRect,
            (IMediaCodecMediaImage**)&image);

    *buf = IImage::Probe(image);
    REFCOUNT_ADD(*buf)
    return android::OK;
}

status_t JMediaCodec::getName(String *nameStr) const
{
    AString name;

    status_t err = mCodec->getName(&name);

    if (err != android::OK) {
        return err;
    }

    *nameStr = String(name.c_str());

    return android::OK;
}

status_t JMediaCodec::setParameters(const sp<AMessage> &msg) {
    return mCodec->setParameters(msg);
}

void JMediaCodec::setVideoScalingMode(int mode) {
    if (mSurfaceTextureClient != NULL) {
        native_window_set_scaling_mode(mSurfaceTextureClient.get(), mode);
    }
}

// static jthrowable createCodecException(
//         JNIEnv *env, status_t err, int32_t actionCode, const char *msg = NULL) {
//     ScopedLocalRef<jclass> clazz(
//             env, env->FindClass("android/media/MediaCodec$CodecException"));
//     CHECK(clazz.get() != NULL);

//     const jmethodID ctor = env->GetMethodID(clazz.get(), "<init>", "(IILjava/lang/String;)V");
//     CHECK(ctor != NULL);

//     ScopedLocalRef<jstring> msgObj(
//             env, env->NewStringUTF(msg != NULL ? msg : String8::format("Error %#x", err)));

//     // translate action code to Java equivalent
//     switch (actionCode) {
//     case ACTION_CODE_TRANSIENT:
//         actionCode = gCodecActionCodes.codecActionTransient;
//         break;
//     case ACTION_CODE_RECOVERABLE:
//         actionCode = gCodecActionCodes.codecActionRecoverable;
//         break;
//     default:
//         actionCode = 0;  // everything else is fatal
//         break;
//     }

//     return (jthrowable)env->NewObject(clazz.get(), ctor, err, actionCode, msgObj.get());
// }

ECode JMediaCodec::handleCallback(const sp<AMessage> &msg)
{
    int32_t arg1, arg2 = 0;
    AutoPtr<IInterface> obj;
    CHECK(msg->findInt32("callbackID", &arg1));

    switch (arg1) {
        case android::MediaCodec::CB_INPUT_AVAILABLE:
        {
            CHECK(msg->findInt32("index", &arg2));
            break;
        }

        case android::MediaCodec::CB_OUTPUT_AVAILABLE:
        {
            CHECK(msg->findInt32("index", &arg2));

            size_t size, offset;
            int64_t timeUs;
            uint32_t flags;
            CHECK(msg->findSize("size", &size));
            CHECK(msg->findSize("offset", &offset));
            CHECK(msg->findInt64("timeUs", &timeUs));
            CHECK(msg->findInt32("flags", (int32_t *)&flags));

            AutoPtr<IMediaCodecBufferInfo> bufferInfo;
            CMediaCodecBufferInfo::New((IMediaCodecBufferInfo**)&bufferInfo);

            if (bufferInfo == NULL) {
                ALOGE("Could not create MediaCodec.BufferInfo.");
                // jniThrowException(env, "java/lang/IllegalStateException", NULL);
                return E_ILLEGAL_STATE_EXCEPTION;
            }

            bufferInfo->Set((Int32)offset, (Int32)size, timeUs, flags);
            obj = bufferInfo;
            break;
        }

        case android::MediaCodec::CB_ERROR:
        {
            int32_t err, actionCode;
            CHECK(msg->findInt32("err", &err));
            CHECK(msg->findInt32("actionCode", &actionCode));

            // // note that DRM errors could conceivably alias into a CodecException
            // obj = createCodecException(err, actionCode);

            // if (obj == NULL) {
            //     ALOGE("Could not create CodecException object.");
            //     // jniThrowException(env, "java/lang/IllegalStateException", NULL);
            //     return E_ILLEGAL_STATE_EXCEPTION;
            // }

            return E_MEDIACODEC_CODEC_EXCEPTION;
            break;
        }

        case android::MediaCodec::CB_OUTPUT_FORMAT_CHANGED:
        {
            sp<AMessage> format;
            CHECK(msg->findMessage("format", &format));

            AutoPtr<IMap> map;
            status_t err = Media_Utils::ConvertMessageToMap(format, (IMap**)&map);
            if (android::OK != err) {
                // jniThrowException(env, "java/lang/IllegalStateException", NULL);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            obj = map;
            break;
        }

        default:
            TRESPASS();
    }

    mObject->PostEventFromNative(CMediaCodec::EVENT_CALLBACK, arg1, arg2, obj);
    return NOERROR;
}

void JMediaCodec::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatCallbackNotify:
        {
            handleCallback(msg);
            break;
        }
        default:
            TRESPASS();
    }
}

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
            {    AutoLock syncLock(lock);
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
            {    AutoLock syncLock(lock);
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

ECode CMediaCodec::CreateInputSurface(
    /* [out] */ ISurface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    ALOGV("android_media_MediaCodec_createInputSurface");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NOERROR;
    }

    // Tell the MediaCodec that we want to use a Surface as input.
    android::sp<android::IGraphicBufferProducer> bufferProducer;
    android::status_t err = codec->createInputSurface(&bufferProducer);
    if (err != android::NO_ERROR) {
        // throwExceptionAsNecessary(env, err);
        return NOERROR;
    }

    // Wrap the IGBP in a Java-language Surface.
    android::sp<android::Surface> surface(new android::Surface(bufferProducer, true));
    if (surface == NULL) {
        return NOERROR;
    }
    CSurface::New((Int64)surface.get(), result);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodec::Start()
{
    NativeStart();
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
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
        {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
        InvalidateByteBuffer(mCachedOutputBuffers, index);
        mDequeuedOutputBuffers->Remove(index);
    }
    NativeReleaseOutputBuffer(
            index, TRUE /* render */, TRUE /* updatePTS */, renderTimestampNs);
    return NOERROR;
}

ECode CMediaCodec::SignalEndOfInputStream()
{
    ALOGV("android_media_MediaCodec_signalEndOfInputStream");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NOERROR;
    }

    android::status_t err = codec->signalEndOfInputStream();

    // throwExceptionAsNecessary(env, err);
    return NOERROR;
}

ECode CMediaCodec::GetOutputFormat(
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = NativeGetFormatNative(FALSE /* input */);
    return CMediaFormat::New(map, result);
}

ECode CMediaCodec::GetInputFormat(
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = NativeGetFormatNative(TRUE /* input */);
    return CMediaFormat::New(map, result);
}

ECode CMediaCodec::GetOutputFormat(
    /* [in] */ Int32 index,
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMap> map = NativeGetOutputFormatNative(index);
    return CMediaFormat::New(map, result);
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
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
        InvalidateByteBuffer(mCachedOutputBuffers, index);
        mDequeuedOutputBuffers->Put(index, newImage);
    }
    *result = newImage;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodec::SetVideoScalingMode(
    /* [in] */ Int32 mode)
{
    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NOERROR;
    }

    if (mode != NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW
            && mode != NATIVE_WINDOW_SCALING_MODE_SCALE_CROP) {
        // jniThrowException(env, "java/lang/InvalidArgumentException", NULL);
        return NOERROR;
    }

    codec->setVideoScalingMode(mode);
    return NOERROR;
}

ECode CMediaCodec::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    ALOGV("android_media_MediaCodec_getName");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        *result = String(NULL);
        return NOERROR;
    }

    String name;
    android::status_t err = codec->getName(&name);

    if (err == android::OK) {
        *result = name;
        return NOERROR;
    }

    // throwExceptionAsNecessary(env, err);

    *result = String(NULL);
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

void CMediaCodec::NativeReset()
{
    ALOGV("android_media_MediaCodec_reset");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    android::status_t err = codec->reset();
    if (err != android::OK) {
        // treat all errors as fatal for now, though resource not available
        // errors could be treated as transient.
        // we also should avoid sending INVALID_OPERATION here due to
        // the transitory nature of reset(), it should not inadvertently
        // trigger an IllegalStateException.
        err = android::UNKNOWN_ERROR;
    }
    // throwExceptionAsNecessary(env, err);
}

void CMediaCodec::NativeRelease()
{
    mNativeContext = 0;
}

void CMediaCodec::NativeSetCallback(
    /* [in] */ IMediaCodecCallback* cb)
{
    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    android::status_t err = codec->setCallback(cb);

    // throwExceptionAsNecessary(env, err);
}

ECode CMediaCodec::NativeConfigure(
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [in] */ ISurface* jSurface,
    /* [in] */ IMediaCrypto* jCrypto,
    /* [in] */ Int32 flags)
{
    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NOERROR;
    }

    android::sp<android::AMessage> format;
    android::status_t err;
    err = Media_Utils::ConvertKeyValueArraysToMessage(keys, values, &format);

    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::IGraphicBufferProducer> bufferProducer;
    if (jSurface != NULL) {
        CSurface* surImpl = (CSurface*)jSurface;
        android::sp<android::Surface> surface = reinterpret_cast<android::Surface*>(surImpl->mNativeObject);

        if (surface != NULL) {
            bufferProducer = surface->getIGraphicBufferProducer();
        }
        else {
            // jniThrowException(
            //         env,
            //         "java/lang/IllegalArgumentException",
            //         "The surface has been released");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    android::sp<android::ICrypto> crypto;
    if (jCrypto != NULL) {
        crypto = JCrypto::GetCrypto(jCrypto);
    }

    err = codec->configure(format, bufferProducer, crypto, flags);

    // throwExceptionAsNecessary(env, err);
    return NOERROR;
}

void CMediaCodec::NativeStart()
{
    ALOGV("android_media_MediaCodec_start");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    android::status_t err = codec->start();

    // throwExceptionAsNecessary(env, err, ACTION_CODE_FATAL, "start failed");
}

void CMediaCodec::NativeStop()
{
    ALOGV("android_media_MediaCodec_stop");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    android::status_t err = codec->stop();

    // throwExceptionAsNecessary(env, err);
}

void CMediaCodec::NativeFlush()
{
    ALOGV("android_media_MediaCodec_flush");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    android::status_t err = codec->flush();

    // throwExceptionAsNecessary(env, err);
}

void CMediaCodec::NativeQueueInputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ Int64 timestampUs,
    /* [in] */ Int32 flags)
{
    ALOGV("android_media_MediaCodec_queueInputBuffer");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    AString errorDetailMsg;

    android::status_t err = codec->queueInputBuffer(
            index, offset, size, timestampUs, flags, &errorDetailMsg);

    // throwExceptionAsNecessary(
    //         env, err, ACTION_CODE_FATAL, errorDetailMsg.empty() ? NULL : errorDetailMsg.c_str());
}

void CMediaCodec::NativeQueueSecureInputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Int32 offset,
    /* [in] */ IMediaCodecCryptoInfo* cryptoInfoObj,
    /* [in] */ Int64 timestampUs,
    /* [in] */ Int32 flags)
{
    ALOGV("android_media_MediaCodec_queueSecureInputBuffer");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    Int32 numSubSamples;
    cryptoInfoObj->GetNumSubSamples(&numSubSamples);

    AutoPtr<ArrayOf<Int32> > numBytesOfClearDataObj;
    cryptoInfoObj->GetNumBytesOfClearData((ArrayOf<Int32>**)&numBytesOfClearDataObj);

    AutoPtr<ArrayOf<Int32> > numBytesOfEncryptedDataObj;
    cryptoInfoObj->GetNumBytesOfEncryptedData((ArrayOf<Int32>**)&numBytesOfEncryptedDataObj);

    AutoPtr<ArrayOf<Byte> > keyObj;
    cryptoInfoObj->GetKey((ArrayOf<Byte>**)&keyObj);

    AutoPtr<ArrayOf<Byte> > ivObj;
    cryptoInfoObj->GetIv((ArrayOf<Byte>**)&ivObj);

    Int32 mode;
    cryptoInfoObj->GetMode(&mode);

    android::CryptoPlugin::Pattern pattern;
    pattern.mEncryptBlocks = 0;
    pattern.mSkipBlocks = 0;

    android::status_t err = android::OK;

    android::CryptoPlugin::SubSample *subSamples = NULL;
    Byte *key = NULL;
    Byte *iv = NULL;

    if (numSubSamples <= 0) {
        err = -EINVAL;
    }
    else if (numBytesOfClearDataObj == NULL
            && numBytesOfEncryptedDataObj == NULL) {
        err = -EINVAL;
    }
    else if (numBytesOfEncryptedDataObj != NULL
            && numBytesOfEncryptedDataObj->GetLength() < numSubSamples) {
        err = -ERANGE;
    }
    else if (numBytesOfClearDataObj != NULL
            && numBytesOfClearDataObj->GetLength() < numSubSamples) {
        err = -ERANGE;
    // subSamples array may silently overflow if number of samples are too large.  Use
    // INT32_MAX as maximum allocation size may be less than SIZE_MAX on some platforms
    }
    else if ( CC_UNLIKELY(numSubSamples >= (signed)(INT32_MAX / sizeof(*subSamples))) ) {
        err = -EINVAL;
    }
    else {
        Int32 *numBytesOfClearData =
            (numBytesOfClearDataObj == NULL)
                ? NULL
                : numBytesOfClearDataObj->GetPayload();

        Int32 *numBytesOfEncryptedData =
            (numBytesOfEncryptedDataObj == NULL)
                ? NULL
                : numBytesOfEncryptedDataObj->GetPayload();

        subSamples = new android::CryptoPlugin::SubSample[numSubSamples];

        for (Int32 i = 0; i < numSubSamples; ++i) {
            subSamples[i].mNumBytesOfClearData =
                (numBytesOfClearData == NULL) ? 0 : numBytesOfClearData[i];

            subSamples[i].mNumBytesOfEncryptedData =
                (numBytesOfEncryptedData == NULL)
                    ? 0 : numBytesOfEncryptedData[i];
        }
    }

    if (err == android::OK && keyObj != NULL) {
        if (keyObj->GetLength() != 16) {
            err = -EINVAL;
        }
        else {
            key = keyObj->GetPayload();
        }
    }

    if (err == android::OK && ivObj != NULL) {
        if (ivObj->GetLength() != 16) {
            err = -EINVAL;
        }
        else {
            iv = ivObj->GetPayload();
        }
    }

    AString errorDetailMsg;

    if (err == android::OK) {
        err = codec->queueSecureInputBuffer(
                index, offset,
                subSamples, numSubSamples,
                (const uint8_t *)key, (const uint8_t *)iv,
                (android::CryptoPlugin::Mode)mode,
                pattern,
                timestampUs,
                flags,
                &errorDetailMsg);
    }

    delete[] subSamples;
    subSamples = NULL;

    // throwExceptionAsNecessary(
    //         env, err, ACTION_CODE_FATAL, errorDetailMsg.empty() ? NULL : errorDetailMsg.c_str());
}

Int32 CMediaCodec::NativeDequeueInputBuffer(
    /* [in] */ Int64 timeoutUs)
{
    ALOGV("android_media_MediaCodec_dequeueInputBuffer");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return -1;
    }

    size_t index;
    android::status_t err = codec->dequeueInputBuffer(&index, timeoutUs);

    if (err == android::OK) {
        return (Int32) index;
    }

    // return throwExceptionAsNecessary(env, err);
    return 0;
}

Int32 CMediaCodec::NativeDequeueOutputBuffer(
    /* [in] */ IMediaCodecBufferInfo* bufferInfo,
    /* [in] */ Int64 timeoutUs)
{
    ALOGV("android_media_MediaCodec_dequeueOutputBuffer");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return 0;
    }

    size_t index;
    android::status_t err = codec->dequeueOutputBuffer(
            bufferInfo, &index, timeoutUs);

    if (err == android::OK) {
        return (Int32) index;
    }

    // return throwExceptionAsNecessary(env, err);
    return 0;
}

void CMediaCodec::NativeReleaseOutputBuffer(
    /* [in] */ Int32 index,
    /* [in] */ Boolean render,
    /* [in] */ Boolean updatePTS,
    /* [in] */ Int64 timestampNs)
{
    ALOGV("android_media_MediaCodec_renderOutputBufferAndRelease");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    android::status_t err = codec->releaseOutputBuffer(index, render, updatePTS, timestampNs);

    // throwExceptionAsNecessary(env, err);
}

AutoPtr<IMap> CMediaCodec::NativeGetFormatNative(
    /* [in] */ Boolean input)
{
    ALOGV("android_media_MediaCodec_getFormatNative");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NULL;
    }

    AutoPtr<IMap> format;
    android::status_t err = codec->getFormat(input, (IMap**)&format);

    if (err == android::OK) {
        return format;
    }

    // throwExceptionAsNecessary(env, err);

    return NULL;
}

AutoPtr<IMap> CMediaCodec::NativeGetOutputFormatNative(
    /* [in] */ Int32 index)
{
    ALOGV("android_media_MediaCodec_getOutputFormatForIndexNative");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NULL;
    }

    AutoPtr<IMap> format;
    android::status_t err = codec->getOutputFormat(index, (IMap**)&format);

    if (err == android::OK) {
        return format;
    }

    // throwExceptionAsNecessary(env, err);

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
    {    AutoLock syncLock(mBufferLock);
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
    {    AutoLock syncLock(mBufferLock);
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

void CMediaCodec::NativeSetParameters(
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    ALOGV("android_media_MediaCodec_setParameters");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return;
    }

    android::sp<android::AMessage> params;
    android::status_t err;
    err = Media_Utils::ConvertKeyValueArraysToMessage(keys, values, &params);

    if (err == android::OK) {
        err = codec->setParameters(params);
    }

    // throwExceptionAsNecessary(env, err);
}

AutoPtr<ArrayOf<IByteBuffer*> > CMediaCodec::NativeGetBuffers(
    /* [in] */ Boolean input)
{
    ALOGV("android_media_MediaCodec_getBuffers");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NULL;
    }

    AutoPtr<ArrayOf<IByteBuffer*> > buffers;
    android::status_t err = codec->getBuffers(input, (ArrayOf<IByteBuffer*>**)&buffers);

    if (err == android::OK) {
        return buffers;
    }

    // if we're out of memory, an exception was already thrown
    // if (err != NO_MEMORY) {
    //     throwExceptionAsNecessary(env, err);
    // }

    return NULL;
}

AutoPtr<IByteBuffer> CMediaCodec::NativeGetBuffer(
    /* [in] */ Boolean input,
    /* [in] */ Int32 index)
{
    ALOGV("android_media_MediaCodec_getBuffer");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NULL;
    }

    AutoPtr<IByteBuffer> buffer;
    android::status_t err = codec->getBuffer(input, index, (IByteBuffer**)&buffer);

    if (err == android::OK) {
        return buffer;
    }

    // if we're out of memory, an exception was already thrown
    // if (err != NO_MEMORY) {
    //     throwExceptionAsNecessary(env, err);
    // }

    return NULL;
}

AutoPtr<IImage> CMediaCodec::NativeGetImage(
    /* [in] */ Boolean input,
    /* [in] */ Int32 index)
{
    ALOGV("android_media_MediaCodec_getImage");

    android::sp<JMediaCodec> codec = (JMediaCodec*)mNativeContext;

    if (codec == NULL) {
        // throwExceptionAsNecessary(env, INVALID_OPERATION);
        return NULL;
    }

    AutoPtr<IImage> image;
    android::status_t err = codec->getImage(input, index, (IImage**)&image);

    if (err == android::OK) {
        return image;
    }

    // if we're out of memory, an exception was already thrown
    // if (err != NO_MEMORY) {
    //     throwExceptionAsNecessary(env, err);
    // }

    return NULL;
}

void CMediaCodec::NativeInit()
{
}

ECode CMediaCodec::NativeSetup(
    /* [in] */ const String& name,
    /* [in] */ Boolean nameIsType,
    /* [in] */ Boolean encoder)
{
    if (name == NULL) {
        // jniThrowException(env, "java/lang/NullPointerException", NULL);
        return E_NULL_POINTER_EXCEPTION;
    }

    const char *tmp = name.string();

    if (tmp == NULL) {
        return NOERROR;
    }

    android::sp<JMediaCodec> codec = new JMediaCodec(this, tmp, nameIsType, encoder);

    const android::status_t err = codec->initCheck();
    if (err == android::NAME_NOT_FOUND) {
        // fail and do not try again.
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //         String8::format("Failed to initialize %s, error %#x", tmp, err));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (err != android::OK) {
        // believed possible to try again
        // jniThrowException(env, "java/io/IOException",
        //         String8::format("Failed to find matching codec %s, error %#x", tmp, err));
        return E_IO_EXCEPTION;
    }

    codec->registerSelf();

    mNativeContext = (Int64) codec.get();

    return NOERROR;
}

void CMediaCodec::NativeFinalize()
{
    NativeRelease();
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos