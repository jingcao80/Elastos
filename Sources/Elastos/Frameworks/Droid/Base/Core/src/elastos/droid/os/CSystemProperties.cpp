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
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/SystemProperties.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CSystemProperties, Singleton, ISystemProperties)

CAR_SINGLETON_IMPL(CSystemProperties)

ECode CSystemProperties::Get(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    return SystemProperties::Get(key, value);
}

ECode CSystemProperties::Get(
    /* [in] */ const String& key,
    /* [in] */ const String& def,
    /* [out] */ String* value)
{
    return SystemProperties::Get(key, def, value);
}

ECode CSystemProperties::GetInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 def,
    /* [out] */ Int32* value)
{
    return SystemProperties::GetInt32(key, def, value);
}

ECode CSystemProperties::GetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 def,
    /* [out] */ Int64* value)
{
    return SystemProperties::GetInt64(key, def, value);
}

ECode CSystemProperties::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean def,
    /* [out] */ Boolean* value)
{
    return SystemProperties::GetBoolean(key, def, value);
}

ECode CSystemProperties::Set(
    /* [in] */ const String& key,
    /* [in] */ const String& val)
{
    return SystemProperties::Set(key, val);
}

ECode CSystemProperties::AddChangeCallback(
    /* [in] */ IRunnable* cb)
{
    return SystemProperties::AddChangeCallback(cb);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
