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
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::ILog;

using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

const String HttpLog::LOGTAG("http");
const Boolean HttpLog::DEBUG = FALSE;
const Boolean HttpLog::LOGV = FALSE;

ECode HttpLog::V(
    /* [in] */ const char* fmt, ...)
{
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    String name;
    thread->GetName(&name);
    String sFormat("%d %s ");
    va_list ap;
    va_start(ap, fmt);
    sFormat.Append(fmt);
    Logger::V(LOGTAG, sFormat.string(), SystemClock::GetUptimeMillis(), name.string(), ap);
    va_end(ap);
    return NOERROR;
}

ECode HttpLog::E(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Logger::E(LOGTAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
