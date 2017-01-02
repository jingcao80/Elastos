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

#include "elastos/droid/hardware/location/ActivityChangedEvent.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CAR_INTERFACE_IMPL_2(ActivityChangedEvent, Object, IActivityChangedEvent, IParcelable)

ActivityChangedEvent::ActivityChangedEvent()
    : mActivityRecognitionEvents(NULL)
{
}

ActivityChangedEvent::~ActivityChangedEvent()
{
}

ECode ActivityChangedEvent::constructor()
{
    return NOERROR;
}

ECode ActivityChangedEvent::constructor(
    /* [in] */ ArrayOf<IActivityRecognitionEvent*>* activityRecognitionEvents)
{
    if (activityRecognitionEvents == NULL) {
        // throw new InvalidParameterException(
        //         "Parameter 'activityRecognitionEvents' must not be null.");
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    return Arrays::AsList(activityRecognitionEvents, (IList**)&mActivityRecognitionEvents);
}

ECode ActivityChangedEvent::GetActivityRecognitionEvents(
    /* [out] */ IIterable** result)
{
    VALIDATE_NOT_NULL(result);

    *result = IIterable::Probe(mActivityRecognitionEvents);
    REFCOUNT_ADD(*result);

    return NOERROR;
}

ECode ActivityChangedEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    AutoPtr<ArrayOf<IActivityRecognitionEvent*> > activityRecognitionEventArray;
    mActivityRecognitionEvents->ToArray((ArrayOf<IInterface*>**)&activityRecognitionEventArray);
    dest->WriteInt32(activityRecognitionEventArray->GetLength());
    dest->WriteArrayOf((Handle32)activityRecognitionEventArray.Get());
    return NOERROR;
}

ECode ActivityChangedEvent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 length;
    source->ReadInt32(&length);
    AutoPtr<ArrayOf<IActivityRecognitionEvent*> > data  = ArrayOf<IActivityRecognitionEvent*>::Alloc(length);
    source->ReadArrayOf((Handle32*)&data);
    Arrays::AsList(data, (IList**)&mActivityRecognitionEvents);
    return NOERROR;
}

ECode ActivityChangedEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder += "[ ActivityChangedEvent:";

    Int32 size;
    mActivityRecognitionEvents->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mActivityRecognitionEvents->Get(i, (IInterface**)&obj);
        builder += "\n    ";
        String tmp = Object::ToString(obj);
        builder += tmp;
    }

    builder += ("\n]");

    return builder.ToString(str);
}

} //Location
} //Hardware
} //Droid
} //Elastos