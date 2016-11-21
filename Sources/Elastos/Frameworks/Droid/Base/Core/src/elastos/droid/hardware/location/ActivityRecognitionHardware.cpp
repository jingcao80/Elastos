
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/hardware/location/ActivityRecognitionHardware.h"
#include "elastos/droid/hardware/location/ActivityRecognitionEvent.h"
#include "elastos/droid/hardware/location/ActivityChangedEvent.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/CRemoteCallbackList.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "hardware/activity_recognition.h"
#include "hardware/hardware.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CAR_INTERFACE_IMPL_3(ActivityRecognitionHardware, Object, IActivityRecognitionHardware,
        IIActivityRecognitionHardware, IBinder)

const String ActivityRecognitionHardware::TAG("ActivityRecognitionHardware");

const String ActivityRecognitionHardware::HARDWARE_PERMISSION = Elastos::Droid::Manifest::permission::LOCATION_HARDWARE;
const Int32 ActivityRecognitionHardware::INVALID_ACTIVITY_TYPE = -1;
const Int32 ActivityRecognitionHardware::NATIVE_SUCCESS_RESULT = 0;

AutoPtr<IActivityRecognitionHardware> ActivityRecognitionHardware::sSingletonInstance;
Object ActivityRecognitionHardware::sSingletonInstanceLock;

// native bindings
ECode ActivityRecognitionHardware::sStaticBlockInit = NativeClassInit();

ActivityRecognitionHardware::ActivityRecognitionHardware()
{

}

ActivityRecognitionHardware::ActivityRecognitionHardware(
    /* [in] */ IContext* context)
    : mContext(context)
{
    NativeInitialize();

    FetchSupportedActivities((ArrayOf<String>**)&mSupportedActivities);
    CRemoteCallbackList::New((IRemoteCallbackList**)&mSinks);
}

ActivityRecognitionHardware::~ActivityRecognitionHardware()
{
}

ECode ActivityRecognitionHardware::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IActivityRecognitionHardware** instance)
{
    VALIDATE_NOT_NULL(instance);

    {    AutoLock syncLock(sSingletonInstanceLock);
        if (sSingletonInstance == NULL) {
            sSingletonInstance = new ActivityRecognitionHardware(context);
        }

        *instance = sSingletonInstance;
        REFCOUNT_ADD(*instance);
    }
    return NOERROR;
}

ECode ActivityRecognitionHardware::IsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeIsSupported();
    return NOERROR;
}

ECode ActivityRecognitionHardware::GetSupportedActivities(
    /* [out, callee] */ ArrayOf<String>** activities)
{
    VALIDATE_NOT_NULL(activities);

    FAIL_RETURN(CheckPermissions())
    *activities = mSupportedActivities;
    REFCOUNT_ADD(*activities);
    return NOERROR;
}

ECode ActivityRecognitionHardware::IsActivitySupported(
    /* [in] */ const String& activity,
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    FAIL_RETURN(CheckPermissions())
    Int32 activityType = GetActivityType(activity);
    *supported = activityType != INVALID_ACTIVITY_TYPE;
    return NOERROR;
}

ECode ActivityRecognitionHardware::RegisterSink(
    /* [in] */ IIActivityRecognitionHardwareSink* sink,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckPermissions())
    return mSinks->Register(sink, result);
}

ECode ActivityRecognitionHardware::UnregisterSink(
    /* [in] */ IIActivityRecognitionHardwareSink* sink,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckPermissions())
    return mSinks->Unregister(sink, result);
}

ECode ActivityRecognitionHardware::EnableActivityEvent(
    /* [in] */ const String& activity,
    /* [in] */ Int32 eventType,
    /* [in] */ Int64 reportLatencyNs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckPermissions())
    Int32 activityType = GetActivityType(activity);
    if (activityType == INVALID_ACTIVITY_TYPE) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 _result = NativeEnableActivityEvent(activityType, eventType, reportLatencyNs);
    *result =  _result == NATIVE_SUCCESS_RESULT;
    return NOERROR;
}

ECode ActivityRecognitionHardware::DisableActivityEvent(
    /* [in] */ const String& activity,
    /* [in] */ Int32 eventType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckPermissions())
    Int32 activityType = GetActivityType(activity);
    if (activityType == INVALID_ACTIVITY_TYPE) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 _result = NativeDisableActivityEvent(activityType, eventType);
    *result = _result == NATIVE_SUCCESS_RESULT;
    return NOERROR;
}

