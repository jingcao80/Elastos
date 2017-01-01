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

#include "org/alljoyn/bus/NativeSessionListener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeSessionListener::NativeSessionListener(
    /* [in] */ ISessionListener* sessionListener)
{
    IWeakReferenceSource::Probe(sessionListener)->GetWeakReference((IWeakReference**)&mSessionListener);
    assert(mSessionListener != NULL);
}

NativeSessionListener::~NativeSessionListener()
{}

void NativeSessionListener::SessionLost(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ ajn::SessionListener::SessionLostReason reason)
{
    AutoPtr<ISessionListener> sl;
    mSessionListener->Resolve(EIID_ISessionListener, (IInterface**)&sl);
    if (sl == NULL) {
        Logger::E("NativeSessionListener", "SessionLost(): Can't get new local reference to ISessionListener");
        return;
    }

    ECode ec = sl->SessionLost((Int32)sessionId, (Int32)reason);
    if (FAILED(ec)) {
        Logger::E("NativeSessionListener", "SessionLost(): Exception");
    }
}

void NativeSessionListener::SessionMemberAdded(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ const char* uniqueName)
{
    AutoPtr<ISessionListener> sl;
    mSessionListener->Resolve(EIID_ISessionListener, (IInterface**)&sl);
    if (sl == NULL) {
        Logger::E("NativeSessionListener", "SessionMemberAdded(): Can't get new local reference to ISessionListener");
        return;
    }

    ECode ec = sl->SessionMemberAdded((Int32)sessionId, String(uniqueName));
    if (FAILED(ec)) {
        Logger::E("NativeSessionListener", "SessionMemberAdded(): Exception");
    }
}

void NativeSessionListener::SessionMemberRemoved(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ const char* uniqueName)
{
    AutoPtr<ISessionListener> sl;
    mSessionListener->Resolve(EIID_ISessionListener, (IInterface**)&sl);
    if (sl == NULL) {
        Logger::E("NativeSessionListener", "SessionMemberRemoved(): Can't get new local reference to ISessionListener");
        return;
    }

    ECode ec = sl->SessionMemberRemoved((Int32)sessionId, String(uniqueName));
    if (FAILED(ec)) {
        Logger::E("NativeSessionListener", "SessionMemberRemoved(): Exception");
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
