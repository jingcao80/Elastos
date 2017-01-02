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

#ifndef __ELASTOS_DROID_APP_USAGE_USAGE_EVENTS_H__
#define __ELASTOS_DROID_APP_USAGE_USAGE_EVENTS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

/**
 * An event representing a state change for a component.
 */
class UsageEvent
    : public Object
    , public IUsageEvent
{
public:
    CAR_INTERFACE_DECL()

    UsageEvent();

    virtual ~UsageEvent();

    CARAPI constructor();

    CARAPI SetPackageName(
        /* [in] */ const String& value);

    /**
     * The package name of the source of this event.
     */
    CARAPI GetPackageName(
        /* [out] */ String* result);

    CARAPI SetClassName(
        /* [in] */ const String& value);

    /**
     * The class name of the source of this event. This may be null for
     * certain events.
     */
    CARAPI GetClassName(
        /* [out] */ String* result);

    CARAPI SetTimeStamp(
        /* [in] */ Int64 value);

    /**
     * The time at which this event occurred, measured in milliseconds since the epoch.
     * <p/>
     * See {@link System#currentTimeMillis()}.
     */
    CARAPI GetTimeStamp(
        /* [out] */ Int64* result);

    CARAPI SetEventType(
        /* [in] */ Int32 value);

    /**
     * The event type.
     *
     * See {@link #MOVE_TO_BACKGROUND}
     * See {@link #MOVE_TO_FOREGROUND}
     */
    CARAPI GetEventType(
        /* [out] */ Int32* result);

    CARAPI SetConfiguration(
        /* [in] */ IConfiguration* value);

    /**
     * Returns a {@link Configuration} for this event if the event is of type
     * {@link #CONFIGURATION_CHANGE}, otherwise it returns null.
     */
    CARAPI GetConfiguration(
        /* [out] */IConfiguration** config);

public:
    /**
     * {@hide}
     */
    String mPackage;

    /**
     * {@hide}
     */
    String mClass;

    /**
     * {@hide}
     */
    Int64 mTimeStamp;

    /**
     * {@hide}
     */
    Int32 mEventType;

    /**
     * Only present for {@link #CONFIGURATION_CHANGE} event types.
     * {@hide}
     */
    AutoPtr<IConfiguration> mConfiguration;
};


/**
 * A result returned from {@link android.app.usage.UsageStatsManager#queryEvents(Int64, Int64)}
 * from which to read {@link android.app.usage.UsageEvents.Event} objects.
 */
class UsageEvents
    : public Object
    , public IUsageEvents
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    UsageEvents();

    virtual ~UsageEvents();

    CARAPI constructor();

    /**
     * Construct the iterator in preparation for writing it to a parcel.
     * {@hide}
     */
    CARAPI constructor(
        /* [in] */ IList* events,
        /* [in] */ ArrayOf<String>* stringPool);

    /**
     * Returns whether or not there are more events to read using
     * {@link #getNextEvent(android.app.usage.UsageEvents.Event)}.
     *
     * @return true if there are more events, FALSE otherwise.
     */
    CARAPI HasNextEvent(
        /* [out] */ Boolean* result);

    /**
     * Retrieve the next {@link android.app.usage.UsageEvents.Event} from the collection and put the
     * resulting data into {@code eventOut}.
     *
     * @param eventOut The {@link android.app.usage.UsageEvents.Event} object that will receive the
     *                 next event data.
     * @return true if an event was available, FALSE if there are no more events.
     */
    CARAPI GetNextEvent(
        /* [in] */ IUsageEvent* eventOut,
        /* [out] */ Boolean* result);

    /**
     * Resets the collection so that it can be iterated over from the beginning.
     *
     * @hide When this object is iterated to completion, the parcel is destroyed and
     * so resetToStart doesn't work.
     */
    CARAPI ResetToStart();

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

private:
    CARAPI FindStringIndex(
        /* [in] */ const String& str,
        /* [out] */ Int32* index);

    /**
     * Writes a single event to the parcel. Modify this when updating {@link Event}.
     */
    void WriteEventToParcel(
        /* [in] */ IUsageEvent* event,
        /* [in] */ IParcel* p);

    /**
     * Reads a single event from the parcel. Modify this when updating {@link Event}.
     */
    void ReadEventFromParcel(
        /* [in] */ IParcel* p,
        /* [in] */ IUsageEvent* eventOut);

    void Finalize();

private:
    // Only used when creating the resulting events. Not used for reading/unparceling.
    AutoPtr<IList> mEventsToWrite; //List<Event>

    // Only used for reading/unparceling events.
    AutoPtr<IParcel> mParcel;
    Int32 mEventCount;

    Int32 mIndex;

    /*
     * In order to save space, since ComponentNames will be duplicated everywhere,
     * we use a map and index into it.
     */
    AutoPtr<ArrayOf<String> > mStringPool;
};

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_USAGE_USAGE_EVENTS_H__