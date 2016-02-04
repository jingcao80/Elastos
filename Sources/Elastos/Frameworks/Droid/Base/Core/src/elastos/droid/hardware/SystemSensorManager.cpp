
#include "elastos/droid/hardware/SystemSensorManager.h"
#include "elastos/droid/hardware/CSensorEvent.h"
#include "elastos/droid/hardware/CSensor.h"
#include "elastos/droid/hardware/CTriggerEvent.h"
#include "elastos/droid/net/CLocalSocket.h"
#include "elastos/droid/net/CLocalSocketAddress.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/CMessageQueue.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/CIntent.h"
#include <android/sensor.h>
#include <gui/Sensor.h>
#include <gui/SensorManager.h>
#include <unistd.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::CMessageQueue;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CThread;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::IFlushable;

namespace Elastos {
namespace Droid {
namespace Hardware {

Boolean SystemSensorManager::sSensorModuleInitialized = FALSE;
Object SystemSensorManager::sSensorModuleLock;
AutoPtr<List<AutoPtr<ISensor> > > SystemSensorManager::sFullSensorsList;
HashMap<Int32, AutoPtr<ISensor> > SystemSensorManager::sHandleToSensor;

CAR_INTERFACE_IMPL(SystemSensorManager, SensorManager, ISystemSensorManager);

SystemSensorManager::BaseEventQueue::BaseEventQueue(
    /* [in] */ ILooper* looper,
    /* [in] */ ISystemSensorManager* manager)
{
    AutoPtr<IMessageQueue> queue;
    looper->GetQueue((IMessageQueue**)&queue);
    nSensorEventQueue = NativeInitBaseEventQueue(this, queue, mScratch);
    //mCloseGuard.open("dispose");
    mManager = manager;
    mScratch = ArrayOf<Float>::Alloc(16);
}

SystemSensorManager::BaseEventQueue::~BaseEventQueue()
{
    //try {
        Dispose(TRUE);
    //} finally {
        //super.finalize();
    //}
}

void SystemSensorManager::BaseEventQueue::Dispose()
{
    Dispose(FALSE);
}

Boolean SystemSensorManager::BaseEventQueue::AddSensor(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 delayUs,
    /* [in] */ Int32 maxBatchReportLatencyUs,
    /* [in] */ Int32 reservedFlags)
{
    // Check if already present.
    Int32 handle;
    sensor->GetHandle(&handle);
    if (mActiveSensors[handle]) {
        return FALSE;
    }

    // Get ready to receive events before calling enable.
    mActiveSensors[handle] = TRUE;
    AddSensorEvent(sensor);
    Int32 result;
    EnableSensor(sensor, delayUs, maxBatchReportLatencyUs, reservedFlags, &result);
    if (result != 0) {
        // Try continuous mode if batching fails.
        EnableSensor(sensor, delayUs, 0, 0, &result);
        if (maxBatchReportLatencyUs == 0 ||
            (maxBatchReportLatencyUs > 0 && result != 0)) {
            RemoveSensor(sensor, FALSE);
            return FALSE;
        }
    }
    return TRUE;
}

Boolean SystemSensorManager::BaseEventQueue::RemoveAllSensors()
{
    HashMap<Int32, Boolean>::Iterator it;
    for (it = mActiveSensors.Begin(); it != mActiveSensors.End(); ++it) {
        if (it->mSecond == TRUE) {
            Int32 handle = it->mFirst;
            AutoPtr<ISensor> sensor = sHandleToSensor[handle];
            if (sensor != NULL) {
                Int32 result;
                DisableSensor(sensor, &result);
                mActiveSensors[handle] = FALSE;
                RemoveSensorEvent(sensor);
            } else {
                // it should never happen -- just ignore.
            }
        }
    }
    return TRUE;
}

Boolean SystemSensorManager::BaseEventQueue::RemoveSensor(
    /* [in] */ ISensor* sensor,
    /* [in] */ Boolean disable)
{
    Int32 handle;
    sensor->GetHandle(&handle);
    if (mActiveSensors[handle]) {
        if (disable) {
            Int32 result;
            DisableSensor(sensor, &result);
        }
        Int32 _handle;
        sensor->GetHandle(&_handle);
        mActiveSensors[handle] = FALSE;
        RemoveSensorEvent(sensor);
        return TRUE;
    }
    return FALSE;
}

ECode SystemSensorManager::BaseEventQueue::Flush(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (nSensorEventQueue == 0) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    *result = NativeFlushSensor(nSensorEventQueue);
    return NOERROR;
}

Boolean SystemSensorManager::BaseEventQueue::HasSensors()
{
    // no more sensors are set
    HashMap<Int32, Boolean>::Iterator it;
    for (it = mActiveSensors.Begin(); it != mActiveSensors.End(); ++it) {
        if (it->mSecond == TRUE) {
            return TRUE;
        }
    }
    return FALSE;
}

void SystemSensorManager::BaseEventQueue::Dispose(
    /* [in] */ Boolean finalized)
{
    // if (mCloseGuard != null) {
    //     if (finalized) {
    //         mCloseGuard.warnIfOpen();
    //     }
    //     mCloseGuard.close();
    // }
    if (nSensorEventQueue != 0) {
        NativeDestroySensorEventQueue(nSensorEventQueue);
        nSensorEventQueue = 0;
    }
}

ECode SystemSensorManager::BaseEventQueue::EnableSensor(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 rateUs,
    /* [in] */ Int32 maxBatchReportLatencyUs,
    /* [in] */ Int32 reservedFlags,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (nSensorEventQueue == 0) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    if (sensor == NULL) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 handle;
    FAIL_RETURN(sensor->GetHandle(&handle))
    *result = NativeEnableSensor(nSensorEventQueue, handle, rateUs,
            maxBatchReportLatencyUs, reservedFlags);
    return NOERROR;
}

ECode SystemSensorManager::BaseEventQueue::DisableSensor(
    /* [in] */ ISensor* sensor,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (nSensorEventQueue == 0) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    if (sensor == NULL) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 handle;
    FAIL_RETURN(sensor->GetHandle(&handle))
    return NativeDisableSensor(nSensorEventQueue, handle);
}

Int64 SystemSensorManager::BaseEventQueue::NativeInitBaseEventQueue(
    /* [in] */ BaseEventQueue* eventQ,
    /* [in] */ IMessageQueue* msgQ,
    /* [in] */ ArrayOf<Float>* scratch)
{
    android::SensorManager& mgr(android::SensorManager::getInstance());
    android::sp<android::SensorEventQueue> queue(mgr.createEventQueue());

    // android::sp<MessageQueue> messageQueue = android_os_MessageQueue_getMessageQueue(env, msgQ);
    Int64 handle;
    msgQ->GetMPtr(&handle);
    AutoPtr<MessageQueue> messageQueue = reinterpret_cast<NativeMessageQueue*>(handle);
    if (messageQueue == NULL) {
        //jniThrowRuntimeException(env, "MessageQueue is not initialized.");
        return 0;
    }

    android::sp<Receiver> receiver = new Receiver(queue, messageQueue, eventQ, scratch);
    assert(0 && "TODO: decStrong");
    // receiver->incStrong((void*)nativeInitSensorEventQueue);
    return 0;//Int64(receiver.get());
}

Int32 SystemSensorManager::BaseEventQueue::NativeEnableSensor(
    /* [in] */ Int64 eventQ,
    /* [in] */ Int32 handle,
    /* [in] */ Int32 rateUs,
    /* [in] */ Int32 maxBatchReportLatencyUs,
    /* [in] */ Int32 reservedFlags)
{
    android::sp<Receiver> receiver(reinterpret_cast<Receiver *>(eventQ));
    return receiver->getSensorEventQueue()->enableSensor(handle, rateUs, maxBatchReportLatencyUs,
                                                         reservedFlags);
}

Int32 SystemSensorManager::BaseEventQueue::NativeDisableSensor(
    /* [in] */ Int64 eventQ,
    /* [in] */ Int32 handle)
{
    android::sp<Receiver> receiver(reinterpret_cast<Receiver *>(eventQ));
    return receiver->getSensorEventQueue()->disableSensor(handle);
}

void SystemSensorManager::BaseEventQueue::NativeDestroySensorEventQueue(
    /* [in] */ Int64 eventQ)
{
    android::sp<Receiver> receiver(reinterpret_cast<Receiver *>(eventQ));
    receiver->destroy();
    assert(0 && "TODO: decStrong");
    //receiver->decStrong((void*)NativeInitBaseEventQueue);
}

Int32 SystemSensorManager::BaseEventQueue::NativeFlushSensor(
    /* [in] */ Int64 eventQ)
{
    android::sp<Receiver> receiver(reinterpret_cast<Receiver *>(eventQ));
    return receiver->getSensorEventQueue()->flush();
}


SystemSensorManager::Receiver::Receiver(
    /* [in] */ const android::sp<android::SensorEventQueue>& sensorQueue,
    /* [in] */ MessageQueue* messageQueue,
    /* [in] */ BaseEventQueue* receiverObject,
    /* [in] */ ArrayOf<Float>* scratch)
    : mSensorQueue(sensorQueue)
    , mMessageQueue(messageQueue)
    , mReceiverObject(receiverObject)
    , mScratch(scratch)
{
}

SystemSensorManager::Receiver::~Receiver()
{
}

android::sp<android::SensorEventQueue> SystemSensorManager::Receiver::getSensorEventQueue() const
{
    return mSensorQueue;
}

void SystemSensorManager::Receiver::destroy()
{
    mMessageQueue->GetLooper()->removeFd(mSensorQueue->getFd());
}

void SystemSensorManager::Receiver::onFirstRef()
{
    LooperCallback::onFirstRef();
    mMessageQueue->GetLooper()->addFd(mSensorQueue->getFd(), 0,
            ALOOPER_EVENT_INPUT, this, mSensorQueue.get());

}

int SystemSensorManager::Receiver::handleEvent(
    /* [in] */ int fd,
    /* [in] */ int events,
    /* [in] */ void* data)
{
    android::sp<android::SensorEventQueue> q = reinterpret_cast<android::SensorEventQueue *>(data);
    ssize_t n;
    ASensorEvent buffer[16];
    while ((n = q->read(buffer, 16)) > 0) {
        for (int i=0 ; i<n ; i++) {
            if (buffer[i].type == SENSOR_TYPE_STEP_COUNTER) {
                // step-counter returns a uint64, but the java API only deals with floats
                float value = float(buffer[i].u64.step_counter);
                //env->SetFloatArrayRegion(mScratch, 0, 1, &value);
                (*mScratch)[0] = value;
            } else {
                //env->SetFloatArrayRegion(mScratch, 0, 16, buffer[i].data);
                for (int j = 0; j < 16; j++) {
                    (*mScratch)[j] = buffer[i].data[j];
                }
            }

            if (buffer[i].type == SENSOR_TYPE_META_DATA) {
                // This is a flush complete sensor event. Call dispatchFlushCompleteEvent
                // method.
                // env->CallVoidMethod(mReceiverObject,
                //                     gBaseEventQueueClassInfo.dispatchFlushCompleteEvent,
                //                     buffer[i].meta_data.sensor);
                mReceiverObject->DispatchFlushCompleteEvent(buffer[i].meta_data.sensor);
            } else {
                int8_t status;
                switch (buffer[i].type) {
                    case SENSOR_TYPE_ORIENTATION:
                    case SENSOR_TYPE_MAGNETIC_FIELD:
                    case SENSOR_TYPE_ACCELEROMETER:
                    case SENSOR_TYPE_GYROSCOPE:
                        status = buffer[i].vector.status;
                        break;
                    case SENSOR_TYPE_HEART_RATE:
                        status = buffer[i].heart_rate.status;
                        break;
                    default:
                        status = SENSOR_STATUS_ACCURACY_HIGH;
                        break;
                }
                // env->CallVoidMethod(mReceiverObject,
                //                     gBaseEventQueueClassInfo.dispatchSensorEvent,
                //                     buffer[i].sensor,
                //                     mScratch,
                //                     status,
                //                     buffer[i].timestamp);
                mReceiverObject->DispatchSensorEvent(buffer[i].sensor, mScratch,
                        status, buffer[i].timestamp);
            }
            assert(0 && "TODO: env->ExceptionCheck()");
            // if (env->ExceptionCheck()) {
            //     mSensorQueue->sendAck(buffer, n);
            //     ALOGE("Exception dispatching input event.");
            //     return 1;
            // }
        }
        mSensorQueue->sendAck(buffer, n);
    }
    if (n<0 && n != -EAGAIN) {
        // FIXME: error receiving events, what to do in this case?
    }
    return 1;
}

SystemSensorManager::SensorEventQueue::SensorEventQueue(
    /* [in] */ ISensorEventListener* listener,
    /* [in] */ ILooper* looper,
    /* [in] */ ISystemSensorManager* manager)
    : BaseEventQueue(looper, manager)
    , mListener(listener)
{
}

void SystemSensorManager::SensorEventQueue::AddSensorEvent(
    /* [in] */ ISensor* sensor)
{
    AutoPtr<ISensorEvent> t;
    CSensorEvent::New(CSensor::GetMaxLengthValuesArray(sensor,
            ((SystemSensorManager*)mManager.Get())->mTargetSdkLevel), (ISensorEvent**)&t);
    synchronized(mSensorsEventsLock) {
        Int32 handle;
        sensor->GetHandle(&handle);
        mSensorsEvents[handle] = t;
    }
}

void SystemSensorManager::SensorEventQueue::RemoveSensorEvent(
    /* [in] */ ISensor* sensor)
{
    synchronized(mSensorsEventsLock) {
        Int32 handle;
        sensor->GetHandle(&handle);
        mSensorsEvents.Erase(handle);
    }
}

void SystemSensorManager::SensorEventQueue::DispatchSensorEvent(
    /* [in] */ Int32 handle,
    /* [in] */ ArrayOf<Float>* values,
    /* [in] */ Int32 inAccuracy,
    /* [in] */ Int64 timestamp)
{
    AutoPtr<ISensor> sensor = sHandleToSensor[handle];
    AutoPtr<ISensorEvent> t = NULL;
    synchronized(mSensorsEventsLock) {
        t = mSensorsEvents[handle];
    }

    if (t == NULL) {
        // This may happen if the client has unregistered and there are pending events in
        // the queue waiting to be delivered. Ignore.
        return;
    }
    // Copy from the values array.
    //System.arraycopy(values, 0, t.values, 0, t.values.length);
    ((CTriggerEvent*)t.Get())->mValues->Copy(values, 0, ((CTriggerEvent*)t.Get())->mValues->GetLength());
    t->SetTimestamp(timestamp);
    t->SetAccuracy(inAccuracy);
    t->SetSensor(sensor);

    // call onAccuracyChanged() only if the value changes
    const Int32 accuracy = mSensorAccuracies[handle];
    Int32 _accuracy;
    t->GetAccuracy(&_accuracy);
    if ((_accuracy >= 0) && (accuracy != _accuracy)) {
        mSensorAccuracies[handle] = _accuracy;
        AutoPtr<ISensor> _sensor;
        t->GetSensor((ISensor**)&_sensor);
        mListener->OnAccuracyChanged(_sensor, _accuracy);
    }
    mListener->OnSensorChanged(t);
}

void SystemSensorManager::SensorEventQueue::DispatchFlushCompleteEvent(
    /* [in] */ Int32 handle)
{
    if (ISensorEventListener2::Probe(mListener) != NULL) {
        const AutoPtr<ISensor> sensor = sHandleToSensor[handle];
        ISensorEventListener2::Probe(mListener)->OnFlushCompleted(sensor);
    }
    return;
}

SystemSensorManager::TriggerEventQueue::TriggerEventQueue(
    /* [in] */ ITriggerEventListener* listener,
    /* [in] */ ILooper* looper,
    /* [in] */ ISystemSensorManager* manager)
    : BaseEventQueue(looper, manager)
    , mListener(listener)
{
}

void SystemSensorManager::TriggerEventQueue::AddSensorEvent(
    /* [in] */ ISensor* sensor)
{
    AutoPtr<ITriggerEvent> t;
    CTriggerEvent::New(CSensor::GetMaxLengthValuesArray(sensor,
            ((SystemSensorManager*)mManager.Get())->mTargetSdkLevel), (ITriggerEvent**)&t);
    synchronized(mTriggerEventsLock) {
        Int32 handle;
        sensor->GetHandle(&handle);
        mTriggerEvents[handle] = t;
    }
}

void SystemSensorManager::TriggerEventQueue::RemoveSensorEvent(
    /* [in] */ ISensor* sensor)
{
    synchronized(mTriggerEventsLock) {
        Int32 handle;
        sensor->GetHandle(&handle);
        mTriggerEvents.Erase(handle);
    }
}

void SystemSensorManager::TriggerEventQueue::DispatchSensorEvent(
    /* [in] */ Int32 handle,
    /* [in] */ ArrayOf<Float>* values,
    /* [in] */ Int32 accuracy,
    /* [in] */ Int64 timestamp)
{
    const AutoPtr<ISensor> sensor = sHandleToSensor[handle];
    AutoPtr<ITriggerEvent> t = NULL;
    synchronized(mTriggerEventsLock) {
        t = mTriggerEvents[handle];
    }
    if (t == NULL) {
        //Log.e(TAG, "Error: Trigger Event is null for Sensor: " + sensor);
        return;
    }

    // Copy from the values array.
    //System.arraycopy(values, 0, t.values, 0, t.values.length);
    ((CTriggerEvent*)t.Get())->mValues->Copy(values, 0, ((CTriggerEvent*)t.Get())->mValues->GetLength());
    t->SetTimestamp(timestamp);
    t->SetSensor(sensor);

    // A trigger sensor is auto disabled. So just clean up and don't call native
    // disable.
    Boolean result;
    ISensorManager::Probe(mManager)->CancelTriggerSensorImpl(mListener, sensor, FALSE, &result);

    mListener->OnTrigger(t);
}

void SystemSensorManager::TriggerEventQueue::DispatchFlushCompleteEvent(
    /* [in] */ Int32 handle)
{
    return;
}

SystemSensorManager::SystemSensorManager()
{
}

ECode SystemSensorManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* mainLooper)
{
    mMainLooper = mainLooper;
    AutoPtr<IApplicationInfo> info;
    FAIL_RETURN(context->GetApplicationInfo((IApplicationInfo**)&info))
    FAIL_RETURN(info->GetTargetSdkVersion(&mTargetSdkLevel))
    synchronized(sSensorModuleLock) {
        if (!sSensorModuleInitialized) {
            sSensorModuleInitialized = TRUE;

            NativeClassInit();

            // initialize the sensor list
            AutoPtr<List<AutoPtr<ISensor> > > fullList = sFullSensorsList;
            Int32 i = 0;
            do {
                AutoPtr<ISensor> sensor;
                CSensor::New((ISensor**)&sensor);
                i = NativeGetNextSensor(sensor, i);
                if (i>=0) {
                    //Log.d(TAG, "found sensor: " + sensor.getName() +
                    //        ", handle=" + sensor.getHandle());
                    fullList->PushBack(sensor);
                    Int32 handle;
                    FAIL_RETURN(sensor->GetHandle(&handle))
                    sHandleToSensor[handle] = sensor;
                }
            } while (i>0);
        }
    }
    return NOERROR;
}


ECode SystemSensorManager::GetFullSensorList(
    /* [out, callee] */ ArrayOf<ISensor*>** sensors)
{
    VALIDATE_NOT_NULL(sensors);
    *sensors = NULL;
    if (sFullSensorsList->IsEmpty() == FALSE) {
        *sensors = ArrayOf<ISensor*>::Alloc(sFullSensorsList->GetSize());

        List< AutoPtr<ISensor> >::Iterator ator = sFullSensorsList->Begin();
        Int32 index = 0;
        for (; ator != sFullSensorsList->End(); ++ator) {
            (*sensors)->Set(index++, *ator);
        }
    }
    REFCOUNT_ADD(*sensors);

    return NOERROR;
}

ECode SystemSensorManager::RegisterListenerImpl(
    /* [in] */ ISensorEventListener* listener,
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 delayUs,
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 maxBatchReportLatencyUs,
    /* [in] */ Int32 reservedFlags,
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    if (listener == NULL || sensor == NULL) {
        //Log.e(TAG, "sensor or listener is null");
        *supported = FALSE;
        return NOERROR;
    }
    // Trigger Sensors should use the requestTriggerSensor call.
    Int32 mode;
    FAIL_RETURN(sensor->GetReportingMode(&mode))
    if (mode == ISensor::REPORTING_MODE_ONE_SHOT) {
        //Log.e(TAG, "Trigger Sensors should use the requestTriggerSensor.");
        *supported = FALSE;
        return NOERROR;
    }
    if (maxBatchReportLatencyUs < 0 || delayUs < 0) {
        //Log.e(TAG, "maxBatchReportLatencyUs and delayUs should be non-negative");
        *supported = FALSE;
        return NOERROR;
    }

    // Invariants to preserve:
    // - one Looper per SensorEventListener
    // - one Looper per SensorEventQueue
    // We map SensorEventListener to a SensorEventQueue, which holds the looper
    synchronized(mSensorListenersLock) {
        AutoPtr<SensorEventQueue> queue = mSensorListeners[listener];
        if (queue == NULL) {
            AutoPtr<ILooper> looper;
            if (handler != NULL) {
                FAIL_RETURN(handler->GetLooper((ILooper**)&looper))
            }
            else {
                looper = mMainLooper;
            }
            queue = new SensorEventQueue(listener, looper, this);
            if (!queue->AddSensor(sensor, delayUs, maxBatchReportLatencyUs, reservedFlags)) {
                queue->Dispose();
                *supported = FALSE;
                return NOERROR;
            }
            mSensorListeners[listener] = queue;
            *supported = TRUE;
            return NOERROR;
        } else {
            *supported = queue->AddSensor(sensor, delayUs, maxBatchReportLatencyUs, reservedFlags);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode SystemSensorManager::UnregisterListenerImpl(
    /* [in] */ ISensorEventListener* listener,
    /* [in] */ ISensor* sensor)
{
    // Trigger Sensors should use the cancelTriggerSensor call.
    Int32 mode;
    FAIL_RETURN(sensor->GetReportingMode(&mode))
    if (sensor != NULL && mode == ISensor::REPORTING_MODE_ONE_SHOT) {
        return NOERROR;
    }

    synchronized(mSensorListenersLock) {
        AutoPtr<SensorEventQueue> queue = mSensorListeners[listener];
        if (queue != NULL) {
            Boolean result1, result2;
            if (sensor == NULL) {
                result1 = queue->RemoveAllSensors();
            } else {
                result1 = queue->RemoveSensor(sensor, TRUE);
            }
            result2 = queue->HasSensors();
            if (result1 && !result2) {
                mSensorListeners.Erase(listener);
                queue->Dispose();
            }
        }
    }
    return NOERROR;
}


ECode SystemSensorManager::RequestTriggerSensorImpl(
    /* [in] */ ITriggerEventListener* listener,
    /* [in] */ ISensor* sensor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (sensor == NULL) {
        //throw new IllegalArgumentException("sensor cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 mode;
    FAIL_RETURN(sensor->GetReportingMode(&mode))
    if (mode != ISensor::REPORTING_MODE_ONE_SHOT) {
        *result = FALSE;
        return NOERROR;
    }

    synchronized(mTriggerListenersLock) {
        AutoPtr<TriggerEventQueue> queue = mTriggerListeners[listener];
        if (queue == NULL) {
            queue = new TriggerEventQueue(listener, mMainLooper, this);
            Boolean _result;
            _result = queue->AddSensor(sensor, 0, 0, 0);
            if (!_result) {
                queue->Dispose();
                *result = FALSE;
                return NOERROR;
            }
            mTriggerListeners[listener] = queue;
            *result = TRUE;
            return NOERROR;
        } else {
            *result = queue->AddSensor(sensor, 0, 0, 0);
            return NOERROR;
        }
    }
    return NOERROR;
}


ECode SystemSensorManager::CancelTriggerSensorImpl(
    /* [in] */ ITriggerEventListener* listener,
    /* [in] */ ISensor* sensor,
    /* [in] */ Boolean disable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 mode;
    FAIL_RETURN(sensor->GetReportingMode(&mode))
    if (sensor != NULL &&  mode != ISensor::REPORTING_MODE_ONE_SHOT) {
        *result = FALSE;
        return NOERROR;
    }
    synchronized(mTriggerListenersLock) {
        AutoPtr<TriggerEventQueue> queue = mTriggerListeners[listener];
        if (queue != NULL) {
            Boolean result1;
            if (sensor == NULL) {
                result1 = queue->RemoveAllSensors();
            } else {
                result1 = queue->RemoveSensor(sensor, disable);
            }
            Boolean result2 = queue->HasSensors();
            if (result1 && !result2) {
                mTriggerListeners.Erase(listener);
                queue->Dispose();
            }
            *result = result1;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode SystemSensorManager::FlushImpl(
    /* [in] */ ISensorEventListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (listener == NULL) {
        //throw new IllegalArgumentException("listener cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    synchronized(mSensorListenersLock) {
        AutoPtr<SensorEventQueue> queue = mSensorListeners[listener];
        if (queue == NULL) {
            *result = FALSE;
            return NOERROR;
        } else {
            Int32 _result;
            FAIL_RETURN(queue->Flush(&_result))
            *result = (_result == 0);
            return NOERROR;
        }
    }
    return NOERROR;
}

void SystemSensorManager::NativeClassInit()
{
    assert(0);
    // jclass sensorClass = _env->FindClass("android/hardware/Sensor");
    // SensorOffsets& sensorOffsets = gSensorOffsets;
    // sensorOffsets.name        = _env->GetFieldID(sensorClass, "mName",      "Ljava/lang/String;");
    // sensorOffsets.vendor      = _env->GetFieldID(sensorClass, "mVendor",    "Ljava/lang/String;");
    // sensorOffsets.version     = _env->GetFieldID(sensorClass, "mVersion",   "I");
    // sensorOffsets.handle      = _env->GetFieldID(sensorClass, "mHandle",    "I");
    // sensorOffsets.type        = _env->GetFieldID(sensorClass, "mType",      "I");
    // sensorOffsets.range       = _env->GetFieldID(sensorClass, "mMaxRange",  "F");
    // sensorOffsets.resolution  = _env->GetFieldID(sensorClass, "mResolution","F");
    // sensorOffsets.power       = _env->GetFieldID(sensorClass, "mPower",     "F");
    // sensorOffsets.minDelay    = _env->GetFieldID(sensorClass, "mMinDelay",  "I");
    // sensorOffsets.fifoReservedEventCount =
    //         _env->GetFieldID(sensorClass, "mFifoReservedEventCount",  "I");
    // sensorOffsets.fifoMaxEventCount = _env->GetFieldID(sensorClass, "mFifoMaxEventCount",  "I");
    // sensorOffsets.stringType = _env->GetFieldID(sensorClass, "mStringType", "Ljava/lang/String;");
    // sensorOffsets.requiredPermission = _env->GetFieldID(sensorClass, "mRequiredPermission",
    //                                                     "Ljava/lang/String;");
    // sensorOffsets.maxDelay    = _env->GetFieldID(sensorClass, "mMaxDelay",  "I");
    // sensorOffsets.flags = _env->GetFieldID(sensorClass, "mFlags",  "I");
}

Int32 SystemSensorManager::NativeGetNextSensor(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 next)
{
    assert(0);
    // SensorManager& mgr(SensorManager::getInstance());

    // Sensor const* const* sensorList;
    // size_t count = mgr.getSensorList(&sensorList);
    // if (size_t(next) >= count)
    //     return -1;

    // Sensor const* const list = sensorList[next];
    // const SensorOffsets& sensorOffsets(gSensorOffsets);
    // jstring name = env->NewStringUTF(list->getName().string());
    // jstring vendor = env->NewStringUTF(list->getVendor().string());
    // jstring stringType = env->NewStringUTF(list->getStringType().string());
    // jstring requiredPermission = env->NewStringUTF(list->getRequiredPermission().string());
    // env->SetObjectField(sensor, sensorOffsets.name,      name);
    // env->SetObjectField(sensor, sensorOffsets.vendor,    vendor);
    // env->SetIntField(sensor, sensorOffsets.version,      list->getVersion());
    // env->SetIntField(sensor, sensorOffsets.handle,       list->getHandle());
    // env->SetIntField(sensor, sensorOffsets.type,         list->getType());
    // env->SetFloatField(sensor, sensorOffsets.range,      list->getMaxValue());
    // env->SetFloatField(sensor, sensorOffsets.resolution, list->getResolution());
    // env->SetFloatField(sensor, sensorOffsets.power,      list->getPowerUsage());
    // env->SetIntField(sensor, sensorOffsets.minDelay,     list->getMinDelay());
    // env->SetIntField(sensor, sensorOffsets.fifoReservedEventCount,
    //                  list->getFifoReservedEventCount());
    // env->SetIntField(sensor, sensorOffsets.fifoMaxEventCount,
    //                  list->getFifoMaxEventCount());
    // env->SetObjectField(sensor, sensorOffsets.stringType, stringType);
    // env->SetObjectField(sensor, sensorOffsets.requiredPermission,
    //                     requiredPermission);
    // env->SetIntField(sensor, sensorOffsets.maxDelay, list->getMaxDelay());
    // env->SetIntField(sensor, sensorOffsets.flags, list->getFlags());
    // next++;
    // return size_t(next) < count ? next : 0;
    return 0;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
