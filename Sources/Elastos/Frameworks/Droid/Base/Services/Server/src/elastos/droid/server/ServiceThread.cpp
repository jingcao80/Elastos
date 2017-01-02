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

#include "elastos/droid/server/ServiceThread.h"
#include <elastos/droid/os/Process.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::Process;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String ServiceThread::TAG("ServiceThread");

ServiceThread::ServiceThread(
    /* [in] */ const String& name,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean allowIo)
    : HandlerThread(name, priority)
    , mAllowIo(allowIo)
{
}

ECode ServiceThread::Run()
{
    Process::SetCanSelfBackground(FALSE);

    // For debug builds, log event loop stalls to dropbox for analysis.
    AutoPtr<IStrictMode> strictMode;
    CStrictMode::AcquireSingleton((IStrictMode**)&strictMode);
    Boolean bval;
    if (!mAllowIo && (strictMode->ConditionallyEnableDebugLogging(&bval), bval)) {
        String name;
        GetName(&name);
        Slogger::I(TAG, "Enabled StrictMode logging for %s looper.", name.string());
    }

    return HandlerThread::Run();
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos
