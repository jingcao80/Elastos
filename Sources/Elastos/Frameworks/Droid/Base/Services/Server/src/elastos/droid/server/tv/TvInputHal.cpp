
#include "elastos/droid/server/tv/TvInputHal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/AutoLock.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CObject;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Tv {

//=============================================================================
// native
//=============================================================================
#if 0 // edit later
#define LOG_TAG "TvInputHal"
//#define LOG_NDEBUG 0
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/android_view_Surface.h"
#include "JNIHelp.h"
#include "jni.h"
#include <gui/Surface.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/Log.h>
#include <utils/NativeHandle.h>
#include <hardware/tv_input.h>

static struct {
    jmethodID deviceAvailable;
    jmethodID deviceUnavailable;
    jmethodID streamConfigsChanged;
    jmethodID firstFrameCaptured;
} gTvInputHalClassInfo;

static struct {
    jclass clazz;
} gTvStreamConfigClassInfo;

static struct {
    jclass clazz;

    jmethodID constructor;
    jmethodID streamId;
    jmethodID type;
    jmethodID maxWidth;
    jmethodID maxHeight;
    jmethodID generation;
    jmethodID build;
} gTvStreamConfigBuilderClassInfo;

static struct {
    jclass clazz;

    jmethodID constructor;
    jmethodID deviceId;
    jmethodID type;
    jmethodID hdmiPortId;
    jmethodID audioType;
    jmethodID audioAddress;
    jmethodID build;
} gTvInputHardwareInfoBuilderClassInfo;

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

status_t BufferProducerThread::readyToRun() {
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

    static JTvInputHal* createInstance(JNIEnv* env, jobject thiz);

    int addStream(int deviceId, int streamId, const sp<Surface>& surface);
    int removeStream(int deviceId, int streamId);
    const tv_stream_config_t* getStreamConfigs(int deviceId, int* numConfigs);

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

    JTvInputHal(JNIEnv* env, jobject thiz, tv_input_device_t* dev);

    static void notify(
            tv_input_device_t* dev, tv_input_event_t* event, void* data);

    void onDeviceAvailable(const tv_input_device_info_t& info);
    void onDeviceUnavailable(int deviceId);
    void onStreamConfigurationsChanged(int deviceId);
    void onCaptured(int deviceId, int streamId, uint32_t seq, bool succeeded);

    Mutex mLock;
    jweak mThiz;
    tv_input_device_t* mDevice;
    tv_input_callback_ops_t mCallback;

    KeyedVector<int, KeyedVector<int, Connection> > mConnections;
};

JTvInputHal::JTvInputHal(JNIEnv* env, jobject thiz, tv_input_device_t* device) {
    mThiz = env->NewWeakGlobalRef(thiz);
    mDevice = device;
    mCallback.notify = &JTvInputHal::notify;

    mDevice->initialize(mDevice, &mCallback, this);
}

JTvInputHal::~JTvInputHal() {
    mDevice->common.close((hw_device_t*)mDevice);

    JNIEnv* env = AndroidRuntime::getJNIEnv();
    env->DeleteWeakGlobalRef(mThiz);
    mThiz = NULL;
}

JTvInputHal* JTvInputHal::createInstance(JNIEnv* env, jobject thiz) {
    tv_input_module_t* module = NULL;
    status_t err = hw_get_module(TV_INPUT_HARDWARE_MODULE_ID,
            (hw_module_t const**)&module);
    if (err) {
        ALOGE("Couldn't load %s module (%s)",
                TV_INPUT_HARDWARE_MODULE_ID, strerror(-err));
        return 0;
    }

    tv_input_device_t* device = NULL;
    err = module->common.methods->open(
            (hw_module_t*)module,
            TV_INPUT_DEFAULT_DEVICE,
            (hw_device_t**)&device);
    if (err) {
        ALOGE("Couldn't open %s device (%s)",
                TV_INPUT_DEFAULT_DEVICE, strerror(-err));
        return 0;
    }

    return new JTvInputHal(env, thiz, device);
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
        int numConfigs = 0;
        const tv_stream_config_t* configs = NULL;
        if (mDevice->get_stream_configurations(
                mDevice, deviceId, &numConfigs, &configs) != 0) {
            ALOGE("Couldn't get stream configs");
            return UNKNOWN_ERROR;
        }
        int configIndex = -1;
        for (int i = 0; i < numConfigs; ++i) {
            if (configs[i].stream_id == streamId) {
                configIndex = i;
                break;
            }
        }
        if (configIndex == -1) {
            ALOGE("Cannot find a config with given stream ID: %d", streamId);
            return BAD_VALUE;
        }
        connection.mStreamType = configs[configIndex].type;

        tv_stream_t stream;
        stream.stream_id = configs[configIndex].stream_id;
        if (connection.mStreamType == TV_STREAM_TYPE_BUFFER_PRODUCER) {
            stream.buffer_producer.width = configs[configIndex].max_video_width;
            stream.buffer_producer.height = configs[configIndex].max_video_height;
        }
        if (mDevice->open_stream(mDevice, deviceId, &stream) != 0) {
            ALOGE("Couldn't add stream");
            return UNKNOWN_ERROR;
        }
        if (connection.mStreamType == TV_STREAM_TYPE_INDEPENDENT_VIDEO_SOURCE) {
            connection.mSourceHandle = NativeHandle::create(
                    stream.sideband_stream_source_handle, false);
        } else if (connection.mStreamType == TV_STREAM_TYPE_BUFFER_PRODUCER) {
            if (connection.mThread != NULL) {
                connection.mThread->shutdown();
            }
            connection.mThread = new BufferProducerThread(mDevice, deviceId, &stream);
            connection.mThread->run();
        }
    }
    connection.mSurface = surface;
    if (connection.mStreamType == TV_STREAM_TYPE_INDEPENDENT_VIDEO_SOURCE) {
        connection.mSurface->setSidebandStream(connection.mSourceHandle);
    } else if (connection.mStreamType == TV_STREAM_TYPE_BUFFER_PRODUCER) {
        connection.mThread->setSurface(surface);
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
        connection.mSurface.clear();
    }
    if (connection.mSurface != NULL) {
        connection.mSurface->setSidebandStream(NULL);
        connection.mSurface.clear();
    }
    if (connection.mThread != NULL) {
        connection.mThread->shutdown();
        connection.mThread.clear();
    }
    if (mDevice->close_stream(mDevice, deviceId, streamId) != 0) {
        ALOGE("Couldn't remove stream");
        return BAD_VALUE;
    }
    if (connection.mSourceHandle != NULL) {
        connection.mSourceHandle.clear();
    }
    return NO_ERROR;
}

const tv_stream_config_t* JTvInputHal::getStreamConfigs(int deviceId, int* numConfigs) {
    const tv_stream_config_t* configs = NULL;
    if (mDevice->get_stream_configurations(
            mDevice, deviceId, numConfigs, &configs) != 0) {
        ALOGE("Couldn't get stream configs");
        return NULL;
    }
    return configs;
}

// static
void JTvInputHal::notify(
        tv_input_device_t* dev, tv_input_event_t* event, void* data) {
    JTvInputHal* thiz = (JTvInputHal*)data;
    switch (event->type) {
        case TV_INPUT_EVENT_DEVICE_AVAILABLE: {
            thiz->onDeviceAvailable(event->device_info);
        } break;
        case TV_INPUT_EVENT_DEVICE_UNAVAILABLE: {
            thiz->onDeviceUnavailable(event->device_info.device_id);
        } break;
        case TV_INPUT_EVENT_STREAM_CONFIGURATIONS_CHANGED: {
            thiz->onStreamConfigurationsChanged(event->device_info.device_id);
        } break;
        case TV_INPUT_EVENT_CAPTURE_SUCCEEDED: {
            thiz->onCaptured(event->capture_result.device_id,
                             event->capture_result.stream_id,
                             event->capture_result.seq,
                             true /* succeeded */);
        } break;
        case TV_INPUT_EVENT_CAPTURE_FAILED: {
            thiz->onCaptured(event->capture_result.device_id,
                             event->capture_result.stream_id,
                             event->capture_result.seq,
                             false /* succeeded */);
        } break;
        default:
            ALOGE("Unrecognizable event");
    }
}

void JTvInputHal::onDeviceAvailable(const tv_input_device_info_t& info) {
    {
        Mutex::Autolock autoLock(&mLock);
        mConnections.add(info.device_id, KeyedVector<int, Connection>());
    }
    JNIEnv* env = AndroidRuntime::getJNIEnv();

    jobject builder = env->NewObject(
            gTvInputHardwareInfoBuilderClassInfo.clazz,
            gTvInputHardwareInfoBuilderClassInfo.constructor);
    env->CallObjectMethod(
            builder, gTvInputHardwareInfoBuilderClassInfo.deviceId, info.device_id);
    env->CallObjectMethod(
            builder, gTvInputHardwareInfoBuilderClassInfo.type, info.type);
    if (info.type == TV_INPUT_TYPE_HDMI) {
        env->CallObjectMethod(
                builder, gTvInputHardwareInfoBuilderClassInfo.hdmiPortId, info.hdmi.port_id);
    }
    env->CallObjectMethod(
            builder, gTvInputHardwareInfoBuilderClassInfo.audioType, info.audio_type);
    if (info.audio_type != AUDIO_DEVICE_NONE) {
        jstring audioAddress = env->NewStringUTF(info.audio_address);
        env->CallObjectMethod(
                builder, gTvInputHardwareInfoBuilderClassInfo.audioAddress, audioAddress);
        env->DeleteLocalRef(audioAddress);
    }

    jobject infoObject = env->CallObjectMethod(builder, gTvInputHardwareInfoBuilderClassInfo.build);

    env->CallVoidMethod(
            mThiz,
            gTvInputHalClassInfo.deviceAvailable,
            infoObject);

    env->DeleteLocalRef(builder);
    env->DeleteLocalRef(infoObject);
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
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    env->CallVoidMethod(
            mThiz,
            gTvInputHalClassInfo.deviceUnavailable,
            deviceId);
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
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    env->CallVoidMethod(
            mThiz,
            gTvInputHalClassInfo.streamConfigsChanged,
            deviceId);
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
        JNIEnv* env = AndroidRuntime::getJNIEnv();
        env->CallVoidMethod(
                mThiz,
                gTvInputHalClassInfo.firstFrameCaptured,
                deviceId,
                streamId);
    }
}
#endif // if 0
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
    Elastos::Core::CObject::New((IObject**)&mLock);
    CSparseInt32Array::New((ISparseInt32Array**)&mStreamConfigGenerations);
    CSparseArray::New((ISparseArray**)&mStreamConfigs);
    CLinkedList::New((IQueue**)&mPendingMessageQueue);
}

// jlong nativeOpen(JNIEnv* env, jobject thiz)
Int64 TvInputHal::NativeOpen()
{
    return 0l;
#if 0 // TODO: Translate codes below
    return (jlong)JTvInputHal::createInstance(env, thiz);
#endif
}

// int nativeAddStream(JNIEnv* env, jclass clazz,
//         jlong ptr, jint deviceId, jint streamId, jobject jsurface)
Int32 TvInputHal::NativeAddStream(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 streamId,
    /* [in] */ ISurface* surface)
{
    return 0;
#if 0 // TODO: Translate codes below
    JTvInputHal* tvInputHal = (JTvInputHal*)ptr;
    if (!jsurface) {
        return BAD_VALUE;
    }
    sp<Surface> surface(android_view_Surface_getSurface(env, jsurface));
    return tvInputHal->addStream(deviceId, streamId, surface);
#endif
}

// int nativeRemoveStream(JNIEnv* env, jclass clazz,
//         jlong ptr, jint deviceId, jint streamId)
Int32 TvInputHal::NativeRemoveStream(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 streamId)
{
    return 0;
#if 0 // TODO: Translate codes below
    JTvInputHal* tvInputHal = (JTvInputHal*)ptr;
    return tvInputHal->removeStream(deviceId, streamId);
#endif
}

// jobjectArray nativeGetStreamConfigs(JNIEnv* env, jclass clazz,
//         jlong ptr, jint deviceId, jint generation)
AutoPtr<IArrayOf> TvInputHal::NativeGetStreamConfigs(
    /* [in] */ Int64 ptr,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 generation)
{
    AutoPtr<IArrayOf> rev;
    return rev;
#if 0 // TODO: Translate codes below
    JTvInputHal* tvInputHal = (JTvInputHal*)ptr;
    int numConfigs = 0;
    const tv_stream_config_t* configs = tvInputHal->getStreamConfigs(deviceId, &numConfigs);

    jobjectArray result = env->NewObjectArray(numConfigs, gTvStreamConfigClassInfo.clazz, NULL);
    for (int i = 0; i < numConfigs; ++i) {
        jobject builder = env->NewObject(
                gTvStreamConfigBuilderClassInfo.clazz,
                gTvStreamConfigBuilderClassInfo.constructor);
        env->CallObjectMethod(
                builder, gTvStreamConfigBuilderClassInfo.streamId, configs[i].stream_id);
        env->CallObjectMethod(
                builder, gTvStreamConfigBuilderClassInfo.type, configs[i].type);
        env->CallObjectMethod(
                builder, gTvStreamConfigBuilderClassInfo.maxWidth, configs[i].max_video_width);
        env->CallObjectMethod(
                builder, gTvStreamConfigBuilderClassInfo.maxHeight, configs[i].max_video_height);
        env->CallObjectMethod(
                builder, gTvStreamConfigBuilderClassInfo.generation, generation);

        jobject config = env->CallObjectMethod(builder, gTvStreamConfigBuilderClassInfo.build);

        env->SetObjectArrayElement(result, i, config);

        env->DeleteLocalRef(config);
        env->DeleteLocalRef(builder);
    }
    return result;
#endif
}

// void nativeClose(JNIEnv* env, jclass clazz, jlong ptr)
ECode TvInputHal::NativeClose(
    /* [in] */ Int64 ptr)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
    JTvInputHal* tvInputHal = (JTvInputHal*)ptr;
    delete tvInputHal;
#endif
}

ECode TvInputHal::constructor(
    /* [in] */ ITvInputHalCallback* callback)
{
    mCallback = callback;
    CHandler::New(IHandlerCallback::Probe(this), FALSE, (IHandler**)&mHandler);
    return NOERROR;
}

ECode TvInputHal::Init()
{
    synchronized(mLock) {
        mPtr = NativeOpen();
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

    synchronized(mLock) {
        if (mPtr == 0) {
            *result = ERROR_NO_INIT;
            return NOERROR;
        }
        Int32 generation;
        mStreamConfigGenerations->Get(deviceId, 0, &generation);
        if (generation != Ptr(streamConfig)->Func(streamConfig->GetGeneration)) {
            *result = ERROR_STALE_CONFIG;
            return NOERROR;
        }
        if (NativeAddStream(mPtr, deviceId, Ptr(streamConfig)->Func(streamConfig->GetStreamId), surface) == 0) {
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

    synchronized(mLock) {
        if (mPtr == 0) {
            *result = ERROR_NO_INIT;
            return NOERROR;
        }
        Int32 generation;
        mStreamConfigGenerations->Get(deviceId, 0, &generation);
        if (generation != Ptr(streamConfig)->Func(streamConfig->GetGeneration)) {
            *result = ERROR_STALE_CONFIG;
            return NOERROR;
        }
        if (NativeRemoveStream(mPtr, deviceId, Ptr(streamConfig)->Func(streamConfig->GetStreamId)) == 0) {
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
    synchronized (mLock) {
        if (mPtr != 0l) {
            NativeClose(mPtr);
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
            synchronized(mLock) {
                RetrieveStreamConfigsLocked(Ptr(info)->Func(info->GetDeviceId));
                if (DEBUG) {
                    Slogger::D(TAG, "EVENT_DEVICE_AVAILABLE: info = %s", TO_CSTR(info));
                }
                AutoPtr<IInterface> obj;
                mStreamConfigs->Get(Ptr(info)->Func(info->GetDeviceId), (IInterface**)&obj);
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
                Slogger::D(TAG, "EVENT_DEVICE_UNAVAILABLE: deviceId = " + deviceId);
            }
            mCallback->OnDeviceUnavailable(deviceId);
            break;
        }
        case EVENT_STREAM_CONFIGURATION_CHANGED: {
            AutoPtr<IArrayOf> configs;
            Int32 deviceId = arg1;
            synchronized(mLock) {
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
