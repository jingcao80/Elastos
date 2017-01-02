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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>
#include <cutils/properties.h>
#include "utils/misc.h"
#include <stdio.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

const Int32 SystemProperties::SYSTEM_PROPERTY_NAME_MAX = 31;
const Int32 SystemProperties::SYSTEM_PROPERTY_VALUE_MAX = 91;

List< AutoPtr<IRunnable> > SystemProperties::sChangeCallbacks;
Object SystemProperties::sChangeCallbacksLock;

ECode SystemProperties::Get(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);
    if (key.IsNull() || key.GetLength() > SYSTEM_PROPERTY_NAME_MAX) {
        Logger::E("SystemProperties", "IllegalArgumentException: key.length > %d", SYSTEM_PROPERTY_NAME_MAX);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *value = NativeGet(key);
    return NOERROR;
}

ECode SystemProperties::Get(
    /* [in] */ const String& key,
    /* [in] */ const String& def,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = def;
    if (key.IsNull() || key.GetLength() > SYSTEM_PROPERTY_NAME_MAX) {
        Logger::E("SystemProperties", "IllegalArgumentException: key.length > %d", SYSTEM_PROPERTY_NAME_MAX);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *value = NativeGet(key, def);
    return NOERROR;
}

ECode SystemProperties::GetInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 def,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = def;

    if (key.IsNull() || key.GetLength() > SYSTEM_PROPERTY_NAME_MAX) {
        Logger::E("SystemProperties", "IllegalArgumentException: key.length > %d", SYSTEM_PROPERTY_NAME_MAX);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = NativeGetInt32(key, def);
    return NOERROR;
}

ECode SystemProperties::GetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 def,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = def;

    if (key.IsNull() || key.GetLength() > SYSTEM_PROPERTY_NAME_MAX) {
        Logger::E("SystemProperties", "IllegalArgumentException: key.length > %d", SYSTEM_PROPERTY_NAME_MAX);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = NativeGetInt64(key, def);
    return NOERROR;
}

ECode SystemProperties::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean def,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = def;

    if (key.IsNull() || key.GetLength() > SYSTEM_PROPERTY_NAME_MAX) {
        Logger::E("SystemProperties", "IllegalArgumentException: key.length > %d", SYSTEM_PROPERTY_NAME_MAX);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = NativeGetBoolean(key, def);
    return NOERROR;
}

ECode SystemProperties::Set(
    /* [in] */ const String& key,
    /* [in] */ const String& val)
{
    if (key.IsNull() || key.GetLength() > SYSTEM_PROPERTY_NAME_MAX) {
        Logger::E("SystemProperties", "IllegalArgumentException: key.length > %d", SYSTEM_PROPERTY_NAME_MAX);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!val.IsNull() && val.GetLength() > SYSTEM_PROPERTY_VALUE_MAX) {
        Logger::E("SystemProperties", "IllegalArgumentException: val.length > %d", SYSTEM_PROPERTY_NAME_MAX);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    NativeSet(key, val);
    return NOERROR;
}

String SystemProperties::NativeGet(
    /* [in] */ const String& key)
{
    return NativeGet(key, String());
}

String SystemProperties::NativeGet(
    /* [in] */ const String& key,
    /* [in] */ const String& def)
{
    int len;
    char buf[SYSTEM_PROPERTY_VALUE_MAX + 1];

    len = property_get((const char*)key, buf, "");
    if ((len <= 0) && (!def.IsNull())) {
        return def;
    }
    else if (len >= 0) {
        return String(buf);
    }
    else {
        return String("");
    }
}

Int32 SystemProperties::NativeGetInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 def)
{
    int len;
    char buf[PROPERTY_VALUE_MAX + 1];
    Int32 result = def;

    len = property_get(key, buf, "");
    if (len > 0) {
        Int32 temp;
        if (sscanf(buf, "%d", &temp) == 1) {
            result = temp;
        }
    }

    return result;
}

Int64 SystemProperties::NativeGetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 def)
{
    int len;
    char buf[PROPERTY_VALUE_MAX + 1];
    Int64 result = def;

    len = property_get(key, buf, "");
    if (len > 0) {
        Int64 temp;
        if (sscanf(buf, "%lld", &temp) == 1)
            result = temp;
    }

    return result;
}

Boolean SystemProperties::NativeGetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean def)
{
    int len;
    char buf[PROPERTY_VALUE_MAX + 1];
    Boolean result = def;

    len = property_get(key, buf, "");
    if (len == 1) {
        char ch = buf[0];
        if (ch == '0' || ch == 'n') {
            result = FALSE;
        }
        else if (ch == '1' || ch == 'y') {
            result = TRUE;
        }
    }
    else if (len > 1) {
        if (!strcmp(buf, "no") || !strcmp(buf, "false") || !strcmp(buf, "off")) {
            result = FALSE;
        }
        else if (!strcmp(buf, "yes") || !strcmp(buf, "true") || !strcmp(buf, "on")) {
            result = TRUE;
        }
    }

    return result;
}

void SystemProperties::NativeSet(
    /* [in] */ const String& key,
    /* [in] */ const String& val)
{
    Int32 UNUSED(err);
    String value = val;
    if (value.IsNull()) {
        value = String("");       /* NULL pointer not allowed here */
    }

    err = property_set(key, val);
}

void SystemProperties::NativeAddChangeCallback()
{
    // This is called with the Java lock held.
    android::add_sysprop_change_callback(SystemProperties::CallChangeCallbacks, -10000);
}

ECode SystemProperties::AddChangeCallback(
    /* [in] */ IRunnable* callback)
{
    AutoLock lock(sChangeCallbacksLock);
    if (sChangeCallbacks.IsEmpty()) {
        NativeAddChangeCallback();
    }
    sChangeCallbacks.PushBack(callback);
    return NOERROR;
}

void SystemProperties::CallChangeCallbacks()
{
    AutoLock lock(sChangeCallbacksLock);
    //Log.i("foo", "Calling " + sChangeCallbacks.size() + " change callbacks!");
    if (sChangeCallbacks.IsEmpty()) {
        return;
    }

    AutoPtr< List< AutoPtr<IRunnable> > > callbacks = new List< AutoPtr<IRunnable> >(sChangeCallbacks);
    List< AutoPtr<IRunnable> >::Iterator it = callbacks->Begin();
    for (; it != callbacks->End(); ++it) {
        (*it)->Run();
    }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
