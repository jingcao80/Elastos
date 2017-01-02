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

#include "elastos/droid/server/telecom/Timeouts.h"
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::Provider::Settings;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String Timeouts::PREFIX("telecom.");

Timeouts::Timeouts()
{}

ECode Timeouts::Get(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ const String& key,
    /* [in] */ Int64 defaultValue,
    /* [out] */ Int64* result)
{
    return Settings::Secure::GetInt64(contentResolver, PREFIX + key, defaultValue, result);
}

ECode Timeouts::GetDirectToVoicemailMillis(
    /* [in] */ IContentResolver* contentResolver,
    /* [out] */ Int64* result)
{
    return Get(contentResolver, String("direct_to_voicemail_ms"), 500L, result);
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
