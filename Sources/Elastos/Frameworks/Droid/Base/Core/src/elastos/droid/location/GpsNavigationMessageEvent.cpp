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

#include "elastos/droid/location/GpsNavigationMessageEvent.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

const String GpsNavigationMessageEvent::TAG("GpsNavigationMessageEvent");

CAR_INTERFACE_IMPL_2(GpsNavigationMessageEvent, Object, IGpsNavigationMessageEvent, IParcelable)

GpsNavigationMessageEvent::GpsNavigationMessageEvent()
{
}

ECode GpsNavigationMessageEvent::constructor()
{
    return NOERROR;
}

ECode GpsNavigationMessageEvent::constructor(
    /* [in] */ IGpsNavigationMessage* message)
{
    if (message == NULL) {
        Logger::E(TAG, "Parameter 'message' must not be null.");
        return E_INVALID_PARAMETER_EXCEPTION;
    }
    mNavigationMessage = message;
    return NOERROR;
}

ECode GpsNavigationMessageEvent::GetNavigationMessage(
    /* [out] */ IGpsNavigationMessage** message)
{
    VALIDATE_NOT_NULL(message)
    *message = mNavigationMessage;
    REFCOUNT_ADD(*message)
    return NOERROR;
}

ECode GpsNavigationMessageEvent::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    // ClassLoader classLoader = getClass().getClassLoader();
    // GpsNavigationMessage navigationMessage = in.readParcelable(classLoader);
    // return new GpsNavigationMessageEvent(navigationMessage);
    return E_NOT_IMPLEMENTED;
}

ECode GpsNavigationMessageEvent::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    // parcel.writeParcelable(mNavigationMessage, flags);
    return E_NOT_IMPLEMENTED;
}

ECode GpsNavigationMessageEvent::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder builder("[ GpsNavigationMessageEvent:\n\n");
    AutoPtr<IObject> o = IObject::Probe(mNavigationMessage);
    String str;
    o->ToString(&str);
    builder += str;
    builder += "\n]";
    *result = builder.ToString();
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
