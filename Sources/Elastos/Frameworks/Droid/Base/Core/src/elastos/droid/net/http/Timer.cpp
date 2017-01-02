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

#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/Timer.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::SystemClock;

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

Timer::Timer()
    : mStart(0)
    , mLast(0)
{
}

ECode Timer::constructor()
{
    mStart = mLast = SystemClock::GetUptimeMillis();
    return NOERROR;
}

Timer::~Timer()
{}

ECode Timer::Mark(
    /* [in] */ const String& message) {
    Int64 now = SystemClock::GetUptimeMillis();
    if (HttpLog::LOGV) {
        HttpLog::V(message + String(" ") + StringUtils::ToString(now - mLast)
            + String(" total ") + StringUtils::ToString(now - mStart));
    }
    mLast = now;

    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
