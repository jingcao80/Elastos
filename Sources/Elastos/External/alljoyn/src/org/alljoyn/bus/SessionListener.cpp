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

#include "org/alljoyn/bus/SessionListener.h"
#include "org/alljoyn/bus/NativeSessionListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(SessionListener, Object, ISessionListener)

SessionListener::SessionListener()
    : mHandle(0)
{
    Create();
}

SessionListener::~SessionListener()
{
    Destroy();
}

void SessionListener::Create()
{
    NativeSessionListener* nsl = new NativeSessionListener(this);
    assert(nsl != NULL);

    mHandle = reinterpret_cast<Int64>(nsl);
}

void SessionListener::Destroy()
{
    NativeSessionListener* nsl = reinterpret_cast<NativeSessionListener*>(mHandle);

    assert(nsl != NULL);
    delete nsl;

    mHandle = 0;
    return;
}

ECode SessionListener::SessionLost(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 reason)
{
    return NOERROR;
}

ECode SessionListener::SessionMemberAdded(
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& uniqueName)
{
    return NOERROR;
}

ECode SessionListener::SessionMemberRemoved(
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& uniqueName)
{
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