ECode ActivityRecognitionHardware::Flush(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckPermissions())
    Int32 _result = NativeFlush();
    *result = _result == NATIVE_SUCCESS_RESULT;
    return NOERROR;
}

ECode ActivityRecognitionHardware::OnActivityChanged(
    /* [in] */ ArrayOf<Event*>* events)
{
    if (events == NULL || events->GetLength() == 0) {
        Logger::D(TAG, String("No events to broadcast for onActivityChanged."));
        return NOERROR;
    }

    Int32 eventsLength = events->GetLength();
    AutoPtr<ArrayOf<IActivityRecognitionEvent*> > activityRecognitionEventArray
            = ArrayOf<IActivityRecognitionEvent*>::Alloc(eventsLength);

    for (Int32 i = 0; i < eventsLength; ++i) {
        AutoPtr<Event> event = (*events)[i];
        String activityName = GetActivityName(event->mActivity);
        AutoPtr<ActivityRecognitionEvent> tmp = new ActivityRecognitionEvent();
        tmp->constructor(activityName, event->mType, event->mTimestamp);
        activityRecognitionEventArray->Set(i, tmp.Get());
    }
    AutoPtr<ActivityChangedEvent> activityChangedEvent = new ActivityChangedEvent();
    activityChangedEvent->constructor(activityRecognitionEventArray);

    Int32 size;
    mSinks->BeginBroadcast(&size);
    ECode ec = NOERROR;
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mSinks->GetBroadcastItem(i, (IInterface**)&obj);
        AutoPtr<IIActivityRecognitionHardwareSink> sink = IIActivityRecognitionHardwareSink::Probe(obj);
        //try {
        ec = sink->OnActivityChanged(activityChangedEvent.Get());
        //} catch (RemoteException e) {
        if (FAILED(ec)) {
            Logger::E(TAG, String("Error delivering activity changed event.")/*, e*/);
        }
        //}
    }
    mSinks->FinishBroadcast();
    return ec;
}

String ActivityRecognitionHardware::GetActivityName(
    /* [in] */ Int32 activityType)
{
    if (activityType < 0 || activityType >= mSupportedActivities->GetLength()) {
        StringBuilder sb;
        sb += "Invalid ActivityType: ";
        sb += activityType;
        sb += ", SupportedActivities: ";
        sb += mSupportedActivities->GetLength();
        Logger::E(TAG, sb.ToString());
        return String(NULL);
    }

    return (*mSupportedActivities)[activityType];
}

Int32 ActivityRecognitionHardware::GetActivityType(
    /* [in] */ const String& activity)
{
    if (TextUtils::IsEmpty(activity)) {
        return INVALID_ACTIVITY_TYPE;
    }

    Int32 supportedActivitiesLength = mSupportedActivities->GetLength();
    for (Int32 i = 0; i < supportedActivitiesLength; ++i) {
        if (activity.Equals((*mSupportedActivities)[i])) {
            return i;
        }
    }

    return INVALID_ACTIVITY_TYPE;
}

ECode ActivityRecognitionHardware::CheckPermissions()
{
    StringBuilder sb;
    sb += "Permission \'";
    sb += HARDWARE_PERMISSION;
    sb += "\' not granted to access ActivityRecognitionHardware";

    return mContext->EnforceCallingPermission(HARDWARE_PERMISSION, sb.ToString());
}

ECode ActivityRecognitionHardware::FetchSupportedActivities(
    /* [out, callee] */ ArrayOf<String>** activities)
{
    AutoPtr<ArrayOf<String> > supportedActivities = NativeGetSupportedActivities();
    if (supportedActivities != NULL) {
        *activities = supportedActivities;
        REFCOUNT_ADD(*activities);
        return NOERROR;
    }

    *activities = ArrayOf<String>::Alloc(0);
    REFCOUNT_ADD(*activities);
    return NOERROR;
}

ECode ActivityRecognitionHardware::ToString(
    /* [out] */ String* info)
{
    return NOERROR;
}

// keep base connection data from the HAL
static activity_recognition_module_t* sModule = NULL;
static activity_recognition_device_t* sDevice = NULL;

static AutoPtr<ActivityRecognitionHardware> sCallbacksObject = NULL;

/**
 * Handle activity recognition events from HAL.
 */
