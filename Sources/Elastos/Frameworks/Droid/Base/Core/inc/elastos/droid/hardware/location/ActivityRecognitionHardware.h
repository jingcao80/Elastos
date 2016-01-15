
#ifndef __ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYRECOGNITIONHARDWARE_H__
#define __ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYRECOGNITIONHARDWARE_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Location::IActivityRecognitionHardware;
using Elastos::Droid::Hardware::Location::IIActivityRecognitionHardware;
using Elastos::Droid::Hardware::Location::IIActivityRecognitionHardwareSink;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

class ActivityRecognitionHardware
    : public Object
    , public IActivityRecognitionHardware
    , public IIActivityRecognitionHardware
    , public IBinder
{
public:
    class Event : public Object
    {
    public:
        Int32 mActivity;
        Int32 mType;
        Int64 mTimestamp;
    };

    CAR_INTERFACE_DECL()

    ActivityRecognitionHardware();

    virtual ~ActivityRecognitionHardware();

    static CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IActivityRecognitionHardware** instance);

    static CARAPI IsSupported(
        /* [out] */ Boolean* result);

    /**
     * Gets an array of supported activities by hardware.
     */
    CARAPI GetSupportedActivities(
        /* [out, callee] */ ArrayOf<String>** activities);

    /**
     * Returns true if the given activity is supported, false otherwise.
     */
    CARAPI IsActivitySupported(
        /* [in] */ const String& activityType,
        /* [out] */ Boolean* supported);

    /**
     * Registers a sink with Hardware Activity-Recognition.
     */
    CARAPI RegisterSink(
        /* [in] */ IIActivityRecognitionHardwareSink* sink,
        /* [out] */ Boolean* result);

    /**
     * Unregisters a sink with Hardware Activity-Recognition.
     */
    CARAPI UnregisterSink(
        /* [in] */ IIActivityRecognitionHardwareSink* sink,
        /* [out] */ Boolean* result);

    /**
     * Enables tracking of a given activity/event type, if the activity is supported.
     */
    CARAPI EnableActivityEvent(
        /* [in] */ const String& activityType,
        /* [in] */ Int32 eventType,
        /* [in] */ Int64 reportLatencyNs,
        /* [out] */ Boolean* result);

    /**
     * Disables tracking of a given activity/eventy type.
     */
    CARAPI DisableActivityEvent(
        /* [in] */ const String& activityType,
        /* [in] */ Int32 eventType,
        /* [out] */ Boolean* result);

    /**
     * Requests hardware for all the activity events detected up to the given point in time.
     */
    CARAPI Flush(
        /* [out] */ Boolean* result);

    CARAPI ToString(
            /* [out] */ String* info);

    /**
     * Called by the Activity-Recognition HAL.
     */
    CARAPI OnActivityChanged(
        /* [in] */ ArrayOf<Event*>* events);

private:
    ActivityRecognitionHardware(
        /* [in] */ IContext* context);

    CARAPI_(String) GetActivityName(
        /* [in] */ Int32 activityType);

    CARAPI_(Int32) GetActivityType(
        /* [in] */ const String& activity);

    CARAPI CheckPermissions();

    CARAPI FetchSupportedActivities(
        /* [out, callee] */ ArrayOf<String>** activities);

    /**
     * Initializes the ActivityRecognitionHardware class from the native side.
     */
    static CARAPI NativeClassInit();

    /**
     * Returns true if ActivityRecognition HAL is supported, false otherwise.
     */
    static CARAPI_(Boolean) NativeIsSupported();

    /**
     * Initializes and connect the callbacks handlers in the HAL.
     */
    CARAPI_(void) NativeInitialize();

    /**
     * De-initializes the ActivityRecognitionHardware from the native side.
     */
    CARAPI_(void) NativeRelease();

    /**
     * Gets an array representing the supported activities.
     */
    CARAPI_(AutoPtr<ArrayOf<String> >) NativeGetSupportedActivities();

    /**
     * Enables a given activity event to be actively monitored.
     */
    CARAPI_(Int32) NativeEnableActivityEvent(
        /* [in] */ Int32 activityType,
        /* [in] */ Int32 eventType,
        /* [in] */ Int64 reportLatenceNs);

    /**
     * Disables a given activity event from being actively monitored.
     */
    CARAPI_(Int32) NativeDisableActivityEvent(
        /* [in] */ Int32 activityType,
        /* [in] */ Int32 eventType);

    /**
     * Request flush for al batch buffers.
     */
    CARAPI_(Int32) NativeFlush();

private:
    static const String TAG;

    static const String HARDWARE_PERMISSION;
    static const Int32 INVALID_ACTIVITY_TYPE;
    static const Int32 NATIVE_SUCCESS_RESULT;

    static AutoPtr<IActivityRecognitionHardware> sSingletonInstance;
    static Object sSingletonInstanceLock;

    // native bindings
    static ECode sStaticBlockInit;

    AutoPtr<IContext> mContext;
    AutoPtr<ArrayOf<String> > mSupportedActivities;

    AutoPtr<IRemoteCallbackList> mSinks;
};

} //Location
} //Hardware
} //Droid
} //Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Hardware::Location::ActivityRecognitionHardware::Event, IInterface)

#endif  //__ELASTOS_DROID_HARDWARE_LOCATION_ACTIVITYRECOGNITIONHARDWARE_H__
