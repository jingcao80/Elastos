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

#include "elastos/droid/server/tv/TvInputHal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.View.h>
#include <elastos/droid/os/NativeMessageQueue.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <android/hardware/tv/input/1.0/ITvInputCallback.h>
#include <android/hardware/tv/input/1.0/ITvInput.h>
#include <android/hardware/tv/input/1.0/types.h>
#include <gui/Surface.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/Log.h>
#include <utils/Looper.h>
#include <utils/NativeHandle.h>
#include <hardware/tv_input.h>

using Elastos::Droid::Media::Tv::CTvInputHardwareInfoBuilder;
using Elastos::Droid::Media::Tv::CTvStreamConfigBuilder;
using Elastos::Droid::Media::Tv::EIID_ITvStreamConfig;
using Elastos::Droid::Media::Tv::ITvInputHardwareInfoBuilder;
using Elastos::Droid::Media::Tv::ITvStreamConfigBuilder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::MessageQueue;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Core::AutoLock;
using Elastos::Core::CArrayOf;
using Elastos::Core::CObject;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Logging::Slogger;

using android::hardware::audio::common::V2_0::AudioDevice;
using android::hardware::tv::input::V1_0::ITvInput;
using android::hardware::tv::input::V1_0::ITvInputCallback;
using android::hardware::tv::input::V1_0::Result;
using android::hardware::tv::input::V1_0::TvInputDeviceInfo;
using android::hardware::tv::input::V1_0::TvInputEvent;
using android::hardware::tv::input::V1_0::TvInputType;
using android::hardware::tv::input::V1_0::TvStreamConfig;
using android::hardware::Return;
using android::hardware::hidl_vec;

//=============================================================================
// native
//=============================================================================
namespace android {

////////////////////////////////////////////////////////////////////////////////

class BufferProducerThread : public Thread {
public:
    BufferProducerThread(tv_input_device_t* device, int deviceId, const tv_stream_t* stream);

    virtual status_t readyToRun();

    void setSurface(const sp<Surface>& surface);
    void onCaptured(uint32_t seq, bool succeeded);
    void shutdown();

private:
    Mutex mLock;
    Condition mCondition;
    sp<Surface> mSurface;
    tv_input_device_t* mDevice;
    int mDeviceId;
    tv_stream_t mStream;
    sp<ANativeWindowBuffer_t> mBuffer;
    enum {
        CAPTURING,
        CAPTURED,
        RELEASED,
    } mBufferState;
    uint32_t mSeq;
    bool mShutdown;

    virtual bool threadLoop();