static void activity_callback(
    const activity_recognition_callback_procs_t* procs,
    const activity_event_t* events,
    int count)
{
    if (events == NULL || count <= 0) {
        Logger::E("ActivityRecognitionHardware", "Invalid activity_callback. Count: %d, Events: %p", count, events);
        return;
    }

    AutoPtr<IThread> thread;
    Thread::Attach((IThread**)&thread);

    AutoPtr<ArrayOf<ActivityRecognitionHardware::Event*> > events_array = ArrayOf<ActivityRecognitionHardware::Event*>::Alloc(count);
    for (int i = 0; i < count; ++i) {
        const activity_event_t* event = &events[i];
        AutoPtr<ActivityRecognitionHardware::Event> event_object = new ActivityRecognitionHardware::Event();
        event_object->mActivity = event->activity;
        event_object->mType = event->event_type;
        event_object->mTimestamp = event->timestamp;

        events_array->Set(i, event_object);
    }

    sCallbacksObject->OnActivityChanged(events_array);

    // TODO: ideally we'd let the HAL register the callback thread only once
    thread->Detach();
}

activity_recognition_callback_procs_t sCallbacks = {
    activity_callback,
};

ECode ActivityRecognitionHardware::NativeClassInit()
{
    // open the hardware module
    int error = hw_get_module(
            ACTIVITY_RECOGNITION_HARDWARE_MODULE_ID,
            (const hw_module_t**) &sModule);
    if (error != 0) {
        Logger::E(TAG, "Error hw_get_module: %d", error);
        return NOERROR;
    }

    error = activity_recognition_open(&sModule->common, &sDevice);
    if (error != 0) {
        Logger::E(TAG, "Error opening device: %d", error);
        return NOERROR;
    }

    // register callbacks
    sDevice->register_activity_callback(sDevice, &sCallbacks);
    return NOERROR;
}

Boolean ActivityRecognitionHardware::NativeIsSupported()
{
    if (sModule != NULL && sDevice != NULL ) {
        return TRUE;
    }
    return FALSE;
}

void ActivityRecognitionHardware::NativeInitialize()
{
    if (sCallbacksObject == NULL) {
        sCallbacksObject = this;
    }
    else {
        Logger::D(TAG, "Callbacks Object was already initialized.");
    }

    if (sDevice != NULL) {
        sDevice->register_activity_callback(sDevice, &sCallbacks);
    }
    else {
        Logger::D(TAG, "ActivityRecognition device not found during initialization.");
    }
    return;
}

void ActivityRecognitionHardware::NativeRelease()
{
    if (sDevice == NULL) {
        return;
    }

    int error = activity_recognition_close(sDevice);
    if (error != 0) {
        Logger::E(TAG, "Error closing device: %d", error);
        return;
    }
    return;
}

AutoPtr<ArrayOf<String> > ActivityRecognitionHardware::NativeGetSupportedActivities()
{
    if (sModule == NULL) {
        return NULL;
    }

    char const* const* list = NULL;
    int list_size = sModule->get_supported_activities_list(sModule, &list);
    if (list_size <= 0 || list == NULL) {
        return NULL;
    }

    AutoPtr<ArrayOf<String> > string_array = ArrayOf<String>::Alloc(list_size);
    if (string_array == NULL) {
        Logger::E(TAG, "Unable to create string array for supported activities.");
        return NULL;
    }

    for (int i = 0; i < list_size; ++i) {
        const char* string_ptr = const_cast<const char*>(list[i]);
        String string(string_ptr);
        string_array->Set(i, string);
    }

    return string_array;
}

Int32 ActivityRecognitionHardware::NativeEnableActivityEvent(
    /* [in] */ Int32 activityType,
    /* [in] */ Int32 eventType,
    /* [in] */ Int64 reportLatenceNs)
{
    return sDevice->enable_activity_event(
            sDevice,
            (uint32_t) activityType,
            (uint32_t) eventType,
            reportLatenceNs);
}

Int32 ActivityRecognitionHardware::NativeDisableActivityEvent(
    /* [in] */ Int32 activityType,
    /* [in] */ Int32 eventType)
{
    return sDevice->disable_activity_event(
            sDevice,
            (uint32_t) activityType,
            (uint32_t) eventType);
}

Int32 ActivityRecognitionHardware::NativeFlush()
{
    return sDevice->flush(sDevice);
}

} //Location
} //Hardware
} //Droid
} //Elastos