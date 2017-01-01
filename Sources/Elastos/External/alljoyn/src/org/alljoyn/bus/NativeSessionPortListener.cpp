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

#include "org/alljoyn/bus/NativeSessionPortListener.h"
#include "org/alljoyn/bus/CSessionOpts.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeSessionPortListener::NativeSessionPortListener(
    /* [in] */ ISessionPortListener* sessionPortListener)
{
    IWeakReferenceSource::Probe(sessionPortListener)->GetWeakReference((IWeakReference**)&mSessionPortListener);
    assert(mSessionPortListener != NULL);
}

NativeSessionPortListener::~NativeSessionPortListener()
{}

bool NativeSessionPortListener::AcceptSessionJoiner(
    /* [in] */ ajn::SessionPort sessionPort,
    /* [in] */ const char* joiner,
    /* [in] */ const ajn::SessionOpts& opts)
{
    AutoPtr<ISessionOpts> so;
    CSessionOpts::New((ISessionOpts**)&so);
    so->SetTraffic(opts.traffic);
    so->SetMultipoint(opts.isMultipoint);
    so->SetProximity(opts.proximity);
    so->SetTransports(opts.transports);

    AutoPtr<ISessionPortListener> spl;
    mSessionPortListener->Resolve(EIID_ISessionPortListener, (IInterface**)&spl);
    if (spl == NULL) {
        Logger::E("NativeSessionPortListener", "AcceptSessionJoiner(): Can't get new local reference to ISessionPortListener");
        return false;
    }

    Boolean result;
    ECode ec = spl->AcceptSessionJoiner((Int16)sessionPort, String(joiner), so, &result);
    if (FAILED(ec)) {
        Logger::E("NativeSessionPortListener", "AcceptSessionJoiner(): Exception");
        return false;
    }

    return (bool)result;
}

void NativeSessionPortListener::SessionJoined(
    /* [in] */ ajn::SessionPort sessionPort,
    /* [in] */ ajn::SessionId id,
    /* [in] */ const char* joiner)
{
    AutoPtr<ISessionPortListener> spl;
    mSessionPortListener->Resolve(EIID_ISessionPortListener, (IInterface**)&spl);
    if (spl == NULL) {
        Logger::E("NativeSessionPortListener", "SessionJoined(): Can't get new local reference to ISessionPortListener");
    }

    ECode ec = spl->SessionJoined((Int16)sessionPort, (Int32)id, String(joiner));
    if (FAILED(ec)) {
        Logger::E("NativeSessionPortListener", "SessionJoined(): Exception");
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
