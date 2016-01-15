
#ifndef __ELASTOS_DROID_HARDWARE_SYSTEMSENSORMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_SYSTEMSENSORMANAGER_H__

#include "elastos/droid/hardware/SensorManager.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/NativeMessageQueue.h"
#include <elastos/utility/etl/List.h>
#include <utils/Looper.h>
#include <gui/SensorEventQueue.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Os::MessageQueue;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Core::IThread;
using Elastos::Utility::Etl::List;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Hardware::ISensorEventListener);
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Hardware::ITriggerEventListener);

namespace Elastos {
namespace Droid {
namespace Hardware {

/**
 * Sensor manager implementation that communicates with the built-in
 * system sensors.
 *
 * @hide
 */
class SystemSensorManager
    : public SensorManager
    , public ISystemSensorManager
{
private:
    class SensorEventQueue;
    class TriggerEventQueue;
    class Receiver;
    /*
     * BaseEventQueue is the communication channel with the sensor service,
     * SensorEventQueue, TriggerEventQueue are subclases and there is one-to-one mapping between
     * the queues and the listeners.
     */
    class BaseEventQueue
        : public Object
    {
    public:
        BaseEventQueue(
            /* [in] */ ILooper* looper,
            /* [in] */ ISystemSensorManager* manager);

        virtual ~BaseEventQueue();

        CARAPI_(void) Dispose();

        CARAPI_(Boolean) AddSensor(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 delayUs,
            /* [in] */ Int32 maxBatchReportLatencyUs,
            /* [in] */ Int32 reservedFlags);

        CARAPI_(Boolean) RemoveAllSensors();

        CARAPI_(Boolean) RemoveSensor(
            /* [in] */ ISensor* sensor,
            /* [in] */ Boolean disable);

        CARAPI Flush(
            /* [out] */ Int32* result);

        CARAPI_(Boolean) HasSensors();

    protected:
        virtual CARAPI_(void) DispatchSensorEvent(
            /* [in] */ Int32 handle,
            /* [in] */ ArrayOf<Float>* values,
            /* [in] */ Int32 accuracy,
            /* [in] */ Int64 timestamp) = 0;

        virtual CARAPI_(void) DispatchFlushCompleteEvent(
            /* [in] */ Int32 handle) = 0;

        virtual CARAPI_(void) AddSensorEvent(
            /* [in] */ ISensor* sensor) = 0;

        virtual CARAPI_(void) RemoveSensorEvent(
            /* [in] */ ISensor* sensor) = 0;

    private:
        CARAPI_(void) Dispose(
            /* [in] */ Boolean finalized);

        CARAPI EnableSensor(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 rateUs,
            /* [in] */ Int32 maxBatchReportLatencyUs,
            /* [in] */ Int32 reservedFlags,
            /* [out] */ Int32* result);

        CARAPI DisableSensor(
            /* [in] */ ISensor* sensor,
            /* [out] */ Int32* result);

        CARAPI_(Int64) NativeInitBaseEventQueue(
            /* [in] */ BaseEventQueue* eventQ,
            /* [in] */ IMessageQueue* msgQ,
            /* [in] */ ArrayOf<Float>* scratch);

        static CARAPI_(Int32) NativeEnableSensor(
            /* [in] */ Int64 eventQ,
            /* [in] */ Int32 handle,
            /* [in] */ Int32 rateUs,
            /* [in] */ Int32 maxBatchReportLatencyUs,
            /* [in] */ Int32 reservedFlags);

        static CARAPI_(Int32) NativeDisableSensor(
            /* [in] */ Int64 eventQ,
            /* [in] */ Int32 handle);

        static CARAPI_(void) NativeDestroySensorEventQueue(
            /* [in] */ Int64 eventQ);

        static CARAPI_(Int32) NativeFlushSensor(
            /* [in] */ Int64 eventQ);

    protected:
        HashMap<Int32, Int32> mSensorAccuracies;// = new SparseIntArray();
        HashMap<Int32, Boolean> mFirstEvent;// = new SparseBooleanArray();

    private:
        friend class SensorEventQueue;
        friend class TriggerEventQueue;
        friend class Receiver;

        Int64 nSensorEventQueue;
        HashMap<Int32, Boolean> mActiveSensors;// = new SparseBooleanArray();

        //CloseGuard mCloseGuard = CloseGuard.get();
        AutoPtr<ArrayOf<Float> > mScratch;// = new float[16];
        AutoPtr<ISystemSensorManager> mManager;
    };

    class Receiver : public android::LooperCallback
    {
    public:
        Receiver(
            /* [in] */ const android::sp<android::SensorEventQueue>& sensorQueue,
            /* [in] */ MessageQueue* messageQueue,
            /* [in] */ BaseEventQueue* receiverObject,
            /* [in] */ ArrayOf<Float>* scratch);

        ~Receiver();

        android::sp<android::SensorEventQueue> getSensorEventQueue() const;

        void destroy();

    private:
        virtual void onFirstRef();

        virtual int handleEvent(
            /* [in] */ int fd,
            /* [in] */ int events,
            /* [in] */ void* data);

    protected:
        android::sp<android::SensorEventQueue> mSensorQueue;
        AutoPtr<MessageQueue> mMessageQueue;
        AutoPtr<BaseEventQueue> mReceiverObject;
        AutoPtr<ArrayOf<Float> > mScratch;
    };

    class SensorEventQueue
       : public BaseEventQueue
    {
    public:
        SensorEventQueue(
            /* [in] */ ISensorEventListener* listener,
            /* [in] */ ILooper* looper,
            /* [in] */ ISystemSensorManager* manager);

        //@Override
        CARAPI_(void) AddSensorEvent(
            /* [in] */ ISensor* sensor);

        //@Override
        CARAPI_(void) RemoveSensorEvent(
            /* [in] */ ISensor* sensor);

        // Called from native code.
        //@SuppressWarnings("unused")
        //@Override
    protected:
        CARAPI_(void) DispatchSensorEvent(
            /* [in] */ Int32 handle,
            /* [in] */ ArrayOf<Float>* values,
            /* [in] */ Int32 accuracy,
            /* [in] */ Int64 timestamp);

        //@SuppressWarnings("unused")
        CARAPI_(void) DispatchFlushCompleteEvent(
            /* [in] */ Int32 handle);

    private:
        AutoPtr<ISensorEventListener> mListener;
        HashMap<Int32, AutoPtr<ISensorEvent> > mSensorsEvents;// = new SparseArray<SensorEvent>();
        Object mSensorsEventsLock;
    };

    class TriggerEventQueue
       : public BaseEventQueue
    {
    public:
        TriggerEventQueue(
            /* [in] */ ITriggerEventListener* listener,
            /* [in] */ ILooper* looper,
            /* [in] */ ISystemSensorManager* manager);

        //@Override
        CARAPI_(void) AddSensorEvent(
            /* [in] */ ISensor* sensor);

        //@Override
        CARAPI_(void) RemoveSensorEvent(
            /* [in] */ ISensor* sensor);

        // Called from native code.
        //@SuppressWarnings("unused")
        //@Override
    protected:
        CARAPI_(void) DispatchSensorEvent(
            /* [in] */ Int32 handle,
            /* [in] */ ArrayOf<Float>* values,
            /* [in] */ Int32 accuracy,
            /* [in] */ Int64 timestamp);

        //@SuppressWarnings("unused")
        CARAPI_(void) DispatchFlushCompleteEvent(
            /* [in] */ Int32 handle);

    private:
        AutoPtr<ITriggerEventListener> mListener;
        HashMap<Int32, AutoPtr<ITriggerEvent> > mTriggerEvents;// = new SparseArray<TriggerEvent>();
        Object mTriggerEventsLock;
    };

public:
    CAR_INTERFACE_DECL()

    SystemSensorManager();

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ILooper* mainLooper);

protected:
    /** @hide */
    //@Override
    CARAPI GetFullSensorList(
        /* [out, callee] */ ArrayOf<ISensor*>** sensors);

