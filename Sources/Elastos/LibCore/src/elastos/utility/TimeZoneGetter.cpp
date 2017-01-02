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

#include "TimeZoneGetter.h"
#include <cutils/properties.h>

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(TimeZoneGetter, Object, ITimeZoneGetter)

AutoPtr<ITimeZoneGetter> TimeZoneGetter::sInstance;

TimeZoneGetter::TimeZoneGetter()
{
}

TimeZoneGetter::~TimeZoneGetter()
{}


ECode TimeZoneGetter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "TimeZoneGetter";
    return NOERROR;
}

AutoPtr<ITimeZoneGetter> TimeZoneGetter::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new DefaultTimeZoneGetter();
    }

    return sInstance;
}

ECode TimeZoneGetter::SetInstance(
    /* [in] */ ITimeZoneGetter* instance)
{
    if (instance == NULL) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    sInstance = instance;
    return NOERROR;
}

ECode DefaultTimeZoneGetter::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);

    char buf[PROP_VALUE_MAX + 1];

    int len = property_get("persist.sys.timezone", buf, "");
    if (len >= 0) {
        *id = String(buf);
    }
    else {
        *id = String("");
    }

    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
