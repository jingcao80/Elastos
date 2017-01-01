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

#include "org/alljoyn/bus/SessionPortListener.h"
#include "org/alljoyn/bus/NativeSessionPortListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(SessionPortListener, Object, ISessionPortListener)

SessionPortListener::SessionPortListener()
{
    Create();
}

SessionPortListener::~SessionPortListener()
{
    Destroy();
}

void SessionPortListener::Create()
{
    NativeSessionPortListener* nspl = new NativeSessionPortListener(this);
    assert(nspl != NULL);

    mHandle = reinterpret_cast<Int64>(nspl);
}

void SessionPortListener::Destroy()
{
    NativeSessionPortListener* nspl = reinterpret_cast<NativeSessionPortListener*>(mHandle);

    assert(nspl != NULL);
    delete nspl;

    mHandle = 0;
    return;
}

ECode SessionPortListener::AcceptSessionJoiner(
    /* [in] */ Int16 sessionPort,
    /* [in] */ const String& joiner,
    /* [in] */ ISessionOpts* opts,
    /* [out] */ Boolean* accepted)
{
    return NOERROR;
}

ECode SessionPortListener::SessionJoined(
    /* [in] */ Int16 sessionPort,
    /* [in] */ Int32 id,
    /* [in] */ const String& joiner)
{
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