    /** @hide */
    //@Override
    CARAPI RegisterListenerImpl(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 delay,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 maxBatchReportLatencyUs,
        /* [in] */ Int32 reservedFlags,
        /* [out] */ Boolean* supported);

    /** @hide */
    //@Override
    CARAPI UnregisterListenerImpl(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor);

    /** @hide */
    //@Override
    CARAPI RequestTriggerSensorImpl(
        /* [in] */ ITriggerEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [out] */ Boolean* result);


    /** @hide */
    //@Override
    CARAPI CancelTriggerSensorImpl(
        /* [in] */ ITriggerEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Boolean disable,
        /* [out] */ Boolean* result);

    CARAPI FlushImpl(
        /* [in] */ ISensorEventListener* listener,
        /* [out] */ Boolean* result);

private:
    static  CARAPI_(void) NativeClassInit();

    static CARAPI_(Int32) NativeGetNextSensor(
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 next);

private:
    static Boolean sSensorModuleInitialized;
    static Object sSensorModuleLock;
    static AutoPtr<List<AutoPtr<ISensor> > > sFullSensorsList;// = new ArrayList<Sensor>();
    static HashMap<Int32, AutoPtr<ISensor> > sHandleToSensor;// = new SparseArray<Sensor>();

    // Listener list
    HashMap<AutoPtr<ISensorEventListener>, AutoPtr<SensorEventQueue> > mSensorListeners;
    HashMap<AutoPtr<ITriggerEventListener>, AutoPtr<TriggerEventQueue> > mTriggerListeners;
    Object mSensorListenersLock;
    Object mTriggerListenersLock;

    // Looper associated with the context in which this instance was created.
    AutoPtr<ILooper> mMainLooper;
    Int32 mTargetSdkLevel;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_SYSTEMSENSORMANAGER_H__
