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

#include "elastos/droid/app/usage/UsageEvents.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/utility/Arrays.h"
#include <elastos/utility/logging/Logger.h>
#include <binder/Parcel.h>

using Elastos::Droid::Os::CParcel;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

//==================================================================
// UsageEvent
//==================================================================

CAR_INTERFACE_IMPL(UsageEvent, Object, IUsageEvent)

UsageEvent::UsageEvent()
    : mTimeStamp(0)
    , mEventType(0)
{}

UsageEvent::~UsageEvent()
{}

ECode UsageEvent::constructor()
{
    return NOERROR;
}

ECode UsageEvent::SetPackageName(
    /* [in] */ const String& value)
{
    mPackage = value;
    return NOERROR;
}

ECode UsageEvent::GetPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPackage;
    return NOERROR;
}

ECode UsageEvent::SetClassName(
    /* [in] */ const String& value)
{
    mClass = value;
    return NOERROR;
}

ECode UsageEvent::GetClassName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mClass;
    return NOERROR;
}

ECode UsageEvent::SetTimeStamp(
    /* [in] */ Int64 value)
{
    mTimeStamp = value;
    return NOERROR;
}

ECode UsageEvent::GetTimeStamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTimeStamp;
    return NOERROR;
}

ECode UsageEvent::SetEventType(
    /* [in] */ Int32 value)
{
    mEventType = value;
    return NOERROR;
}

ECode UsageEvent::GetEventType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEventType;
    return NOERROR;
}

ECode UsageEvent::SetConfiguration(
    /* [in] */ IConfiguration* value)
{
    mConfiguration = value;
    return NOERROR;
}

ECode UsageEvent::GetConfiguration(
    /* [out] */ IConfiguration** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConfiguration;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//==================================================================
// UsageEvents
//==================================================================

CAR_INTERFACE_IMPL_2(UsageEvents, Object, IUsageEvents, IParcelable)

UsageEvents::UsageEvents()
    : mEventCount(0)
    , mIndex(0)
{}

UsageEvents::~UsageEvents()
{
    Finalize();
}

ECode UsageEvents::constructor()
{
    return NOERROR;
}

ECode UsageEvents::constructor(
    /* [in] */ IList* events,
    /* [in] */ ArrayOf<String>* stringPool)
{
    mStringPool = stringPool;
    events->GetSize(&mEventCount);
    mEventsToWrite = events;
    return NOERROR;
}

ECode UsageEvents::HasNextEvent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndex < mEventCount;
    return NOERROR;
}

ECode UsageEvents::GetNextEvent(
    /* [in] */ IUsageEvent* eventOut,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mIndex >= mEventCount) {
        return NOERROR;
    }

    ReadEventFromParcel(mParcel, eventOut);

    mIndex++;
    if (mIndex >= mEventCount) {
        mParcel = NULL;
    }
    *result = TRUE;
    return NOERROR;
}

ECode UsageEvents::ResetToStart()
{
    mIndex = 0;
    if (mParcel != NULL) {
        mParcel->SetDataPosition(0);
    }

    return NOERROR;
}