    void setSurfaceLocked(const sp<Surface>& surface);
};

BufferProducerThread::BufferProducerThread(
        tv_input_device_t* device, int deviceId, const tv_stream_t* stream)
    : Thread(false),
      mDevice(device),
      mDeviceId(deviceId),
      mBuffer(NULL),
      mBufferState(RELEASED),
      mSeq(0u),
      mShutdown(false) {
    memcpy(&mStream, stream, sizeof(mStream));
}

Int32 BufferProducerThread::readyToRun() {
    sp<ANativeWindow> anw(mSurface);
    status_t err = native_window_set_usage(anw.get(), mStream.buffer_producer.usage);
    if (err != NO_ERROR) {
        return err;
    }
    err = native_window_set_buffers_dimensions(
            anw.get(), mStream.buffer_producer.width, mStream.buffer_producer.height);
    if (err != NO_ERROR) {
        return err;
    }
    err = native_window_set_buffers_format(anw.get(), mStream.buffer_producer.format);
    if (err != NO_ERROR) {
        return err;
    }
    return NO_ERROR;
}

void BufferProducerThread::setSurface(const sp<Surface>& surface) {
    Mutex::Autolock autoLock(&mLock);
    setSurfaceLocked(surface);
}

void BufferProducerThread::setSurfaceLocked(const sp<Surface>& surface) {
    if (surface == mSurface) {
        return;
    }

    if (mBufferState == CAPTURING) {
        mDevice->cancel_capture(mDevice, mDeviceId, mStream.stream_id, mSeq);
    }
    while (mBufferState == CAPTURING) {
        status_t err = mCondition.waitRelative(mLock, s2ns(1));
        if (err != NO_ERROR) {
            ALOGE("error %d while wating for buffer state to change.", err);
            break;
        }
    }
    mBuffer.clear();
    mBufferState = RELEASED;

    mSurface = surface;
    mCondition.broadcast();
}

void BufferProducerThread::onCaptured(uint32_t seq, bool succeeded) {
    Mutex::Autolock autoLock(&mLock);
    if (seq != mSeq) {
        ALOGW("Incorrect sequence value: expected %u actual %u", mSeq, seq);
    }
    if (mBufferState != CAPTURING) {
        ALOGW("mBufferState != CAPTURING : instead %d", mBufferState);
    }
    if (succeeded) {
        mBufferState = CAPTURED;
    } else {
        mBuffer.clear();
        mBufferState = RELEASED;
    }
    mCondition.broadcast();
}

void BufferProducerThread::shutdown() {
    Mutex::Autolock autoLock(&mLock);
    mShutdown = true;
    setSurfaceLocked(NULL);
    requestExitAndWait();
}

bool BufferProducerThread::threadLoop() {
    Mutex::Autolock autoLock(&mLock);

    status_t err = NO_ERROR;
    if (mSurface == NULL) {
        err = mCondition.waitRelative(mLock, s2ns(1));
        // It's OK to time out here.
        if (err != NO_ERROR && err != TIMED_OUT) {
            ALOGE("error %d while wating for non-null surface to be set", err);
            return false;
        }
        return true;
    }
    sp<ANativeWindow> anw(mSurface);
    while (mBufferState == CAPTURING) {
        err = mCondition.waitRelative(mLock, s2ns(1));
        if (err != NO_ERROR) {
            ALOGE("error %d while wating for buffer state to change.", err);
            return false;
        }
    }
    if (mBufferState == CAPTURED && anw != NULL) {
        err = anw->queueBuffer(anw.get(), mBuffer.get(), -1);
        if (err != NO_ERROR) {
            ALOGE("error %d while queueing buffer to surface", err);
            return false;
        }
        mBuffer.clear();
        mBufferState = RELEASED;
    }
    if (mBuffer == NULL && !mShutdown && anw != NULL) {
        ANativeWindowBuffer_t* buffer = NULL;
        err = native_window_dequeue_buffer_and_wait(anw.get(), &buffer);
        if (err != NO_ERROR) {
            ALOGE("error %d while dequeueing buffer to surface", err);
            return false;
        }
        mBuffer = buffer;
        mBufferState = CAPTURING;
        mDevice->request_capture(mDevice, mDeviceId, mStream.stream_id,
                                 buffer->handle, ++mSeq);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

class JTvInputHal {
public:
    ~JTvInputHal();

    static JTvInputHal* createInstance(Elastos::Droid::Server::Tv::TvInputHal* thiz, const sp<Looper>& looper);

    int addStream(int deviceId, int streamId, const sp<Surface>& surface);
    int removeStream(int deviceId, int streamId);
    const hidl_vec<TvStreamConfig> getStreamConfigs(int deviceId);

    void onDeviceAvailable(const TvInputDeviceInfo& info);
    void onDeviceUnavailable(int deviceId);
    void onStreamConfigurationsChanged(int deviceId);
    void onCaptured(int deviceId, int streamId, uint32_t seq, bool succeeded);

private:
    // Connection between a surface and a stream.
    class Connection {
    public:
        Connection() {}

        sp<Surface> mSurface;
        tv_stream_type_t mStreamType;

        // Only valid when mStreamType == TV_STREAM_TYPE_INDEPENDENT_VIDEO_SOURCE
        sp<NativeHandle> mSourceHandle;
        // Only valid when mStreamType == TV_STREAM_TYPE_BUFFER_PRODUCER
        sp<BufferProducerThread> mThread;
    };

    class NotifyHandler : public MessageHandler {
    public:
        NotifyHandler(JTvInputHal* hal, const TvInputEvent& event);

        virtual void handleMessage(const Message& message);

    private:
        TvInputEvent mEvent;
        JTvInputHal* mHal;
    };

    class TvInputCallback : public ITvInputCallback {
    public:
        TvInputCallback(JTvInputHal* hal);
        Return<void> notify(const TvInputEvent& event) override;
    private:
        JTvInputHal* mHal;
    };

    JTvInputHal(Elastos::Droid::Server::Tv::TvInputHal* thiz, sp<ITvInput> tvInput, const sp<Looper>& looper);

    Mutex mLock;
    Elastos::Droid::Server::Tv::TvInputHal* mThiz;
    sp<Looper> mLooper;

    KeyedVector<int, KeyedVector<int, Connection> > mConnections;

    sp<ITvInput> mTvInput;
    sp<ITvInputCallback> mTvInputCallback;
};

JTvInputHal::JTvInputHal(Elastos::Droid::Server::Tv::TvInputHal* thiz, sp<ITvInput> tvInput, const sp<Looper>& looper) {
    mThiz = thiz;
    mTvInput = tvInput;
    mLooper = looper;
    mTvInputCallback = new TvInputCallback(this);
    mTvInput->setCallback(mTvInputCallback);
}

JTvInputHal::~JTvInputHal() {
    mTvInput->setCallback(nullptr);

    mThiz = NULL;
}

JTvInputHal* JTvInputHal::createInstance(Elastos::Droid::Server::Tv::TvInputHal* thiz, const sp<Looper>& looper) {
    sp<ITvInput> tvInput = ITvInput::getService();
    if (tvInput == nullptr) {
        ALOGE("Couldn't get tv.input service.");
        return nullptr;
    }

    return new JTvInputHal(thiz, tvInput, looper);
}

int JTvInputHal::addStream(int deviceId, int streamId, const sp<Surface>& surface) {
    KeyedVector<int, Connection>& connections = mConnections.editValueFor(deviceId);
    if (connections.indexOfKey(streamId) < 0) {
        connections.add(streamId, Connection());
    }
    Connection& connection = connections.editValueFor(streamId);
    if (connection.mSurface == surface) {
        // Nothing to do
        return NO_ERROR;
    }
    // Clear the surface in the connection.
    if (connection.mSurface != NULL) {
        if (connection.mStreamType == TV_STREAM_TYPE_INDEPENDENT_VIDEO_SOURCE) {
            if (Surface::isValid(connection.mSurface)) {
                connection.mSurface->setSidebandStream(NULL);
            }
        }
        connection.mSurface.clear();
    }
    if (connection.mSourceHandle == NULL && connection.mThread == NULL) {
        // Need to configure stream
        Result result = Result::UNKNOWN;
        hidl_vec<TvStreamConfig> list;
        mTvInput->getStreamConfigurations(deviceId,
                [&result, &list](Result res, hidl_vec<TvStreamConfig> configs) {
                    result = res;
                    if (res == Result::OK) {
                        list = configs;
                    }
                });
        if (result != Result::OK) {
            ALOGE("Couldn't get stream configs for device id:%d result:%d", deviceId, result);
            return UNKNOWN_ERROR;
        }
        int configIndex = -1;
        for (size_t i = 0; i < list.size(); ++i) {
            if (list[i].streamId == streamId) {
                configIndex = i;
                break;
            }
        }
        if (configIndex == -1) {
            ALOGE("Cannot find a config with given stream ID: %d", streamId);
            return BAD_VALUE;
        }
        connection.mStreamType = TV_STREAM_TYPE_INDEPENDENT_VIDEO_SOURCE;

        result = Result::UNKNOWN;
        const native_handle_t* sidebandStream;
        mTvInput->openStream(deviceId, streamId,
                [&result, &sidebandStream](Result res, const native_handle_t* handle) {
                    result = res;
                    if (res == Result::OK) {
                        sidebandStream = handle;
                    }
                });
        if (result != Result::OK) {
            ALOGE("Couldn't open stream. device id:%d stream id:%d result:%d", deviceId, streamId,
                    result);
            return UNKNOWN_ERROR;
        }
        connection.mSourceHandle = NativeHandle::create(
                (native_handle_t*)sidebandStream, false);
    }
    connection.mSurface = surface;
    if (connection.mSurface != nullptr) {
        connection.mSurface->setSidebandStream(connection.mSourceHandle);
    }
    return NO_ERROR;
}

int JTvInputHal::removeStream(int deviceId, int streamId) {
    KeyedVector<int, Connection>& connections = mConnections.editValueFor(deviceId);
    if (connections.indexOfKey(streamId) < 0) {
        return BAD_VALUE;
    }
    Connection& connection = connections.editValueFor(streamId);
    if (connection.mSurface == NULL) {
        // Nothing to do
        return NO_ERROR;
    }
    if (Surface::isValid(connection.mSurface)) {
        connection.mSurface->setSidebandStream(NULL);
    }
    connection.mSurface.clear();
    if (connection.mThread != NULL) {
        connection.mThread->shutdown();
        connection.mThread.clear();
    }
    if (mTvInput->closeStream(deviceId, streamId) != Result::OK) {
        ALOGE("Couldn't close stream. device id:%d stream id:%d", deviceId, streamId);
        return BAD_VALUE;
    }
    if (connection.mSourceHandle != NULL) {
        connection.mSourceHandle.clear();
    }
    return NO_ERROR;
}

const hidl_vec<TvStreamConfig> JTvInputHal::getStreamConfigs(int deviceId) {
    Result result = Result::UNKNOWN;
    hidl_vec<TvStreamConfig> list;
    mTvInput->getStreamConfigurations(deviceId,
            [&result, &list](Result res, hidl_vec<TvStreamConfig> configs) {
                result = res;
                if (res == Result::OK) {
                    list = configs;
                }
            });
    if (result != Result::OK) {
        ALOGE("Couldn't get stream configs for device id:%d result:%d", deviceId, result);
    }
    return list;
}

void JTvInputHal::onDeviceAvailable(const TvInputDeviceInfo& info) {
    {
        Mutex::Autolock autoLock(&mLock);
        mConnections.add(info.deviceId, KeyedVector<int, Connection>());
    }

    AutoPtr<ITvInputHardwareInfoBuilder> builder;
    CTvInputHardwareInfoBuilder::New((ITvInputHardwareInfoBuilder**)&builder);
    builder->DeviceId(info.deviceId);
    builder->Type(static_cast<Int32>(info.type));
    if (info.type == TvInputType::HDMI) {
        builder->HdmiPortId(info.portId);
    }
    builder->AudioType(static_cast<Int32>(info.audioType));
    if (info.audioType != AudioDevice::NONE) {
        uint8_t buffer[info.audioAddress.size() + 1];
        memcpy(buffer, info.audioAddress.data(), info.audioAddress.size());
        buffer[info.audioAddress.size()] = '\0';
        String audioAddress(reinterpret_cast<const char *>(buffer));
        builder->AudioAddress(audioAddress);
    }

    AutoPtr<ITvInputHardwareInfo> infoObject;
    builder->Build((ITvInputHardwareInfo**)&infoObject);

    mThiz->DeviceAvailableFromNative(infoObject);
}

void JTvInputHal::onDeviceUnavailable(int deviceId) {
    {
        Mutex::Autolock autoLock(&mLock);
        KeyedVector<int, Connection>& connections = mConnections.editValueFor(deviceId);
        for (size_t i = 0; i < connections.size(); ++i) {
            removeStream(deviceId, connections.keyAt(i));
        }
        connections.clear();
        mConnections.removeItem(deviceId);
    }
    mThiz->DeviceUnavailableFromNative(deviceId);
}

void JTvInputHal::onStreamConfigurationsChanged(int deviceId) {
    {
        Mutex::Autolock autoLock(&mLock);
        KeyedVector<int, Connection>& connections = mConnections.editValueFor(deviceId);
        for (size_t i = 0; i < connections.size(); ++i) {
            removeStream(deviceId, connections.keyAt(i));
        }
        connections.clear();
    }
    mThiz->StreamConfigsChangedFromNative(deviceId);
}

void JTvInputHal::onCaptured(int deviceId, int streamId, uint32_t seq, bool succeeded) {
    sp<BufferProducerThread> thread;
    {
        Mutex::Autolock autoLock(&mLock);
        KeyedVector<int, Connection>& connections = mConnections.editValueFor(deviceId);
        Connection& connection = connections.editValueFor(streamId);
        if (connection.mThread == NULL) {
            ALOGE("capture thread not existing.");
            return;
        }
        thread = connection.mThread;
    }
    thread->onCaptured(seq, succeeded);
    if (seq == 0) {
        mThiz->FirstFrameCapturedFromNative(deviceId, streamId);
    }
}

////////////////////////////////////////////////////////////////////////////////

static Int64 nativeOpen(Elastos::Droid::Server::Tv::TvInputHal* thiz, IMessageQueue* messageQueueObj) {
    HANDLE ptr;
    messageQueueObj->GetNativeMessageQueue(&ptr);
    AutoPtr<MessageQueue> messageQueue = reinterpret_cast<NativeMessageQueue*>(ptr);

    return (Int64)JTvInputHal::createInstance(thiz, messageQueue->GetLooper());
}

sp<Surface> android_view_Surface_getSurface(ISurface* surfaceObj) {
    Int64 nativeSurface;
    surfaceObj->GetNativeSurface(&nativeSurface);
    sp<Surface> sur = reinterpret_cast<Surface *>(nativeSurface);
    return sur;
}

static int nativeAddStream(Int64 ptr, Int32 deviceId, Int32 streamId, ISurface* jsurface) {
    JTvInputHal* tvInputHal = (JTvInputHal*)ptr;
    if (!jsurface) {
        return BAD_VALUE;
    }
    sp<Surface> surface(android_view_Surface_getSurface(jsurface));
    if (!Surface::isValid(surface)) {
        return BAD_VALUE;
    }
    return tvInputHal->addStream(deviceId, streamId, surface);
}

static int nativeRemoveStream(Int64 ptr, Int32 deviceId, Int32 streamId) {
    JTvInputHal* tvInputHal = (JTvInputHal*)ptr;
    return tvInputHal->removeStream(deviceId, streamId);
}

static AutoPtr<IArrayOf> nativeGetStreamConfigs(Int64 ptr, Int32 deviceId, Int32 generation) {
    JTvInputHal* tvInputHal = (JTvInputHal*)ptr;
    const hidl_vec<TvStreamConfig> configs = tvInputHal->getStreamConfigs(deviceId);

    AutoPtr<IArrayOf> result;
    CArrayOf::New(EIID_ITvStreamConfig, configs.size(), (IArrayOf**)&result);
    for (size_t i = 0; i < configs.size(); ++i) {
        AutoPtr<ITvStreamConfigBuilder> builder;
        CTvStreamConfigBuilder::New((ITvStreamConfigBuilder**)&builder);
        builder->StreamId(configs[i].streamId);
        builder->Type(TV_STREAM_TYPE_INDEPENDENT_VIDEO_SOURCE);
        builder->MaxWidth(configs[i].maxVideoWidth);
        builder->MaxHeight(configs[i].maxVideoHeight);
        builder->Generation(generation);

        AutoPtr<ITvStreamConfig> config;
        builder->Build((ITvStreamConfig**)&config);

        result->Set(i, config);
    }
    return result;
}

static void nativeClose(Int64 ptr) {
    JTvInputHal* tvInputHal = (JTvInputHal*)ptr;
    delete tvInputHal;
}

} // namespace android

namespace Elastos {
namespace Droid {
namespace Server {
namespace Tv {

//=============================================================================
// TvInputHal
//=============================================================================
const Boolean TvInputHal::DEBUG = FALSE;
const String TvInputHal::TAG("TvInputHal");
const Int32 TvInputHal::SUCCESS = 0;
const Int32 TvInputHal::ERROR_NO_INIT = -1;
const Int32 TvInputHal::ERROR_STALE_CONFIG = -2;
const Int32 TvInputHal::ERROR_UNKNOWN = -3;

const Int32 TvInputHal::EVENT_DEVICE_AVAILABLE = 1;
const Int32 TvInputHal::EVENT_DEVICE_UNAVAILABLE = 2;
const Int32 TvInputHal::EVENT_STREAM_CONFIGURATION_CHANGED = 3;
const Int32 TvInputHal::EVENT_FIRST_FRAME_CAPTURED = 4;

CAR_INTERFACE_IMPL(TvInputHal, Object, IHandlerCallback)

TvInputHal::TvInputHal()
    : mPtr(0)
{
    CSparseInt32Array::New((ISparseInt32Array**)&mStreamConfigGenerations);
    CSparseArray::New((ISparseArray**)&mStreamConfigs);
    CLinkedList::New((IQueue**)&mPendingMessageQueue);
}

TvInputHal::~TvInputHal()
{
    Close();
}

Int64 TvInputHal::NativeOpen(
    /* [in] */ TvInputHal* thiz,
    /* [in] */ IMessageQueue* queue)
{
    return android::nativeOpen(thiz,queue);
}

Int32 TvInputHal::NativeAddStream(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 streamId,
    /* [in] */ ISurface* surface)
{
    return android::nativeAddStream(ptr, deviceId, streamId, surface);
}

Int32 TvInputHal::NativeRemoveStream(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 streamId)
{
    return android::nativeRemoveStream(ptr, deviceId, streamId);
}

AutoPtr<IArrayOf> TvInputHal::NativeGetStreamConfigs(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 generation)
{
    return android::nativeGetStreamConfigs(ptr, deviceId, generation);
}

// void nativeClose(JNIEnv* env, jclass clazz, jlong ptr)
ECode TvInputHal::NativeClose(
    /* [in] */ Int64 ptr)
{
    android::nativeClose(ptr);
    return NOERROR;
}

ECode TvInputHal::constructor(
    /* [in] */ ITvInputHalCallback* callback)
{
    mCallback = callback;
    CHandler::New(this, FALSE, FALSE, (IHandler**)&mHandler);
    return NOERROR;
}

ECode TvInputHal::Init()
{
    {
        AutoLock syncLock(mLock);
        AutoPtr<ILooper> looper;
        mHandler->GetLooper((ILooper**)&looper);
        AutoPtr<IMessageQueue> msgQueue;
        looper->GetQueue((IMessageQueue**)&msgQueue);
        mPtr = NativeOpen(this, msgQueue);
    }
    return NOERROR;
}

ECode TvInputHal::AddStream(
    /* [in] */ Int32 deviceId,
    /* [in] */ ISurface* surface,
    /* [in] */ ITvStreamConfig* streamConfig,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock syncLock(mLock);
        if (mPtr == 0) {
            *result = ERROR_NO_INIT;
            return NOERROR;
        }
        Int32 generation, othGeneration;
        mStreamConfigGenerations->Get(deviceId, 0, &generation);
        streamConfig->GetGeneration(&othGeneration);
        if (generation != othGeneration) {
            *result = ERROR_STALE_CONFIG;
            return NOERROR;
        }
        Int32 streamId;
        streamConfig->GetStreamId(&streamId);
        if (NativeAddStream(mPtr, deviceId, streamId, surface) == 0) {
            *result = SUCCESS;
            return NOERROR;
        } else {
            *result = ERROR_UNKNOWN;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode TvInputHal::RemoveStream(
    /* [in] */ Int32 deviceId,
    /* [in] */ ITvStreamConfig* streamConfig,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock syncLock(mLock);
        if (mPtr == 0) {
            *result = ERROR_NO_INIT;
            return NOERROR;
        }
        Int32 generation, othGeneration;
        mStreamConfigGenerations->Get(deviceId, 0, &generation);
        streamConfig->GetGeneration(&othGeneration);
        if (generation != othGeneration) {
            *result = ERROR_STALE_CONFIG;
            return NOERROR;
        }
        Int32 streamId;
        streamConfig->GetStreamId(&streamId);
        if (NativeRemoveStream(mPtr, deviceId, streamId) == 0) {
            *result = SUCCESS;
            return NOERROR;
        } else {
            *result = ERROR_UNKNOWN;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode TvInputHal::Close()
{
    {    AutoLock syncLock(mLock);
        if (mPtr != 0l) {
            NativeClose(mPtr);
            mPtr = 0;
        }
    }
    return NOERROR;
}

ECode TvInputHal::RetrieveStreamConfigsLocked(
    /* [in] */ Int32 deviceId)
{
    Int32 generation;
    mStreamConfigGenerations->Get(deviceId, 0, &generation);
    generation += 1;
    mStreamConfigs->Put(deviceId, NativeGetStreamConfigs(mPtr, deviceId, generation));
    mStreamConfigGenerations->Put(deviceId, generation);
    return NOERROR;
}

ECode TvInputHal::DeviceAvailableFromNative(
    /* [in] */ ITvInputHardwareInfo* info)
{
    if (DEBUG) {
        Slogger::D(TAG, "deviceAvailableFromNative: info = %s", TO_CSTR(info));
    }
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_DEVICE_AVAILABLE, info, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TvInputHal::DeviceUnavailableFromNative(
    /* [in] */ Int32 deviceId)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_DEVICE_UNAVAILABLE, deviceId, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TvInputHal::StreamConfigsChangedFromNative(
    /* [in] */ Int32 deviceId)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_STREAM_CONFIGURATION_CHANGED, deviceId, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TvInputHal::FirstFrameCapturedFromNative(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 streamId)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_STREAM_CONFIGURATION_CHANGED, deviceId, streamId, (IMessage**)&msg);
    Boolean bNoUse;
    mHandler->SendMessage(msg, &bNoUse);
    return NOERROR;
}

ECode TvInputHal::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_DEVICE_AVAILABLE: {
            AutoPtr<IArrayOf> configs;
            AutoPtr<ITvInputHardwareInfo> info = ITvInputHardwareInfo::Probe(obj);
            {
                AutoLock syncLock(mLock);
                Int32 deviceId;
                info->GetDeviceId(&deviceId);
                RetrieveStreamConfigsLocked(deviceId);
                if (DEBUG) {
                    Slogger::D(TAG, "EVENT_DEVICE_AVAILABLE: info = %s", TO_CSTR(info));
                }
                AutoPtr<IInterface> obj;
                mStreamConfigs->Get(deviceId, (IInterface**)&obj);
                configs = IArrayOf::Probe(obj);
            }
            Int32 size;
            configs->GetLength(&size);
            AutoPtr<ArrayOf<ITvStreamConfig*> > array = ArrayOf<ITvStreamConfig*>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> obj;
                configs->Get(i, (IInterface**)&obj);
                array->Set(i, ITvStreamConfig::Probe(obj));
            }
            mCallback->OnDeviceAvailable(info, array);
            break;
        }
        case EVENT_DEVICE_UNAVAILABLE: {
            Int32 deviceId = arg1;
            if (DEBUG) {
                Slogger::D(TAG, "EVENT_DEVICE_UNAVAILABLE: deviceId = %d", deviceId);
            }
            mCallback->OnDeviceUnavailable(deviceId);
            break;
        }
        case EVENT_STREAM_CONFIGURATION_CHANGED: {
            AutoPtr<IArrayOf> configs;
            Int32 deviceId = arg1;
            {    AutoLock syncLock(mLock);
                if (DEBUG) {
                    Slogger::D(TAG, "EVENT_STREAM_CONFIGURATION_CHANGED: deviceId = %d", deviceId);
                }
                RetrieveStreamConfigsLocked(deviceId);
                AutoPtr<IInterface> obj;
                mStreamConfigs->Get(deviceId, (IInterface**)&obj);
                configs = IArrayOf::Probe(obj);
            }
            Int32 size;
            configs->GetLength(&size);
            AutoPtr<ArrayOf<ITvStreamConfig*> > array = ArrayOf<ITvStreamConfig*>::Alloc(size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> obj;
                configs->Get(i, (IInterface**)&obj);
                array->Set(i, ITvStreamConfig::Probe(obj));
            }
            mCallback->OnStreamConfigurationChanged(deviceId, array);
            break;
        }
        case EVENT_FIRST_FRAME_CAPTURED: {
            Int32 deviceId = arg1;
            Int32 streamId = arg2;
            mCallback->OnFirstFrameCaptured(deviceId, streamId);
            break;
        }
        default:
            Slogger::E(TAG, "Unknown event: %s", TO_CSTR(msg));
            *result = FALSE;
            return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

} // namespace Tv
} // namespace Server
} // namespace Droid
} // namespace Elastos