ECode UsageEvents::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mEventCount);
    dest->WriteInt32(mIndex);
    if (mEventCount > 0) {
        dest->WriteArrayOfString(mStringPool);

        if (mEventsToWrite != NULL) {
            // Write out the events
            // Parcel p = Parcel.obtain();
            AutoPtr<IParcel> p;
            CParcel::New((IParcel**)&p);
            // try {
                p->SetDataPosition(0);
                for (Int32 i = 0; i < mEventCount; i++) {
                    AutoPtr<IInterface> obj;
                    mEventsToWrite->Get(i, (IInterface**)&obj);
                    IUsageEvent* event = IUsageEvent::Probe(obj);
                    WriteEventToParcel(event, p);
                }

                Int32 listByteLength;
                p->GetDataPosition(&listByteLength);

                // Write the total length of the data.
                dest->WriteInt32(listByteLength);

                // Write our current position into the data.
                dest->WriteInt32(0);

                // Write the data.
                dest->AppendFrom(p, 0, listByteLength);
            // } finally {
            //     p->Recycle();
            // }
        }
        else if (mParcel != NULL) {
            // Write the total length of the data.
            Int32 size, position;
            mParcel->GetDataSize(&size);
            mParcel->GetDataPosition(&position);
            dest->WriteInt32(size);

            // Write out current position into the data.
            dest->WriteInt32(position);

            // Write the data.
            dest->AppendFrom(mParcel, 0, size);
        }
        else {
            Logger::E("UsageEvents", "Either mParcel or mEventsToWrite must not be NULL");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode UsageEvents::ReadFromParcel(
    /* [in] */ IParcel*in)
{
    in->ReadInt32(&mEventCount);
    in->ReadInt32(&mIndex);
    if (mEventCount > 0) {
        in->ReadArrayOfString((ArrayOf<String>**)&mStringPool);

        Int32 listByteLength, positionInParcel;
        in->ReadInt32(&listByteLength);
        in->ReadInt32(&positionInParcel);

        CParcel::New((IParcel**)&mParcel);
        mParcel->SetDataPosition(0);
        Int32 position;
        in->GetDataPosition(&position);
        mParcel->AppendFrom(in, position, listByteLength);
        mParcel->GetDataPosition(&position);

        android::Parcel* parcelObj;
        mParcel->GetDataPayload((Handle32*)&parcelObj);
        parcelObj->setDataSize(position);
        parcelObj->setDataPosition(positionInParcel);
    }
    return NOERROR;
}

ECode UsageEvents::FindStringIndex(
    /* [in] */ const String& str,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    Int32 index;
    Arrays::BinarySearch(mStringPool, str, &index);
    if (index < 0) {
        Logger::E("UsageEvents", "String '%s' is not in the string pool", str.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = index;
    return NOERROR;
}

void UsageEvents::WriteEventToParcel(
    /* [in] */ IUsageEvent* event,
    /* [in] */ IParcel* p)
{
    Int32 packageIndex = -1;
    String package;
    event->GetPackageName(&package);
    if (!package.IsNull()) {
        FindStringIndex(package, &packageIndex);
    }

    Int32 classIndex = -1;
    String className;
    event->GetClassName(&className);
    if (!className.IsNull()) {
        FindStringIndex(className, &classIndex);
    }

    p->WriteInt32(packageIndex);
    p->WriteInt32(classIndex);
    Int32 eventType;
    event->GetEventType(&eventType);
    p->WriteInt32(eventType);
    Int64 stamp;
    event->GetTimeStamp(&stamp);
    p->WriteInt64(stamp);

    if (eventType == IUsageEvent::CONFIGURATION_CHANGE) {
        AutoPtr<IConfiguration> config;
        event->GetConfiguration((IConfiguration**)&config);
        IParcelable::Probe(config)->WriteToParcel(p);
    }
}

void UsageEvents::ReadEventFromParcel(
    /* [in] */ IParcel* p,
    /* [in] */ IUsageEvent* eo)
{
    UsageEvent* eventOut = (UsageEvent*)eo;
    Int32 packageIndex;
    p->ReadInt32(&packageIndex);
    if (packageIndex >= 0) {
        eventOut->mPackage = (*mStringPool)[packageIndex];
    }

    Int32 classIndex;
    p->ReadInt32(&classIndex);
    if (classIndex >= 0) {
        eventOut->mClass = (*mStringPool)[classIndex];
    }
    p->ReadInt32(&eventOut->mEventType);
    p->ReadInt64(&eventOut->mTimeStamp);

    // Extract the configuration for configuration change events.
    if (eventOut->mEventType == IUsageEvent::CONFIGURATION_CHANGE) {
        CConfiguration::New((IConfiguration**)&eventOut->mConfiguration);
        IParcelable::Probe(eventOut->mConfiguration)->ReadFromParcel(p);
    }
}

void UsageEvents::Finalize()
{
    mParcel = NULL;
}


} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos
